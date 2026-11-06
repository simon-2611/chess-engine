#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <thread>

#include <QCoreApplication>
#include <QMetaObject>

#include "../src/qt/QtUI.h"
#include "../src/Board.h"
#include "../src/MoveGenerator.h"

using namespace chess;

namespace {

int argc = 1;
char appName[] = "qt_ui_bridge_test";
char* argv[] = {appName, nullptr};

QCoreApplication& ensureApp() {
    static QCoreApplication app(argc, argv);
    return app;
}

} // namespace

TEST_CASE("QtUI requestHumanMove blocks until submitHumanMove", "[qt][bridge]") {
    ensureApp();
    QtUI ui;
    Board board = Board::startingPosition();

    std::atomic<bool> started{false};
    std::atomic<bool> finished{false};
    Move resultMove;

    std::thread worker([&] {
        started = true;
        resultMove = ui.requestHumanMove(board);
        finished = true;
    });

    // Wait until worker is blocked inside requestHumanMove
    while (!started) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    REQUIRE_FALSE(finished.load());

    const auto legal = MoveGenerator::generateLegalMoves(board);
    REQUIRE_FALSE(legal.empty());
    const Move expected = legal.front();

    // GUI-thread submission
    REQUIRE(QMetaObject::invokeMethod(&ui, [&] { ui.submitHumanMove(expected); },
                                      Qt::QueuedConnection));

    // Process queued invoke and wake-ups
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!finished && std::chrono::steady_clock::now() < deadline) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    worker.join();
    REQUIRE(finished.load());
    REQUIRE(resultMove == expected);
}

TEST_CASE("QtUI cancelHumanInput throws InputCancelled", "[qt][bridge]") {
    ensureApp();
    QtUI ui;
    Board board = Board::startingPosition();

    std::atomic<bool> gotCancel{false};
    std::thread worker([&] {
        try {
            (void)ui.requestHumanMove(board);
        } catch (const InputCancelled& ex) {
            gotCancel = (ex.reason() == InputCancelled::Reason::Undo);
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    REQUIRE(QMetaObject::invokeMethod(
        &ui, [&] { ui.cancelHumanInput(InputCancelled::Reason::Undo); }, Qt::QueuedConnection));

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!gotCancel && std::chrono::steady_clock::now() < deadline) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    worker.join();
    REQUIRE(gotCancel.load());
}

TEST_CASE("QtUI displayBoard emits boardUpdated FEN", "[qt][bridge]") {
    ensureApp();
    QtUI ui;
    QString receivedFen;
    bool signaled = false;
    QObject::connect(&ui, &QtUI::boardUpdated, [&](const QString& fen) {
        receivedFen = fen;
        signaled = true;
    });

    Board board = Board::startingPosition();
    ui.displayBoard(board);
    QCoreApplication::processEvents();

    REQUIRE(signaled);
    REQUIRE(receivedFen.toStdString() == board.toFEN());
}
