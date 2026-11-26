// 182/184插值中的零除结果在本模块保留，有限计算仍使用公共精度运算。
#pragma once
#include "core/stages/ExBossLateOtherArithmetic.hpp"
namespace kaikeidou { namespace ex_boss_late_other_detail {
struct InterpolationValue{
    Number finite{};std::uint64_t special=0;
    bool nan()const{return special&&(special&0x000fffffffffffffull)!=0;}
    bool negative()const{return special?(special>>63)!=0:finite.negative;}
    static InterpolationValue bits(std::uint64_t v,const Arithmetic& m){if((v&0x7ff0000000000000ull)==0x7ff0000000000000ull)return {{},(v&0x000fffffffffffffull)?v|0x0008000000000000ull:v};return {m.q(v),0};}
    std::uint64_t store(const Arithmetic& m)const{return special?special:m.store(finite);}
};
inline InterpolationValue interpolation_add(InterpolationValue a,InterpolationValue b,const Arithmetic& m){if(a.nan())return a;if(b.nan())return b;if(a.special&&b.special)return a.negative()==b.negative()?a:InterpolationValue{{},0xfff8000000000000ull};if(a.special)return a;if(b.special)return b;return {m.add(a.finite,b.finite),0};}
inline InterpolationValue interpolation_sub(InterpolationValue a,InterpolationValue b,const Arithmetic& m){if(a.nan())return a;if(b.nan())return b;if(b.special)b.special^=0x8000000000000000ull;else b.finite.negative=!b.finite.negative;return interpolation_add(a,b,m);}
inline InterpolationValue interpolation_mul(InterpolationValue a,InterpolationValue b,const Arithmetic& m){if(a.nan())return a;if(b.nan())return b;if(a.special||b.special){if((!a.special&&!a.finite.mantissa)||(!b.special&&!b.finite.mantissa))return {{},0xfff8000000000000ull};return {{},a.negative()!=b.negative()?0xfff0000000000000ull:0x7ff0000000000000ull};}return {m.mul(a.finite,b.finite),0};}
inline void interpolate(ProjectilePools& pool,Bullet child,std::uint32_t denominator,const Arithmetic& m){
    const auto age=child.word(12);InterpolationValue ratio;
    if(!denominator)ratio.special=!age?0xfff8000000000000ull:signed_word(age)<0?0xfff0000000000000ull:0x7ff0000000000000ull;
    else ratio=InterpolationValue::bits(m.store(m.div(m.i(age),m.i(denominator))),m);
    for(auto offset:{0x40u,0x48u}){const Bullet first{pool,child.word(0x78)*240u},second{pool,child.word(0x7c)*240u};const auto delta=interpolation_sub(InterpolationValue::bits(second.quad(offset),m),InterpolationValue::bits(first.quad(offset),m),m);const auto product=interpolation_mul(delta,ratio,m);child.quad(offset,interpolation_add(product,InterpolationValue::bits(first.quad(offset),m),m).store(m));}
}
inline bool interpolation_outside(std::uint64_t value,std::uint32_t low,std::uint32_t high,const Arithmetic& m){if((value&0x7ff0000000000000ull)==0x7ff0000000000000ull)return(value&0x000fffffffffffffull)==0;return compare(m.c(low),m.q(value))>0||compare(m.c(high),m.q(value))<0;}
}}
