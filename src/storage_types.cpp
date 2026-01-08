//
// Created by milen on 6.01.2026.
//
#include "storage_types.hpp"

PackageQueue::PackageQueue(PackageQueueType type) : type_(type) {}

void PackageQueue::push(Package&& package) //dodawanie paczki
{
    queue_.emplace_back(std::move(package));
}

bool PackageQueue::empty() const //czy pusta
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
    Package p;

    if (type_ == PackageQueueType::FIFO)
    {
        p = std::move(queue_.front());
        queue_.pop_front();
    } else
    {
        p = std::move(queue_.back());
        queue_.pop_back();
    }
    return p;
}

PackageQueueType PackageQueue::get_queue_type() const
{
    return type_;
}