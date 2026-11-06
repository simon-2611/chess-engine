#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

#include <QCoreApplication>
#include <QEventLoop>

#include "../src/Board.h"
#include "../src/MaterialEvaluator.h"
#include "../src/MoveGenerator.h"
#include "../src/SearchEngine.h"
#include "../src/qt/QtGameRunner.h"
#include "../src/qt/QtUI.h"
#include "../src/qt/ValidatingEnginePlayer.h"

using namespace chess;

namespace {

int argc = 1;
char appName[] = "qt_game_runner_test";
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

} // namespace

TEST_CASE("QtGameRunner start returns immediately and quit joins while human waits",
          "[qt][runner][shutdown]") {
    ensureApp();
    auto ui = std::make_shared<QtUI>();
    QtGameRunner runner(ui);

    // start() must not block the caller (no forever started-slot on a QThread).
    const auto startedAt = std::chrono::steady_clock::now();
    runner.start();
    const auto elapsed = std::chrono::steady_clock::now() - startedAt;
    REQUIRE(elapsed < std::chrono::milliseconds(500));
    REQUIRE(runner.isRunning());

    REQUIRE(waitUntil([&] { return ui->isAwaitingHumanMove(); }, std::chrono::seconds(2)));

    // Thread-safe control from the "GUI" thread — must not depend on a worker event loop.
    runner.requestQuit();
    REQUIRE(runner.waitForFinished(3000));
    REQUIRE_FALSE(runner.isRunning());
}

TEST_CASE("QtGameRunner control requests work while gameplay waits without queued worker slots",
          "[qt][runner][controls]") {
    ensureApp();
    auto ui = std::make_shared<QtUI>();
    QtGameRunner runner(ui);

    std::atomic<int> newGameStatusCount{0};
    QObject::connect(&runner, &QtGameRunner::statusChanged, &runner,
                     [&](const QString& text) {
                         if (text.contains(QStringLiteral("New game"), Qt::CaseInsensitive)) {
                             newGameStatusCount.fetch_add(1);
                         }
                     });

    runner.start();
    REQUIRE(waitUntil([&] { return ui->isAwaitingHumanMove(); }, std::chrono::seconds(2)));

    // Direct call (not QMetaObject QueuedConnection onto a starved QThread).
    runner.requestNewGame();

    REQUIRE(waitUntil([&] { return newGameStatusCount.load() >= 1; }, std::chrono::seconds(2)));
    REQUIRE(waitUntil([&] { return ui->isAwaitingHumanMove(); }, std::chrono::seconds(2)));

    runner.requestUndo(); // no history yet — must not hang
    runner.requestResign();
    REQUIRE(waitUntil(
        [&] {
            return !ui->isAwaitingHumanMove() || !runner.isRunning();
        },
        std::chrono::seconds(2)));

    runner.requestQuit();
    REQUIRE(runner.waitForFinished(3000));
}

TEST_CASE("QtGameRunner default player configuration is human vs human",
          "[qt][runner][defaults]") {
    ensureApp();
    auto ui = std::make_shared<QtUI>();
    QtGameRunner runner(ui);
    REQUIRE(runner.whitePlayerType() == QtGameRunner::PlayerType::Human);
    REQUIRE(runner.blackPlayerType() == QtGameRunner::PlayerType::Human);
}

TEST_CASE("Invalid engine move never reaches Board::makeMove via ValidatingEnginePlayer",
          "[qt][runner][engine-guard]") {
    ensureApp();
    Board board = Board::startingPosition();
    const Board before = board;

    class BadEngine : public SearchEngine {
    public:
        BadEngine() : SearchEngine(std::make_shared<MaterialEvaluator>()) {}
        SearchResult findBestMove(Board&, const SearchLimits&) override {
            return SearchResult{};
        }
    };

    ValidatingEnginePlayer player(std::make_unique<BadEngine>());
    SearchLimits limits;
    try {
        (void)player.getMove(board, limits);
        FAIL("Expected InvalidEngineMove");
    } catch (const InvalidEngineMove&) {
        // Board must be untouched — defense happens before makeMove.
        REQUIRE(board.toFEN() == before.toFEN());
    }
}
