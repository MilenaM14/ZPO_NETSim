//
// Created by jowit on 08.01.2026.
//
#include "nodes.hpp"

ReceiverPreferences::ReceiverPreferences(ProbabilityGenerator pg)
    : pg_(pg) {
} // Zapis generatora do pola klasy

void ReceiverPreferences::add_receiver(IPackageReceiver* r)
{
    preferences_[r] = 0.0;
    std::size_t n = preferences_.size(); //ilość odbiorców
    for (auto& receiver : preferences_) {
        receiver.second = 1.0/n; //przypisnie wartosci
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {
    preferences_.erase(r); //usuwa odbiorce
    if (preferences_.empty()) return;

    double p = 1.0 / preferences_.size(); //nowa wartosc po usunieciu odbiorcy

    for (auto& receiver : preferences_) {
        receiver.second = p; // przypisanie nowych wartosci
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    if (preferences_.empty()) return nullptr; //żeby nie iterowac po pustej mapie

    auto p = pg_(); // losuje liczbę z [0,1]
    double cumulative = 0.0;

    for (auto& receiver : preferences_) {
        cumulative += receiver.second;
        if (p <= cumulative) {
            return receiver.first; // wybrany odbiorca
        }
    }

    return preferences_.rbegin()->first;
}
void PackageSender::send_package()
{
    if (!buffer_.has_value()) {
        return; //sprawdzanie czy bufor ma wartosc
    }

    IPackageReceiver* receiver = receiver_preferences_.choose_receiver(); //wybor odbiorcy
    if (receiver == nullptr) { //sprawdza czy odbiorca istnieje
        return;
    }

    receiver->receive_package(std::move(*buffer_)); //przypisuje paczke
    buffer_.reset(); //resetuje bufor
}

Ramp::Ramp(ElementID id, TimeOffset di)
    : id_(id),
      delivery_interval_(di){} //konstruktor

void Ramp::deliver_goods(Time t)
{
    if (t%delivery_interval_==0.0)
    {
        push_package(Package());
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
    :PackageSender(), id_(id), processing_duration_(pd), package_queue_(std::move(q)), start_processing_time_(0)
{}// konstruktor


void Worker::do_work(Time t)
{
    if (!buffer_.has_value() && !package_queue_->empty()){ //sprawdzenie czy jets paczka
        start_processing_time_ = t; //zapamiętanie czasu
        buffer_.emplace(package_queue_->pop());
    }
    if (buffer_.has_value() && t - start_processing_time_ >= processing_duration_) {
        push_package(std::move(*buffer_));
        buffer_.reset();
    }// sprawdza czy czas minął i przekazuje paczke do odbiorcy
}



void Storehouse::receive_package(Package&& p) {
    d_->push(std::move(p));
}