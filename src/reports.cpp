//
// Created by milen on 15.01.2026.
//
#include "reports.hpp"
#include <algorithm>
#include <vector>
#include <iostream>

//pomocnicza funkcja do sortowania odbiorców ramp < storehouse < worker
bool compare_receivers(const IPackageReceiver* a, const IPackageReceiver* b) {
    ReceiverType type_a = a->get_receiver_type();
    ReceiverType type_b = b->get_receiver_type();

    //jeśli typy są różne, ustalamy kolejność: STOREHOUSE < WORKER
    if (type_a != type_b) {
        //Storehouse ma być pierwszy. Jeśli 'a' to Storehouse, to jest wcześniej
        return type_a == ReceiverType::STOREHOUSE;
    }

    //jeśli typy są te same, decyduje ID
    return a->get_id() < b->get_id();
}

//drukuje listę odbiorców dla danego nadawcy
void print_receivers(std::ostream& os, const PackageSender& sender) {
    auto preferences = sender.receiver_preferences_.get_preferences();

    //kopiujemy klucze do wektora, żeby je posortować
    std::vector<const IPackageReceiver*> receivers;
    for (const auto& pair : preferences) {
        receivers.push_back(pair.first);
    }

    //sortujemy zgodnie z wymaganiami
    std::sort(receivers.begin(), receivers.end(), compare_receivers);

    os << "  Receivers:\n";  //os to skrot od Output Stream (zmienna), jest unwersalana
    if (receivers.empty()) {
    } else {
        for (const auto* receiver : receivers) {
            std::string type_str = (receiver->get_receiver_type() == ReceiverType::WORKER) ? "worker" : "storehouse";
            os << "    " << type_str << " #" << receiver->get_id() << "\n";
        }
    }
}

// Drukuje zawartość kontenera jako listę ID paczek, jeśli puste empty
template<typename Iterator>
void print_package_container(std::ostream& os, Iterator begin, Iterator end) {
    if (begin == end) {
        os << "(empty)";
    } else {
        bool first = true;
        for (auto it = begin; it != end; ++it) {
            if (!first) os << ", ";
            os << "#" << it->get_id();
            first = false;
        }
    }
}

void generate_structure_report(const Factory& f, std::ostream& os) {
    os << "\n== LOADING RAMPS ==\n\n";

    //lambda do sortowania węzłów dowolnego typu przed wypisaniem
    auto get_sorted_nodes = [](auto begin, auto end) {
        using NodeType = typename std::iterator_traits<decltype(begin)>::value_type;
        std::vector<const NodeType*> nodes;
        for (auto it = begin; it != end; ++it) {
            nodes.push_back(&(*it));
        }
        std::sort(nodes.begin(), nodes.end(), [](const NodeType* a, const NodeType* b) {
            return a->get_id() < b->get_id();
        });
        return nodes;
    };

    auto sorted_ramps = get_sorted_nodes(f.ramp_cbegin(), f.ramp_cend());
    for (const auto* ramp : sorted_ramps) {
        os << "LOADING RAMP #" << ramp->get_id() << "\n";
        os << "  Delivery interval: " << ramp->get_delivery_interval() << "\n";
        print_receivers(os, *ramp);
        os << "\n";
    }

    os << "\n== WORKERS ==\n\n";
    auto sorted_workers = get_sorted_nodes(f.worker_cbegin(), f.worker_cend());
    for (const auto* worker : sorted_workers) {
        os << "WORKER #" << worker->get_id() << "\n";
        os << "  Processing time: " << worker->get_processing_duration() << "\n";

        std::string q_type = (worker->get_queue()->get_queue_type() == PackageQueueType::LIFO) ? "LIFO" : "FIFO";
        os << "  Queue type: " << q_type << "\n";

        print_receivers(os, *worker);
        os << "\n";
    }

    os << "\n== STOREHOUSES ==\n\n";
    auto sorted_storehouses = get_sorted_nodes(f.storehouse_cbegin(), f.storehouse_cend());
    for (const auto* store : sorted_storehouses) {
        os << "STOREHOUSE #" << store->get_id() << "\n\n";
    }
}

void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t) {
    os << "=== [ Turn: " << t << " ] ===\n\n";

    auto get_sorted_nodes = [](auto begin, auto end) {
        using NodeType = typename std::iterator_traits<decltype(begin)>::value_type;
        std::vector<const NodeType*> nodes;
        for (auto it = begin; it != end; ++it) nodes.push_back(&(*it));
        std::sort(nodes.begin(), nodes.end(), [](const NodeType* a, const NodeType* b) {
            return a->get_id() < b->get_id();
        });
        return nodes;
    };

    os << "== WORKERS ==\n\n";
    auto sorted_workers = get_sorted_nodes(f.worker_cbegin(), f.worker_cend());

    for (const auto* worker : sorted_workers) {
        os << "WORKER #" << worker->get_id() << "\n";

        os << "  PBuffer: ";
        if (worker->get_processing_buffer().has_value()) {
            //obliczanie czasu przetwarzania
            Time pt = t - worker->get_package_processing_start_time() + 1;
            os << "#" << worker->get_processing_buffer()->get_id() << " (pt = " << pt << ")";
        } else {
            os << "(empty)";
        }
        os << "\n";

        os << "  Queue: ";
        print_package_container(os, worker->begin(), worker->end());
        os << "\n";

        os << "  SBuffer: ";
        if (worker->get_sending_buffer().has_value()) {
            os << "#" << worker->get_sending_buffer()->get_id();
        } else {
            os << "(empty)";
        }
        os << "\n\n";
    }

    os << "== STOREHOUSES ==\n\n";
    auto sorted_storehouses = get_sorted_nodes(f.storehouse_cbegin(), f.storehouse_cend());

    for (const auto* store : sorted_storehouses) {
        os << "STOREHOUSE #" << store->get_id() << "\n";
        os << "  Stock: ";
        print_package_container(os, store->begin(), store->end());
        os << "\n\n";
    }
}