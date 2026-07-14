# Header-Uebersicht

Reine Klassendeklarationen entsprechend der besprochenen Architektur, noch ohne `.cpp`-Implementierungen.

| Datei | Verantwortung |
|---|---|
| `Types.h` | Grundtypen: `Color`, `PieceType`, `Square`, `GameResult`, Rochaderechte |
| `Move.h` | Leichtgewichtiger Zug-Wertetyp inkl. Spezialfaellen (Rochade, En passant, Promotion) |
| `Board.h` | Stellung, `makeMove`/`undoMove`, Schach-/Ergebnisabfragen, FEN |
| `MoveGenerator.h` | Pseudo-legale und legale Zugerzeugung |
| `Evaluator.h` / `MaterialEvaluator.h` / `PositionalEvaluator.h` | Austauschbare Bewertungsfunktionen (Strategy-Pattern) |
| `TranspositionTable.h` | Zobrist-Hash-Cache fuer bereits berechnete Stellungen |
| `SearchEngine.h` / `MinimaxSearch.h` / `AlphaBetaSearch.h` / `IterativeDeepeningSearch.h` | Austauschbare Suchalgorithmen (Strategy-Pattern) |
| `Player.h` | Abstrahiert Mensch vs. Engine als Zugquelle |
| `UserInterface.h` | Entkoppelt Anzeige (Konsole/GUI/UCI) vom Rest |
| `GameController.h` | Orchestriert Spielablauf, Historie, Spielerwechsel |

## Abhaengigkeitsrichtung

```
UserInterface  <---  GameController  --->  Player (Human/Engine)
                          |                     |
                          v                     v
                        Board  <---  MoveGenerator
                                          ^
                                          |
                                   SearchEngine ---> Evaluator
                                          |
                                          v
                                TranspositionTable (optional)
```

## Naechste sinnvolle Schritte

1. `Board`/`MoveGenerator` implementieren und per Perft-Test gegen bekannte Referenzwerte validieren (z.B. Kiwipete-Stellung), **bevor** die Suche implementiert wird.
2. `MaterialEvaluator` + `MinimaxSearch` als einfachste lauffaehige Kombination implementieren.
3. Danach `AlphaBetaSearch` implementieren und mit `MinimaxSearch` auf identische Ergebnisse (bei geringerer Knotenzahl) pruefen.
4. `TranspositionTable` und `IterativeDeepeningSearch` als Ausbaustufe ergaenzen.
