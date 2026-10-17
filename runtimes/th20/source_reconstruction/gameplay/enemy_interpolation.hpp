#pragma once
#include "enemy_state.hpp"
namespace th20::source::gameplay {
sprite::Vec3 sample_enemy_motion_interpolation(EnemyMotionInterpolation&,const float* rate); //4a8d70
float sample_enemy_scalar_interpolation(sprite::Interpolation<float>&,const float* rate); //42a110
sprite::Vec2 sample_enemy_vector_interpolation(sprite::Interpolation<sprite::Vec2>&,const float* rate); //42a980
}
