#include "nodes.hpp"
#include <stdexcept>


ReceiverPreferences::ReceiverPreferences(ProbabilityGenerator pg)
    : pg_(pg) {}

void ReceiverPreferences::add_receiver(IPackageReceiver* r) {
    preferences_[r] = 1.0;
    // Po dodaniu nowego odbiorcy, resetujemy wagi (1/n)
    double new_prob = 1.0 / preferences_.size();
    for (auto& pair : preferences_) {
        pair.second = new_prob;
    }
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {
    auto it = preferences_.find(r);
    if (it != preferences_.end()) {
        preferences_.erase(it);
        // Przelicz wagi po usunięciu
        if (!preferences_.empty()) {
            double new_prob = 1.0 / preferences_.size();
            for (auto& pair : preferences_) {
                pair.second = new_prob;
            }
        }
    }
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    double probability = pg_();
    double cumulative = 0.0;
    for (const auto& pair : preferences_) {
        cumulative += pair.second;
        if (probability <= cumulative) {
            return pair.first;
        }
    }
    if (!preferences_.empty()) return preferences_.begin()->first;
    return nullptr;
}

void PackageSender::send_package() {
    if (buffer_) {
        IPackageReceiver* receiver = receiver_preferences_.choose_receiver();
        if (receiver) {
            receiver->receive_package(std::move(*buffer_));
            buffer_.reset();
        }
    }
}


Ramp::Ramp(ElementID ID, TimeOffset di)
    : ID_(ID), delivery_interval_(di) {}

void Ramp::deliver_goods(Time t) {
    if ((t - 1) % delivery_interval_ == 0) {
        Package p;
        push_package(std::move(p));
    }
}

TimeOffset Ramp::get_delivery_interval() const {
    return delivery_interval_;
}

ElementID Ramp::get_id() const {
    return ID_;
}

Worker::Worker(ElementID ID, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
    : ID_(ID), processing_duration_(pd), package_queue_(std::move(q)), start_processing_time_(0) {}

void Worker::do_work(Time t) {
    if (!processing_buffer_ && !package_queue_->empty()) {
        processing_buffer_ = package_queue_->pop();
        start_processing_time_ = t;
    }

    if (processing_buffer_) {
        if (t - start_processing_time_ + 1 >= processing_duration_) {
            push_package(std::move(*processing_buffer_));
            processing_buffer_.reset();
        }
    }
}

void Worker::receive_package(Package&& p) {
    package_queue_->push(std::move(p));
}

Storehouse::Storehouse(ElementID ID, std::unique_ptr<IPackageStockpile> d)
    : ID_(ID), d_(std::move(d)) {}

void Storehouse::receive_package(Package&& package) {
    d_->push(std::move(package));
}