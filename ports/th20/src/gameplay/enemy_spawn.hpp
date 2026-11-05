#pragma once
#include "enemy_state.hpp"
#include "enemy_frame.hpp"
namespace th20::source::gameplay {
struct SpawnParameters {                                    //47bb30, real84-byte record
    sprite::Vec3 position;
    std::int32_t field_0c,field_10,health;
    std::uint32_t flags_18,flags_1c,variables[12],field_50;
};
static_assert(sizeof(SpawnParameters)==0x54&&offsetof(SpawnParameters,variables)==0x20);
void construct_spawn_parameters(SpawnParameters&) noexcept; //47bb30
int apply_enemy_spawn(void* entity,const SpawnParameters&,game_session::Session&,EnemyFrameServices&); //4a89c0
class Enemy;
Enemy* spawn_enemy(EnemyController&,const char*,const SpawnParameters&,Enemy* parent=nullptr); //4a8920
}
