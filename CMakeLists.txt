cmake_minimum_required(VERSION 3.20)
project(bush-tasks VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(core_lib src/core.cpp)
target_include_directories(core_lib PUBLIC include)

add_executable(bush-tasks
    src/main.cpp
    src/cli.cpp
)
target_link_libraries(bush-tasks PRIVATE core_lib)

enable_testing()
add_executable(test_core tests/test_core.cpp)
target_link_libraries(test_core PRIVATE core_lib)
add_test(NAME core_test COMMAND test_core)