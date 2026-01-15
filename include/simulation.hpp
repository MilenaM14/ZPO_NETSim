//
// Created by milen on 15.01.2026.
//

#ifndef AAANETSIM_SIMULATION_HPP
#define AAANETSIM_SIMULATION_HPP

#include "factory.hpp"
#include "types.hpp"
#include <functional>
#include <set>

//funkcja przeprowadzająca symulację
//f - fabryka (sieć)
//duration - czas trwania symulacji (liczba tur)
//rf - funkcja raportująca (Strategy Pattern), wywoływana w każdej turze
void simulate(Factory& f, TimeOffset duration, std::function<void(Factory&, Time)> rf);

//klasa decydująca o raportowaniu w konkretnych turach
class SpecificTurnsReportNotifier {
public:
    SpecificTurnsReportNotifier(std::set<Time> turns);
    bool should_generate_report(Time t) const;

private:
    std::set<Time> turns_;
};

//klasa decydująca o raportowaniu w stałych interwałach
class IntervalReportNotifier {
public:
    IntervalReportNotifier(TimeOffset interval);
    bool should_generate_report(Time t) const;

private:
    TimeOffset interval_;
};

#endif //AAANETSIM_SIMULATION_HPP