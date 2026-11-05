#include "BackgroundMath.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace th08;
#ifndef TH_BACKGROUND_MATH_TEST
#error This oracle requires TH_BACKGROUND_MATH_TEST.
#endif
namespace {
u32 checks=0,rng=0x89abcdefu;
u32 bits(float x){u32 v;std::memcpy(&v,&x,4);return v;}
float value(u32 v){float x;std::memcpy(&x,&v,4);return x;}
u32 random_bits(){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
[[noreturn]] void fail(const char* name){std::fprintf(stderr,"background-math check=%u precision=%u rounding=%u expression=%s\n",checks,unsigned(arithmetic_precision()),unsigned(arithmetic_rounding()),name);std::exit(1);}
void same(float got,float expected,const char* name){++checks;if(bits(got)!=bits(expected)){std::fprintf(stderr,"got=%08x expected=%08x\n",bits(got),bits(expected));fail(name);}}
void same(const Extended& got,const Extended& expected){++checks;if(got.significand!=expected.significand||got.exponent!=expected.exponent)fail("dot3 Extended bits");}
void check(const std::array<float,6>& v){
    const auto a=v[0],b=v[1],c=v[2],d=v[3],e=v[4],f=v[5];
    // Frozen old expressions, independent of the guarded compound functions.
    same(background_math::dot3(a,b,c,d,e,f),number(a)*number(d)+number(b)*number(e)+number(c)*number(f));
    same(background_math::center(a,b,c,d),(number(a)+number(b)-number(c)+number(d)/number(2)).to_float(),"center");
    same(background_math::translated(a,b,c),(number(a)+number(b)-number(c)).to_float(),"translated");
    same(background_math::coordinate(a,b,c,d),(number(a)+number(b)+number(c)-number(d)).to_float(),"coordinate");
    same(background_math::scaled_add(a,b,c),(number(b)*number(c)+number(a)).to_float(),"scaled_add");
}
}
int main(){
    const u32 edges[]{0,0x80000000,1,0x80000001,0x007fffff,0x807fffff,0x00800000,0x80800000,0x00800001,0x01000000,
        0x3f000000,0xbf000000,0x3f800000,0xbf800000,0x3f800001,0x3f7fffff,0x40000000,0xc0000000,0x4b800000,0x4b800001,
        0x7f000000,0xff000000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc54321,0x7f812345,0xff812345};
    u32 modes=0;
    for(auto precision:{Precision::Single,Precision::Double,Precision::Extended})
    for(auto rounding:{Rounding::NearestEven,Rounding::TowardZero,Rounding::Down,Rounding::Up}){
        arithmetic_mode(precision,rounding);++modes;
        const auto start=background_math::stats;
        check({123.375f,-19.625f,1000.25f,.25f,-.125f,17.75f});
        check({0,0,0,0,0,0});
        // Typical geometry and raw f32 inputs exercise both accepting and
        // fallback paths; every reference retains the full Extended range.
        for(u32 i=0;i<2048;++i){
            std::array<float,6> v;
            for(auto& x:v)x=float(i32(random_bits()%2000001)-1000000)*.015625f;
            check(v);
            for(auto& x:v)x=value(random_bits());
            check(v);
        }
        const std::array<float,6> base{123.375f,-19.625f,1000.25f,.25f,-.125f,17.75f};
        for(u32 field=0;field<6;++field)for(u32 raw:edges){auto v=base;v[field]=value(raw);check(v);}
        for(u32 a:edges)for(u32 b:edges)check({value(a),value(b),value(a^0x80000000u),value(b^0x80000000u),.5f,2});
        // Intermediate float overflow/underflow followed by cancellation or
        // recovery must fall back as one whole expression.
        check({value(0x7f7fffff),value(0x7f7fffff),value(0x7f7fffff),2,2,-2});
        check({value(0x00800000),.5f,value(0x00800000),2,2,2});
        check({value(0x00800001),value(0x3f7fffff),0,0,0,0});
        if(precision!=Precision::Single||rounding!=Rounding::NearestEven)
            if(background_math::stats.fast!=start.fast)fail("non-default mode entered float path");
    }
    if(!background_math::stats.fast||!background_math::stats.fallback)fail("missing fast/fallback coverage");
    std::printf("background-math: PASS modes=%u checks=%u fast=%llu fallback=%llu\n",modes,checks,
        static_cast<unsigned long long>(background_math::stats.fast),static_cast<unsigned long long>(background_math::stats.fallback));
}
