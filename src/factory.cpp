//
// Created by milen on 10.01.2026.
//
#include "factory.hpp"
#include <map>
#include <stdexcept>
#include <sstream>
#include <vector>

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

const std::map<std::string, ElementType> element_type_map = {
    {"LOADING_RAMP", ElementType::RAMP},
    {"WORKER", ElementType::WORKER},
    {"STOREHOUSE", ElementType::STOREHOUSE},
    {"LINK", ElementType::LINK}
};
ParsedLineData parse_line(const std::string& line) {
    ParsedLineData data;

    std::istringstream stream(line);

    std::string token;
    stream >> token; //pierwszy wyraz

    if (element_type_map.count(token) == 0) {
        throw std::runtime_error("nieznany typ"); // sprawdzamy, czy to znany typ
    }

    data.type = element_type_map.at(token);

    //  key=value
    while (stream >> token) {
        size_t equal_pos = token.find('=');

        if (equal_pos == std::string::npos) {
            throw std::runtime_error("nie ma spacji");
        }

        std::string key = token.substr(0, equal_pos); //klucz
        std::string value = token.substr(equal_pos + 1); //wartość
        data.parameters[key] = value;
    }

    return data;
}

Factory load_factory_structure(std::istream& is)
{
    Factory factory;
    std::string line;

    while (std::getline(is, line)) {
        if (line.empty()) continue;

        ParsedLineData data = parse_line(line);

        switch (data.type)
        {
        case ElementType::RAMP: {
            ElementID id = std::stoi(data.parameters.at("id"));
            TimeOffset di = std::stoi(data.parameters.at("delivery-interval"));
            factory.add_ramp(Ramp(id, di));
            break;
        }

        case ElementType::WORKER: {
            ElementID id = std::stoi(data.parameters.at("id"));
            TimeOffset pd = std::stoi(data.parameters.at("processing-time"));
            std::string qt = data.parameters.at("queue-type");

            PackageQueueType queue_type;
            if (qt == "FIFO") queue_type = PackageQueueType::FIFO;
            else if (qt == "LIFO") queue_type = PackageQueueType::LIFO;
            else throw std::runtime_error("nieznany typ kolejki");

            auto queue = std::make_unique<PackageQueue>(queue_type);
            factory.add_worker(Worker(id, pd, std::move(queue)));
            break;
        }

        case ElementType::STOREHOUSE: {
            ElementID id = std::stoi(data.parameters.at("id"));
            factory.add_storehouse(Storehouse(id));
            break;
        }

        case ElementType::LINK: {
            PackageSender* sender = nullptr;
            IPackageReceiver* receiver = nullptr;

            std::string src = data.parameters.at("src");
            std::string dest = data.parameters.at("dest");

            auto pos = src.find('-');
            std::string src_type = src.substr(0, pos);
            ElementID src_id = std::stoi(src.substr(pos + 1));

            if (src_type == "ramp") {
                sender = &(*factory.find_ramp_by_id(src_id));
            }
            else if (src_type == "worker") {
                sender = &(*factory.find_worker_by_id(src_id));
            }
            else {
                throw std::runtime_error("blad src");
            }

            auto pos2 = dest.find('-');
            std::string dest_type = dest.substr(0, pos2);
            ElementID dest_id = std::stoi(dest.substr(pos2 + 1));

            if (dest_type == "worker") {
                receiver = &(*factory.find_worker_by_id(dest_id));
            }
            else if (dest_type == "storehouse") {
                receiver = &(*factory.find_storehouse_by_id(dest_id));
            }
            else {
                throw std::runtime_error("blad dest");
            }

            sender->receiver_preferences_.add_receiver(receiver);
            break;
        }
        }
    }

    return factory;
}

void save_links(std::ostream& os,
                const PackageSender& sender,
                ElementID sender_id,
                const std::string& sender_type)
{
    for (const auto& pref : sender.receiver_preferences_.get_preferences()) {
        IPackageReceiver* receiver = pref.first;

        std::string receiver_type;
        if (receiver->get_receiver_type() == ReceiverType::WORKER) {
            receiver_type = "worker";
        } else {
            receiver_type = "store";
        }

        os << "LINK src=" << sender_type << "-" << sender_id
           << " dest=" << receiver_type << "-" << receiver->get_id()
           << "\n";
    }
}
std::string queue_type_str(PackageQueueType package_queue_type) {
    switch(package_queue_type) {
    case PackageQueueType::FIFO: return "FIFO";
    case PackageQueueType::LIFO: return "LIFO";
    }
    return {};
}
void save_factory_structure(Factory& factory, std::ostream& os)
{
    std::stringstream link_stream;
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        const Ramp& ramp = *it;

        os << "LOADING_RAMP id=" << ramp.get_id()
           << " delivery-interval=" << ramp.get_delivery_interval()
           << "\n";

        save_links(link_stream, ramp, ramp.get_id(), "ramp");
    }
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        const Worker& worker = *it;

        os << "WORKER id=" << worker.get_id()
           << " processing-time=" << worker.get_processing_duration()
           << " queue-type=" << queue_type_str(worker.get_queue()->get_queue_type())
           << "\n";

        save_links(link_stream, worker, worker.get_id(), "worker");
    }
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        const Storehouse& storehouse = *it;

        os << "STOREHOUSE id=" << storehouse.get_id() << "\n";
    }
    os << link_stream.str();
    os.flush();
}