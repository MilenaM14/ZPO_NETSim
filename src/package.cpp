//
// Created by milen on 14.12.2025.
//
 interfejs
=======
#include "package.hpp"
#include <stdexcept> //rzucanie blędów przy np. utorzeniu dwóch paczek z tym samym ID

std::set<ElementID> Package::assigned_IDs;
std::set<ElementID> Package::freed_IDs;

//konstruktor domyślny
Package::Package()
{
    if (freed_IDs.empty())
    {
        if (assigned_IDs.empty())
        {
            ID_ = 1;
        } else
        {
            ID_ = *assigned_IDs.rbegin() + 1; //najwiekszy istniejący nr ID + 1
        }
    } else
    {
        ID_ = *freed_IDs.begin();
        freed_IDs.erase(freed_IDs.begin());
    }
    assigned_IDs.insert(ID_);
}

//konstruktor z parametrem
Package::Package(ElementID ID)
{
    if (assigned_IDs.find(ID) != assigned_IDs.end())
    {
        throw std::invalid_argument("Podane ID już istnieje!");
    }
    ID_ = ID;
    assigned_IDs.insert(ID_); //przypisanie ID
}

//konstruktor przenoszący
Package::Package(Package&& other) noexcept
{
    ID_ = other.ID_;
    other.ID_ = -1;
}

//operator przypisania przenoszący
Package& Package::operator=(Package&& other) noexcept
{
    if (this != &other)
    {
        if (ID_ != -1)
        {
            assigned_IDs.erase(ID_);
            freed_IDs.insert(ID_);
        }
        ID_ = other.ID_;
        other.ID_ = -1;
    }
    return *this;
}

//destruktor
Package::~Package()
{
    if (ID_ != -1)
    {
        assigned_IDs.erase(ID_);
        freed_IDs.insert(ID_);
    }
}
 main
