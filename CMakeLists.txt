cmake_minimum_required(VERSION 3.20)

set(BUSH_TASKS_VERSION "2.2.1" CACHE STRING "Project version")

project(bush_tasks
    VERSION     ${BUSH_TASKS_VERSION}
    DESCRIPTION "A small cross-platform CLI task manager"
    HOMEPAGE_URL "https://github.com/Bushmasterson/bush-tasks"
    LANGUAGES   CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(BUSH_TASKS_BUILD_CLI   "Build the bush-tasks CLI executable"       ON)
option(BUSH_TASKS_BUILD_TESTS "Build the unit tests"                      OFF)
option(BUSH_TASKS_STATIC      "Link the CLI statically (where supported)" OFF)
option(BUSH_TASKS_STRIP       "Strip release binaries (GCC/Clang)"        ON)
option(BUSH_TASKS_WERROR      "Treat warnings as errors"                  OFF)

function(bush_tasks_setup_target target)
    target_compile_features(${target} PUBLIC cxx_std_17)

    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wnon-virtual-dtor
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
        )
    endif()

    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:RelWithDebInfo>:-O2>
    )

    if(BUSH_TASKS_WERROR)
        if(MSVC)
            target_compile_options(${target} PRIVATE /WX)
        else()
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()

set(BUSH_TASKS_GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")

configure_file(
    "${PROJECT_SOURCE_DIR}/include/bush_tasks/version.h.in"
    "${BUSH_TASKS_GENERATED_INCLUDE_DIR}/bush_tasks/version.h"
    @ONLY
)

file(READ "${PROJECT_SOURCE_DIR}/locales/en.json" LOCALES_EN_JSON)
file(READ "${PROJECT_SOURCE_DIR}/locales/ru.json" LOCALES_RU_JSON)

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/locales_data.h.in"
    "${BUSH_TASKS_GENERATED_INCLUDE_DIR}/bush_tasks/locales_data.h"
    @ONLY
)

add_library(bush_tasks
    src/core.cpp
    src/cli.cpp
    src/config.cpp
    src/i18n.cpp
)
add_library(bush_tasks::bush_tasks ALIAS bush_tasks)

target_include_directories(bush_tasks
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${BUSH_TASKS_GENERATED_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/third_party>
)

bush_tasks_setup_target(bush_tasks)

if(BUSH_TASKS_BUILD_CLI)
    add_executable(bush-tasks src/main.cpp)
    target_link_libraries(bush-tasks PRIVATE bush_tasks)
    bush_tasks_setup_target(bush-tasks)

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

if(BUSH_TASKS_BUILD_TESTS)
    enable_testing()

    add_executable(bush_tasks_tests
        tests/test_core.cpp
    )
    target_link_libraries(bush_tasks_tests PRIVATE bush_tasks)
    bush_tasks_setup_target(bush_tasks_tests)

    add_test(
        NAME bush_tasks_tests
        COMMAND bush_tasks_tests
    )
endif()

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
    DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PATTERN "*.in" EXCLUDE
)

install(
    FILES
        "${BUSH_TASKS_GENERATED_INCLUDE_DIR}/bush_tasks/version.h"
        "${BUSH_TASKS_GENERATED_INCLUDE_DIR}/bush_tasks/locales_data.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bush_tasks
)

install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/locales/"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/bush-tasks/locales
)

install(
    FILES "${PROJECT_SOURCE_DIR}/packaging/bush-tasks.1"
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
)

install(
    FILES "${PROJECT_SOURCE_DIR}/packaging/completions/bush-tasks.bash"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/bash-completion/completions
    RENAME bush-tasks
)

install(
    FILES "${PROJECT_SOURCE_DIR}/packaging/completions/_bush-tasks"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/zsh/site-functions
)

install(
    FILES "${PROJECT_SOURCE_DIR}/packaging/completions/bush-tasks.fish"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/fish/vendor_completions.d
)

install(
    FILES "${PROJECT_SOURCE_DIR}/packaging/completions/bush-tasks.ps1"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/powershell-completion
)

install(
    EXPORT bush_tasksTargets
    FILE bush_tasksTargets.cmake
    NAMESPACE bush_tasks::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bush_tasks
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/bush_tasksConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/bush_tasksConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bush_tasks
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/bush_tasksConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/bush_tasksConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/bush_tasksConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bush_tasks
)
