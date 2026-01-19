//
// Created by milen on 10.01.2026.
//
#include "factory.hpp"
#include <map>
#include <string>
#include <sstream>
#include <vector>

//Dzieli napis na części po danym znaku
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
//wczytanie fabryki z pliku
Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;
    // Czytamy plik linia po linii
    while (std::getline(is, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') continue; //ignorownie komentarzy i pustych linii

        std::istringstream iss(line);
        // Pierwsze słowo w linii określa typ obiektu
        std::string type;
        iss >> type;

        if (type.empty()) continue;
//parsowanie argumentow
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
        //Definicja rampy
        if (type == "LOADING_RAMP") {
            if (args.count("id") && args.count("delivery-interval")) {
                ElementID id = std::stoi(args["id"]);
                TimeOffset di = std::stoi(args["delivery-interval"]);
                factory.add_ramp(Ramp(id, di));// Dodajemy rampę do fabryki
            }
        } // Definicja workera
        else if (type == "WORKER") {
            if (args.count("id") && args.count("processing-time") && args.count("queue-type")) {
                ElementID id = std::stoi(args["id"]);
                TimeOffset pt = std::stoi(args["processing-time"]);
                std::string qt_str = args["queue-type"];
                // Określenie typu kolejki paczek
                PackageQueueType qt = (qt_str == "LIFO") ? PackageQueueType::LIFO : PackageQueueType::FIFO;
                factory.add_worker(Worker(id, pt, std::make_unique<PackageQueue>(qt)));  // Worker otrzymuje własną kolejkę
            }
        }//Definicja magazynu
        else if (type == "STOREHOUSE") {
            if (args.count("id")) {
                ElementID id = std::stoi(args["id"]);
                factory.add_storehouse(Storehouse(id));
            }
        }
        else if (type == "LINK") { //  // Definicja połączenia
            if (args.count("src") && args.count("dest")) {
                // Rozbijamy np. "ramp-1" → ["ramp", "1"]
                auto src_parts = split(args["src"], '-');
                auto dest_parts = split(args["dest"], '-');

                if (src_parts.size() >= 2 && dest_parts.size() >= 2) {
                    std::string src_type = src_parts[0];// Źródło
                    ElementID src_id = std::stoi(src_parts[1]);
                    // Cel
                    std::string dest_type = dest_parts[0];
                    ElementID dest_id = std::stoi(dest_parts[1]);
                    // Szukamy nadawcy paczek (rampa lub worker)
                    PackageSender* sender = nullptr;
                    if (src_type == "ramp") {
                        auto it = factory.find_ramp_by_id(src_id);
                        if (it != factory.ramp_cend()) sender = &(*it);
                    } else if (src_type == "worker") {
                        auto it = factory.find_worker_by_id(src_id);
                        if (it != factory.worker_cend()) sender = &(*it);
                    }
                    // Szukamy odbiorcy paczek (worker lub magazyn)
                    IPackageReceiver* receiver = nullptr;
                    if (dest_type == "worker") {
                        auto it = factory.find_worker_by_id(dest_id);
                        if (it != factory.worker_cend()) receiver = &(*it);
                    } else if (dest_type == "storehouse" || dest_type == "store") {
                        auto it = factory.find_storehouse_by_id(dest_id);
                        if (it != factory.storehouse_cend()) receiver = &(*it);
                    }
                    // Jeśli oba węzły istnieją – tworzymy połączenie w grafie
                    if (sender && receiver) {
                        sender->receiver_preferences_.add_receiver(receiver);
                    }
                }
            }
        }
    }
    return factory;  // Zwracamy kompletnie zbudowaną fabrykę
}

void save_factory_structure(const Factory& factory, std::ostream& os) {
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {  // Zapis wszystkich ramp załadunkowych
        os << "LOADING_RAMP id=" << it->get_id() << " delivery-interval=" << it->get_delivery_interval() << "\n";
    }
    // Zapis wszystkich workerów wraz z parametrami przetwarzania
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        os << "WORKER id=" << it->get_id() << " processing-time=" << it->get_processing_duration()
           << " queue-type=" << (it->get_queue()->get_queue_type() == PackageQueueType::LIFO ? "LIFO" : "FIFO") << "\n";
    }
    // Zapis wszystkich magazynów
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << "\n";
    }
//połączenia (LINK) dla danego nadawcy paczek
    auto save_links = [&](const PackageSender& sender, std::string src_prefix, ElementID src_id) {
        for (const auto& [receiver, prob] : sender.receiver_preferences_.get_preferences()) {
            std::string dest_prefix = (receiver->get_receiver_type() == ReceiverType::WORKER) ? "worker" : "store";  // Określenie typu odbiorcy (worker lub storehouse)
            os << "LINK src=" << src_prefix << "-" << src_id << " dest=" << dest_prefix << "-" << receiver->get_id() << "\n";
        }// Zapis połączenia w formacie LINK
    };
    // Zapis połączeń wychodzących z ramp
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) save_links(*it, "ramp", it->get_id());
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) save_links(*it, "worker", it->get_id());
    // Zapis połączeń wychodzących z workerów
}

enum class NodeColor { UNVISITED, VISITED, VERIFIED };
//sprawdzanie, czy istnieje ścieżka do magazynu
bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) {
    if (node_colors[sender] == NodeColor::VERIFIED) return true;
    if (node_colors[sender] == NodeColor::VISITED) throw std::logic_error("Cycle detected");

    node_colors[sender] = NodeColor::VISITED;
// // Nadawca bez odbiorców – błąd struktury fabryki
    if (sender->receiver_preferences_.begin() == sender->receiver_preferences_.end()) {
        throw std::logic_error("Sender has no receivers");
    }

    bool has_any = false;
    for (const auto& pair : sender->receiver_preferences_) {  // Przeglądamy wszystkich odbiorców nadawcy
        IPackageReceiver* receiver = pair.first;
        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_any = true;
        } else if (receiver->get_receiver_type() == ReceiverType::WORKER) {  // Jeśli odbiorcą jest worker – schodzimy rekurencyjnie dalej
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
//sprawdzenie poprawności fabrykii
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