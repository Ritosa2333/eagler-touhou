#include "enemy_state.hpp"
#include <cstring>
namespace th20::source::gameplay {
EnemyMovementRecord::EnemyMovementRecord() noexcept {std::memset(this,0,sizeof(*this));}
EnemyState::EnemyState() noexcept:
    identifier(0),field_04(0),entity(nullptr),animations{},fields_1c{},bounds_5c{},bounds_64{},vector_6c{},fields_78{},
    timer_a8{},timer_b8{},motion_c8{},motion_110{},movements{},queued{},vector_170{},fields_178{},field_188(0),
    auxiliary_18c{},pattern_1a8{},fields_250{},timer_288{},timer_298{},timer_2a8{},auxiliary{},fields_2c8{} {
    fields_250[10]=10;fields_250[11]=0xffffffffu;
}
void reset_enemy_auxiliary(EnemyAuxiliary28& state) noexcept {
    state.words[0]=state.words[1]=state.words[2]=state.words[5]=0;state.words[6]&=~2u;
}
void reset_enemy_pattern(EnemyPatternState& state) noexcept {
    std::memset(&state,0,sizeof(state));state.field_a0=state.field_a4=32.0f;recovered::timer_set(state.timer_90,0);
}
void EnemyState::initialize() {
    movements.clear();movements.resize(1);                   //497f40,49c2c0/489fa0
    fields_250[6]=0x14;fields_250[7]=3;fields_250[5]=0xffffffffu;
    fields_250[1]=0;fields_250[1]=0;                         //two original writes retained
    std::memset(&motion_110,0,sizeof(motion_110));             //4a73d0
    bounds_5c={24.0f,24.0f};bounds_64={24.0f,24.0f};fields_1c[7]=0;
    fields_250[8]=0xffffffffu;reset_enemy_pattern(pattern_1a8);
    recovered::timer_set(timer_a8,0);recovered::timer_set(timer_b8,0);
    recovered::timer_set(timer_288,0);recovered::timer_set(timer_298,0);
    fields_1c[6]=1;fields_250[11]=0xffffffffu;fields_1c[8]=fields_1c[9]=0;
    reset_enemy_auxiliary(auxiliary_18c);auxiliary.clear();
    animations.resize(1); //489ea0 preserves existing first element; new elements call48b3d0
    fields_1c[12]=0x3f800000u;fields_250[13]=0;
}
}
