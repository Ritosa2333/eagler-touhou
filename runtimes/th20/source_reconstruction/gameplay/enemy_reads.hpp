#pragma once
#include "enemy_entity.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::gameplay {
struct EnemyReadEnvironment {
    game_session::Session& session;
    state::Random& random;
    const std::int32_t& restart_mode;
    const std::uint32_t& new_game_state;                    //Graphics+b18, unique5c5858
    const std::int32_t& replay_selection;
    sprite::Animation* (*animation)(std::uint32_t&);        //44ced0, preserves handle clearing
};
std::int32_t read_enemy_integer(Enemy&,std::int32_t,EnemyReadEnvironment&); //49abc0
float read_enemy_float(Enemy&,std::int32_t,EnemyReadEnvironment&);         //4995d0
}
