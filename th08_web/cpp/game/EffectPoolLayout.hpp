#pragma once

#include "Types.hpp"

namespace th08::effect_pool_layout {

inline constexpr i32 active_pool_begin=0;
inline constexpr i32 active_pool_end=512;
inline constexpr i32 overlay_pool_begin=512;
inline constexpr i32 fixed_pool_begin=640;
inline constexpr i32 shared_fixed_count=13;
inline constexpr i32 player_fixed_count=12;

#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
inline constexpr i32 fixed_count=shared_fixed_count+2*player_fixed_count;
#else
inline constexpr i32 fixed_count=shared_fixed_count;
#endif

inline constexpr i32 dummy_index=fixed_pool_begin+fixed_count;
inline constexpr i32 object_count=dummy_index+1;
inline constexpr i32 active_object_count=dummy_index;

static_assert(fixed_pool_begin+shared_fixed_count==653);
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
static_assert(object_count==678&&dummy_index==677);
#else
static_assert(object_count==654&&dummy_index==653);
#endif

} // namespace th08::effect_pool_layout
