#pragma once
#include "EnemyAnimations.hpp"
#include <functional>
namespace th11 {
bool spell_th16_create_visuals(AnmManager&,EnemyAnimations&,AnmResource& ascii,AnmResource& text,AnmResource& effect,
    i32 stage,i32 section,i32 effect_variant,i32 timeout,Vec3 boss,u32* titles,u32& circle,u32& background,u32& flags,
    const std::function<bool(u32)>& title_delivery,const std::function<bool(i32)>& sound_delivery);
}
