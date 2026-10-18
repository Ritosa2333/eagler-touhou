#pragma once
#include "Types.hpp"
#include <array>
namespace th11 {
// Plain owner metadata, journaled with the pool. It only skips empty slots:
// callers ask again after each callback so newly occupied higher indices are
// visited on this tick, lower indices next tick, just like the original loop.
template<u32 Capacity>struct ActiveSlotIndex {
    std::array<u32,(Capacity+31)/32> words{};bool valid=false;
    void invalidate()noexcept{valid=false;}
    void clear()noexcept{words.fill(0);valid=true;}
    template<class Occupied>void ensure(Occupied occupied){
        if(valid)return;words.fill(0);
        for(u32 i=0;i<Capacity;++i)if(occupied(i))words[i/32]|=u32(1)<<(i%32);
        valid=true;
    }
    void set(u32 slot,bool occupied)noexcept{
        if(!valid||slot>=Capacity)return;
        const auto mask=u32(1)<<(slot%32);if(occupied)words[slot/32]|=mask;else words[slot/32]&=~mask;
    }
    u32 next(u32 first)const noexcept{
        if(first>=Capacity)return Capacity;
        u32 word=first/32,bits=words[word]&(0xffffffffu<<(first%32));
        while(!bits){if(++word==words.size())return Capacity;bits=words[word];}
        const u32 result=word*32+u32(__builtin_ctz(bits));return result<Capacity?result:Capacity;
    }
};
// Escaped mutable references retain their original meaning across arbitrary
// later callbacks and Undo. This physical alias set is NEVER rewound. Keep it
// in stable external storage so the manager's snapshot metadata stays plain.
template<u32 Capacity>struct ExposedSlotIndex {
    std::array<u32,(Capacity+31)/32> words{};u32 count=0;
    void remember(u32 slot)noexcept{
        if(slot>=Capacity)return;const auto mask=u32(1)<<(slot%32);
        if(!(words[slot/32]&mask)){words[slot/32]|=mask;++count;}
    }
    template<class Read>void refresh(ActiveSlotIndex<Capacity>& index,Read occupied)const{
        for(u32 word=0;word<words.size();++word){auto bits=words[word];
            while(bits){const auto i=word*32+u32(__builtin_ctz(bits));index.set(i,occupied(i));bits&=bits-1;}
        }
    }
};
}
