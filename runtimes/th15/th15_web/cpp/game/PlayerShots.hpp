#pragma once
#include "PlayerMotion.hpp"
#include "DamageSources.hpp"
#include "ShtResource.hpp"
#include "AnmManager.hpp"
#include "EnemyState.hpp"
#include <functional>
namespace th15 {
struct PlayerShot {
    u32 flags=0; i32 index=0;u32 animation=0;Timer age,behavior,auxiliary;MotionState motion;
    i32 state=0,target=0,contact_this_frame=0,contact_effect=0,damage=0;Vec2 hitbox{};i32 reserved=0;
    u32 specification=0;i32 damage_source=0;Vec3 target_position{};
};
static_assert(sizeof(PlayerShot)==0xc0);
struct PlayerShotContext {
    PlayerMotion& player;std::array<float,8> option_angles{};std::array<i32,9> laser_power{};
    i32 power=0,power_step=40,shoot_frame=0;bool enemy_scene=true,bomb_active=false;EnemyWorldState* enemies=nullptr;Vec2 screen_origin{};Vec3 background_delta{};
};
class PlayerShots {
    const ShtResource& resource;PlayerShotContext& context;DamageSources& damage;AnmManager& animations;Rng& random;Rng& visual_random;
    i32 player_resource,effect_resource;
    bool initialize(PlayerShot&,u32,const Vec3&);
    bool update_behavior(PlayerShot&,const ShotSpec&);
    bool update_one(PlayerShot&);
    bool hit_rewards(const Vec3&);
    i32 contact(PlayerShot&,const ShotSpec&,const DamageQuery&);
    i32 ordinary_hit(PlayerShot&);
    i32 laser_hit(PlayerShot&,const DamageQuery&);
public:
    enum class SoundAction:i32 {play,stop,pan};
    std::array<PlayerShot,256> shots{};std::string error;float rate=1;
    std::function<bool(i32,float,SoundAction)> sound;
    std::function<bool(i32,const Vec3&,float,float)> item;
    std::function<void(u32)> track_effect;
    std::array<i32,3> reward_counters{};
    PlayerShots(const ShtResource& s,PlayerShotContext& p,DamageSources& d,AnmManager& a,Rng& r,Rng& visual,i32 player_id,i32 effects_id):resource(s),context(p),damage(d),animations(a),random(r),visual_random(visual),player_resource(player_id),effect_resource(effects_id){for(i32 i=0;i<256;i++)shots[i].index=i;damage.hit_callback=[this](DamageSource& source,const DamageQuery& q){return hit(source,q);};}
    ~PlayerShots(){damage.hit_callback={};}
    i32 hit(DamageSource&,const DamageQuery&);
    const ShotSpec* specification(u32 id)const;
    void set_rate(float value){rate=value;animations.rate=value;}
    bool fire(i32 shot_frame,i32 sound_frame);
    bool update();
    // Preserve first-free shot slots and the separate rotating damage pool.
    i32 spawn(u32 id,const Vec3&);
};
}
