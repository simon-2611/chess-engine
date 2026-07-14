#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Board.h"
#include "Move.h"
#include "Player.h"
#include "Types.h"
#include "UserInterface.h"

namespace chess {

// Ein Eintrag der Partie-Historie: Zug plus der zum Zuruecknehmen noetige
// Zustand sowie der Zobrist-Hash vor dem Zug (fuer Stellungswiederholung).
struct HistoryEntry {
    Move move;
    UndoInfo undoInfo;
    uint64_t hashBeforeMove = 0;
};

// Zentrale Orchestrierungsklasse. Kennt weder Details der Zuggenerierung
// noch der Suchalgorithmen - sie delegiert an Board/MoveGenerator bzw. an
// die jeweiligen Player-Implementierungen.
class GameController {
public:
    explicit GameController(std::shared_ptr<UserInterface> ui);

    void newGame();
    void loadFromFEN(const std::string& fen);

    void setWhitePlayer(std::unique_ptr<Player> player);
    void setBlackPlayer(std::unique_ptr<Player> player);

    // Fuehrt genau einen Halbzug der aktuell am Zug befindlichen Seite aus
    // (fragt je nach Player-Typ den Menschen oder die Suche) und
    // aktualisiert Historie sowie Spielergebnis.
    void playNextHalfMove(const SearchLimits& limits);

    // Spielt die Partie bis zum Ende (Matt/Patt/Remis) oder bis maxHalfMoves
    // erreicht ist - nuetzlich fuer automatisierte Engine-vs-Engine-Tests.
    void runUntilGameOver(const SearchLimits& limits, int maxHalfMoves = 500);

    bool undoLastHalfMove();

    const Board& board() const;
    GameResult currentResult() const;
    const std::vector<HistoryEntry>& history() const;

    std::string exportPGN() const;

private:
    Board board_;
    std::shared_ptr<UserInterface> ui_;
    std::unique_ptr<Player> whitePlayer_;
    std::unique_ptr<Player> blackPlayer_;
    std::vector<HistoryEntry> history_;
    GameResult result_ = GameResult::Ongoing;

    Player& currentPlayer();
};

} // namespace chess
