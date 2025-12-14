//
// Created by milen on 14.12.2025.
//

#ifndef AAANETSIM_TYPES_H
#define AAANETSIM_TYPES_H

#include <functional>  //pozwala traktować funkcje jak zmienne

//nr identyfikacyjny
using ElementID = int;
//czas symulacji, która tura
using Time = int;
//długość trwania procesu
using TimeOffset = int;
//generator losowości
using ProbabilityGenerator = std::function<double()>;

#endif //AAANETSIM_TYPES_H