cmake_minimum_required(VERSION 3.16)

project(QuantMatchingEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# CORE LIBRARY
# ============================================================

add_library(matching_engine
    src/Order.cpp
    src/OrderBook.cpp
    src/MatchingEngine.cpp
    src/MarketData.cpp
)

target_include_directories(
    matching_engine
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# ============================================================
# MAIN EXECUTABLE
# ============================================================

add_executable(matching_engine_demo
    src/main.cpp
)

target_link_libraries(
    matching_engine_demo
    PRIVATE
        matching_engine
)

# ============================================================
# TESTS
# ============================================================

enable_testing()

add_executable(
    matching_tests
    tests/test_matching.cpp
)

target_link_libraries(
    matching_tests
    PRIVATE
        matching_engine
)

add_test(
    NAME matching_tests
    COMMAND matching_tests
)