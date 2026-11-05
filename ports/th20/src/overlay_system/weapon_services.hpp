#pragma once
#include "weapon.hpp"
#include "../effect_system/effect.hpp"
#include "../damage_regions/regions.hpp"
#include <functional>
namespace th20::source::overlay {
class WeaponServices {
public:
 virtual ~WeaponServices()=default;
 virtual void delete_animation(std::uint32_t&)=0;
 virtual void interrupt(std::uint32_t&,int)=0;
 virtual void show_animation(std::uint32_t&,bool)=0; //44fa30/450120
 virtual void animation_position(std::uint32_t&,const sprite::Vec3&)=0;
 virtual std::uint32_t spawn_player_animation(const char* name,int script,const sprite::Vec3&)=0;
 virtual void clear_bullet_rectangle(const sprite::Vec3&,const sprite::Vec3&)=0;
 virtual void clear_laser_rectangle(const sprite::Vec3&,const sprite::Vec3&)=0;
 virtual sprite::Animation& animation(std::uint32_t&)=0; //44ced0, includes original fallback
 virtual std::uint32_t spawn_effect(int,const effects::Parameters&)=0;
 virtual void pause_animation(std::uint32_t&,bool)=0; //44f130/44f280
 virtual void spawn_item(int,const sprite::Vec3&,float angle,float speed)=0; //4c3c90
 virtual std::uint32_t create_damage_circle(const sprite::Vec3&,float radius,float growth,int frames,int damage)=0;
 virtual damage::Region& damage(std::uint32_t&)=0;
 virtual void retire_damage(std::uint32_t&)=0;
 virtual void damage_position(std::uint32_t&,const sprite::Vec3&)=0;
 virtual int cancel_filtered(const sprite::Vec3&,float,std::function<int(const sprite::Vec3&)>)=0; //47d240
 virtual int cancel_counted(const sprite::Vec3&,float)=0; //47cf60, retain exact EAX count
 virtual void fire_shots_at_position(int pattern,int frame,int secondary,const sprite::Vec3&)=0; //505e40
};
WeaponServices& weapon_services();
}
