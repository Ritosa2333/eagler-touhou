#pragma once
#include "AnmVm.hpp"
#include <array>
#include <list>
#include <memory>
#include <unordered_map>
namespace th13 {
class AnmRegistry;
using ChainEntryObserver = void (*)(const AnmRegistry&, bool);
extern ChainEntryObserver chain_entry_observer;
class AnmRegistry {
    struct Node {
        AnmVm vm;
        AnmVm* borrowed = nullptr;
        u32 slot = 0x1fff, handle = 0;
        bool active = false, registered = false, alternate = false;
        bool frozen_root = false, queued_deletion = false;
        Node* parent = nullptr;
        std::list<Node*> children;
        std::list<Node*>::iterator position;
    };
    std::array<std::unique_ptr<Node>, 0x1fff> pool;
    std::unordered_map<AnmVm*, Node*> nodes;
    std::unordered_map<Node*, std::unique_ptr<Node>> overflow;
    std::unordered_map<AnmVm*, std::unique_ptr<Node>> external;
    std::vector<u32> free_slots;
    std::array<std::list<Node*>, 2> groups;
    std::array<std::vector<AnmVm*>, 33> layers;
    std::vector<Node*> pending_deletion;
    u32 generation = 0;
    u32 active_count = 0;
    void collect(Node&);
    void retire(Node&);
    void erase(Node&);
    Node* node(AnmVm&) const noexcept;
    bool tick_chain(u32 alternate, GameRandom&, float rate);

public:
    std::string error;
    AnmRegistry();
    AnmVm* allocate();
    u32 submit(AnmVm&, u32 ordering = 0);
    AnmVm* find(u32 handle) const noexcept;
    AnmVm* find_child_script(AnmVm&, i32 script, i32 occurrence) const noexcept;
    bool attach(AnmVm& child, AnmVm& parent);
    std::vector<AnmVm*> children(const AnmVm&) const;
    bool discard(AnmVm&);
    bool destroy_tree(AnmVm&);
    void requeue_to_back(AnmVm&);
    bool preserve_embedded_children(AnmVm&);
    bool retire_children(AnmVm&);
    void retire_resource(const AnmResource* resource) noexcept;
    bool references_resource(const AnmResource*) const noexcept;
    bool update(GameRandom&, float rate);
    bool update_chain(bool alternate, GameRandom&, float rate);
    bool pause_tree(AnmVm&, bool paused);
    bool interrupt(AnmVm&, i32 label, GameRandom&, float rate, bool immediate);
    const std::vector<AnmVm*>& layer(u32 index) const noexcept {
        static const std::vector<AnmVm*> empty;
        return index < layers.size() ? layers[index] : empty;
    }
    u32 count() const noexcept { return active_count; }
    u32 handle(const AnmVm&) const noexcept;
    u32 slot(const AnmVm&) const noexcept;
    u32 ordered_handle(bool alternate, u32 index) const noexcept;
    void append_draw(AnmVm& vm);
    u32 ordered_count(bool alternate) const noexcept { return groups[alternate].size(); }
    u32& generation_counter() noexcept { return generation; }
};
}
