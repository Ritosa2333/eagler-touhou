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
        u64 serialization_ordinal = 0;
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
    u64 next_serialization_ordinal = 1;
    struct CaptureScratch {
        std::vector<const Node*> closure_queue;
        std::vector<const Node*> ordered;
        std::vector<const Node*> overflow_nodes;
        std::vector<const Node*> external_nodes;
        std::vector<std::pair<const Node*, u32>> external_indices;
        std::vector<std::array<u32, 2>> chain_indices;
        std::vector<std::pair<const AnmVm*, u32>> embedded_index;
    };
    mutable CaptureScratch capture_scratch_;
    void compute_live_pool_mask(std::array<bool, 0x1fff>& keep) const noexcept;
    struct RestoreScratch {
        std::vector<Node*> restored;
        std::vector<std::pair<u32, Node*>> chain_members;
    };
    RestoreScratch restore_scratch_;
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
    struct SnapshotData {
        static constexpr u32 kContractVersion = 2;
        static constexpr u32 kNoVm = 0xffffffffu;
        u32 version = kContractVersion;
        struct Entry {
            AnmVm vm;
            u32 slot = 0x1fff;
            u32 handle = 0;
            u64 serialization_ordinal = 0;
            bool active = false, registered = false, alternate = false;
            bool borrowed = false, frozen_root = false, queued_deletion = false;
            u32 parent = 0xFFFFFFFFu;
            u32 creation_parent = kNoVm, rotation_parent = kNoVm;
            std::vector<u32> children;
            bool in_overflow = false;
            u32 chain_index[2] = {0xFFFFFFFFu, 0xFFFFFFFFu};
        };
        std::vector<Entry> entries;
        struct DrawReference {
            u32 entry = kNoVm, embedded = kNoVm;
            const AnmVm* original = nullptr;
        };
        std::array<std::vector<DrawReference>, 33> layers;
        std::vector<u32> free_slots;
        u32 generation = 0, active_count = 0;
        u64 next_serialization_ordinal = 1;
        std::vector<AnmVm*> borrowed_roots;
    };
    bool capture(SnapshotData& out, const std::vector<const AnmVm*>& embedded = {}) const;
    bool restore(const SnapshotData& source, const std::unordered_map<const AnmVm*, AnmVm*>& remapped = {},
                 const std::vector<AnmVm*>* embedded_vms = nullptr);
    u32 pool_slot(const AnmVm& vm) const noexcept;
    bool is_embedded_referenced(const AnmVm* vm) const noexcept;
    void poison_dead_slots_for_test();
};
}
