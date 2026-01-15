//
// Created by milen on 6.01.2026.
//
#include "storage_types.hpp"

// --- Implementacja PackageQueue (Kolejka dla Workera) ---

PackageQueue::PackageQueue(PackageQueueType type) : type_(type) {}

void PackageQueue::push(Package&& package)
{
    queue_.emplace_back(std::move(package));
}

bool PackageQueue::empty() const
{
    return queue_.empty();
}

IPackageStockpile::size_type PackageQueue::size() const
{
    return queue_.size();
}

IPackageStockpile::const_iterator PackageQueue::begin() const
{
    return queue_.cbegin();
}

IPackageStockpile::const_iterator PackageQueue::end() const
{
    return queue_.cend();
}

IPackageStockpile::const_iterator PackageQueue::cbegin() const
{
    return queue_.cbegin();
}

IPackageStockpile::const_iterator PackageQueue::cend() const
{
    return queue_.cend();
}

Package PackageQueue::pop() // FIFO/LIFO
{
    if (type_ == PackageQueueType::FIFO)
    {
        Package p(std::move(queue_.front()));
        queue_.pop_front();
        return p;
    } else
    {
        Package p(std::move(queue_.back()));
        queue_.pop_back();
        return p;
    }
}

PackageQueueType PackageQueue::get_queue_type() const
{
    return type_;
}

// --- Implementacja PackageStockpile (Magazyn dla Storehouse) ---
// To jest ta część, której brakowało, a jest wymagana przez Storehouse

void PackageStockpile::push(Package&& package)
{
    stock_.emplace_back(std::move(package));
}

bool PackageStockpile::empty() const
{
    return stock_.empty();
}

IPackageStockpile::size_type PackageStockpile::size() const
{
    return stock_.size();
}

IPackageStockpile::const_iterator PackageStockpile::begin() const
{
    return stock_.cbegin();
}

IPackageStockpile::const_iterator PackageStockpile::end() const
{
    return stock_.cend();
}

IPackageStockpile::const_iterator PackageStockpile::cbegin() const
{
    return stock_.cbegin();
}

IPackageStockpile::const_iterator PackageStockpile::cend() const
{
    return stock_.cend();
}