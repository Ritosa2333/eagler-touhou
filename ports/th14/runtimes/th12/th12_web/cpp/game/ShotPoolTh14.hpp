#pragma once
#include "Types.hpp"
#include <array>
namespace th11 {
// Retail 44cdb7 initializes 256 nodes by pushing ascending slots onto the
// free-list head. 450e40/450ed0 remove only successful allocations; 451380
// returns retired slots to the head while updating the array in ascending order.
struct Th14ShotPool {
    static constexpr u32 capacity=256;
    std::array<u32,capacity> next{};u32 head=capacity;
    Th14ShotPool(){for(u32 i=0;i<capacity;++i){next[i]=head;head=i;}}
    u32 peek()const noexcept{return head;}
    void consume()noexcept{if(head<capacity){const u32 slot=head;head=next[slot];next[slot]=capacity;}}
    void recycle(u32 slot)noexcept{if(slot<capacity){next[slot]=head;head=slot;}}
};
}
