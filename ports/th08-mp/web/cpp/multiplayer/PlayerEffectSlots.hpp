#pragma once

#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error PlayerEffectSlots.hpp is exclusive to multiplayer gameplay builds.
#endif

#include "../game/EffectPoolLayout.hpp"

namespace th08::player_effect_slots {

using namespace effect_pool_layout;
inline constexpr i32 seat_count=3;

// P0 keeps the native bank. Slot 12 remains the shared moon; P1 and P2 each
// receive twelve physical slots after that shared entry.
constexpr i32 player_relative_slot(i32 seat,i32 local_slot) noexcept {
    if(seat<0||seat>=seat_count||local_slot<0||local_slot>=player_fixed_count)return -1;
    return seat==0?local_slot:seat==1?shared_fixed_count+local_slot:shared_fixed_count+player_fixed_count+local_slot;
}

constexpr i32 seat_for_relative_slot(i32 relative_slot) noexcept {
    if(relative_slot>=0&&relative_slot<player_fixed_count)return 0;
    if(relative_slot>=shared_fixed_count&&relative_slot<shared_fixed_count+player_fixed_count)return 1;
    if(relative_slot>=shared_fixed_count+player_fixed_count&&relative_slot<fixed_count)return 2;
    return -1;
}

constexpr i32 local_slot_for_relative_slot(i32 relative_slot) noexcept {
    if(relative_slot>=0&&relative_slot<player_fixed_count)return relative_slot;
    if(relative_slot==player_fixed_count)return player_fixed_count; // shared moon
    if(relative_slot>=shared_fixed_count&&relative_slot<shared_fixed_count+player_fixed_count)return relative_slot-shared_fixed_count;
    if(relative_slot>=shared_fixed_count+player_fixed_count&&relative_slot<fixed_count)return relative_slot-shared_fixed_count-player_fixed_count;
    return -1;
}

constexpr i32 relative_slot_for_global(i32 global_slot) noexcept {
    return global_slot>=0&&global_slot<shared_fixed_count?global_slot:-1;
}

struct FixedSlotAddress {
    i32 storage_index=-1;
    i32 relative_slot=-1;
    i32 local_slot=-1;
    i32 seat=-1;
    constexpr explicit operator bool()const noexcept{return storage_index>=fixed_pool_begin&&storage_index<dummy_index&&relative_slot>=0&&relative_slot<fixed_count;}
};

constexpr FixedSlotAddress fixed_for_player(i32 seat,i32 local_slot) noexcept {
    const i32 relative=player_relative_slot(seat,local_slot);
    return relative<0?FixedSlotAddress{}:FixedSlotAddress{fixed_pool_begin+relative,relative,local_slot,seat};
}

constexpr FixedSlotAddress fixed_for_global(i32 global_slot) noexcept {
    const i32 relative=relative_slot_for_global(global_slot);
    return relative<0?FixedSlotAddress{}:FixedSlotAddress{fixed_pool_begin+relative,relative,global_slot,seat_for_relative_slot(relative)};
}

constexpr FixedSlotAddress fixed_for_storage_index(i32 storage_index) noexcept {
    const i32 relative=storage_index-fixed_pool_begin;
    if(relative<0||relative>=fixed_count)return {};
    return {storage_index,relative,local_slot_for_relative_slot(relative),seat_for_relative_slot(relative)};
}

} // namespace th08::player_effect_slots
