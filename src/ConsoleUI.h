#pragma once

#include "UserInterface.h"

namespace chess {

// Einfachste UserInterface-Implementierung: Textausgabe auf stdout,
// Zugeingabe als Text auf stdin. Dient als lauffaehiger Platzhalter, bis
// eine grafische Oberflaeche oder ein UCI-Adapter existiert.
class ConsoleUI : public UserInterface {
public:
    void displayBoard(const Board& board) override;
    Move requestHumanMove(const Board& board) override;
    void announceResult(GameResult result) override;
    void reportEngineMove(const Move& move, int scoreCentipawns) override;
};

} // namespace chess
