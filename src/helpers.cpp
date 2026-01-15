#include "helpers.hpp"
#include <cstdlib>

// Tu są definicje zmiennych (bez extern)
std::random_device rd;
std::mt19937 rng(rd());

double default_probability_generator() {
    return std::generate_canonical<double, 10>(rng);
}

// Inicjalizacja zmiennej globalnej
std::function<double()> probability_generator = default_probability_generator;