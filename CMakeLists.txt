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
        include/nodes_preferences.hpp
        src/nodes_preferences.cpp
        include/helpers.hpp
        src/helpers.cpp
        include/factory.hpp
        src/factory.cpp
        tests/test_package.cpp
)]]

cmake_minimum_required(VERSION 4.0)
project(AAANetSim)

set(CMAKE_CXX_STANDARD 20)

# Mówimy, że pliki nagłówkowe są w folderze include/
include_directories(include)

# ==========================================
# 1. POBIERANIE GOOGLETEST
# ==========================================
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# Ważne dla Windowsa
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ==========================================
# 2. LISTA PLIKÓW LOGIKI (BEZ main.cpp i BEZ testów)
# ==========================================
# Wymieniamy tu wszystkie pliki .cpp, które tworzą logikę Twojego programu.
set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        src/nodes_preferences.cpp
        src/helpers.cpp
        src/factory.cpp
)

# ==========================================
# 3. PROGRAM GŁÓWNY (SYMULACJA)
# ==========================================
add_executable(AAANetSim
        main.cpp            # Twój główny main
        ${SOURCE_FILES}     # Dołączamy logikę
)

# ==========================================
# 4. PROGRAM TESTOWY
# ==========================================
enable_testing()

add_executable(AAANetSim_Tests
        tests/test_package.cpp  # Twój plik z testami
        ${SOURCE_FILES}         # Dołączamy logikę (musi być też tutaj!)
)

# Linkujemy bibliotekę GoogleTest do testów
target_link_libraries(AAANetSim_Tests GTest::gtest_main)

# Rejestracja testów w IDE
include(GoogleTest)
gtest_discover_tests(AAANetSim_Tests)
