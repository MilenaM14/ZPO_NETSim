//
// Created by milen on 10.01.2026.
//

#ifndef NETSIM_FACTORY_HPP
#define NETSIM_FACTORY_HPP

#include "types.hpp"
#include "nodes.hpp"
#include <list>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <sstream>

template <typename Node>
class NodeCollection {
public:
    using container_t = std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    iterator begin() { return collection_.begin(); }
    iterator end() { return collection_.end(); }
    const_iterator begin() const { return collection_.cbegin(); }
    const_iterator end() const { return collection_.cend(); }
    const_iterator cbegin() const { return collection_.cbegin(); }
    const_iterator cend() const { return collection_.cend(); }

    void add(Node&& node) {
        collection_.emplace_back(std::move(node));
    }

    iterator find_by_id(ElementID ID) {
        return std::find_if(collection_.begin(), collection_.end(),
            [ID](const Node& node) {
                return node.get_id() == ID;
            });
    }

    const_iterator find_by_id(ElementID ID) const {
        return std::find_if(collection_.cbegin(), collection_.cend(),
            [ID](const Node& node) {
                return node.get_id() == ID;
            }); //kazdy wezel ma metode get id
    }

    void remove_by_id(ElementID ID) { //usuwanie obiektow po id
        auto it = find_by_id(ID);
        if (it != collection_.end()) {
            collection_.erase(it);
        }
    }

private:
    container_t collection_;
};

class Factory {
public:
    //dodaje rampę do fabryki
    void add_ramp(Ramp&& ramp);
    void remove_ramp(ElementID ID);
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID ID);
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID ID) const;
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const;
    NodeCollection<Ramp>::const_iterator ramp_cend() const;

    // dodaje workera do fabryki
    void add_worker(Worker&& worker);
    void remove_worker(ElementID ID);
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID ID);
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID ID) const;
    NodeCollection<Worker>::const_iterator worker_cbegin() const;
    NodeCollection<Worker>::const_iterator worker_cend() const;

    // dodaje magazyn do fabryki
    void add_storehouse(Storehouse&& storehouse);
    void remove_storehouse(ElementID ID);
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID ID);
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID ID) const;
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const;
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const;

    bool is_consistent() const;
    void do_deliveries(Time t);
    void do_package_passing();
    void do_work(Time t);

private:
    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;

    // Szablon metody pomocniczej
    template <typename Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID ID) {
        auto it = collection.find_by_id(ID);
        if (it == collection.end()) return;

        IPackageReceiver* receiver_ptr = dynamic_cast<IPackageReceiver*>(&(*it));

        for (auto& ramp : ramps_) {
            ramp.receiver_preferences_.remove_receiver(receiver_ptr);
        }
        for (auto& worker : workers_) {
            worker.receiver_preferences_.remove_receiver(receiver_ptr);
        }
        collection.remove_by_id(ID);
    }
};

// Funkcje globalne IO

Factory load_factory_structure(std::istream& is);
void save_factory_structure(const Factory& factory, std::ostream& os);

#endif // NETSIM_FACTORY_HPP