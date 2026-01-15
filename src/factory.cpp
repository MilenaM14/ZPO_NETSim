//
// Created by milen on 10.01.2026.
//
#include "factory.hpp"
#include <map>
#include <string>
#include <sstream>
#include <vector>

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);

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
    return tokens;
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;

    while (std::getline(is, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type.empty()) continue;

        std::map<std::string, std::string> args;
        std::string token;
        while (iss >> token) {
            auto pos = token.find('=');
            if (pos != std::string::npos) {
                std::string key = token.substr(0, pos);
                std::string value = token.substr(pos + 1);
                args[key] = value;
            }
        }

        if (type == "LOADING_RAMP") {
            if (args.count("id") && args.count("delivery-interval")) {
                ElementID id = std::stoi(args["id"]);
                TimeOffset di = std::stoi(args["delivery-interval"]);
                factory.add_ramp(Ramp(id, di));
            }
        }
        else if (type == "WORKER") {
            if (args.count("id") && args.count("processing-time") && args.count("queue-type")) {
                ElementID id = std::stoi(args["id"]);
                TimeOffset pt = std::stoi(args["processing-time"]);
                std::string qt_str = args["queue-type"];

                PackageQueueType qt = (qt_str == "LIFO") ? PackageQueueType::LIFO : PackageQueueType::FIFO;
                factory.add_worker(Worker(id, pt, std::make_unique<PackageQueue>(qt)));
            }
        }
        else if (type == "STOREHOUSE") {
            if (args.count("id")) {
                ElementID id = std::stoi(args["id"]);
                factory.add_storehouse(Storehouse(id));
            }
        }
        else if (type == "LINK") {
            if (args.count("src") && args.count("dest")) {
                auto src_parts = split(args["src"], '-');
                auto dest_parts = split(args["dest"], '-');

                if (src_parts.size() >= 2 && dest_parts.size() >= 2) {
                    std::string src_type = src_parts[0];
                    ElementID src_id = std::stoi(src_parts[1]);

                    std::string dest_type = dest_parts[0];
                    ElementID dest_id = std::stoi(dest_parts[1]);

                    PackageSender* sender = nullptr;
                    if (src_type == "ramp") {
                        auto it = factory.find_ramp_by_id(src_id);
                        if (it != factory.ramp_cend()) sender = &(*it);
                    } else if (src_type == "worker") {
                        auto it = factory.find_worker_by_id(src_id);
                        if (it != factory.worker_cend()) sender = &(*it);
                    }

                    IPackageReceiver* receiver = nullptr;
                    if (dest_type == "worker") {
                        auto it = factory.find_worker_by_id(dest_id);
                        if (it != factory.worker_cend()) receiver = &(*it);
                    } else if (dest_type == "storehouse" || dest_type == "store") {
                        auto it = factory.find_storehouse_by_id(dest_id);
                        if (it != factory.storehouse_cend()) receiver = &(*it);
                    }

                    if (sender && receiver) {
                        sender->receiver_preferences_.add_receiver(receiver);
                    }
                }
            }
        }
    }
    return factory;
}

void save_factory_structure(const Factory& factory, std::ostream& os) {
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        os << "LOADING_RAMP id=" << it->get_id() << " delivery-interval=" << it->get_delivery_interval() << "\n";
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        os << "WORKER id=" << it->get_id() << " processing-time=" << it->get_processing_duration()
           << " queue-type=" << (it->get_queue()->get_queue_type() == PackageQueueType::LIFO ? "LIFO" : "FIFO") << "\n";
    }

    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << "\n";
    }

    auto save_links = [&](const PackageSender& sender, std::string src_prefix, ElementID src_id) {
        for (const auto& [receiver, prob] : sender.receiver_preferences_.get_preferences()) {
            std::string dest_prefix = (receiver->get_receiver_type() == ReceiverType::WORKER) ? "worker" : "store";
            os << "LINK src=" << src_prefix << "-" << src_id << " dest=" << dest_prefix << "-" << receiver->get_id() << "\n";
        }
    };

    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) save_links(*it, "ramp", it->get_id());
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) save_links(*it, "worker", it->get_id());
}

enum class NodeColor { UNVISITED, VISITED, VERIFIED };

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) {
    if (node_colors[sender] == NodeColor::VERIFIED) return true;
    if (node_colors[sender] == NodeColor::VISITED) throw std::logic_error("Cycle detected");

    node_colors[sender] = NodeColor::VISITED;

    if (sender->receiver_preferences_.begin() == sender->receiver_preferences_.end()) {
        throw std::logic_error("Sender has no receivers");
    }

    bool has_any = false;
    for (const auto& pair : sender->receiver_preferences_) {
        IPackageReceiver* receiver = pair.first;
        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_any = true;
        } else if (receiver->get_receiver_type() == ReceiverType::WORKER) {
            auto worker = dynamic_cast<Worker*>(receiver);
            auto sender_node = dynamic_cast<PackageSender*>(worker);
            if (sender_node == sender) continue;
            has_any = true;
            has_reachable_storehouse(sender_node, node_colors);
        }
    }

    node_colors[sender] = NodeColor::VERIFIED;
    if (has_any) return true;
    else throw std::logic_error("No reachable storehouse");
}

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> node_colors;
    for (auto it = ramps_.cbegin(); it != ramps_.cend(); ++it) node_colors[&(*it)] = NodeColor::UNVISITED;
    for (auto it = workers_.cbegin(); it != workers_.cend(); ++it) node_colors[&(*it)] = NodeColor::UNVISITED;

    try {
        for (auto it = ramps_.cbegin(); it != ramps_.cend(); ++it) {
            has_reachable_storehouse(&(*it), node_colors);
        }
    } catch (...) {
        return false;
    }
    return true;
}

void Factory::do_deliveries(Time t) {
    for (auto& ramp : ramps_) ramp.deliver_goods(t);
}
void Factory::do_package_passing() {
    for (auto& ramp : ramps_) ramp.send_package();
    for (auto& worker : workers_) worker.send_package();
}

void Factory::do_work(Time t) {
    for (auto& worker : workers_) worker.do_work(t);
}

void Factory::add_ramp(Ramp&& ramp) { ramps_.add(std::move(ramp)); }
void Factory::remove_ramp(ElementID ID) { ramps_.remove_by_id(ID); }
NodeCollection<Ramp>::iterator Factory::find_ramp_by_id(ElementID ID) { return ramps_.find_by_id(ID); }
NodeCollection<Ramp>::const_iterator Factory::find_ramp_by_id(ElementID ID) const { return ramps_.find_by_id(ID); }
NodeCollection<Ramp>::const_iterator Factory::ramp_cbegin() const { return ramps_.cbegin(); }
NodeCollection<Ramp>::const_iterator Factory::ramp_cend() const { return ramps_.cend(); }

void Factory::add_worker(Worker&& worker) { workers_.add(std::move(worker)); }
void Factory::remove_worker(ElementID ID) { remove_receiver(workers_, ID); }
NodeCollection<Worker>::iterator Factory::find_worker_by_id(ElementID ID) { return workers_.find_by_id(ID); }
NodeCollection<Worker>::const_iterator Factory::find_worker_by_id(ElementID ID) const { return workers_.find_by_id(ID); }
NodeCollection<Worker>::const_iterator Factory::worker_cbegin() const { return workers_.cbegin(); }
NodeCollection<Worker>::const_iterator Factory::worker_cend() const { return workers_.cend(); }

void Factory::add_storehouse(Storehouse&& storehouse) { storehouses_.add(std::move(storehouse)); }
void Factory::remove_storehouse(ElementID ID) { remove_receiver(storehouses_, ID); }
NodeCollection<Storehouse>::iterator Factory::find_storehouse_by_id(ElementID ID) { return storehouses_.find_by_id(ID); }
NodeCollection<Storehouse>::const_iterator Factory::find_storehouse_by_id(ElementID ID) const { return storehouses_.find_by_id(ID); }
NodeCollection<Storehouse>::const_iterator Factory::storehouse_cbegin() const { return storehouses_.cbegin(); }
NodeCollection<Storehouse>::const_iterator Factory::storehouse_cend() const { return storehouses_.cend(); }

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
