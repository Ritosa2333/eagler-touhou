#pragma once
#include "Types.hpp"
namespace th15 {
enum class BulletFrameMode:u32 {Skipped=0,MotionAndVisual=1,VisualOnly=2};
inline bool battle_enemy_frame(u32 flags)noexcept{return !(flags&0x407u);}
inline BulletFrameMode battle_bullet_frame(u32 flags)noexcept{if(flags&5)return BulletFrameMode::Skipped;return flags&0x400?BulletFrameMode::VisualOnly:BulletFrameMode::MotionAndVisual;}
inline bool battle_animation_frame(u32 flags)noexcept{return !(flags&5)||!(flags&2);}
}
