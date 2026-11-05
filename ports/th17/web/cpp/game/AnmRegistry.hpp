#pragma once
#include "AnmVm.hpp"
#include <array>
#include <memory>
#include <unordered_map>
namespace th17 {
// A typed instance pool. Handles retain the original generation/slot scheme
// so stale owners cannot accidentally address a reused animation.
class AnmRegistry {
public:
    static constexpr u32 slot_bits=14,slot_mask=0x3fff,generation_mask=0x3ffff;
    static constexpr u32 scene_layer_count=43;
private:
    struct Node;
    struct Link {Node* value=nullptr;Link* previous=this;Link* next=this;explicit Link(Node* v=nullptr):value(v){}};
    // Registration and parent/child membership used to allocate two separate
    // std::list cells for each particle even when its VM came from the pool.
    // Embedded links retain stable sentinel/iterator semantics and exact order.
    class NodeList {
        Link root;u32 length=0;
    public:
        struct iterator {
            Link* link=nullptr;
            Node* operator*()const{return link->value;}
            iterator& operator++(){link=link->next;return *this;}
            iterator operator++(int){auto old=*this;++*this;return old;}
            bool operator==(iterator other)const{return link==other.link;}
            bool operator!=(iterator other)const{return link!=other.link;}
        };
        NodeList()=default;NodeList(const NodeList&)=delete;NodeList& operator=(const NodeList&)=delete;
        iterator begin()const{return {root.next};}iterator end()const{return {const_cast<Link*>(&root)};}
        bool empty()const{return length==0;}u32 size()const{return length;}
        Node* front()const{return root.next->value;}
        iterator insert(iterator at,Link& link){link.previous=at.link->previous;link.next=at.link;link.previous->next=&link;link.next->previous=&link;++length;return {&link};}
        void erase(iterator at){auto* link=at.link;link->previous->next=link->next;link->next->previous=link->previous;link->previous=link->next=link;--length;}
        void clear(){while(!empty())erase(begin());}
    };
    struct Node {
        AnmVm vm;AnmVm* borrowed=nullptr;u32 slot=slot_mask,handle=0;bool active=false,registered=false,alternate=false;
        bool frozen_root=false;
        Node* parent=nullptr;NodeList children;
        Link group_link{this},child_link{this};
        NodeList::iterator position,child_position;
    };
    // Allocate the native-sized pool once, before gameplay, with adjacent VMs
    // in one slab. Bursts must not allocate thousands of individual pool nodes.
    std::unique_ptr<Node[]> pool;
    std::unordered_map<AnmVm*,Node*> nodes;
    std::unordered_map<Node*,std::unique_ptr<Node>> overflow;
    std::vector<Node*> free_overflow;
    // Overflow handles share slot 0x3fff. Index their generation instead of
    // scanning every animation when a cancellation burst exceeds the pool.
    std::unordered_multimap<u32,Node*> overflow_handles;
    std::unordered_map<AnmVm*,std::unique_ptr<Node>> external;
    std::vector<u32> free_slots;
    std::array<NodeList,2> groups;
    // The original alternate-group mapping also accepts layer 31 -> 43;
    // ordinary scene drawing consumes layers 0..42. Keep that mapping intact.
    std::array<std::vector<AnmVm*>,44> layers;
    std::vector<Node*> pending_deletion;
    u32 generation=0;u32 active_count=0;
    void collect(Node&);void retire(Node&);void erase(Node&);Node* node(AnmVm&)const noexcept;
public:
    std::string error;
    AnmRegistry();
    AnmVm* allocate();u32 submit(AnmVm&,u32 ordering=0);
    AnmVm* find(u32 handle)const noexcept;
    AnmVm* find_child_script(AnmVm&,i32 script,i32 occurrence)const noexcept;
    bool attach(AnmVm& child,AnmVm& parent);
    std::vector<AnmVm*> children(const AnmVm&)const;
    bool discard(AnmVm&);bool destroy_tree(AnmVm&);
    bool preserve_embedded_children(AnmVm&);
    bool retire_children(AnmVm&);
    void retire_resource(const AnmResource* resource)noexcept;
    bool references_resource(const AnmResource*)const noexcept;
    bool update(bool alternate,Rng&,float rate);
    bool pause_tree(AnmVm&,bool paused);
    bool interrupt(AnmVm&,i32 label,Rng&,float rate,bool immediate);
    const std::vector<AnmVm*>& layer(u32 index)const noexcept{return layers[index];}
    u32 count()const noexcept{return active_count;}
    u32 handle(const AnmVm&)const noexcept;
    u32 slot(const AnmVm&)const noexcept;
    u32 ordered_handle(bool alternate,u32 index)const noexcept;
    u32 ordered_count(bool alternate)const noexcept{return groups[alternate].size();}
    u32& generation_counter()noexcept{return generation;}
};
}
