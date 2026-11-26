// 原42FA00/42FA70/42FAE0；byte指针和逐次读取保留未对齐及部分重叠语义。
#pragma once
#include "core/stages/Stage6Float3RotationMath.hpp"
#include <cstring>
namespace kaikeidou {
inline void stage6_rotate_float3(std::uint32_t axis,std::uint8_t* destination,const std::uint8_t* source,std::uint64_t angle,unsigned precision){
 using namespace binary_float;if(axis>2||(precision!=24&&precision!=53&&precision!=64))throw std::out_of_range("Stage6 float3 axis or precision outside verified domain");
 const auto coefficients=stage6_float3_rotation_detail::sincos32(angle);const auto sine=finite_binary32(coefficients[0]),cosine=finite_binary32(coefficients[1]);
 const auto load=[&](std::size_t offset){std::uint32_t value;std::memcpy(&value,source+offset,4);return value;};
 const auto read=[&](std::size_t offset){return finite_binary32(load(offset));};
 const auto write=[&](std::size_t offset,std::uint32_t value){std::memcpy(destination+offset,&value,4);};
 const auto sub=[&](Number a,Number b){b.negative=!b.negative;return add(a,b,precision);};
 if(axis==0){
  write(0,load(0));
  const auto cz=multiply(cosine,read(8),precision);const auto sy=multiply(sine,read(4),precision);const auto temporary=to_binary32(sub(cz,sy));
  const auto cy=multiply(cosine,read(4),precision);const auto sz=multiply(sine,read(8),precision);write(4,to_binary32(add(cy,sz,precision)));write(8,temporary);
 }else if(axis==1){
  write(4,load(4));
  const auto cx=multiply(cosine,read(0),precision);const auto sz=multiply(sine,read(8),precision);const auto temporary=to_binary32(sub(cx,sz));
  const auto cz=multiply(cosine,read(8),precision);const auto sx=multiply(sine,read(0),precision);write(8,to_binary32(add(cz,sx,precision)));write(0,temporary);
 }else{
  write(8,load(8));
  const auto cx=multiply(cosine,read(0),precision);const auto sy=multiply(sine,read(4),precision);const auto temporary=to_binary32(sub(cx,sy));
  const auto cy=multiply(cosine,read(4),precision);const auto sx=multiply(sine,read(0),precision);write(4,to_binary32(add(cy,sx,precision)));write(0,temporary);
 }
}
}
