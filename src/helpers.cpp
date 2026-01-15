#include "helpers.hpp"
#include <cstdlib>
#include <random>

// Inicjalizacja generatorów
std::random_device rd;
std::mt19937 rng(rd());

double default_probability_generator() {
    return std::generate_canonical<double, 10>(rng);
}

// Przypisanie domyślnej funkcji do zmiennej globalnej
std::function<double()> probability_generator = default_probability_generator;