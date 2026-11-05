#pragma once
#include <cstddef>
#include <cstdint>
#include <memory_resource>
#include <vector>
namespace th20::source::menu {
// Original deque storage includes an iterator proxy even in this executable's
// release build. Keeping its actual ownership avoids a second hidden container.
struct DequeStorage {
    struct Proxy {DequeStorage* container;void* first_iterator;};
    Proxy* proxy;
    std::int32_t** blocks;
    std::uint32_t block_count,first,size;
    DequeStorage(); //4aee90->4aed10/4ad720
    ~DequeStorage(); //4af9e0->4b7b80
    DequeStorage(const DequeStorage&)=delete;
    DequeStorage& operator=(const DequeStorage&)=delete;
};
static_assert(sizeof(DequeStorage)==20);
struct Cursor {
    std::int32_t current,previous,count,minimum;
    std::pmr::vector<std::int32_t> excluded;
    DequeStorage history,secondary_history;
    std::int32_t wrapping;
    Cursor(); //4aef00
    ~Cursor()=default; //4afa70, reverse member destruction
    bool is_excluded(std::int32_t) const; //46a4d0->469fc0
    std::int32_t select(std::int32_t); //4bed50
    std::int32_t move(std::int32_t); //4bfa00
    void snapshot() noexcept{previous=current;} //4bfc00
    bool changed() const noexcept{return current!=previous;} //45d030
    bool selected(std::int32_t value) const noexcept{return current==value;} //46b1b0
};
static_assert(sizeof(std::pmr::vector<std::int32_t>)==16&&sizeof(Cursor)==0x4c&&offsetof(Cursor,history)==0x20&&offsetof(Cursor,wrapping)==0x48);
}
