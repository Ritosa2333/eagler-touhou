#pragma once
#include "BulletFormation.hpp"
#include "Timer.hpp"
#include "Interpolation.hpp"
#include "PlayerCollision.hpp"
#include "BulletAppearance.hpp"
#include <string>
namespace th15 {
struct EnemySpawnRequest;struct MovingLaserRequest;struct StationaryLaserRequest;
struct BulletAcceleration {Timer timer{};float speed=0,angle=0;Vec3 vector{};i32 duration=0;};
struct BulletAngularAcceleration {Timer timer{};float speed=0,angle=0;i32 duration=0;};
struct BulletReflection {float speed=0;Vec2 bounds{384,448};i32 count=0,limit=0;u32 sides=0;};
struct BulletWrapping {i32 count=0,limit=0;u32 sides=0;};
struct BulletDrift {Timer timer{};Vec3 velocity{};i32 duration=0;float speed=0,angle=0;};
struct BulletPositionCurve {Timer timer{};float speed=0;Vec3 target{};i32 duration=0;u32 mode=0;Vec3Interpolation interpolation{};};
struct BulletTurn {Timer timer{};float speed=0,angle=0;i32 duration=0,limit=0,count=0,mode=0,extra=0;};
struct BulletSoundHost {virtual ~BulletSoundHost()=default;virtual void play(i32 sound)=0;};
struct BulletAnimationHost {
    virtual ~BulletAnimationHost()=default;virtual bool interrupt(i32 id)=0;
    virtual bool bind_appearance(i32 script,i32 overlay)=0;
    virtual bool spawn_animation()=0;
    virtual bool emit_children(const BulletShooter& shooter)=0;
    virtual bool cancel_bullet(i32 kind)=0;
    virtual bool spawn_enemy(const EnemySpawnRequest&){return false;}
    virtual bool create_laser(const MovingLaserRequest&){return false;}
    virtual bool create_laser(const StationaryLaserRequest&){return false;}
};
struct BulletState;
struct BulletFrameHost:BulletSoundHost,BulletAnimationHost {
    virtual i32 contact(BulletState& bullet,bool graze_only)=0;
    virtual bool animation_finished(bool overlay)=0;
    virtual bool sprite_dimensions(Vec2&)const noexcept{return false;}
    virtual void cancellation_effect(i32 script,const Vec3& position,const Vec3& velocity)=0;
    virtual void recycle()=0;
};
struct CancellationRewardHost {virtual ~CancellationRewardHost()=default;virtual void item(i32 type,const Vec3& position,float angle,float speed)=0;};
struct BulletContactHost:PlayerDamageHost,CancellationRewardHost {
    virtual void play(i32 sound)=0;
    virtual bool hit_animation()=0;
    virtual bool overlay_interrupt(i32 id)=0;
    virtual void cancellation_effect(i32 script,const Vec3& position,const Vec3& velocity)=0;
    virtual void graze_spark(const Vec3& position)=0;
    virtual void graze()=0;
    virtual void graze_resonance(float value)=0;
    virtual void item(i32 type,const Vec3& position,float angle,float speed)=0;
};
struct BulletFrameContext {
    float rate=1;Vec2 sprite_size{},reflection_bounds{};Vec3 player{};
    bool sprite_available=true;
};
struct BulletCancellationRewards {i32 count=0;bool spell_active=false;};
void cancellation_reward(const Vec3&,i32 kind,Rng&,BulletCancellationRewards&,CancellationRewardHost&);
enum class BulletFrameResult:i32 { recycled=-1,alive=0,error=-2 };
struct BulletState {
    BulletInitialMotion motion{};u32 transform_flags=0;Timer boost{};
    BulletAcceleration acceleration{},approach{};BulletAngularAcceleration angular{};
    BulletReflection reflection{};BulletWrapping wrapping{};BulletDrift drift{};
    BulletPositionCurve position_curve{};i32 transform_sound=-1;
    BulletTurn turn{};BulletInitialMotion saved{};
    std::array<BulletTransform,18> transforms{};i32 transform_index=0;
    Timer wait{},invulnerability{},freeze{};i32 wait_outside=0,boost_stage=0,step_limit=0;
    i32 collision_delay=0,cancel_item=0,phase=1;u32 flags=0,visual_flags=0;
    ScalarInterpolation scale_curve{};float scale=1;
    Timer lifetime{},spawn{};i32 offscreen_grace=0,cancellation_script=-1;
    bool spawn_animation_finished=false,overlay_active=false;
    Vec2 hitbox{};Timer graze_flash{},graze_duration{};i32 graze_interval=1,hit_interrupt=0;
    Vec3 visual_jitter{};u32 graze_color=0;
    i16 sprite_type=0,color=0;u32 initial_transform_flags=0;
    bool activate(const Vec3& player,Rng& random,BulletSoundHost* sounds,BulletAnimationHost* animations,std::string& error);
    BulletFrameResult update(const BulletFrameContext& context,Rng& random,BulletFrameHost& host,std::string& error);
    PlayerContact collide(const PlayerCollision& player,bool graze_only,float rate,Rng& random,Rng& visual_random,BulletContactHost& host,std::string& error);
    bool appearance(i32 type,i32 color,BulletAnimationHost& host,bool replacement,std::string& error);
    bool initialize(const BulletShooter&,i32 column,i32 row,float aim,const Vec3& player,float minimum_distance_squared,Rng&,BulletFrameHost&,std::string& error);
    bool cancel(i32 item_kind,float rate,Rng&,BulletCancellationRewards&,BulletFrameHost&,BulletContactHost&,std::string& error);
    void velocity(float speed,float angle)noexcept;
    bool update_boost(float rate)noexcept;
    bool update_acceleration(float rate,bool maintain_speed=false)noexcept;
    bool update_angular(float rate)noexcept;
    bool update_drift(float rate)noexcept;
    bool reflect_edge(u32 edge,const Vec2& bounds_override)noexcept;
    bool update_reflection(const Vec2& bounds_override,BulletSoundHost* sounds=nullptr);
    bool update_wrap(const Vec2& sprite_size,BulletSoundHost* sounds=nullptr);
    bool update_position_curve(float rate)noexcept;
    bool update_turn(float rate,const Vec3& player,BulletSoundHost* sounds=nullptr);
    bool update_wait(float rate,const Vec2& sprite_size)noexcept;
};
}
