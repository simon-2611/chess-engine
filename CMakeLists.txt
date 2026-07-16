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

add_subdirectory(src)

if(CHESS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
