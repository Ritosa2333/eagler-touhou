#pragma once
#include "background.hpp"
namespace th20::source::background {
void pack_fog_color(FogState&) noexcept; //473400
FogState make_fog(float near_distance,float far_distance,float blue,float green,float red,float alpha) noexcept; //4718d0
FogState scale_fog(const FogState&,float) noexcept; //471db0
FogState subtract_fog(const FogState&,const FogState&) noexcept; //471e60
FogState add_fog(const FogState&,const FogState&) noexcept; //471f50
FogState sample_fog(sprite::Interpolation<FogState>&,const float* rate); //473590
}
