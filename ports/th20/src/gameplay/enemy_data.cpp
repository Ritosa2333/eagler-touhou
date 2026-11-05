#include "enemy.hpp"
#include <cstring>
namespace th20::source::gameplay {
std::uint32_t previous_enemy_generation=0,current_enemy_generation=0;
void construct_enemy_data(EnemyData& data) noexcept {std::memset(&data,0,sizeof(data));}
void reset_enemy_counters(EnemyData& data) noexcept {std::memset(data.fields_00,0,sizeof(data.fields_00));}
std::uint32_t advance_enemy_generation(std::int32_t player) noexcept {
    const auto previous=current_enemy_generation;previous_enemy_generation=previous;
    current_enemy_generation=(current_enemy_generation+1)&0xffffu;
    if(!current_enemy_generation)current_enemy_generation=1;
    current_enemy_generation|=static_cast<std::uint32_t>(player)<<16;
    return previous;
}
}
