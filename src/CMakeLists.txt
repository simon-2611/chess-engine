set(CHESS_CORE_SOURCES
    Move.cpp
    Board.cpp
    MoveGenerator.cpp
    MaterialEvaluator.cpp
    PositionalEvaluator.cpp
    TranspositionTable.cpp
    SearchEngine.cpp
    MinimaxSearch.cpp
    AlphaBetaSearch.cpp
    IterativeDeepeningSearch.cpp
    Player.cpp
    GameController.cpp
    ConsoleUI.cpp
)

add_library(chess_core STATIC ${CHESS_CORE_SOURCES})

target_include_directories(chess_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(chess main.cpp)
target_link_libraries(chess PRIVATE chess_core)
