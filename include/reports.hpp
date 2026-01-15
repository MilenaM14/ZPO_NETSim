#ifndef NETSIM_REPORTS_HPP
#define NETSIM_REPORTS_HPP

#include "factory.hpp"
#include "types.hpp"
#include <iostream>

// --- Interfejsy raportów ---

class SpecificTurnReport {
public:
    virtual void add_worker_entry(const Worker& worker) = 0;
    virtual void add_storehouse_entry(const Storehouse& storehouse) = 0;
    virtual ~SpecificTurnReport() = default;
};

class IntervalReport {
public:
    virtual void add_turn_report(const SpecificTurnReport& report) = 0;
    virtual ~IntervalReport() = default;
};

// --- Funkcje generujące raporty ---

void generate_structure_report(const Factory& factory, std::ostream& os);

// POPRAWKA: Nazwa funkcji zgodna z oczekiwaniami testów
void generate_simulation_turn_report(const Factory& factory, std::ostream& os, Time t);

#endif // NETSIM_REPORTS_HPP