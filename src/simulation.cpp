//
// Created by milen on 15.01.2026.
//
#include "simulation.hpp"
#include <stdexcept>

void simulate(Factory& f, TimeOffset duration, std::function<void(Factory&, Time)> rf) {
    //sprawdzenie spójności sieci przed rozpoczęciem
    if (!f.is_consistent()) {
        throw std::logic_error("Network is inconsistent");
    }

    //główna pętla symulacji
    //czas symulacji zaczyna się od 1
    for (Time t = 1; t <= duration; ++t) {

        //kolejność etapów tury:
        f.do_deliveries(t);
        f.do_package_passing();
        f.do_work(t);

        //raportowanie (wywołanie funkcji przekazanej jako strategia)
        rf(f, t);
    }
}


SpecificTurnsReportNotifier::SpecificTurnsReportNotifier(std::set<Time> turns)
    : turns_(std::move(turns)) {}

bool SpecificTurnsReportNotifier::should_generate_report(Time t) const {
    //sprawdzamy, czy tura 't' znajduje się w zbiorze
    return turns_.find(t) != turns_.end();
}


IntervalReportNotifier::IntervalReportNotifier(TimeOffset interval)
    : interval_(interval) {}

bool IntervalReportNotifier::should_generate_report(Time t) const {
    // Raport co 'interval' tur, począwszy od pierwszej.
    // Dla interval=2: tury 1, 3, 5...
    // (1-1)%2 == 0 -> OK
    // (2-1)%2 == 1 -> NIE
    // (3-1)%2 == 0 -> OK
    if (interval_ == 0) return false; //zabezpieczenie przed dzieleniem przez 0
    return (t - 1) % interval_ == 0;
}