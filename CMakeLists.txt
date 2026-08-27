cmake_minimum_required(VERSION 3.20)

project(bush_tasks
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =========================
# Library
# =========================

add_library(bush_tasks
    src/core.cpp
)

target_include_directories(bush_tasks
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# =========================
# Tests
# =========================

include(CTest)

if(BUILD_TESTING)
    add_executable(bush_tasks_tests
        tests/test_core.cpp
    )

    target_link_libraries(bush_tasks_tests
        PRIVATE
            bush_tasks
    )

    add_test(
        NAME bush_tasks_tests
        COMMAND bush_tasks_tests
    )
endif()

# =========================
# Installation
# =========================

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
    TARGETS bush_tasks
    EXPORT bush_tasksTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

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

# =========================
# CMake Package
# =========================

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
