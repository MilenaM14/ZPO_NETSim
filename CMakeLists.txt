cmake_minimum_required(VERSION 4.0)
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
        include/nodes_preferences.hpp
        src/nodes_preferences.cpp
)