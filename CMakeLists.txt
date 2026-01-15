#[[cmake_minimum_required(VERSION 4.0)
project(AAANetSim)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(AAANetSim
        main.cpp
        src/package.cpp
        include/types.hpp
        include/package.hpp
        include/storage_types.hpp
        src/storage_types.cpp
        include/nodes.hpp
        src/nodes.cpp
        include/helpers.hpp
        src/helpers.cpp
        include/factory.hpp
        src/factory.cpp
)]]

cmake_minimum_required(VERSION 4.0)
project(NetSim)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall)

# Set default compile flags for GCC
set(GOOD_COMPILERS Clang GNU)
if (CMAKE_CXX_COMPILER_ID IN_LIST GOOD_COMPILERS)
    message(STATUS "GCC detected, adding compile flags")
    add_compile_options(-Wextra -Werror -Wpedantic -pedantic-errors)
endif ()

set(SOURCE_DIR ./src)
set(SOURCE_FILES
        ${SOURCE_DIR}/package.cpp
        ${SOURCE_DIR}/storage_types.cpp
        ${SOURCE_DIR}/helpers.cpp
        ${SOURCE_DIR}/nodes.cpp
        ${SOURCE_DIR}/factory.cpp
        ${SOURCE_DIR}/reports.cpp
        ${SOURCE_DIR}/simulation.cpp
        include/reports.hpp
        include/simulation.hpp
        src/reports.cpp
        src/simulation.cpp)

add_executable(${PROJECT_NAME} main.cpp ${SOURCE_FILES})

include_directories(
        src
        include
)

include(GTestSetup.cmake)