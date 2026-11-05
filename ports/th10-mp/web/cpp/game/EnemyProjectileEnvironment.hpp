#pragma once
#include "Enemy.hpp"
#include "LaserManager.hpp"
namespace th10 {
struct EnemyProjectileEnvironment {
    const Vec3* player_position;
    const i32 *rank,*difficulty;
    virtual void fire(const BulletEmitter& emitter)=0;
    virtual u32 spawn_straight(const StraightLaserParameters& parameters)=0;
    virtual u32 spawn_timed(const TimedLaserParameters& parameters)=0;
    virtual EnemyLaser* laser(u32 id)=0;
    virtual void cancel_rectangle(bool convert_items)=0;
    virtual void cancel_circle(const Vec3& center,float radius,bool convert_items)=0;
};
}
