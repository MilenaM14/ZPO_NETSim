//
// Created by jowit on 08.01.2026.
//

#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

#include "helpers.hpp"
#include "package.hpp"
#include "storage_types.hpp"
#include "types.hpp"
#include <map>
#include <optional>
#include <memory>

// Typ wyliczeniowy potrzebny dla Factory do rozróżniania węzłów
enum class ReceiverType {
    WORKER,
    STOREHOUSE
};

class IPackageReceiver {
public:
    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;
    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;

    // Metoda niezbędna do weryfikacji spójności sieci (DFS)
    virtual ReceiverType get_receiver_type() const = 0;

    virtual ~IPackageReceiver() = default;
};

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    const_iterator begin() const { return preferences_.begin(); }
    const_iterator end() const { return preferences_.end(); }
    const_iterator cbegin() const { return preferences_.cbegin(); }
    const_iterator cend() const { return preferences_.cend(); }

    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator);
    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver();
    const preferences_t& get_preferences() const { return preferences_; }

private:
    preferences_t preferences_;
    ProbabilityGenerator pg_;
};

class PackageSender {
public:
    PackageSender() = default;
    ReceiverPreferences receiver_preferences_;
    PackageSender(PackageSender&&) = default;
    void send_package();
    const std::optional<Package>& get_sending_buffer() const { return buffer_; };

protected:
    void push_package(Package&& p) {
        buffer_ = std::move(p);
    }
    std::optional<Package> buffer_ = std::nullopt;
};

class Ramp : public PackageSender {
public:
    Ramp(ElementID ID, TimeOffset di);
    void deliver_goods(Time t);
    TimeOffset get_delivery_interval() const;
    ElementID get_id() const;

private:
    ElementID ID_;
    TimeOffset delivery_interval_;
};

class Worker : public PackageSender, public IPackageReceiver {
public:
    Worker(ElementID ID, TimeOffset pd, std::unique_ptr<IPackageQueue> q);

    void do_work(Time t);
    TimeOffset get_processing_duration() const { return processing_duration_; };
    Time get_package_processing_start_time() const { return start_processing_time_; };

    // Metody z IPackageReceiver
    void receive_package(Package&& p) override;
    ElementID get_id() const override { return ID_; }

    IPackageStockpile::const_iterator cbegin() const override { return package_queue_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return package_queue_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return package_queue_->begin(); }
    IPackageStockpile::const_iterator end() const override { return package_queue_->end(); }

    ReceiverType get_receiver_type() const override {
        return ReceiverType::WORKER;
    }

    // Metody do raportów i IO
    const std::optional<Package>& get_processing_buffer() const {
        return processing_buffer_;
    }

    IPackageQueue* get_queue() const {
        return package_queue_.get();
    }

private:
    ElementID ID_;
    TimeOffset processing_duration_;
    std::unique_ptr<IPackageQueue> package_queue_;
    Time start_processing_time_;
    std::optional<Package> processing_buffer_;
};

class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID ID, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO));

    ElementID get_id() const override { return ID_; };
    void receive_package(Package&& package) override;

    IPackageStockpile::const_iterator cbegin() const override { return d_->cbegin(); }
    IPackageStockpile::const_iterator cend() const override { return d_->cend(); }
    IPackageStockpile::const_iterator begin() const override { return d_->begin(); }
    IPackageStockpile::const_iterator end() const override { return d_->end(); }

    ReceiverType get_receiver_type() const override {
        return ReceiverType::STOREHOUSE;
    }

private:
    ElementID ID_;
    std::unique_ptr<IPackageStockpile> d_;
};

#endif // NETSIM_NODES_HPP