#include "../th10_web/cpp/game/AnmUvScroll.hpp"
#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace th10;
extern "C" {extern std::uint_fast8_t softfloat_exceptionFlags,softfloat_detectTininess;extern u8 softfloat_roundingMode,extF80_roundingPrecision;}
namespace {
u32 bits(float x){u32 u;std::memcpy(&u,&x,4);return u;}
float value(u32 u){float x;std::memcpy(&x,&u,4);return x;}
u32 rng(u32& s){s^=s<<13;s^=s>>17;s^=s<<5;return s;}
struct Result {u32 result;unsigned flags,tininess,precision,rounding;int error;};
Result evaluate(float input,float velocity,float rate,unsigned seed,int error,bool fast){
 softfloat_exceptionFlags=seed;errno=error;
 const auto r=number(rate);
 float result;
 if(fast&&anm_stationary_uv(input,velocity,anm_stationary_uv_mode(rate)))result=input;
 else {auto next=r*number(velocity)+number(input);if(number(1.f)<next||number(1.f)==next)next=next-number(1.f);else if(next<number(0.f))next=next+number(1.f);result=next.to_float();}
 return {bits(result),unsigned(softfloat_exceptionFlags),unsigned(softfloat_detectTininess),extF80_roundingPrecision,softfloat_roundingMode,errno};
}
unsigned checks=0,hits=0;
void check(u32 input,u32 velocity,u32 rate,unsigned seed,int error){
 const auto a=evaluate(value(input),value(velocity),value(rate),seed,error,false);
 const auto b=evaluate(value(input),value(velocity),value(rate),seed,error,true);
 if(a.result!=b.result||a.flags!=b.flags||a.tininess!=b.tininess||a.precision!=b.precision||a.rounding!=b.rounding||a.error!=b.error){std::fprintf(stderr,"mismatch input=%08x velocity=%08x rate=%08x mode=%u/%u tiny=%u flags=%u expected=%08x/%u actual=%08x/%u\n",input,velocity,rate,a.precision,a.rounding,a.tininess,seed,a.result,a.flags,b.result,b.flags);std::abort();}
 hits+=anm_stationary_uv(value(input),value(velocity),anm_stationary_uv_mode(value(rate)));++checks;
}
}
int main(){
 const std::array<u32,34> special={0,0x80000000,1,0x80000001,0x007fffff,0x807fffff,0x00800000,0x80800000,0x00800001,0x3e000000,0xbe000000,0x3f000000,0xbf000000,0x3f800000,0xbf800000,0x3fc90fdb,0xbfc90fdb,0x40490fdb,0xc0490fdb,0x40c90fdb,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc00000,0x7fc12345,0xffc12345,0x7f800001,0xff800001,0x33800000,0xb3800000,0x4b800000,0xcb800000,0x3f7fffff};
 for(auto precision:{Precision::Single,Precision::Double,Precision::Extended})for(unsigned rounding=0;rounding<4;++rounding)for(unsigned tininess=0;tininess<2;++tininess)for(unsigned seed:{0u,1u,16u,31u}){
  arithmetic_mode(precision,static_cast<Rounding>(rounding));softfloat_detectTininess=tininess;
  for(u32 input:special)for(u32 velocity:special)for(u32 rate:special)check(input,velocity,rate,seed,EDOM);
  u32 state=0x24abc782;
  for(unsigned n=0;n<10000;++n){u32 input=rng(state),velocity=rng(state),rate=rng(state);check(input,velocity,rate,seed,0);check(input,0,0x3f800000,seed,ERANGE);check(input,0x80000000,0x3f800000,seed,0);}
 }
 for(unsigned n=0;n<4096;++n){arithmetic_mode(Precision::Single,static_cast<Rounding>(n%4));softfloat_detectTininess=(n/4)%2;check(0,((n/8)%2)?0x80000000:0,0x3f800000,n%32,ERANGE);}
 std::printf("{\"suite\":\"anm-stationary-uv\",\"checks\":%u,\"admitted\":%u,\"bits_flags_modes_errno_equal\":true}\n",checks,hits);
}
