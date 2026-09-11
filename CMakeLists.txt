cmake_minimum_required(VERSION 3.20)

project(bush_tasks
    VERSION 1.0.0
    DESCRIPTION "Bush Tasks"
    LANGUAGES CXX
)

# ─────────────────────────────────────────────
# Global settings
# ─────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ─────────────────────────────────────────────
# Options
# ─────────────────────────────────────────────
option(BUSH_TASKS_BUILD_CLI "Build the bush-tasks CLI executable" ON)
option(BUSH_TASKS_BUILD_TESTS "Build the unit tests" OFF)
option(BUSH_TASKS_STATIC "Link the CLI statically (where supported)" OFF)
option(BUSH_TASKS_STRIP "Strip release binaries (GCC/Clang)" ON)

# ─────────────────────────────────────────────
# Common target settings
# ─────────────────────────────────────────────
function(bush_tasks_common_settings target)
    target_compile_features(${target} PUBLIC cxx_std_17)

    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )
        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:-O3>
            $<$<CONFIG:RelWithDebInfo>:-O2>
        )
    endif()
endfunction()

# ─────────────────────────────────────────────
# Library
# ─────────────────────────────────────────────
add_library(bush_tasks
    src/core.cpp
)
add_library(bush_tasks::bush_tasks ALIAS bush_tasks)

target_include_directories(bush_tasks
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

bush_tasks_common_settings(bush_tasks)

# ─────────────────────────────────────────────
# CLI executable
# ─────────────────────────────────────────────
if(BUSH_TASKS_BUILD_CLI)
    add_executable(bush-tasks src/main.cpp)
    target_link_libraries(bush-tasks PRIVATE bush_tasks)
    bush_tasks_common_settings(bush-tasks)

    if(BUSH_TASKS_STATIC)
        if(MSVC)
            set_property(TARGET bush-tasks PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
        elseif(MINGW)
            target_link_options(bush-tasks PRIVATE
                -static
                -static-libgcc
                -static-libstdc++
            )
        elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            target_link_options(bush-tasks PRIVATE
                -static-libgcc
                -static-libstdc++
            )
        endif()
    endif()

    if(BUSH_TASKS_STRIP
       AND CMAKE_BUILD_TYPE STREQUAL "Release"
       AND NOT MSVC
       AND NOT APPLE)
        target_link_options(bush-tasks PRIVATE -s)
    endif()
endif()

# ─────────────────────────────────────────────
# Tests
# ─────────────────────────────────────────────
if(BUSH_TASKS_BUILD_TESTS)
    enable_testing()

    add_executable(bush_tasks_tests
        tests/test_core.cpp
    )
    target_link_libraries(bush_tasks_tests PRIVATE bush_tasks)
    bush_tasks_common_settings(bush_tasks_tests)

    add_test(
        NAME bush_tasks_tests
        COMMAND bush_tasks_tests
    )
endif()

# ─────────────────────────────────────────────
# Install
# ─────────────────────────────────────────────
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
    TARGETS bush_tasks
    EXPORT bush_tasksTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUSH_TASKS_BUILD_CLI)
    install(
        TARGETS bush-tasks
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT bush_tasksTargets
    FILE bush_tasksTargets.cmake
    NAMESPACE bush_tasks::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bush_tasks
)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/bush_tasksConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/bush_tasksConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bush_tasks
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/bush_tasksConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/bush_tasksConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/bush_tasksConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bush_tasks
)
