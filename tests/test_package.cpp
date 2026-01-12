//
// Created by milen on 10.01.2026.
//
#include "gtest/gtest.h"
#include "package.hpp"   // Dzięki include_directories(include) w CMake, to zadziała
#include "types.hpp"

// Test 1: Czy nowo stworzone paczki mają różne ID?
TEST(PackageTest, IsIdUniqueAfterCreation) {
    Package p1;
    Package p2;
    EXPECT_NE(p1.get_id(), p2.get_id());
}

// Test 2: Czy można stworzyć paczkę z konkretnym ID?
TEST(PackageTest, CanCreateWithSpecificId) {
    ElementID id = 999;
    Package p(id);
    EXPECT_EQ(p.get_id(), id);
}

// Test 3: Czy rzuci błędem przy duplikacie ID?
TEST(PackageTest, ThrowsErrorOnDuplicateId) {
    ElementID id = 500;
    Package p1(id);

    // Druga paczka z tym samym ID powinna wywołać błąd
    EXPECT_THROW({
        Package p2(id);
    }, std::invalid_argument);
}

// Test 4: Przenoszenie (Move Semantics)
TEST(PackageTest, MoveConstructorTransfersOwnership) {
    Package p1;
    ElementID original_id = p1.get_id();

    // Przenosimy p1 do p2
    Package p2(std::move(p1));

    // p2 powinna mieć ID p1
    EXPECT_EQ(p2.get_id(), original_id);

    // p1 powinna być teraz "pusta" (według Twojego kodu ID = -1)
    EXPECT_EQ(p1.get_id(), -1);
}