#include "reports.hpp"
#include <algorithm>
#include <vector>

void generate_structure_report(const Factory& factory, std::ostream& os) {
    // 1. Rampy
    os << "\n== LOADING RAMPS ==\n\n";

    std::vector<const Ramp*> ramps;
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        ramps.push_back(&(*it));
    }
    std::sort(ramps.begin(), ramps.end(), [](const Ramp* a, const Ramp* b) {
        return a->get_id() < b->get_id();
    });

    for (const auto* ramp : ramps) {
        os << "LOADING RAMP #" << ramp->get_id() << "\n";
        os << "  Delivery interval: " << ramp->get_delivery_interval() << "\n";
        os << "  Receivers:\n";

        std::vector<ElementID> worker_ids;
        std::vector<ElementID> store_ids;

        for (const auto& [receiver, prob] : ramp->receiver_preferences_.get_preferences()) {
            if (receiver->get_receiver_type() == ReceiverType::WORKER) {
                worker_ids.push_back(receiver->get_id());
            } else {
                store_ids.push_back(receiver->get_id());
            }
        }
        std::sort(worker_ids.begin(), worker_ids.end());
        std::sort(store_ids.begin(), store_ids.end());

        // POPRAWKA: Małe litery "storehouse" i "worker" (wymagane przez testy)
        for (auto id : store_ids) os << "    storehouse #" << id << "\n";
        for (auto id : worker_ids) os << "    worker #" << id << "\n";
        os << "\n";
    }

    // 2. Robotnicy
    os << "\n== WORKERS ==\n\n";
    std::vector<const Worker*> workers;
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        workers.push_back(&(*it));
    }
    std::sort(workers.begin(), workers.end(), [](const Worker* a, const Worker* b) {
        return a->get_id() < b->get_id();
    });

    for (const auto* worker : workers) {
        os << "WORKER #" << worker->get_id() << "\n";
        os << "  Processing time: " << worker->get_processing_duration() << "\n";
        os << "  Queue type: " << (worker->get_queue()->get_queue_type() == PackageQueueType::LIFO ? "LIFO" : "FIFO") << "\n";
        os << "  Receivers:\n";

        std::vector<ElementID> worker_ids;
        std::vector<ElementID> store_ids;

        for (const auto& [receiver, prob] : worker->receiver_preferences_.get_preferences()) {
            if (receiver->get_receiver_type() == ReceiverType::WORKER) {
                worker_ids.push_back(receiver->get_id());
            } else {
                store_ids.push_back(receiver->get_id());
            }
        }
        std::sort(worker_ids.begin(), worker_ids.end());
        std::sort(store_ids.begin(), store_ids.end());

        // POPRAWKA: Małe litery "storehouse" i "worker"
        for (auto id : store_ids) os << "    storehouse #" << id << "\n";
        for (auto id : worker_ids) os << "    worker #" << id << "\n";
        os << "\n";
    }

    // 3. Magazyny
    os << "\n== STOREHOUSES ==\n\n";
    std::vector<const Storehouse*> storehouses;
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        storehouses.push_back(&(*it));
    }
    std::sort(storehouses.begin(), storehouses.end(), [](const Storehouse* a, const Storehouse* b) {
        return a->get_id() < b->get_id();
    });

    for (const auto* store : storehouses) {
        os << "STOREHOUSE #" << store->get_id() << "\n\n";
    }
}

// POPRAWKA: Zmiana nazwy funkcji na zgodną z testami
void generate_simulation_turn_report(const Factory& factory, std::ostream& os, Time t) {
    os << "=== [ Turn: " << t << " ] ===\n\n";

    // --- WORKERS ---
    os << "== WORKERS ==\n\n";

    std::vector<const Worker*> workers;
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        workers.push_back(&(*it));
    }
    std::sort(workers.begin(), workers.end(), [](const Worker* a, const Worker* b) {
        return a->get_id() < b->get_id();
    });

    for (const auto* worker : workers) {
        os << "WORKER #" << worker->get_id() << "\n";

        os << "  PBuffer: ";
        if (worker->get_processing_buffer()) {
            os << "#" << worker->get_processing_buffer()->get_id() << " (pt = " << (t - worker->get_package_processing_start_time() + 1) << ")";
        } else {
            os << "(empty)";
        }
        os << "\n";

        os << "  Queue: ";
        if (worker->cbegin() == worker->cend()) {
            os << "(empty)";
        } else {
            bool first = true;
            for (auto it = worker->cbegin(); it != worker->cend(); ++it) {
                if (!first) os << ", ";
                os << "#" << it->get_id();
                first = false;
            }
        }
        os << "\n";

        os << "  SBuffer: ";
        if (worker->get_sending_buffer()) {
            os << "#" << worker->get_sending_buffer()->get_id();
        } else {
            os << "(empty)";
        }
        os << "\n\n";
    }

    // --- SEPARATOR ---
    os << "\n";

    // --- STOREHOUSES ---
    os << "== STOREHOUSES ==\n\n";

    std::vector<const Storehouse*> storehouses;
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        storehouses.push_back(&(*it));
    }
    std::sort(storehouses.begin(), storehouses.end(), [](const Storehouse* a, const Storehouse* b) {
        return a->get_id() < b->get_id();
    });

    for (const auto* store : storehouses) {
        os << "STOREHOUSE #" << store->get_id() << "\n";
        os << "  Stock: ";
        if (store->cbegin() == store->cend()) {
            os << "(empty)";
        } else {
            bool first = true;
            for (auto it = store->cbegin(); it != store->cend(); ++it) {
                if (!first) os << ", ";
                os << "#" << it->get_id();
                first = false;
            }
        }
        os << "\n\n";
    }
}