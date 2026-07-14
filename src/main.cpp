#include <iostream>
#include <memory>

#include "ConsoleUI.h"
#include "GameController.h"
#include "MaterialEvaluator.h"
#include "MinimaxSearch.h"
#include "Player.h"

using namespace chess;

int main() {
    auto ui = std::make_shared<ConsoleUI>();
    GameController controller(ui);

    auto evaluator = std::make_shared<MaterialEvaluator>();
    controller.setWhitePlayer(std::make_unique<HumanPlayer>(ui));
    controller.setBlackPlayer(
        std::make_unique<EnginePlayer>(std::make_unique<MinimaxSearch>(evaluator)));

    std::cout << "Chess Engine - Project skeleton is running.\n";
    std::cout << "(Board/MoveGenerator are still placeholders - see TODOs in src/.)\n";

    SearchLimits limits;
    limits.maxDepth = 4;
    controller.newGame();
    controller.runUntilGameOver(limits, /*maxHalfMoves=*/1);

    return 0;
}
