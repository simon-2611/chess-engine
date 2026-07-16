# Chess Engine

Practice project for learning C++.

## Architecture
(Diagram in German)
![vgl.|598](resources/schach_architektur_uebersicht.svg)

## Module Overview
| File | Responsibility |
|---|---|
| `Types.h` | Constants & basic types |
| `Move.h` | Move definitions and special cases |
| `Board.h` | Board state, move execution, and FEN |
| `MoveGenerator.h` | Move generation logic |
| `Evaluator.*` | Evaluation heuristics (Material/Position) |
| `TranspositionTable.h` | Zobrist Hash cache |
| `SearchEngine.*` | Search algorithms (Minimax, Alpha-Beta, ID) |
| `Player.h` | Input abstraction (Human vs. Engine) |
| `UserInterface.h` | I/O and UI decoupling |
| `GameController.h` | Game loop and turn management |

## Dependency Graph
```
UserInterface  <---  GameController  --->  Player (Human/Engine)
                        |                    |
                        v                    v
                      Board  <---  MoveGenerator
                                       ^
                                       |
                                SearchEngine ---> Evaluator
                                       |
                                       v
                                TranspositionTable (optional)
```