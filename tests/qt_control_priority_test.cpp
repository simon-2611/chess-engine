#include <catch2/catch_test_macros.hpp>
#include <QCoreApplication>
#include <QEventLoop>
#include <atomic>
#include <chrono>
#include <thread>
#include <functional>

#include "../src/qt/QtGameRunner.h"
#include "../src/qt/QtUI.h"
#include "../src/qt/ValidatingEnginePlayer.h"
#include "../src/Board.h"

using namespace chess;

namespace {

int argc = 1;
char appName[] = "qt_control_priority_test";
char* argv[] = {appName, nullptr};

QCoreApplication& ensureApp() {
    static QCoreApplication app(argc, argv);
    return app;
}

bool waitUntil(const std::function<bool()>& pred, std::chrono::milliseconds timeout) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        if (pred()) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return pred();
}

// Test seam: A QtUI that can be instructed to throw InvalidEngineMove
// to trigger the runner's error handler.
class TestUI : public QtUI {
public:
    std::atomic<bool> shouldThrowInvalidMove{false};

    Move requestHumanMove(const Board& board) override {
        if (shouldThrowInvalidMove.load()) {
            throw InvalidEngineMove("Simulated engine failure");
        }
        return QtUI::requestHumanMove(board);
    }
};

} // namespace

TEST_CASE("QtGameRunner burst Resign then NewGame priority", "[priority]") {
    ensureApp();
    auto ui = std::make_shared<QtUI>();
    QtGameRunner runner(ui);
    runner.start();

    // Wait for the game to start and be ready for input
    REQUIRE(waitUntil([&] { return ui->isAwaitingHumanMove(); }, std::chrono::seconds(2)));

    // Burst: Resign then NewGame immediately. NewGame should win.
    runner.requestResign();
    runner.requestNewGame();

    // The game should stay active (awaiting human move) and NOT be stopped by resignation.
    // NewGame resets sessionStopped_ and clears the resign request.
    REQUIRE(waitUntil([&] { return ui->isAwaitingHumanMove(); }, std::chrono::seconds(2)));
    
    runner.requestQuit();
    REQUIRE(runner.waitForFinished(3000));
}

TEST_CASE("QtGameRunner resign stopping and emitting completion", "[priority]") {
    ensureApp();
    auto ui = std::make_shared<QtUI>();
    QtGameRunner runner(ui);
    
    std::atomic<bool> finishedEmitted{false};
    QObject::connect(&runner, &QtGameRunner::gameFinished, &runner, [&](const QString&) {
        finishedEmitted = true;
    });

    runner.start();
    REQUIRE(waitUntil([&] { return ui->isAwaitingHumanMove(); }, std::chrono::seconds(2)));

    runner.requestResign();

    // Should emit gameFinished and stop the session (not the thread).
    REQUIRE(waitUntil([&] { return finishedEmitted.load(); }, std::chrono::seconds(2)));
    
    runner.requestQuit();
    REQUIRE(runner.waitForFinished(3000));
}

TEST_CASE("QtGameRunner invalid engine move recovery", "[priority]") {
    ensureApp();
    auto ui = std::make_shared<TestUI>();
    QtGameRunner runner(ui);
    
    std::atomic<bool> errorFinishedEmitted{false};
    QObject::connect(&runner, &QtGameRunner::gameFinished, &runner, [&](const QString& text) {
        if (text.contains("Engine returned an illegal/unset move")) {
            errorFinishedEmitted = true;
        }
    });

    runner.start();
    REQUIRE(waitUntil([&] { return ui->isAwaitingHumanMove(); }, std::chrono::seconds(2)));

    // Trigger an "engine" error via our TestUI seam
    ui->shouldThrowInvalidMove = true;
    ui->submitHumanMove(Move()); // Submit something to wake it up, it will throw instead

    REQUIRE(waitUntil([&] { return errorFinishedEmitted.load(); }, std::chrono::seconds(2)));
    
    // Now recover with a New Game
    errorFinishedEmitted = false;
    ui->shouldThrowInvalidMove = false;
    runner.requestNewGame();
    
    // Should be back to awaiting human move
    REQUIRE(waitUntil([&] { return ui->isAwaitingHumanMove(); }, std::chrono::seconds(2)));
    
    runner.requestQuit();
    REQUIRE(runner.waitForFinished(3000));
}
