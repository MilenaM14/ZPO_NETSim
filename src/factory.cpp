//
// Created by milen on 10.01.2026.
//
#include "factory.hpp"
#include <map>
#include <stdexcept>

//implementacja algorytmów spójności
enum class NodeColor { UNVISITED, VISITED, VERIFIED };

//pomocnicza funkcja DFS do sprawdzania spójności
bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) {
    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }
    if (node_colors[sender] == NodeColor::VISITED) {
        throw std::logic_error("Coś o strukturze sieci");
    }
    node_colors[sender] = NodeColor::VISITED;
    if (sender->receiver_preferences_.begin() == sender->receiver_preferences_.end()) {
        throw std::logic_error("Błąd odbiorcy");
    }
    bool has_any_valid_receiver = false;
    for (const auto& receiver_pair : sender->receiver_preferences_) {
        IPackageReceiver* receiver_ptr = receiver_pair.first;

        if (receiver_ptr->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_any_valid_receiver = true;
        }
        else if (receiver_ptr->get_receiver_type() == ReceiverType::WORKER) {
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);

            if (sendrecv_ptr == sender) {
                continue;
            }
            has_any_valid_receiver = true;

            has_reachable_storehouse(sendrecv_ptr, node_colors);
        }
    }

    node_colors[sender] = NodeColor::VERIFIED;

    if (has_any_valid_receiver) {
        return true;
    } else {
        throw std::logic_error("Błąd magazynu");
    }
}

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> node_colors;

    for (const auto& ramp : ramps_) {
        node_colors[&ramp] = NodeColor::UNVISITED;
    }
    for (const auto& worker : workers_) {
        node_colors[&worker] = NodeColor::UNVISITED;
    }

    try {
        for (const auto& ramp : ramps_) {
            has_reachable_storehouse(&ramp, node_colors);
        }
    } catch (const std::logic_error&) {
        return false;
    }

    return true;
}

void Factory::do_deliveries(Time t) {
    for (auto& ramp : ramps_) {
        ramp.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for (auto& ramp : ramps_) {
        ramp.send_package();
    }
    for (auto& worker : workers_) {
        worker.send_package();
    }
}

void Factory::do_work(Time t) {
    for (auto& worker : workers_) {
        worker.do_work(t);
    }
}

void Factory::add_ramp(Ramp&& ramp) {
    ramps_.add(std::move(ramp));
}

void Factory::remove_ramp(ElementID ID) {
    ramps_.remove_by_id(ID);
}

NodeCollection<Ramp>::iterator Factory::find_ramp_by_id(ElementID ID) {
    return ramps_.find_by_id(ID);
}

NodeCollection<Ramp>::const_iterator Factory::find_ramp_by_id(ElementID ID) const {
    return ramps_.find_by_id(ID);
}

NodeCollection<Ramp>::const_iterator Factory::ramp_cbegin() const {
    return ramps_.cbegin();
}

NodeCollection<Ramp>::const_iterator Factory::ramp_cend() const {
    return ramps_.cend();
}


void Factory::add_worker(Worker&& worker) {
    workers_.add(std::move(worker));
}

void Factory::remove_worker(ElementID ID) {
    remove_receiver(workers_, ID);
}

NodeCollection<Worker>::iterator Factory::find_worker_by_id(ElementID ID) {
    return workers_.find_by_id(ID);
}

NodeCollection<Worker>::const_iterator Factory::find_worker_by_id(ElementID ID) const {
    return workers_.find_by_id(ID);
}

NodeCollection<Worker>::const_iterator Factory::worker_cbegin() const {
    return workers_.cbegin();
}

NodeCollection<Worker>::const_iterator Factory::worker_cend() const {
    return workers_.cend();
}

void Factory::add_storehouse(Storehouse&& storehouse) {
    storehouses_.add(std::move(storehouse));
}

void Factory::remove_storehouse(ElementID ID) {
    remove_receiver(storehouses_, ID);
}

NodeCollection<Storehouse>::iterator Factory::find_storehouse_by_id(ElementID ID) {
    return storehouses_.find_by_id(ID);
}

NodeCollection<Storehouse>::const_iterator Factory::find_storehouse_by_id(ElementID ID) const {
    return storehouses_.find_by_id(ID);
}

NodeCollection<Storehouse>::const_iterator Factory::storehouse_cbegin() const {
    return storehouses_.cbegin();
}

NodeCollection<Storehouse>::const_iterator Factory::storehouse_cend() const {
    return storehouses_.cend();
}