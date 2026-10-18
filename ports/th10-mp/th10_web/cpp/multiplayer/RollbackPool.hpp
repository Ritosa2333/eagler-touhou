#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error RollbackPool is multiplayer-only
#endif

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace th10::multiplayer {

template<std::size_t BlockSize,std::size_t Capacity>
struct RollbackPool {
    using Storage=std::aligned_storage_t<BlockSize,alignof(std::max_align_t)>;
    Storage blocks[Capacity]{};
    std::uint8_t occupied[Capacity]{};
    std::uint32_t cursor=0;

    void* allocate(std::size_t bytes,bool zero=false) noexcept {
        if(!bytes||bytes>BlockSize)return nullptr;
        for(std::size_t n=0;n<Capacity;++n){
            const auto index=(cursor+static_cast<std::uint32_t>(n))%Capacity;
            if(occupied[index])continue;
            occupied[index]=1;cursor=(index+1)%Capacity;
            void* result=&blocks[index];
            if(zero)std::memset(result,0,BlockSize);
            return result;
        }
        return nullptr;
    }
    bool owns(const void* pointer) const noexcept {
        if(!pointer)return false;
        const auto begin=reinterpret_cast<std::uintptr_t>(&blocks[0]);
        const auto end=reinterpret_cast<std::uintptr_t>(&blocks[Capacity]);
        const auto value=reinterpret_cast<std::uintptr_t>(pointer);
        return value>=begin&&value<end&&((value-begin)%sizeof(Storage))==0;
    }
    bool release(void* pointer) noexcept {
        if(!owns(pointer))return false;
        const auto begin=reinterpret_cast<std::uintptr_t>(&blocks[0]);
        const auto value=reinterpret_cast<std::uintptr_t>(pointer);
        const auto index=(value-begin)/sizeof(Storage);
        if(index>=Capacity||!occupied[index])return false;
        occupied[index]=0;return true;
    }
    void clear() noexcept {std::memset(occupied,0,sizeof(occupied));cursor=0;}
    bool active(std::size_t index) const noexcept {return index<Capacity&&occupied[index]!=0;}
    void* at(std::size_t index) noexcept {return index<Capacity?static_cast<void*>(&blocks[index]):nullptr;}
    const void* at(std::size_t index) const noexcept {return index<Capacity?static_cast<const void*>(&blocks[index]):nullptr;}
};

} // namespace th10::multiplayer
