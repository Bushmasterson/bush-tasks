cmake_minimum_required(VERSION 3.20)

project(bush_tasks_consumer
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(bush_tasks CONFIG REQUIRED)

add_executable(bush_tasks_consumer
    main.cpp
)

target_link_libraries(bush_tasks_consumer
    PRIVATE
        bush_tasks::bush_tasks
)
