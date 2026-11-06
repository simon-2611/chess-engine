include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.4
)
FetchContent_MakeAvailable(Catch2)

# Pure (Qt-free) unit tests, including GUI helper logic that does not need Widgets.
add_executable(chess_tests
    move_test.cpp
    board_test.cpp
    move_generator_test.cpp
    console_ui_test.cpp
    evaluate_game_result_test.cpp
    board_square_mapping_test.cpp
    move_selection_model_test.cpp
    piece_asset_names_test.cpp
    validating_engine_player_test.cpp
    ../src/qt/MoveSelectionModel.cpp
    ../src/qt/ValidatingEnginePlayer.cpp
)

target_include_directories(chess_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/qt
)
target_link_libraries(chess_tests PRIVATE chess_core Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(chess_tests)

# Bridge / Qt tests (only when the Qt GUI target is available).
if(CHESS_QT_AVAILABLE)
    add_executable(chess_qt_tests
        qt_ui_bridge_test.cpp
        qt_game_runner_test.cpp
        qt_control_priority_test.cpp
        validating_engine_player_test.cpp
        ../src/qt/QtUI.cpp
        ../src/qt/QtGameRunner.cpp
        ../src/qt/ValidatingEnginePlayer.cpp
        ../src/qt/MoveSelectionModel.cpp
    )
    target_include_directories(chess_qt_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/qt
    )
    target_link_libraries(chess_qt_tests PRIVATE
        chess_core
        Catch2::Catch2WithMain
        Qt6::Core
        Qt6::Widgets
    )
    if(Qt6Svg_FOUND)
        target_link_libraries(chess_qt_tests PRIVATE Qt6::Svg)
    endif()
    # Runner integration tests drive std::thread workers; keep Catch discovery single-process.
    catch_discover_tests(chess_qt_tests)
endif()
