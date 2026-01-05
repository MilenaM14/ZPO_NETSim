//
// Created by milen on 14.12.2025.
//

#ifndef AAANETSIM_TYPES_H
#define AAANETSIM_TYPES_H

#include <functional>  //pozwala traktować funkcje jak zmienne

using ElementID = int; //nr identyfikacyjny
using Time = int; //czas symulacji, która tura
using TimeOffset = int; //długość trwania procesu
using ProbabilityGenerator = std::function<double()>; //generator losowości

#endif //AAANETSIM_TYPES_H