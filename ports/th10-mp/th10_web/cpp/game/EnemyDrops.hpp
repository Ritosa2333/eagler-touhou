#pragma once
#include "GameEconomy.hpp"
#include "Rng.hpp"
namespace th10 {
struct ItemDropEnvironment {
    Rng* drop_rng;
    virtual void spawn_item(const Vec3& position,i32 kind,i32 color,float angle,float speed)=0;
};
struct EnemyDropEnvironment : ItemDropEnvironment {
    GameEconomy* economy;
    const float* timer_rate;
    virtual void play_sound(i32 sound,float horizontal_position)=0;
    virtual void spawn_death_animation(i32 file,i32 script,const Vec3& position)=0;
};
}
