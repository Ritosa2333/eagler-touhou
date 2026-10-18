#pragma once
#include "enemy_state.hpp"
namespace th20::source::gameplay {
class EnemyDropServices {
public:
    virtual ~EnemyDropServices()=default;
    virtual float random_angle()=0; //4297d0, shared RNG0 signed_unit*pi
    virtual float random_unit()=0; //429830, shared RNG0
    virtual void spawn(int player,int type,const sprite::Vec3&)=0; //4c3c90, original default arguments
};
void reset_enemy_drop_counts(EnemyPatternState&) noexcept; //497270, preserves base/type/sizes/second array
void emit_enemy_pattern_items(EnemyPatternState&,const sprite::Vec3&,EnemyDropServices&); //4a5350
void emit_enemy_drop(EnemyPatternState&,const sprite::Vec3&,bool bomb_mark,EnemyDropServices&); //48bf10
EnemyDropServices& enemy_drop_services();
inline void emit_enemy_drop(EnemyPatternState& p,const sprite::Vec3& position,bool bomb_mark){emit_enemy_drop(p,position,bomb_mark,enemy_drop_services());}
}
