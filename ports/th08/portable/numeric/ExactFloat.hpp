#pragma once
#include <cstdint>
#include <cstring>
#include <cmath>

// Exact fast paths for the games' x87-compatible values. Extended-range,
// subnormal, non-finite and non-nearest cases remain owned by SoftFloat.
namespace touhou::numeric {
inline std::uint32_t bits(float f) { std::uint32_t u; std::memcpy(&u,&f,4); return u; }
inline float value(std::uint32_t u) { float f; std::memcpy(&f,&u,4); return f; }
template<class E> bool from_float(float f,E& out) {
    const auto u=bits(f),exponent=(u>>23)&255u;
    if(exponent==255u||(!exponent&&(u&0x7fffffu)))return false;
    out=E{};out.exponent=(u>>16)&0x8000u;
    if(exponent){out.exponent|=exponent+16256u;out.significand=std::uint64_t((u&0x7fffffu)|0x800000u)<<40;}
    return true;
}
template<class E> bool to_float(const E& in,float& out) {
    const auto exponent=in.exponent&0x7fffu,sign=std::uint32_t(in.exponent&0x8000u)<<16;
    if(!exponent&&!in.significand){out=value(sign);return true;}
    if(exponent<16257u||exponent>16510u||!(in.significand>>63)||(in.significand&0xffffffffffull))return false;
    out=value(sign|((exponent-16256u)<<23)|std::uint32_t((in.significand>>40)&0x7fffffu));return true;
}
template<class E> bool from_double(double f,E& out) {
    std::uint64_t u;std::memcpy(&u,&f,8);const auto exponent=(u>>52)&2047u;
    if(exponent==2047u||(!exponent&&(u&0xfffffffffffffull)))return false;
    out=E{};out.exponent=(u>>48)&0x8000u;
    if(exponent){out.exponent|=exponent+15360u;out.significand=0x8000000000000000ull|((u&0xfffffffffffffull)<<11);}
    return true;
}
template<class E> bool to_double(const E& in,double& out) {
    const auto exponent=in.exponent&0x7fffu;const auto sign=std::uint64_t(in.exponent&0x8000u)<<48;
    std::uint64_t u;
    if(!exponent&&!in.significand)u=sign;
    else{if(exponent<15361u||exponent>17406u||!(in.significand>>63)||(in.significand&2047u))return false;
        u=sign|(std::uint64_t(exponent-15360u)<<52)|((in.significand>>11)&0xfffffffffffffull);}
    std::memcpy(&out,&u,8);return true;
}
template<class E> E from_integer(std::int64_t n) {
    E out{};if(!n)return out;const auto magnitude=n<0?0ull-std::uint64_t(n):std::uint64_t(n);
    const auto shift=__builtin_clzll(magnitude);out.significand=magnitude<<shift;out.exponent=(n<0?0x8000u:0u)|(16383u+63u-shift);return out;
}
// MSVC's conversion helper returns the low word of a truncated signed i64.
// Handle only canonical finite values strictly inside that range. Exceptional
// values and overflow keep their SoftFloat result, including its sentinel.
template<class E> bool truncate_low_word(const E& in,std::int32_t& out) {
    const auto exponent=in.exponent&0x7fffu;
    if(!exponent&&!in.significand){out=0;return true;}
    if(!exponent||!(in.significand>>63)||exponent>=16446u)return false;
    std::uint32_t low=exponent<16383u?0u:std::uint32_t(in.significand>>(16446u-exponent));
    if(in.exponent&0x8000u)low=0u-low;
    std::memcpy(&out,&low,4);return true;
}
template<class E> bool binary(const E& a,const E& b,E& out,int op) {
    float x,y;if(!to_float(a,x)||!to_float(b,y))return false;float result;
    switch(op){
    case 0:result=x+y;if(result==0&&x!=-y)return false;break;
    case 1:result=x-y;if(result==0&&x!=y)return false;break;
    case 2:result=x*y;if(result==0&&x!=0&&y!=0)return false;break;
    default:result=x/y;if(result==0&&x!=0)return false;break;
    }
    // x87 single precision retains its extended exponent range. A result at
    // the f32 underflow/overflow boundary must therefore use the original path.
    const auto exponent=(bits(result)>>23)&255u;
    if(exponent==1u)return false; // Rounding up across the f32 normal boundary.
    return from_float(result,out);
}
// Scalar game expressions need neither an Extended allocation nor conversion
// when their operands and rounded result remain in the normal float range.
inline bool scalar_binary(float x,float y,float& result,int op){
    const auto normal=[](float f){const auto u=bits(f)&0x7fffffffu;return u==0||(u>=0x00800000u&&u<0x7f800000u);};
    if(!normal(x)||!normal(y))return false;
    switch(op){
    case 0:result=x+y;if(result==0&&x!=-y)return false;break;
    case 1:result=x-y;if(result==0&&x!=y)return false;break;
    case 2:result=x*y;if(result==0&&x!=0&&y!=0)return false;break;
    default:result=x/y;if(result==0&&x!=0)return false;break;
    }
    const auto exponent=(bits(result)>>23)&255u;
    return exponent!=1u&&normal(result);
}
template<class E> bool square_root(const E& a,E& out) {
    float x;if(!to_float(a,x))return false;return from_float(std::sqrt(x),out);
}
}
