cmake_minimum_required(VERSION 4.0)
project(AAANetSim)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall)

# Flagi kompilatora
set(GOOD_COMPILERS Clang GNU)
if (CMAKE_CXX_COMPILER_ID IN_LIST GOOD_COMPILERS)
    message(STATUS "GCC/Clang detected, adding compile flags")
    add_compile_options(-Wextra -Werror -Wpedantic -pedantic-errors)
endif ()

include_directories(src include)

# --- 1. DEFINICJA LOGIKI BIZNESOWEJ (BEZ TESTÓW I BEZ MAIN.CPP) ---
# To jest kluczowe. Tutaj tylko pliki .cpp z folderu src/
set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        src/helpers.cpp
        src/nodes.cpp
        src/factory.cpp
        src/reports.cpp
        src/simulation.cpp
)

# --- 2. CEL: GŁÓWNA APLIKACJA ---
# Tworzy AAANetSim.exe z main.cpp + logika biznesowa
add_executable(${PROJECT_NAME} main.cpp ${SOURCE_FILES})

# --- 3. KONFIGURACJA TESTÓW ---
# Wczytuje drugi plik (GTestSetup.cmake), który zbuduje AAANetSim_Tests
include(GTestSetup.cmake)