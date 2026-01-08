//
// Created by jowit on 08.01.2026.
//
#include "nodes_preferences.hpp"

ReceiverPreferences::ReceiverPreferences(ProbabilityGenerator pg)
    : pg_(pg) {
} // Zapis generatora do pola klasy

void ReceiverPreferences::add_receiver(IPackageReceiver* r)
{
    preferences_[r] = 0.0;
    std::size_t n = preferences_.size(); //ilość odbiorców
    for (auto& receiver : preferences_) {
        receiver.second = 1.0/n; //przypisnie wartosci
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {
    preferences_.erase(r); //usuwa odbiorce
    if (preferences_.empty()) return;

    double p = 1.0 / preferences_.size(); //nowa wartosc po usunieciu odbiorcy

    for (auto& receiver : preferences_) {
        receiver.second = p; // przypisanie nowych wartosci
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    if (preferences_.empty()) return nullptr; //żeby nie iterowac po pustej mapie

    auto p = pg_(); // losuje liczbę z [0,1]
    double cumulative = 0.0;

    for (auto& receiver : preferences_) {
        cumulative += receiver.second;
        if (p <= cumulative) {
            return receiver.first; // wybrany odbiorca
        }
    }

    return preferences_.rbegin()->first;
}
