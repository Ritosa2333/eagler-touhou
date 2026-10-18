#pragma once
#include "ShtResource.hpp"
#include "ShotSchedule.hpp"
#include "AnmManager.hpp"
#include "EnemyState.hpp"
#include "DamageArea.hpp"
#include "GameEconomy.hpp"
#include <array>
namespace th11 {
struct ShotState {
    Timer timer;Movement movement;
    i32 state=0;u32 animation=0,extra_animation=0;
    Enemy* target=nullptr;i32 hit_this_frame=0,hit_last_frame=0,damage=0;
    float acceleration=0;const ShotSpec* spec=nullptr;
};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(ShotState)==0x6c);
#endif
struct ShotOption {i32 x=0,y=0;float angle=0;u32 animation=0;};
struct ShotPlayer {
    Vec3 position;std::array<ShotOption,8> options{};
    i32 option_count=0,state=1,warp_state=0,power=0,power_step=20,character=0,subtype=0,weapon_mode=0;
    u32 held=0,focused=0,bomb_active=0,enemies_present=0,special_active=0;
};
struct ShotWorld {
    virtual ~ShotWorld()=default;
    virtual bool sound(i32,float){return false;}
    virtual bool special_damage(const Vec3&,const Vec2&,i32&){return false;}
    // The owner supplies the live enemy list. Targets must be invalidated before deletion.
    EnemyLink* enemies=nullptr;
};
class ShotManager {
public:
    ShotManager(ShtResource&,AnmResource&,AnmManager&,ShotWorld&,u16 file=7);
    ShotPlayer player;ShotSchedule schedule;
    std::array<u32,9> laser_active{};
    DamageAreas damage_areas;
    Enemy* locked_target=nullptr;u8 target_locked=0;
    i32 last_error=0;
    i32 spawn(const ShotSpec&,i32 firing_frame,const Vec3& origin);
    // Native laser-cut record 0x4a3a50 and callbacks 435030/435080.
    i32 spawn_converted_laser(Vec3,float);
    bool fire_frame(i32);
    bool fire();
    bool update();
    bool damage(const Vec3&,const Vec2&,bool frame_advanced,GameEconomy&,i32& result);
    void invalidate_target(Enemy*)noexcept;
    ShotState& at(u32 index)noexcept{return shots[index];}
private:
    ShtResource& resource;AnmResource& animation_resource;AnmManager& animations;ShotWorld& world;u16 file_id;
    std::array<ShotState,256> shots{};
    ShotSpec converted_laser_spec{30,0,20,{0,0},{24,24},0,12,0,0,4,5,24,ShotSpawn::ConvertedLaser,ShotUpdate::ConvertedLaser,0,0};
    bool on_spawn(ShotState&,i32);
    bool on_update(ShotState&);
    Enemy* nearest(Vec3,float)const noexcept;
    void interrupt(u32,i16);void erase(u32);
};
}
