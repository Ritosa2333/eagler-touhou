#pragma once
#include "overlay.hpp"
namespace th20::source::overlay {
class FrameEnvironment {
public:
 virtual ~FrameEnvironment()=default;
 virtual bool game_requests_end()=0; //534300 bit6, false when actual game pointer null
 virtual bool game_active()=0; //464350, actual scheduler enabled bit
 virtual bool hud_present()=0;
 virtual bool boss_collecting()=0; //478160
 virtual sprite::Vec3 player_position()=0;
 virtual bool animation_exists(std::uint32_t&)=0; //485a10
 virtual void animation_position(std::uint32_t&,const sprite::Vec3&)=0; //4645e0
 virtual std::uint32_t spawn_aura(sprite::AnimationFile&,const sprite::Vec3&)=0; //4790e0,script29
 virtual void interrupt(std::uint32_t&,int)=0; //44ef90
 virtual void delete_animation(std::uint32_t&)=0; //44fcd0
 virtual void phase_visuals(WeaponStoneInf&,bool ending)=0; //5348d0/534ab0
 virtual void clear_bullets(const sprite::Vec3&,float)=0; //47cf60
 virtual void clear_lasers(const sprite::Vec3&,float)=0; //4caad0
 virtual void sound(int,float)=0; //426eb0
};
FrameEnvironment& frame_environment();
int update(WeaponStoneInf&,FrameEnvironment&); //532b20
void start_phase(WeaponStoneInf&,FrameEnvironment&); //534d00
void start_phase(WeaponStoneInf&);
void phase_visuals(WeaponStoneInf&,bool ending); //5348d0/534ab0
void update_mesh(WeaponStoneInf&); //533970
}
