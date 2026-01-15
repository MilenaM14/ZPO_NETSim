#ifndef NETSIM_HELPERS_HPP
#define NETSIM_HELPERS_HPP

#include <functional>
#include <random>

#include "types.hpp"

// Definicja typu generatora prawdopodobieństwa
extern std::random_device rd;
extern std::mt19937 rng;

extern double default_probability_generator();

// Kluczowe: probability_generator musi być obiektem std::function,
// aby testy mogły go podmienić na mocka.
extern std::function<double()> probability_generator;

#endif // NETSIM_HELPERS_HPP