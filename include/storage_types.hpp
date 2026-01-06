//
// Created by milen on 6.01.2026.
//

#ifndef AAANETSIM_STORAGE_TYPES_HPP
#define AAANETSIM_STORAGE_TYPES_HPP

#include "package.hpp"
#include <list>

enum class PackageQueueType
{
    FIFO,
    LIFO
};

class IPackageStockpile
{
}; //dokonczyc

class IPackageQueue : public IPackageStockpile
{
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
};

class PackageQueue : public IPackageQueue
{

}; //dokonczyc


#endif //AAANETSIM_STORAGE_TYPES_HPP