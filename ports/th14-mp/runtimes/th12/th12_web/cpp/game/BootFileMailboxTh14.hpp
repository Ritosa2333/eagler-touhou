#pragma once
#include "AnmResource.hpp"
#include <array>
#include <atomic>
#if !defined(__wasm__)
#include <chrono>
#include <thread>
#endif
namespace th11 {
// One immutable owner per retail ANM slot. Close bits bypass waiting, never
// acknowledge Main's templates or discard a still-pending earlier resource.
class BootFileMailboxTh14 {
    struct Slot {std::atomic<int> state{0};AnmResource* resource=nullptr;};
    std::array<Slot,26> slots;
    const u32* plain_flags=nullptr;
    const std::atomic<u32>* atomic_flags=nullptr;
    std::atomic<int> cancelled{0};
public:
    explicit BootFileMailboxTh14(const u32& flags):plain_flags(&flags){}
    explicit BootFileMailboxTh14(const std::atomic<u32>& flags):atomic_flags(&flags){}
    BootFileMailboxTh14(const u32* live,const std::atomic<u32>& fallback):plain_flags(live),atomic_flags(live?nullptr:&fallback){}
    bool publish_file(AnmResource& resource,u16 file){
        if(file>=slots.size()||cancelled.load(std::memory_order_acquire))return false;
        auto& slot=slots[file];if(slot.state.load(std::memory_order_acquire)!=0)return false;
        slot.resource=&resource;slot.state.store(1,std::memory_order_release);
        for(;;){
            if(cancelled.load(std::memory_order_acquire))return false;
            // Retail47d0f0 checks live close bits before Sleep(1). An ACK
            // alone never wakes the timed wait, including a very early ACK.
            const u32 flags=plain_flags?__atomic_load_n(plain_flags,__ATOMIC_ACQUIRE):atomic_flags->load(std::memory_order_acquire);
            if(flags&0x180u)return true;
#if defined(__wasm__)
            __builtin_wasm_memory_atomic_wait32(reinterpret_cast<int*>(&cancelled),0,1000000);
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif
            if(cancelled.load(std::memory_order_acquire))return false;
            const auto state=slot.state.load(std::memory_order_acquire);
            if(state==2)return true;
            if(state!=1)return false;
        }
    }
    bool pending_file(AnmResource*& resource,u16& file)const noexcept{
        if(cancelled.load(std::memory_order_acquire))return false;
        // Main47d720 rescans ascending slots before every upload chunk.
        for(u16 index=0;index<slots.size();++index){
            const auto& slot=slots[index];
            if(slot.state.load(std::memory_order_acquire)==1){resource=slot.resource;file=index;return true;}
        }
        return false;
    }
    bool complete_file(AnmResource& resource,u16 file)noexcept{
        if(file>=slots.size()||cancelled.load(std::memory_order_acquire))return false;
        auto& slot=slots[file];
        if(slot.state.load(std::memory_order_acquire)!=1||slot.resource!=&resource)return false;
        slot.state.store(2,std::memory_order_release);return true;
    }
    void cancel()noexcept{
        cancelled.store(1,std::memory_order_release);
#if defined(__wasm__)
        __builtin_wasm_memory_atomic_notify(reinterpret_cast<int*>(&cancelled),1);
#endif
    }
};
}
