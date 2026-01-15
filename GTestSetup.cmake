include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG 58d77fa8070e8cec2dc1ed015d66b454c8d78850
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(TEST_DIR ./tests)

# Tu wypisz wszystkie pliki zawierające testy
set(SOURCE_TESTS_FILES
        ${TEST_DIR}/NetSimBasic_Tests.cpp
        ${TEST_DIR}/test_storage_types.cpp
        ${TEST_DIR}/test_nodes.cpp
        ${TEST_DIR}/test_factory.cpp
        ${TEST_DIR}/test_package.cpp
        ${TEST_DIR}/test_reports.cpp
        ${TEST_DIR}/test_simulate.cpp
        # ${TEST_DIR}/test_factory_io.cpp  <-- Odkomentuj jeśli ten plik istnieje fizycznie!
)

# Tworzymy osobny plik wykonywalny TYLKO dla testów
# Łączymy: Logikę biznesową (SOURCE_FILES) + Pliki testowe (SOURCE_TESTS_FILES)
add_executable(
        ${PROJECT_NAME}_Tests
        ${SOURCE_FILES}
        ${SOURCE_TESTS_FILES}
)

# Linkujemy bibliotekę GTest/GMock
target_link_libraries(
        ${PROJECT_NAME}_Tests
        GTest::gmock_main
)

include_directories(mocks)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_Tests)