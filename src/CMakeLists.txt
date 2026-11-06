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
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(chess main.cpp)
target_link_libraries(chess PRIVATE chess_core)

# ---------------------------------------------------------------------------
# Optional Qt 6 Widgets GUI (chess_qt). chess and chess_core stay Qt-free.
# ---------------------------------------------------------------------------
if(CHESS_QT_AVAILABLE)
    set(CHESS_QT_SOURCES
            qt/qt_main.cpp
            qt/QtUI.cpp
            qt/QtGameRunner.cpp
            qt/ValidatingEnginePlayer.cpp
            qt/ChessBoardWidget.cpp
            qt/MainWindow.cpp
            qt/MoveHistoryPanel.cpp
            qt/EngineAnalysisPanel.cpp
            qt/GameControlsWidget.cpp
            qt/MoveSelectionModel.cpp
            qt/pieces.qrc
    )

    add_executable(chess_qt ${CHESS_QT_SOURCES})
    target_include_directories(chess_qt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/qt)
    target_link_libraries(chess_qt PRIVATE chess_core Qt6::Widgets)
    if(Qt6Svg_FOUND)
        target_link_libraries(chess_qt PRIVATE Qt6::Svg)
        target_compile_definitions(chess_qt PRIVATE CHESS_HAS_QT_SVG=1)
    endif()
endif()
