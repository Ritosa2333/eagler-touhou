#pragma once
#include "LaserManager.hpp"
#include "AnmEnvironment.hpp"
namespace th10 {
enum class LaserFeature : u32 { Acceleration,VectorAcceleration,AngularAcceleration,Turn,TurnRepeated,SpeedChange,Reflect,LengthChange,WidthChange,Blend };
struct LaserBehaviorEnvironment {
    const float* rate;
    const i32* sprite_scripts;
    const Vec3* player_position;
    const Vec3* boss_position;
    AnmFile* animation_file;
    AnmFile* bullet_file;
    AnmEnvironment* animations;
    u32* started_animations;
    virtual void run_commands(StraightLaser& laser)=0;
    virtual void update_feature(StraightLaser& laser,LaserFeature feature)=0;
    virtual i32 collide_player(const Vec3& origin,float angle,float width,float length)=0;
    virtual void cancel_rectangle(EnemyLaser& laser,const Vec3& center,const Vec3& size,i32 convert)=0;
    virtual void graze_effect(const Vec3& position)=0;
    virtual void play_sound(i32 sound,float horizontal_position)=0;
    virtual void play_turn_sound(i32 sound)=0;
    virtual void spawn_straight(const StraightLaserParameters& parameters)=0;
    virtual void cancel_effect(i32 script,const Vec3& position)=0;
    virtual void spawn_faith(const Vec3& position)=0;
    virtual void submit(AnmVm& animation)=0;
    virtual bool presentation(const EnemyLaser& laser,Vec3& position,float& angle,float& length,float& width)=0;
};
// Touching a playfield edge is outside; unordered comparisons remain inside.
bool laser_outside_playfield(const Vec3& point,float half_width,float half_height) noexcept;
void initialize_embedded_animation(AnmFile& file,AnmVm& vm,i32 script,AnmEnvironment& environment,u32& started);
}
