#pragma once
#include "enemy_state.hpp"
namespace th20::source::gameplay {
// Original EnemyState+18c health/accounting subobject, exact28-byte storage.
std::int32_t apply_enemy_damage(EnemyAuxiliary28&,std::int32_t amount) noexcept; //4a3f80
void record_enemy_damage(EnemyAuxiliary28&,std::int32_t amount) noexcept;       //4aa050
bool enemy_health_positive(const EnemyAuxiliary28&) noexcept;                 //4ab240
bool enemy_health_forced_end(const EnemyAuxiliary28&) noexcept;               //4ab290
bool boss_damage_suppressed(const void* hud) noexcept;                        //478160 +1bc
void set_enemy_hit_color(sprite::Animation&,std::uint32_t) noexcept;           //4ab610 +494
void extend_enemy_hit_feedback(void* feedback,std::int32_t delta,std::int32_t limit,const float* rate); //4aa000, Player+2244
}
