//
// Created by milen on 6.01.2026.
//

#ifndef AAANETSIM_STORAGE_TYPES_HPP
#define AAANETSIM_STORAGE_TYPES_HPP

#include "package.hpp"
#include <list>

//tutaj definiujemy jak wygląda i działa magazyn

enum class PackageQueueType
{
    FIFO,
    LIFO
};

class IPackageStockpile
{
public:
    using const_iterator = std::list<Package>::const_iterator;
    using size_type = std::list<Package>::size_type;

    virtual ~IPackageStockpile() = default;

    virtual void push(Package&& package) = 0;
    virtual bool empty() const = 0;
    virtual size_type size() const = 0;

    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
};

class IPackageQueue : public IPackageStockpile
{
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
};

class PackageQueue : public IPackageQueue
{
public:
    explicit PackageQueue(PackageQueueType type);

    void push(Package&& package) override;
    bool empty() const override;
    size_type size() const override;

    const_iterator begin() const override;
    const_iterator end() const override;
    const_iterator cbegin() const override;
    const_iterator cend() const override;

    Package pop() override;
    PackageQueueType get_queue_type() const override;

private:
    std::list<Package> queue_;
    PackageQueueType type_;
};
#endif //AAANETSIM_STORAGE_TYPES_HPP
