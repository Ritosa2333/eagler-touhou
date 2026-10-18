#include "enemy_damage_helpers.hpp"
#include <cstring>
namespace th20::source::gameplay {
std::int32_t apply_enemy_damage(EnemyAuxiliary28& health,std::int32_t amount) noexcept {
    health.words[5]+=static_cast<std::uint32_t>(amount);
    if(!(health.words[6]&1u))health.words[0]-=static_cast<std::uint32_t>(amount);
    else {
        health.words[3]-=static_cast<std::uint32_t>(amount);
        const auto scaled=recovered::signed_bits(health.words[3]-health.words[4]*7u)/7;
        health.words[0]=static_cast<std::uint32_t>(scaled)+health.words[4];
    }
    return recovered::signed_bits(health.words[0]);
}
void record_enemy_damage(EnemyAuxiliary28& health,std::int32_t amount) noexcept {health.words[5]+=static_cast<std::uint32_t>(amount);}
bool enemy_health_positive(const EnemyAuxiliary28& health) noexcept{return recovered::signed_bits(health.words[0])>0;}
bool enemy_health_forced_end(const EnemyAuxiliary28& health) noexcept{return (health.words[6]&2u)!=0;}
bool boss_damage_suppressed(const void* hud) noexcept {std::uint32_t value;std::memcpy(&value,static_cast<const std::uint8_t*>(hud)+0x1bc,4);return value!=0;}
void set_enemy_hit_color(sprite::Animation& animation,std::uint32_t color) noexcept {animation.base.field_494=color;}
void extend_enemy_hit_feedback(void* feedback,std::int32_t delta,std::int32_t limit,const float* rate) {
    std::int32_t enabled;std::memcpy(&enabled,static_cast<std::uint8_t*>(feedback)+0x38,4);if(enabled<=0)return;
    auto& timer=*static_cast<recovered::Timer*>(feedback);
    const auto prospective=recovered::signed_bits(static_cast<std::uint32_t>(timer.current)+static_cast<std::uint32_t>(delta));
    if(prospective<limit)recovered::timer_add(timer,recovered::int_float(delta),rate);
    else recovered::timer_set(timer,limit);
}
}
