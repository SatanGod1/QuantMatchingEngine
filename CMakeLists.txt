cmake_minimum_required(VERSION 3.16)

project(QuantMatchingEngine
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# =========================
# Matching Engine
# =========================

add_executable(
    matching_engine

    src/main.cpp
    src/OrderBook.cpp
    src/MatchingEngine.cpp
)

target_include_directories(
    matching_engine
    PRIVATE
    include
)


# =========================
# Tests
# =========================

add_executable(
    test_matching

    tests/test_matching.cpp
    src/OrderBook.cpp
    src/MatchingEngine.cpp
)

target_include_directories(
    test_matching
    PRIVATE
    include
)