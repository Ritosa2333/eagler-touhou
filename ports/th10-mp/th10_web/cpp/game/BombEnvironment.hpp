#pragma once
#include "Bomb.hpp"
#include "PlayerLifecycle.hpp"
namespace th10 {
struct BombEnvironment {
    PlayerLifecycleEnvironment* shared;
    const Vec3* player_position;
    const i32* spell_number;
    virtual void play_sound(i32 sound,float horizontal_position)=0;
    virtual void update_power(i32 level,i32 percent)=0;
    virtual void cancel_bullets(const Vec3& position,float radius,bool convert_items,bool respect_protection)=0;
    virtual void cancel_lasers(const Vec3& position,float radius,bool convert_items)=0;
};
}
