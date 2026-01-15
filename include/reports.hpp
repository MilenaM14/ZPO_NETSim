//
// Created by milen on 15.01.2026.
//

#ifndef AAANETSIM_REPORTS_HPP
#define AAANETSIM_REPORTS_HPP

#include "factory.hpp"
#include <iostream>

//generuje raport o strukturze sieci
void generate_structure_report(const Factory& f, std::ostream& os);

//generuje raport o stanie symulacji w danej turze
void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t);

#endif //AAANETSIM_REPORTS_HPP