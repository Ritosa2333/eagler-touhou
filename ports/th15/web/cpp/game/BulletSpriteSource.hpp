#pragma once
#include "AnmVm.hpp"
#include "BulletState.hpp"
namespace th15 {
// Each bullet script uses parameters 0..3 for its body, impact, spawn and
// cancellation sprites. Keep the source live when a transform changes colour.
class BulletSpriteSource final:public AnmSpriteSource {
    const BulletState& state;
public:
    explicit BulletSpriteSource(const BulletState& bullet):state(bullet){}
    i32 sprite_index(i32 parameter)const noexcept override {
        return bullet_animation_parameter(state.sprite_type,state.color,parameter);
    }
};
}
