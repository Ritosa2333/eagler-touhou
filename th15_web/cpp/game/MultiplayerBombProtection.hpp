#pragma once
#include "Types.hpp"
namespace th15 {
// Share native bomb protection, not Reisen's persistent shield ownership.
constexpr i32 multiplayer_bomb_protection(i32 character,bool active,bool shield_broken)noexcept{
 return !active?0:character==3?(shield_broken?30:0):40;
}
constexpr i32 multiplayer_bomb_start_protection(i32 character)noexcept{return character==3?30:120;}
constexpr i32 multiplayer_protection_timer(i32 current,i32 protection)noexcept{return current<protection?protection:current;}
}
