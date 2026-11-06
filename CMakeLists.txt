cmake_minimum_required(VERSION 3.16)
project(chess-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Unit tests enabled by default; can be disabled via -DCHESS_BUILD_TESTS=OFF.
# Requires internet access during initial configuration (Catch2 via FetchContent).
option(CHESS_BUILD_TESTS "Build Unit-Tests" ON)

# Optional Qt 6 Widgets GUI (chess_qt). chess and chess_core stay Qt-free.
option(CHESS_BUILD_QT "Build the Qt 6 Widgets GUI (chess_qt)" ON)

set(CHESS_QT_AVAILABLE FALSE)
if(CHESS_BUILD_QT)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    find_package(Qt6 QUIET COMPONENTS Widgets)
    if(Qt6Widgets_FOUND)
        find_package(Qt6 QUIET COMPONENTS Svg)
        set(CHESS_QT_AVAILABLE TRUE)
        if(Qt6Svg_FOUND)
            message(STATUS "Qt6 Widgets+Svg found: building chess_qt GUI")
        else()
            message(WARNING "Qt6 Svg not found; chess_qt will use Unicode piece fallback only. "
                    "Hint: -DQt6Svg_DIR=\"$(brew --prefix qtsvg)/lib/cmake/Qt6Svg\"")
        endif()
    else()
        message(WARNING "CHESS_BUILD_QT is ON but Qt6 Widgets was not found. "
                "Skipping chess_qt. Install Qt 6 or pass -DCHESS_BUILD_QT=OFF. "
                "Hint: -DCMAKE_PREFIX_PATH=\"$(brew --prefix qtbase);$(brew --prefix qtsvg)\"")
    endif()
endif()

add_subdirectory(src)

if(CHESS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
