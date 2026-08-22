cmake_minimum_required(VERSION 3.20)
project(bush-tasks VERSION 1.0 LANGUAGES CXX)

add_executable(bush-tasks
    src/main.cpp
    src/core.cpp
    src/cli.cpp
)

target_include_directories(bush-tasks PRIVATE include)

add_executable(test_core
    tests/test_core.cpp
    src/core.cpp
)

target_include_directories(test_core PRIVATE include)

enable_testing()
add_test(NAME core_test COMMAND test_core)