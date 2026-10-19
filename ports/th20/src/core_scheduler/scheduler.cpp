#include "scheduler.hpp"
#include <cstring>
#include <new>

namespace th20::source::scheduler {
void Environment::enter_dispatch() {
    if (dispatch_locking) { chain_mutex.lock(); ++dispatch_depth; }
}
void Environment::leave_dispatch() {
    if (dispatch_locking) { --dispatch_depth; chain_mutex.unlock(); }
}
Link& initialize_link(Link& x, Node* value) noexcept {
    x.value = value; x.next = nullptr; x.previous = nullptr; x.owner = nullptr; x.iterator = nullptr;
    return x;
}
Node& initialize_node(Node& n) noexcept {
    n.priority = 0; n.flags = 0; n.callback = nullptr; n.before_insert = nullptr; n.on_shutdown = nullptr;
    initialize_link(n.link, &n); n.userdata = nullptr; return n;
}
List& initialize_list(List& x) noexcept {
    initialize_link(x.sentinel, nullptr); x.tail = &x.sentinel; return x;
}
State& initialize_state(State& s) noexcept {
    s.current = nullptr; initialize_list(s.update); initialize_list(s.draw); s.shutdown = 0; return s;
}
Node* create(Callback callback) {
    Node* node = new Node;
    initialize_node(*node); set_callback(*node, callback); set_owned(*node); return node;
}
void set_callback(Node& n, Callback x) noexcept { n.callback = x; n.before_insert = nullptr; n.on_shutdown = nullptr; }
void set_userdata(Node& n, void* x) noexcept { n.userdata = x; }
void set_owned(Node& n) noexcept { n.flags |= 1u; }
void enable(Node& n) noexcept { n.flags |= 2u; }
void disable(Node& n) noexcept { n.flags &= ~2u; }
void set_before_insert(Node& n, Callback x) noexcept { n.before_insert = x; }
void set_shutdown_callback(Node& n, Callback x) noexcept { n.on_shutdown = x; }
void clear_callbacks(Node& n) noexcept { n.callback = nullptr; n.before_insert = nullptr; n.on_shutdown = nullptr; }
Iterator::Iterator(Link* first) noexcept : current(first), next(nullptr) {
    if (current) current->iterator = this;
    next = first ? first->next : nullptr;
    if (next) next->iterator = this;
}
Iterator::~Iterator() {
    if (current) current->iterator = nullptr;
    if (next) next->iterator = nullptr;
}
Iterator& Iterator::advance() noexcept {
    if (current) current->iterator = nullptr;
    current = next; next = current;
    if (next) {
        next = current->next;
        if (next) next->iterator = this;
    }
    return *this;
}
void insert_after(Link& existing, Link& added) noexcept {
    if (existing.next) { added.next = existing.next; existing.next->previous = &added; }
    existing.next = &added; added.owner = existing.owner; added.previous = &existing;
}
void insert_before(Link& existing, Link& added) noexcept {
    if (existing.previous) { added.previous = existing.previous; existing.previous->next = &added; }
    added.owner = existing.owner; added.next = &existing; existing.previous = &added;
}
void append(List& list, Link& link) noexcept {
    insert_after(*list.tail, link); link.owner = &list; list.tail = &link;
}
void unlink_raw(Link& link) noexcept {
    if (link.iterator) {
        if (link.iterator->current == &link) link.iterator->current = nullptr;
        if (link.iterator->next == &link && link.iterator->next) {
            link.iterator->next = link.next;
            if (link.iterator->next) link.iterator->next->iterator = link.iterator;
        }
        link.iterator = nullptr;
    }
    if (link.next) link.next->previous = link.previous;
    if (link.previous) link.previous->next = link.next;
    link.next = nullptr; link.previous = nullptr; link.owner = nullptr;
}
void unlink(Link& link) noexcept {
    if (link.owner && link.owner->tail == &link) link.owner->tail = link.previous;
    unlink_raw(link);
}
Link* find(List& list, Node* node) noexcept {
    // The original begins at the sentinel; null-node callers are checked by
    // remove before this search, preserving the original null-sentinel behavior.
    for (Link* link = &list.sentinel; link; link = link->next) if (link->value == node) return link;
    return nullptr;
}
std::int32_t insert(State& state, Environment& env, Node& node, std::int32_t priority, bool draw) {
    std::int32_t result=0;
    if (node.before_insert) { result=node.before_insert(node.userdata); node.before_insert = nullptr; }
    std::lock_guard<std::recursive_mutex> lock(env.chain_mutex);
    node.priority = priority;
    List& list = draw ? state.draw : state.update;
    Iterator it(list.sentinel.next);
    for (; it.current; it.advance()) {
        if (priority <= it.current->value->priority) { insert_before(*it.current, node.link); return result; }
    }
    append(list, node.link);
    return result;
}
void remove_unlocked(State& state, Environment& env, Node* node) {
    if (!node) return;
    Link* link = find(state.update, node);
    if (!link) link = find(state.draw, node);
    if (!link) return;
    if (state.current == link) state.current = link->next;
    unlink(*link); node->callback = nullptr;
    if (node->flags & 1u) {
        clear_callbacks(*node);
        std::lock_guard<std::recursive_mutex> lock(env.allocation_mutex);
        delete node;
    }
}
void remove(State& state, Environment& env, Node* node) {
    if (node) { std::lock_guard<std::recursive_mutex> lock(env.chain_mutex); remove_unlocked(state, env, node); }
}
Node* register_callback(State& state, Environment& env, std::int32_t priority,
                        Callback callback, void* userdata, bool draw, bool enabled) {
    Node* node = create(callback); set_userdata(*node, userdata);
    if (enabled) enable(*node); else disable(*node);
    insert(state, env, *node, priority, draw); return node;
}
namespace {
std::int32_t signed_count(std::uint32_t value) noexcept {
    std::int32_t result; std::memcpy(&result, &value, sizeof(result)); return result;
}
// Keeps original locking boundaries. No added catch/cleanup around callbacks:
// exception unwinding across a game callback is outside the recovered contract.
std::int32_t dispatch(State& state, Environment& env, bool draw) {
    env.enter_dispatch();
    std::uint32_t count = 0;
    List& list = draw ? state.draw : state.update;
    bool restart;
    do {
        restart = false;
        count = 0;
        Iterator it(list.sentinel.next);
        bool stop = false;
        while (it.current && !stop && !restart) {
            Link* link = it.current;
            Node* node = link->value;
            if (node->callback) {
                bool count_current = true;
                while (node->flags & 2u) {
                    if (!draw && state.shutdown) {
                        if (node->on_shutdown) node->on_shutdown(node->userdata);
                        break;
                    }
                    env.leave_dispatch();
                    const std::int32_t result = node->callback(node->userdata);
                    env.enter_dispatch();
                    if (result == 2) continue;
                    if (result == 0) remove_unlocked(state, env, node);
                    else if (result == 3 || result == 4 || result == 5 || (!draw && result == 8)) {
                        count = result == 3 ? 1u : result == 5 ? 0xffffffffu : 0u;
                        stop = true; count_current = false;
                    } else if (!draw && result == 6) {
                        restart = true; count_current = false;
                    } else if (!draw && result == 7) {
                        if (node->on_shutdown) node->on_shutdown(node->userdata);
                    }
                    break;
                }
                if (count_current) ++count;
            }
            if (!stop && !restart) it.advance();
        }
    } while (restart);
    if (draw) {
        // 0x412c4c-0x412cc2 clears LINK+0x10 observers, not NODE callbacks.
        Iterator it(list.sentinel.next);
        for (; it.current; it.advance()) it.current->iterator = nullptr;
    }
    env.leave_dispatch();
    return signed_count(count);
}
}
std::int32_t dispatch_update(State& state, Environment& env) { return dispatch(state, env, false); }
std::int32_t dispatch_draw(State& state, Environment& env) { return dispatch(state, env, true); }
void shutdown_chains(State& state, Environment& env) {
    state.shutdown = 1;
    dispatch_update(state, env);
    for (List* list : {&state.update, &state.draw}) {
        Iterator it(list->sentinel.next);
        for (; it.current; it.advance()) remove(state, env, it.current->value);
    }
}
}
