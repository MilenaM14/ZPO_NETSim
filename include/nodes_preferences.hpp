//
// Created by jowit on 08.01.2026.
//

#ifndef NODES_PREFERENCES_HPP
#define NODES_PREFERENCES_HPP

#include "helpers.hpp"
#include "package.hpp"
#include "storage_types.hpp"
#include "types.hpp"
#include <map>
#include <optional>

enum class ReceiverType {
    WORKER,
    STOREHOUSE
}; //informacje o obiekcie
class IPackageReceiver
{
public:
    virtual IPackageStockpile::const_iterator cbegin() const = 0;

    virtual IPackageStockpile::const_iterator cend() const = 0;

    virtual IPackageStockpile::const_iterator begin() const = 0;

    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual ReceiverType get_receiver_type() const = 0; //identyfikacja typu obiektu

    virtual void receive_package(Package&& p) = 0; //przekazuje wskaźnik

    virtual ElementID get_id() const = 0; //identyfikacja ID

    virtual ~IPackageReceiver() = default; //destruktor
};
class ReceiverPreferences
{
public:
    using preferences_t = std::map<IPackageReceiver*, double>; //alias na typ kontenera
    using const_iterator = preferences_t::const_iterator; //alias na typ kontenera
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    explicit ReceiverPreferences(ProbabilityGenerator pg = probability_generator);

    void add_receiver(IPackageReceiver* r); // metoda która dodaje nowego odbiorce
    void remove_receiver(IPackageReceiver* r); //metora, która usuwa odbiorce

    IPackageReceiver* choose_receiver();

    const preferences_t& get_preferences() const;

private:
    preferences_t preferences_;
    ProbabilityGenerator pg_;
};

class PackageSender {
public:
    ReceiverPreferences receiver_preferences_;

    PackageSender(PackageSender&&) = default;  // konstruktor przenoszący (domyślny)

    void send_package();
    const std::optional<Package>& get_sending_buffer() const { return buffer_; };

private:
    void push_package(Package&& p) {
        buffer_ = std::move(p);
    }
    std::optional<Package> buffer_ = std::nullopt;
};

#endif //NODES_PREFERENCES_HPP
