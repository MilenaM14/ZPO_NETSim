//
// Created by jowit on 05.01.2026.
//
#include <package.hpp>
#ifndef STORAGE_HPP
#define STORAGE_HPP

#endif //STORAGE_HPP
#include <iostream>
class IPackageStockPile
{
public:
    using const_iterator = std::list<Package>::const_iterator;
    virtual~IPackageStockPile() = default;
    virtual void push(const Package&) = 0;
    virtual std::size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual const_iterator begin() = 0;
    virtual const_iterator end()  = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
};