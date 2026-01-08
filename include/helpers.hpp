//
// Created by milen on 8.01.2026.
//

#ifndef AAANETSIM_HELPERS_HPP
#define AAANETSIM_HELPERS_HPP

#include <random>
#include "types.hpp"

//globarny generator
extern std::random_device rd;
extern std::mt19937 rng;

extern double default_probability_generator(); //domyslna funkcja losujaca
extern  ProbabilityGenerator probability_generator; //obiekt funkcyjny

#endif //AAANETSIM_HELPERS_HPP