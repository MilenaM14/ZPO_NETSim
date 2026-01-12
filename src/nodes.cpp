//
// Created by jowit on 08.01.2026.
//

#include "nodes.hpp"

ReceiverPreferences::ReceiverPreferences(ProbabilityGenerator pg)
    : pg_(pg) {
}

void ReceiverPreferences::add_receiver(IPackageReceiver* r)
{
    //dodajemy odbiorcę z początkową wartością
    preferences_[r] = 0.0;

    //przeliczamy prawdopodobieństwa dla wszystkich (rozkład równomierny)
    if (preferences_.empty()) return;
    double new_probability = 1.0 / preferences_.size();

    for (auto& receiver : preferences_) {
        receiver.second = new_probability;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {
    preferences_.erase(r);

    //przeliczamy prawdopodobieństwa po usunięciu
    if (preferences_.empty()) return;
    double new_probability = 1.0 / preferences_.size();

    for (auto& receiver : preferences_) {
        receiver.second = new_probability;
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    if (preferences_.empty()) return nullptr;

    double p = pg_(); //losuje liczbę z zakresu [0, 1]
    double cumulative = 0.0;

    for (const auto& receiver : preferences_) {
        cumulative += receiver.second;
        if (p <= cumulative) {
            return receiver.first;
        }
    }

    //zabezpieczenie na wypadek błędów zaokrągleń double
    return preferences_.rbegin()->first;
}

void PackageSender::send_package()
{
    //jeśli bufor jest pusty, nie ma co wysyłać
    if (!buffer_.has_value()) {
        return;
    }

    IPackageReceiver* receiver = receiver_preferences_.choose_receiver();

    if (receiver) {
        receiver->receive_package(std::move(*buffer_));
        buffer_.reset(); // Opróżnij bufor po wysłaniu
    }
}

Ramp::Ramp(ElementID id, TimeOffset di)
    : id_(id),
      delivery_interval_(di)
{}

void Ramp::deliver_goods(Time t)
{
    //generowanie paczki co określony interwał
    if ((t-1) % delivery_interval_ == 0)
    {
        //jeśli bufor nadawczy jest wolny, utwórz nową paczkę
        if (!buffer_.has_value()) {
            push_package(Package());
        }
    }
}

TimeOffset Ramp::get_delivery_interval() const
{
    return delivery_interval_;
}

ElementID Ramp::get_id() const
{
    return id_;
}

Worker::Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
    : PackageSender(),
      id_(id),
      processing_duration_(pd),
      package_queue_(std::move(q)),
      start_processing_time_(0)
{}

void Worker::do_work(Time t)
{
    //jeśli bufor (miejsce pracy) jest pusty, a mamy coś w kolejce -> pobierz do pracy
    if (!buffer_.has_value() && !package_queue_->empty()) {
        buffer_.emplace(package_queue_->pop());
        start_processing_time_ = t;
    }
}

void Worker::receive_package(Package&& p) {
    package_queue_->push(std::move(p));
}

Storehouse::Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d)
    : id_(id),
      d_(std::move(d))
{}

void Storehouse::receive_package(Package&& p) {
    d_->push(std::move(p));
}