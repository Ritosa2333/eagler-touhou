#pragma once
#include <cstddef>
#include <cstdint>
#include <mutex>

// Recovered source, not a loader/bridge. No original executable addresses are
// called by this library. VA annotations identify reverse-engineering evidence.
namespace th20::source::scheduler {
struct Node;
struct List;
struct Iterator;
using Callback = std::int32_t (__cdecl*)(void*);

struct Link {
    Node* value;                 // +00
    Link* next;                 // +04
    Link* previous;             // +08
    List* owner;                // +0c
    Iterator* iterator;         // +10: deletion repairs current/next observer
};
struct Node {
    std::int32_t priority;      // +00
    std::uint32_t flags;        // +04: bit 0 heap ownership, bit 1 enabled
    Callback callback;         // +08
    Callback before_insert;    // +0c: called once before insertion, then zeroed
    Callback on_shutdown;      // +10: update shutdown / callback result 7
    Link link;                 // +14
    void* userdata;             // +28
};
struct List {
    Link sentinel;             // +00: next is first real link
    Link* tail;                // +14: sentinel when empty
};
struct State {
    Link* current;             // +00: separate pointer adjusted by remove
    List update;               // +04
    List draw;                 // +1c
    std::uint32_t shutdown;    // +34
};
static_assert(sizeof(void*) == 4, "Recovered engine uses the original x86 layouts");
static_assert(sizeof(Link) == 20 && sizeof(Node) == 44 && sizeof(List) == 24 && sizeof(State) == 56);
static_assert(offsetof(Node, link) == 0x14 && offsetof(Node, userdata) == 0x28);
static_assert(offsetof(State, draw) == 0x1c && offsetof(State, shutdown) == 0x34);

// Owns the process-level locks previously at 0x5c0240. This is real C++ locking,
// not a fake implementation of original CRT internals. The update/draw lock is
// dropped during callbacks, exactly at 0x4128ee/0x412b6f and reacquired after.
// Allocator/debug tracking and cross-module lock integration remain external.
struct Environment {
private:
    std::recursive_mutex owned_chain_mutex_;
    std::recursive_mutex owned_allocation_mutex_;
    bool owned_dispatch_locking_ = true;
    std::uint8_t owned_dispatch_depth_ = 0;
public:
    std::recursive_mutex& chain_mutex;
    std::recursive_mutex& allocation_mutex;
    bool& dispatch_locking;
    std::uint8_t& dispatch_depth;
    Environment() : chain_mutex(owned_chain_mutex_), allocation_mutex(owned_allocation_mutex_),
        dispatch_locking(owned_dispatch_locking_), dispatch_depth(owned_dispatch_depth_) {}
    Environment(std::recursive_mutex& chain, std::recursive_mutex& allocation,
                bool& enabled, std::uint8_t& depth) : chain_mutex(chain), allocation_mutex(allocation),
        dispatch_locking(enabled), dispatch_depth(depth) {}
    void enter_dispatch();
    void leave_dispatch();
};

Link& initialize_link(Link&, Node*) noexcept;                 // 0x411970
Node& initialize_node(Node&) noexcept;                       // 0x411a70
List& initialize_list(List&) noexcept;                       // 0x418a50
State& initialize_state(State&) noexcept;                    // 0x418a80
Node* create(Callback);                                     // 0x4127b0/0x411910
void set_callback(Node&, Callback) noexcept;                 // 0x412d50
void set_userdata(Node&, void*) noexcept;                    // 0x412d10
void set_owned(Node&) noexcept;                             // 0x412d30
void enable(Node&) noexcept;                                // 0x412d80
void disable(Node&) noexcept;                               // 0x4127f0
void set_before_insert(Node&, Callback) noexcept;             // 0x412dc0
void set_shutdown_callback(Node&, Callback) noexcept;         // 0x412da0
void clear_callbacks(Node&) noexcept;                       // 0x411b80

struct Iterator {
    Link* current;
    Link* next;
    explicit Iterator(Link* first) noexcept;                // 0x4119b0
    ~Iterator();                                            // 0x411b00
    Iterator& advance() noexcept;                            // 0x411c30
    Iterator(const Iterator&) = delete;
    Iterator& operator=(const Iterator&) = delete;
};
static_assert(sizeof(Iterator) == 8);
void insert_after(Link& existing, Link& added) noexcept;      // 0x411ee0
void insert_before(Link& existing, Link& added) noexcept;     // 0x411f30
void append(List&, Link&) noexcept;                         // 0x411ea0
void unlink_raw(Link&) noexcept;                            // 0x411d50
void unlink(Link&) noexcept;                                // 0x411ce0/0x411d20
Link* find(List&, Node*) noexcept;                          // 0x411e70/0x411e30
std::int32_t insert(State&, Environment&, Node&, std::int32_t priority, bool draw); // 0x411f80/0x412100
void remove_unlocked(State&, Environment&, Node*);           // 0x412400
void remove(State&, Environment&, Node*);                    // 0x4124b0
Node* register_callback(State&, Environment&, std::int32_t, Callback, void*, bool draw, bool enabled);
std::int32_t dispatch_update(State&, Environment&);          // 0x412810
std::int32_t dispatch_draw(State&, Environment&);            // 0x412aa0
// Implements the scheduling portion of 0x4125b0. Its leading graphics flush at
// 0x4d9e30 is an unrecovered renderer dependency and must be called by the owner.
void shutdown_chains(State&, Environment&);
}
