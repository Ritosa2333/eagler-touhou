#pragma once
#include "ShotResource.hpp"
#include "AnmLayout.hpp"
#include "AttackAreas.hpp"
#include "PoolCheckpoint.hpp"
namespace th09 {
struct PlayerShot {
    AnmVm animation;Vec3 position;std::array<Vec3,32> trail;Vec3 hitbox;Vec2 velocity,offset;float speed=0,angle=0;Timer time;
    i16 damage=0,state=0,type=0,reserved_type=0,option=0,trail_length=0,animation_index=0;u8 hit_flash=0,reserved=0;i16 hits=0;
    const ShotDefinition* definition=nullptr;
};
struct PlayerShotActions {
    virtual ~PlayerShotActions()=default;
    virtual void start_animation(AnmVm&,i32 script)=0;
    virtual bool advance_animation(AnmVm&)=0;
    virtual void draw_animation(AnmVm&,bool fading)=0;
    virtual void play_positioned_sound(i32,float)=0;
    virtual void play_sound(i32,i32)=0;
    virtual void effect(i32,const Vec3&,i32 slot)=0;
    virtual bool effect_active(i32 slot)=0;
    virtual Vec3 effect_position(i32 slot)=0;
};
class PlayerShots {
    PlayerShotActions& actions;
    void initialize(PlayerShot&,const ShotDefinition&);
    bool create(PlayerShot&,const ShotDefinition&,i32 frame);
    bool update_special(PlayerShot&,const FrameTiming&);
    void draw_special(PlayerShot&,const PlayfieldGeometry&);
public:
    static constexpr u32 capacity=128;
    PoolCheckpoint* checkpoint=nullptr;
    void before_write(PlayerShot& shot){if(checkpoint)checkpoint->BeforePlayerShot(*this,&shot-shots.data());}
    std::array<PlayerShot,capacity> shots;
    Vec3 player_position;std::array<Vec3,4> option_positions;Vec3 target;
    Vec2 player_scale{1,1};i32 side=0;Timer beam_time{0,0,0};PlayerShot* beam=nullptr;
    AttackAreas areas;
    explicit PlayerShots(PlayerShotActions& a):actions(a){}
    void fire(const ShotResource&,u32 set,i32 frame);
    void deactivate_all();
    void update(const FrameTiming&,u32 field_flags);
    void draw(const PlayfieldGeometry&,bool fading);
    i32 hit(const Vec3&,const Vec3& extent,const Timer& protection,i32& direct_damage,u32* token,i32& special_damage);
};
}
