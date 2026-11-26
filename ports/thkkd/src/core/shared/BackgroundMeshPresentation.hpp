// 原65F630/6602E0/6603F0的中心滚动、顶点、透明度、整行排序与显示边界。
#pragma once
#include "core/stages/StageResourceContext.hpp"
#include "core/shared/BackgroundMeshPresentationServices.hpp"
#include "core/math/BinaryFloat32.hpp"
#include "core/math/FiniteDivide.hpp"
namespace kaikeidou {
namespace background_mesh_presentation_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:std::int64_t(v);}
inline bool greater(binary_float::Number a,binary_float::Number b){
 if(!a.mantissa&&!b.mantissa)return false;
 if(a.negative!=b.negative)return !a.negative;
 const auto order=a.mantissa==0?-1:b.mantissa==0?1:a.exponent!=b.exponent?(a.exponent>b.exponent?1:-1):a.mantissa!=b.mantissa?(a.mantissa>b.mantissa?1:-1):0;
 return a.negative?order<0:order>0;
}
inline void swap_rows(ObjectRecord<0x6f0>& mesh,std::uint32_t first,std::uint32_t second){
 const auto a=0x3cu+first*0x9cu,b=0x3cu+second*0x9cu;std::array<std::uint32_t,39> temporary{};
 for(unsigned i=0;i<39;++i)temporary[i]=mesh.word(a+i*4);
 for(unsigned i=0;i<39;++i)mesh.word(a+i*4,mesh.word(b+i*4));
 for(unsigned i=0;i<39;++i)mesh.word(b+i*4,temporary[i]);
}
inline void update(BackgroundMeshPool& pool,unsigned precision){
 using namespace binary_float;const auto sub=[&](Number a,Number b){b.negative=!b.negative;return add(a,b,precision);};
 const auto mul=[&](Number a,Number b){return multiply(a,b,precision);};const auto sum=[&](Number a,Number b){return add(a,b,precision);};
 constexpr std::array<std::array<std::uint32_t,2>,6> axes{{{{0xbf800000u,0x3f800000u}},{{0x3f800000u,0x3f800000u}},{{0xbf800000u,0xbf800000u}},{{0x3f800000u,0xbf800000u}},{{0xbf800000u,0xbf800000u}},{{0x3f800000u,0x3f800000u}}}};
 const auto half=binary64(0x3fe0000000000000ull),two=binary64(0x4000000000000000ull),three=binary64(0x4008000000000000ull),full=binary64(0x406fe00000000000ull);
 for(std::uint32_t index=0;signed_word(index)<signed_word(pool.count);++index){auto& mesh=pool.meshes.at(index);const auto q=[&](std::size_t off){return finite_binary32(mesh.word(off));};
  if(signed_word(mesh.word(56))>11)throw std::out_of_range("Background mesh rows exceed recovered initializer domain");
  for(std::uint32_t row=0;signed_word(row)<signed_word(mesh.word(56));++row){const auto base=0x3cu+row*0x9cu;mesh.word(base+8,to_binary32(sub(q(base+8),three)));
   for(unsigned vertex=0;vertex<6;++vertex){const auto at=base+vertex*24;const auto x=finite_binary32(axes[vertex][0]),y=finite_binary32(axes[vertex][1]);
    switch(mesh.word(0)){
    case 0:mesh.word(at+12,to_binary32(sum(mul(q(28),x),q(base))));mesh.word(at+16,to_binary32(sum(mul(q(32),y),q(base+4))));mesh.word(at+20,to_binary32(q(base+8)));break;
    case 1:mesh.word(at+12,to_binary32(q(base)));mesh.word(at+16,to_binary32(sum(mul(q(32),y),q(base+4))));mesh.word(at+20,to_binary32(sum(mul(divide(q(36),two,precision),x),q(base+8))));break;
    case 2:mesh.word(at+12,to_binary32(sum(mul(q(28),x),q(base))));mesh.word(at+16,to_binary32(q(base+4)));mesh.word(at+20,to_binary32(sum(mul(divide(q(36),two,precision),y),q(base+8))));break;
    default:break;
    }
   }
  }
  if(!greater(sub(q(40),q(48)),{})||!greater(sub(q(52),q(44)),{}))continue;
  for(std::uint32_t row=0;signed_word(row)<signed_word(mesh.word(56));++row){const auto base=0x3cu+row*0x9cu;
   if(greater(sub(q(44),mul(q(36),half)),q(base+8))){const auto delta=to_binary32(sub(sub(q(44),mul(q(36),half)),q(base+8)));mesh.word(base+8,to_binary32(sub(sum(mul(q(36),half),q(40)),finite_binary32(delta))));}
   else if(greater(q(base+8),sum(mul(q(36),half),q(40)))){const auto delta=to_binary32(sub(q(base+8),sum(mul(q(36),half),q(40))));mesh.word(base+8,to_binary32(sum(sub(q(44),mul(q(36),half)),finite_binary32(delta))));}
   // 原顺序使用回绕前已生成的顶点深度计算alpha，中心回绕后本帧不重建顶点。
   for(unsigned vertex=0;vertex<6;++vertex){const auto at=base+vertex*24;const auto z=finite_binary32(to_binary32(q(at+20)));
    if(!greater(z,q(44)))mesh.data[at+27]=0;
    else if(greater(z,q(44))&&greater(q(52),z)){const auto gradient=divide(full,sub(q(52),q(44)),precision);mesh.data[at+27]=static_cast<std::uint8_t>(truncate(mul(gradient,sub(z,q(44)))));}
    else if(!greater(q(52),z)&&!greater(z,q(48)))mesh.data[at+27]=255;
    else if(greater(z,q(48))&&greater(q(40),z)){const auto gradient=divide(full,sub(q(40),q(48)),precision);mesh.data[at+27]=static_cast<std::uint8_t>(truncate(mul(gradient,sub(q(40),z))));}
    else if(!greater(q(40),z))mesh.data[at+27]=0;
   }
  }
 }
}
inline void sort(BackgroundMeshPool& pool){
 for(std::uint32_t index=0;signed_word(index)<signed_word(pool.count);++index){auto& mesh=pool.meshes.at(index);
  if(signed_word(mesh.word(56))>11)throw std::out_of_range("Background mesh rows exceed recovered initializer domain");
  for(std::uint32_t i=0;signed_word(i)<signed_word(mesh.word(56));++i)for(std::uint32_t j=i+1;signed_word(j)<signed_word(mesh.word(56));++j)
   if(greater(binary_float::finite_binary32(mesh.word(0x44+j*0x9c)),binary_float::finite_binary32(mesh.word(0x44+i*0x9c))))swap_rows(mesh,i,j);
 }
}
}
inline bool background_mesh_presentation_enabled(std::uint32_t enabled,std::uint32_t stage,std::uint32_t part,std::uint32_t subphase,std::uint32_t boss_state){
 using background_mesh_presentation_detail::signed_word;
 return enabled!=0&&boss_state!=2&&(stage==4||stage==5||(stage==6&&(signed_word(part)>1||signed_word(subphase)>3)));
}
inline void present_background_meshes(BackgroundMeshPool& pool,const std::uint32_t& scene,unsigned precision,BackgroundMeshPresentationServices& services){
 using namespace background_mesh_presentation_detail;
 if(scene==0){update(pool,precision);sort(pool);}
 services.set_clip(32,16,416,464);
 for(std::uint32_t index=0;signed_word(index)<signed_word(pool.count);++index){auto& mesh=pool.meshes.at(index);
  if(signed_word(mesh.word(56))>11)throw std::out_of_range("Background mesh rows exceed recovered initializer domain");
  for(std::uint32_t row=0;signed_word(row)<signed_word(mesh.word(56));++row)services.draw_triangles(mesh.data.data()+0x48+row*0x9c,2,mesh.word(4),1);
 }
 services.set_clip(0,0,640,480);
}
}
