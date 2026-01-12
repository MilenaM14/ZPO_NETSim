//
// Created by milen on 10.01.2026.
//

#ifndef AAANETSIM_FACTORY_HPP
#define AAANETSIM_FACTORY_HPP

#include "types.hpp"
#include "nodes.hpp"
#include <list>
#include <algorithm>
#include <stdexcept>
#include <pstl/execution_impl.h>

template <typename Node>
class NodeCollection //klasa pomocnicza, żeby nie pisać trzy razy tego samego kodu dla std::list<Ramp/Worker/Storehouse>
{
public:
    using container_t = std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    //teraz beda metody do petli for-each na obiekcie NodeCollection
    iterator begin() {return collection_.begin();}
    iterator end() {return collection_.end();}
    const_iterator begin() const {return collection_.cbegin();}
    const_iterator end() const {return collection_.cend();}
    const_iterator cbegin() const {return collection_.cbegin();}
    const_iterator cend() const {return collection_.cend();}

    //dodawanie wezla
    void add(Node&& node)
    {
        collection_emplace_back(std::move(node));
    }

    //wyszukiwanie elementu po ID
    iterator find_by_id(ElementID id)
    {
        return std::find_if(collection_.begin(), collection_.end(),
            [id](const Node& node) {return node.get_id == id;});
    }

    //wyszukiwanie elementow dla obiektow const
    const_iterator find_by_id(ElementID id) const
    {
        return std::find_if(collection_.cbegin(), collection_.cend(),
            [id](const Node& node) {return node.get_id == id;});
    }

    //usuwanie po ID
    void remove_by_id(ElementID id)
    {
        auto it = find_by_id(id);
        if (it != collection_.end())
        {
            collection_.erase(it);
        }
    }

private:
    container_t collection_; //lista przechowująca obiekty
};

class Factory {
private:
    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;

    void remove_receiver(NodeCollection<IPackageReceiver>& collection,
                         ElementID id);

public:
    //ogolnie to tu trzeba dopisac wszystko, bo to tylko z diagramu uml jest wziete

    void add_ramp(Ramp&& ramp);
    void remove_ramp(ElementID id);

    void add_worker(Worker&& worker);
    void remove_worker(ElementID id);

    void add_storehouse(Storehouse&& storehouse);
    void remove_storehouse(ElementID id);

    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id);
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const;

    NodeCollection<Ramp>::const_iterator ramp_cbegin() const;
    NodeCollection<Ramp>::const_iterator ramp_cend() const;

    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id);
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const;

    NodeCollection<Worker>::const_iterator worker_cbegin() const;
    NodeCollection<Worker>::const_iterator worker_cend() const;

    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id);
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const;

    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const;
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const;



    bool is_consistent() const;

    void do_deliveries(Time t);
    void do_work(Time t);
    void do_package_passing();
};


#endif //AAANETSIM_FACTORY_HPP