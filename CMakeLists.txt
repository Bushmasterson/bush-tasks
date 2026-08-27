cmake_minimum_required(VERSION 3.20)

project(bush_tasks
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(bush_tasks
    src/core.cpp
)

target_include_directories(bush_tasks
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
