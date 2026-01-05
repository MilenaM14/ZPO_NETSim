//
// Created by milen on 14.12.2025.
//

#ifndef AAANETSIM_PACKAGE_HPP
#define AAANETSIM_PACKAGE_HPP

 interfejs
#endif //AAANETSIM_PACKAGE_HPP
=======
#include "types.hpp"
#include <set> //unikalne ID, nie powstaną dwie paczki o tym samym nr, od razu posortowane

class Package
{
public:
    Package(); //konstruktor domyślny
    Package(ElementID); //konstruktor z parametrem, używany np. przy wczytaniu stanu fabryki z pliku
    Package(Package&& other) noexcept; //konstruktor przenoszący
    Package& operator = (Package&& other) noexcept; //operator przypisania przenoszący (działa jak konstruktor przenoszący ale dla istniejących obiektów)
    ElementID get_id() const {return ID_; } //odczytuje ID z paczki
    ~Package(); //destruktor

private:
    ElementID ID_; //przechowuje nr konkretnej paczki
    static std::set<ElementID> assigned_IDs; //przechowywać będzie przydzielone obecnie ID
    static std::set<ElementID> freed_IDs; //przechowuje kiedyś użyte ID, ale obecnie zwolnione
};
#endif //AAANETSIM_PACKAGE_HPP
 main
