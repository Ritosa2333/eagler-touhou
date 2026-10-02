#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error RollbackPoolCapture is multiplayer-only
#endif

#include "RollbackPool.hpp"
#include <array>

namespace th10::multiplayer {

// Coverage for one checkpoint, kept outside rewindable pool ownership. Pool
// slots have an aligned Storage stride which may exceed the payload size.
// Save whole Storage runs so subsequent slot touches never overlap a run with
// a differently sized block (notably StageHint's pre-initialization touch).
template<std::size_t Capacity>
class RollbackPoolCapture {
public:
    void Clear() noexcept {base_=nullptr;stride_=payload_=0;}

    template<std::size_t BlockSize,class Touch>
    bool Capture(RollbackPool<BlockSize,Capacity>& pool,Touch touch){
        using Storage=typename RollbackPool<BlockSize,Capacity>::Storage;
        base_=reinterpret_cast<std::uint8_t*>(pool.blocks);
        stride_=sizeof(Storage);payload_=BlockSize;
        captured_.fill(false);
        if(!touch(pool.occupied,sizeof(pool.occupied))||
           !touch(&pool.cursor,sizeof(pool.cursor)))return false;
        for(std::size_t index=0;index<Capacity;){
            if(!pool.active(index)){++index;continue;}
            const auto first=index++;
            while(index<Capacity&&pool.active(index))++index;
            if(!touch(base_+first*stride_,(index-first)*stride_))return false;
            for(auto slot=first;slot<index;++slot)captured_[slot]=true;
        }
        return true;
    }

    bool OwnsSlot(const void* address,std::size_t bytes)const noexcept {
        if(!base_||!address||(bytes!=payload_&&bytes!=stride_))return false;
        const auto begin=reinterpret_cast<std::uintptr_t>(base_);
        const auto value=reinterpret_cast<std::uintptr_t>(address);
        return value>=begin&&value-begin<stride_*Capacity&&
               (value-begin)%stride_==0;
    }

    template<class Touch>
    bool TouchSlot(void* address,std::size_t bytes,Touch touch){
        // A foreign/interior/partial touch must retain the journal's normal
        // validation. Never claim coverage for bytes outside an exact slot.
        if(!OwnsSlot(address,bytes))return touch(address,bytes);
        const auto index=(reinterpret_cast<std::uintptr_t>(address)-
                          reinterpret_cast<std::uintptr_t>(base_))/stride_;
        if(captured_[index])return true;
        if(!touch(address,stride_))return false;
        captured_[index]=true;
        return true;
    }

private:
    std::array<bool,Capacity> captured_{};
    std::uint8_t* base_=nullptr;
    std::size_t stride_=0,payload_=0;
};

} // namespace th10::multiplayer
