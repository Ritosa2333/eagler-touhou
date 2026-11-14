#pragma once
#include "EnemyAnimations.hpp"
#include <functional>
namespace th11 {
// Resource slots and scripts come from the 0xcc-stride retail stage descriptors.
// These are separate from TH11's directly bound spell-background VMs.
bool spell_th14_create_visuals(AnmManager&,EnemyAnimations&,AnmResource& ascii,AnmResource& text,AnmResource& effect,
    i32 stage,i32 section,i32 id,i32 timeout,Vec3 boss,u32* titles,u32& circle,u32& background,
    const std::function<bool(u32)>& title_delivery,const std::function<bool(i32)>& sound_delivery);
}
