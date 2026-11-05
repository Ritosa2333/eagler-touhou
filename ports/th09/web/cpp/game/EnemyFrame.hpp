#pragma once
#include "CharacterCapture.hpp"
#include <array>
namespace th09 {
// State shared with the player controller. Targets are native game objects;
// scene adapters never need the executable's structure offsets.
struct EnemyPlayerState {
    i32 state=0,focus=0;CaptureArea capture;
    Vec3 nearest_position;EclVm* target=nullptr;Timer protection{0,0,0};
};
struct EnemyFrameSettings {
    u32 game_flags=0;i32 timeline_blocked=0,dialogue=-1;
    std::array<u8,256> patterns{};
};
struct EnemyFrameActions {
    virtual ~EnemyFrameActions()=default;
    virtual void attack_position(i32 side,const Vec3&)=0;
    virtual bool advance_animation(AnmVm&)=0;
    virtual void body_collision(const Vec3&,const Vec3& extent)=0;
    virtual bool emit_capture_bullets(const BulletEmission&)=0;
    virtual i32 shot_damage(const Vec3&,const Vec3& extent,i32& primary,i32& token,i32& secondary)=0;
    virtual void add_score(i32)=0;
    virtual void effect(i32,const Vec3&)=0;
    virtual void play_positioned_sound(i32,float x)=0;
    virtual void sound_pan(i32,i32)=0;
    virtual void boss_position(i32,const Vec3&)=0;
    virtual void boss_state(i32,u32)=0;
    virtual void release_attached_effects(EclVm&)=0;
    virtual void update_attached_effects(EclVm&)=0;
    virtual bool enemy_death(EclVm&,i32 kill_source)=0;
    virtual void finish_match()=0;
};
}
