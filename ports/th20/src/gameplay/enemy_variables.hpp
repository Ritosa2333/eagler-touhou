#pragma once
#include "enemy.hpp"
namespace th20::source::gameplay {
struct EnemyState;
extern std::uint32_t enemy_script_globals[4];                 //BSS5c49d8/dc/e0/e4
void* find_enemy_in_list(scheduler::List&,std::uint32_t identifier); //498a80 loop
void* selected_enemy(void* controller,unsigned slot);         //485660/4aaac0, lookup always in player0's controller
sprite::Vec3 enemy_position(const void* entity) noexcept;      //47a2c0, Enemy+198
std::uint32_t enemy_identifier(const void* entity) noexcept;   //498f90, Enemy+88
bool enemy_state_excluded(const EnemyState&) noexcept;        //47a370
bool enemy_excluded(const void* entity) noexcept;              //47a3e0
void set_enemy_bomb_mark(void* entity,std::uint32_t) noexcept; //478260, Enemy+358 bit5
// The strict radius test excludes boundary equality and preserves list order
// for ties. Position comparison uses x/y only, including original IEEE cases.
std::uint32_t nearest_enemy_identifier(scheduler::List&,const sprite::Vec2&,float radius); //4aac00
inline std::uint32_t nearest_enemy_identifier(EnemyController& owner,const sprite::Vec2& origin,float radius) {
    return nearest_enemy_identifier(owner.enemies,origin,radius);
}
// Original vtable+0c/+14 return nullptr for unsupported variables. The owner
// adapter must not invent a destination for those invalid script operands.
std::uint32_t* enemy_integer_destination(void* entity,std::int32_t variable); //498600
std::uint32_t* enemy_float_destination(void* entity,std::int32_t variable);   //498210, raw float bits
}
