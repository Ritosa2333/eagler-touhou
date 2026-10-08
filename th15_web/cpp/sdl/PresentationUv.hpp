#pragma once
#include "../../../portable/sdl/GraphicsState.hpp"
#include <cmath>
#include <array>
namespace th15::sdl {
// Only periodic samplers admit equivalent UVs across a wrap boundary.
// CLAMP must interpolate the authored coordinates, not an invented +1 tile.
inline float presentation_uv(float before,float current,float alpha,
                            touhou::graphics::Address address,bool scrolling){
 if(alpha==1)return current;
 float delta=current-before;
 if(scrolling&&address!=touhou::graphics::Address::Clamp){
  const float period=address==touhou::graphics::Address::Mirror?2.f:1.f;
  if(std::abs(delta)>period*.5f)delta-=std::round(delta/period)*period;
 }
 return before+delta*alpha;
}
inline std::array<float,16> presentation_texture_matrix(const std::array<float,16>& before,
 const std::array<float,16>& current,float alpha,touhou::graphics::Address u,
 touhou::graphics::Address v,bool scrolling,bool same_u=true,bool same_v=true){
 auto result=current;
 for(unsigned j=0;j<16;j++){
  // ANM uses a three-component texture coordinate (u,v,1), not (u,v,0,1).
  // anm_texture_transform writes m[8]/m[9]; Shaders.hpp multiplies vec4(uv,1,0).
  if(j==8||j==9){const bool same=j==8?same_u:same_v;
   if(same)result[j]=presentation_uv(before[j],current[j],alpha,j==8?u:v,scrolling);
  }else result[j]=alpha==1?current[j]:before[j]+(current[j]-before[j])*alpha;
 }return result;
}
}
