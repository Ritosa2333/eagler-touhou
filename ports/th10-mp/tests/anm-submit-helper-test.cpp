#include "AnmSubmit.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
using namespace th10;
extern "C" {extern std::uint_fast8_t softfloat_exceptionFlags,softfloat_detectTininess,softfloat_roundingMode,extF80_roundingPrecision;}
static float frombits(u32 b){float f;std::memcpy(&f,&b,4);return f;}
static u32 bits(float f){u32 b;std::memcpy(&b,&f,4);return b;}
static u32 next(u32& s){s^=s<<13;s^=s>>17;s^=s<<5;return s;}
static bool original_bounds(float a,float b,float c,float d,const RenderViewport& v){
 const auto u=[](u32 bits){auto r=Extended::from_int(static_cast<i32>(bits));if(bits&0x80000000u)r=r+number(4294967296.f);return r;};
 return number(a)<u(v.x)||number(b)<u(v.y)||u(v.x+v.width)<number(c)||u(v.y+v.height)<number(d);
}
int main(){
 const std::array<u32,34> special={0,0x80000000u,1,0x80000001u,0x007fffffu,0x807fffffu,0x00800000u,0x80800000u,0x00800001u,0x80800001u,0x3f000000u,0xbf000000u,0x3f800000u,0xbf800000u,0x7f7fffffu,0xff7fffffu,0x7f800000u,0xff800000u,0x7fc00000u,0xffc00000u,0x7fc12345u,0xffc12345u,0x7f800001u,0xff800001u,0x33800000u,0xb3800000u,0x4b7fffffu,0x4b800000u,0x4b800001u,0xcb800000u,0x4effffffu,0x4f000000u,0x4f7fffffu,0x4f800000u};
 const std::array<u32,15> coords={0,1,640,480,0x00ffffffu,0x01000000u,0x01000001u,0x01000002u,0x7fffffffu,0x80000000u,0x80000001u,0xfffffffeu,0xffffffffu,0xfffffe00u,0x40000000u};
 u64 add_checks=0,bounds_checks=0;u32 state=0x7492381u;
 for(auto precision:{Precision::Single,Precision::Double,Precision::Extended})for(u32 rounding=0;rounding<4;++rounding)for(u32 tiny=0;tiny<2;++tiny)for(u32 initial=0;initial<32;++initial){
  arithmetic_mode(precision,static_cast<Rounding>(rounding));softfloat_detectTininess=tiny;
  const auto checkadd=[&](float a,float b){softfloat_exceptionFlags=initial;errno=ERANGE;const float expected=Scalar::add(a,b);const u32 flags=softfloat_exceptionFlags;
   softfloat_exceptionFlags=initial;errno=ERANGE;const float actual=anm_submission_add(a,b);
   if(bits(expected)!=bits(actual)||flags!=softfloat_exceptionFlags||errno!=ERANGE||softfloat_roundingMode!=rounding||extF80_roundingPrecision!=unsigned(precision)||softfloat_detectTininess!=tiny){std::fprintf(stderr,"add mismatch %08x %08x precision%u round%u tiny%u initial%u got%08x expected%08x flags%u/%u\n",bits(a),bits(b),unsigned(precision),rounding,tiny,initial,bits(actual),bits(expected),unsigned(softfloat_exceptionFlags),flags);std::exit(1);}++add_checks;};
  for(u32 a:special)for(u32 b:special)checkadd(frombits(a),frombits(b));
  for(u32 i=0;i<512;++i){const auto a=next(state),b=next(state);checkadd(frombits(a),frombits(i%3?b:i%2?0u:0x80000000u));}
  const auto checkbounds=[&](float a,float b,float c,float d,const RenderViewport& v){softfloat_exceptionFlags=initial;errno=EDOM;const bool expected=original_bounds(a,b,c,d,v);const auto flags=softfloat_exceptionFlags;
   softfloat_exceptionFlags=initial;errno=EDOM;const bool actual=anm_submission_outside(a,b,c,d,v);
   if(expected!=actual||flags!=softfloat_exceptionFlags||errno!=EDOM||softfloat_roundingMode!=rounding||extF80_roundingPrecision!=unsigned(precision)||softfloat_detectTininess!=tiny){std::fprintf(stderr,"bounds mismatch p%u r%u t%u initial%u\n",unsigned(precision),rounding,tiny,initial);std::exit(1);}++bounds_checks;};
  checkbounds(640,480,640,480,{0,0,640,480});
  checkbounds(0,0,0,0,{0,0,0,0});
  checkbounds(1,1,0,0,{1,1,0xffffffffu,0xffffffffu});
  checkbounds(16777216.f,16777216.f,16777216.f,16777216.f,{0x01000000u,0x01000000u,0,0});
  for(u32 coord:coords)for(u32 value:special)for(u32 lane=0;lane<4;++lane){float values[]={640,480,0,0};values[lane]=frombits(value);const RenderViewport v{coord,coord,640,480};checkbounds(values[0],values[1],values[2],values[3],v);}
  for(u32 i=0;i<512;++i){const RenderViewport v{next(state),next(state),next(state),next(state)};float a=frombits(next(state)),b=frombits(next(state)),c=frombits(next(state)),d=frombits(next(state));checkbounds(a,b,c,d,i%2?v:RenderViewport{0,0,640,480});}
 }
 std::printf("{\"suite\":\"anm-submit-helpers\",\"add_comparisons\":%llu,\"bounds_comparisons\":%llu,\"precisions\":3,\"roundings\":4,\"tininess_modes\":2,\"initial_flag_patterns\":32,\"passed\":true}\n",(unsigned long long)add_checks,(unsigned long long)bounds_checks);
}
