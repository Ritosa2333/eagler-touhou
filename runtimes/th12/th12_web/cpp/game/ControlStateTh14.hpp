#pragma once
#include "LoadingWordTh14.hpp"
#include <atomic>
#include <cstdlib>
namespace th11 {
// Control's lifetime starts before initial Player construction. Its scalar
// flags and callback-enable publication are independent of pthread return.
struct ControlStateTh14 {
    LoadingWordTh14<u32> flags=4;
    LoadingWordTh14<u32> callbacks_enabled=0;
};
// Main owns retirement, after all loading writers have joined. Readers use
// the published owner only; no reader touches unique_ptr ownership storage.
class ControlPublicationTh14 {
    std::atomic<ControlStateTh14*> owner{nullptr};
public:
    ControlStateTh14* get()const noexcept{return owner.load(std::memory_order_acquire);}
    void publish(ControlStateTh14& value)noexcept{if(get())std::abort();owner.store(&value,std::memory_order_release);}
    void retire(ControlStateTh14& value)noexcept{if(get()!=&value)std::abort();owner.store(nullptr,std::memory_order_release);}
    u32 callback_flags()const noexcept{const auto* value=get();return value?u32(value->flags):0u;}
    bool enemy_updates_blocked()const noexcept{const auto* value=get();return !value||(u32(value->flags)&0x407u);}
    bool item_updates_blocked()const noexcept{const auto* value=get();return value&&(u32(value->flags)&0x405u);}
    bool combat_draw_blocked()const noexcept{const auto* value=get();return value&&(u32(value->flags)&4u);}
    bool frozen()const noexcept{const auto* value=get();return value&&(u32(value->flags)&5u);}
    bool phased_shake_blocked()const noexcept{const auto* value=get();return !value||(u32(value->flags)&0x77u);}
    bool normal_anm_blocked()const noexcept{const auto* value=get();if(!value)return false;const u32 flags=value->flags;return (flags&5u)&&(flags&2u);}
    bool callbacks_enabled()const noexcept{const auto* value=get();return !value||u32(value->callbacks_enabled)!=0;}
};
}
