#pragma once
#include <array>
#include <cmath>
namespace th15::sdl {
inline bool presentation_camera_continuous(const std::array<std::array<float,16>,4>& before,
                                           const std::array<std::array<float,16>,4>& current){
 // The existing 128-unit discontinuity boundary applies to the whole camera,
 // never individual elements. Mixing half of a loop-reset camera creates a
 // view that existed in neither authored frame.
 for(unsigned k=1;k<3;k++)for(unsigned j=0;j<16;j++){
  const float delta=current[k][j]-before[k][j];
  if(!std::isfinite(delta)||std::abs(delta)>=128.f)return false;
 }return true;
}
}
