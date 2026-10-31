#pragma once
#include "BulletScene.hpp"
#include "LaserRequests.hpp"
#include "EffectManager.hpp"
#include "LaserManager.hpp"
#include "LaserVisual.hpp"
#include "LaserContact.hpp"
#include "LaserCancellation.hpp"
#include "LaserProgram.hpp"
#include "StationaryLaserProgram.hpp"
#include "CurveProgram.hpp"
#include "CurveCancellation.hpp"
namespace th17 {
struct LaserSceneHost:BulletSceneHost,LaserProgramHost {virtual const Vec3* stationary_laser_anchor()const noexcept{return nullptr;}};
// Owns concrete lasers and connects their embedded animations to the shared
// player predicate, cancellation rewards, effects and typed game services.
class LaserScene {
    struct MovingLaser;
    struct StationaryLaser;
    struct CurveLaser;
    struct SegmentedLaser;
    LaserSceneHost& host;AnmManager& animations;EffectManager& effects;
    Rng& game_random;Rng& visual_random;BulletCancellationRewards& rewards;i32 resource;BulletScene* bullets=nullptr;
public:
    LaserManager manager;std::string error;float rate=1;
    LaserScene(LaserSceneHost&,AnmManager&,EffectManager&,Rng& game,Rng& visual,BulletCancellationRewards&,i32 resource);
    ~LaserScene();
    u32 create_moving(const MovingLaserRequest&);
    u32 create_stationary(const StationaryLaserRequest&);
    u32 create_curved(const CurveLaserRequest&);
    u32 create_segmented(const SegmentedLaserRequest&);
    void attach_bullets(BulletScene& scene)noexcept{bullets=&scene;}
    bool set_position(u32 id,const Vec3&);
    void offset_origins(float without_identifier,float with_identifier);
    Vec3* origin_at(u32 index)noexcept;
    bool set_script_vector(u32 id,const Vec3&);
    bool set_speed(u32 id,float);bool set_width(u32 id,float);bool set_angle(u32 id,float);
    bool set_script_angular_velocity(u32 id,float);bool cancel_identifier(u32 id);
    bool update(float rate,u32 game_flags=0);
    i32 cancel_circle(const Vec3&,float radius,i32 reward,bool honor_protection);
    i32 cancel_rectangle(const Vec3&,const Vec2&,float angle,i32 reward);
    bool cancel_all(i32 reward,bool honor_protection);
    bool draw(AnmRenderer&);
    MovingLaserMotion* moving_motion(u32 id)noexcept;
    LaserVisual* moving_visual(u32 id)noexcept;
    LaserContact* moving_contact(u32 id)noexcept;
    LaserProgram* moving_program(u32 id)noexcept;
    StationaryLaserMotion* stationary_motion(u32 id)noexcept;
    LaserVisual* stationary_visual(u32 id)noexcept;
    LaserContact* stationary_contact(u32 id)noexcept;
    StationaryLaserProgram* stationary_program(u32 id)noexcept;
    CurveLaserMotion* curve_motion(u32 id)noexcept;
    LaserVisual* curve_visual(u32 id)noexcept;
    LaserContact* curve_contact(u32 id)noexcept;
    CurveProgram* curve_program(u32 id)noexcept;
    MovingLaserMotion* segmented_motion(u32 id)noexcept;
};
}
