#ifndef NETSIM_HELPERS_HPP
#define NETSIM_HELPERS_HPP

#include <functional>
#include <random>
#include "types.hpp"

// Deklarujemy, że te zmienne istnieją, ale są zdefiniowane w pliku .cpp
extern std::random_device rd;
extern std::mt19937 rng;

// To jest funkcja, której szuka test
extern double default_probability_generator();

// To jest zmienna globalna, którą test podmienia
// Słowo 'extern' jest tu KLUCZOWE
extern std::function<double()> probability_generator;

#endif //NETSIM_HELPERS_HPP