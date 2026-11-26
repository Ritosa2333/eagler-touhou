// 原4FCA68..4FCFA4的SSE2分区、双表和多项式；每次运算独立舍入。
#pragma once
#include "core/stages/OmniverseAcosConstants.hpp"
#include "core/math/CrtSse2Trig.hpp"
#include "core/math/FiniteSqrt.hpp"
#include <cerrno>
namespace kaikeidou {
struct OmniverseAcosStatus{std::uint32_t error_code=0;int errno_before=0,errno_after=0;};
inline void check_omniverse_acos_environment(std::uint32_t sse2,std::uint32_t mxcsr,std::uint32_t control){if(sse2!=1||mxcsr!=0x1f80||(control!=0x7f&&control!=0x27f&&control!=0x37f))throw std::domain_error("Omniverse acos requires verified SSE2 environment");}
namespace omniverse_acos_detail {
struct Pair{std::uint64_t lo=0,hi=0;};
inline std::uint64_t load(std::uint32_t a){if(a<0x6bc710||a>0x6bdee8||(a&7u))throw std::out_of_range("acos table offset");return table[(a-0x6bc710)/8];}
inline Pair load_pair(std::uint32_t a){return {load(a),load(a+8)};}
inline std::uint64_t add(std::uint64_t a,std::uint64_t b){return crt_sse2::bits(crt_sse2::add(crt_sse2::real(a),crt_sse2::real(b)));}
inline std::uint64_t sub(std::uint64_t a,std::uint64_t b){return crt_sse2::bits(crt_sse2::sub(crt_sse2::real(a),crt_sse2::real(b)));}
inline std::uint64_t mul(std::uint64_t a,std::uint64_t b){return crt_sse2::bits(crt_sse2::mul(crt_sse2::real(a),crt_sse2::real(b)));}
inline std::uint64_t div(std::uint64_t a,std::uint64_t b){volatile double q=crt_sse2::real(a)/crt_sse2::real(b);return crt_sse2::bits(q);}
inline std::uint64_t root(std::uint64_t a){return binary_float::to_binary64(finite_square_root(a,53));}
inline Pair add(Pair a,Pair b){return {add(a.lo,b.lo),add(a.hi,b.hi)};}
inline Pair sub(Pair a,Pair b){return {sub(a.lo,b.lo),sub(a.hi,b.hi)};}
inline Pair mul(Pair a,Pair b){return {mul(a.lo,b.lo),mul(a.hi,b.hi)};}
inline Pair bxor(Pair a,Pair b){return {a.lo^b.lo,a.hi^b.hi};}
inline Pair band(Pair a,Pair b){return {a.lo&b.lo,a.hi&b.hi};}
inline Pair bor(Pair a,Pair b){return {a.lo|b.lo,a.hi|b.hi};}
inline Pair bandnot(Pair a,Pair b){return {~a.lo&b.lo,~a.hi&b.hi};}
inline Pair shift_left(Pair a,unsigned n){return {a.lo<<n,a.hi<<n};}
inline Pair shift_right(Pair a,unsigned n){return {a.lo>>n,a.hi>>n};}
inline Pair shuffle(Pair a,unsigned n){const std::uint32_t words[4]{static_cast<std::uint32_t>(a.lo),static_cast<std::uint32_t>(a.lo>>32),static_cast<std::uint32_t>(a.hi),static_cast<std::uint32_t>(a.hi>>32)};return {words[n&3u]|(std::uint64_t(words[(n>>2)&3u])<<32),words[(n>>4)&3u]|(std::uint64_t(words[(n>>6)&3u])<<32)};}
inline Pair insert_word(Pair a,std::uint32_t v,unsigned n){auto& z=n<4?a.lo:a.hi;const auto shift=(n%4)*16;z=(z&~(std::uint64_t(65535)<<shift))|(std::uint64_t(v&65535u)<<shift);return a;}
inline std::uint32_t mask_bytes(Pair a){std::uint32_t v=0;for(unsigned i=0;i<8;++i){v|=((a.lo>>(i*8+7))&1u)<<i;v|=((a.hi>>(i*8+7))&1u)<<(i+8);}return v;}
inline std::uint64_t finite_core(std::uint64_t argument){
Pair r0{argument,0},r1,r2,r3,r4,r5,r6,r7;std::uint32_t eax=0,ecx=0,edx=0;
r4.lo=load(0x6bded0u);
r3.lo=load(0x6bdee0u);
r5=bxor(r5,r5);
r2.lo=load(0x6bded8u);
r1.lo=r0.lo;
r0=shift_right(r0,44);
edx=static_cast<std::uint32_t>(r0.lo);
r7.lo=r1.lo;
ecx=0x2000u;
r5=insert_word(r5,ecx,2);
r0.lo=r1.lo;
eax=0x7ffffu;
eax&=edx;
eax-=0x3fb00u;
if(eax >= 0x3bbu)goto L_4fcb96;
r1.lo=mul(r1.lo,r1.lo);
edx&=0xffffu;
r3.lo=sub(r3.lo,r1.lo);
r3.lo=root(r3.lo);
r2=band(r2,r7);
edx&=0xfffffffcu;
edx-=0xfb00u;
r1.lo=load(edx*2u+0x6bd610u);
r2=bor(r2,r5);
r4=load_pair(edx*4u+0x6bc710u);
r6.lo=r7.lo;
r7.lo=add(r7.lo,r2.lo);
r0.lo=sub(r0.lo,r2.lo);
r7.lo=mul(r7.lo,r0.lo);
r6.lo=mul(r6.lo,r1.lo);
r3.lo=mul(r3.lo,r2.lo);
r1.lo=r6.lo;
r6.lo=add(r6.lo,r3.lo);
r7.lo=div(r7.lo,r6.lo);
r0.lo=load(0x6bde88u);
r5.lo=load(0x6bde78u);
r1.lo=sub(r1.lo,r3.lo);
r2=shift_right(r2,63);
r3.lo=r1.lo;
r2=shift_left(r2,63);
r1.lo=mul(r1.lo,r1.lo);
r2=shuffle(r2,68);
r6.lo=load(0x6bde80u);
r3.lo=mul(r3.lo,r1.lo);
r0.lo=mul(r0.lo,r1.lo);
r4=bxor(r4,r2);
r5.lo=mul(r5.lo,r3.lo);
r4=sub(r4,load_pair(0x6bde10u));
r3.lo=mul(r3.lo,r1.lo);
r0.lo=add(r0.lo,r6.lo);
r0.lo=mul(r0.lo,r3.lo);
r5.lo=sub(r5.lo,r4.lo);
r4=shuffle(r4,238);
r0.lo=add(r0.lo,r5.lo);
r0.lo=sub(r0.lo,r7.lo);
r0.lo=sub(r0.lo,r4.lo);
return r0.lo;
L_4fcb96:
eax-=0x3bbu;
if(eax >= 0x41u)goto L_4fccd1;
r7=shift_right(r7,38);
r7=shift_left(r7,38);
eax=mask_bytes(r0);
r4=bandnot(r4,r0);
r1.lo=sub(r1.lo,r7.lo);
r6.lo=r7.lo;
r7.lo=mul(r7.lo,r7.lo);
r0.lo=add(r0.lo,r6.lo);
r5=bor(r5,r4);
r3.lo=sub(r3.lo,r7.lo);
r0.lo=mul(r0.lo,r1.lo);
r4.lo=r3.lo;
r3.lo=sub(r3.lo,r0.lo);
r3.lo=root(r3.lo);
eax&=0x80u;
eax>>=7;
eax=0u-eax;
r7.lo=r3.lo;
r2=band(r2,r3);
r3=shift_left(r3,2);
edx=static_cast<std::uint32_t>(r3.lo>>48)&65535u;
r2=bor(r2,r5);
r3={eax,0};
r3=shuffle(r3,0);
edx-=0xfec0u;
edx+=edx;
r7.lo=mul(r7.lo,load(edx*4u+0x6bd610u));
r6.lo=mul(r6.lo,r2.lo);
r1.lo=mul(r1.lo,r2.lo);
r2.lo=mul(r2.lo,r2.lo);
r6.lo=sub(r6.lo,r7.lo);
r3=band(r3,load_pair(0x6bde20u));
r6.lo=add(r6.lo,r1.lo);
r4.lo=sub(r4.lo,r2.lo);
r7.lo=add(r7.lo,r7.lo);
r5.lo=load(0x6bde78u);
r4.lo=sub(r4.lo,r0.lo);
r7.lo=add(r7.lo,r6.lo);
r0.lo=load(0x6bde88u);
r4.lo=div(r4.lo,r7.lo);
r2.lo=load(0x6bde80u);
r3=add(r3,load_pair(edx*8u+0x6bc710u));
r1.lo=r6.lo;
r6.lo=mul(r6.lo,r6.lo);
r0.lo=mul(r0.lo,r6.lo);
r1.lo=mul(r1.lo,r6.lo);
r5.lo=mul(r5.lo,r1.lo);
r1.lo=mul(r1.lo,r6.lo);
r0.lo=add(r0.lo,r2.lo);
r6=bxor(r6,r6);
r0.lo=mul(r0.lo,r1.lo);
r5.lo=add(r5.lo,r3.lo);
r0.lo=add(r0.lo,r5.lo);
eax&=0x8000u;
r6=insert_word(r6,eax,3);
r5.lo=r4.lo;
r3=shuffle(r3,238);
r4.lo=add(r4.lo,r3.lo);
r3.lo=sub(r3.lo,r4.lo);
r5.lo=add(r5.lo,r3.lo);
r0.lo=add(r0.lo,r5.lo);
r0.lo=add(r0.lo,r4.lo);
r0=bxor(r0,r6);
return r0.lo;
L_4fccd1:
eax+=0x3bbbu;
if(eax >= 0x3800u)goto L_4fcd71;
r0={r0.lo,r0.lo};
r6=load_pair(0x6bde90u);
r1={r1.lo,r0.lo};
r2=load_pair(0x6bdea0u);
r4=load_pair(0x6bdeb0u);
r0=mul(r0,r0);
r5=load_pair(0x6bde10u);
r1=mul(r1,r0);
r6=mul(r6,r0);
r0=mul(r0,r0);
r3.lo=r1.lo;
r1.lo=mul(r1.lo,r1.lo);
r6=add(r6,r2);
r4=mul(r4,r0);
r1.lo=mul(r1.lo,r3.lo);
r6=add(r6,r4);
r0=shuffle(r5,238);
r1=mul(r1,r6);
r6=shuffle(r5,238);
r0.lo=sub(r0.lo,r7.lo);
r2=shuffle(r1,238);
r5.lo=sub(r5.lo,r1.lo);
r6.lo=sub(r6.lo,r0.lo);
r5.lo=sub(r5.lo,r2.lo);
r7.lo=sub(r7.lo,r6.lo);
r5.lo=sub(r5.lo,r7.lo);
r0.lo=add(r0.lo,r5.lo);
return r0.lo;
L_4fcd71:
eax-=0x3bfcu;
if(eax >= 0x4u)goto L_4fce73;
r6=bxor(r6,r6);
r7=band(r7,load_pair(0x6bded0u));
r4.lo=load(0x6bdee8u);
r1=load_pair(0x6bde90u);
r7.lo=mul(r7.lo,r4.lo);
r2=load_pair(0x6bdea0u);
r4.lo=sub(r4.lo,r7.lo);
r3=load_pair(0x6bdeb0u);
r7=shuffle(r4,68);
r4.lo=root(r4.lo);
r1=mul(r1,r7);
r5=shuffle(r7,68);
eax=static_cast<std::uint32_t>(r0.lo>>48)&65535u;
r7=mul(r7,r7);
r2=add(r2,r1);
r1.lo=load(0x6bde50u);
r3=mul(r3,r7);
r0.lo=crt_sse2::real(r0.lo)<crt_sse2::real(r6.lo)?~std::uint64_t(0):0;
r7.lo=mul(r7.lo,r5.lo);
r2=add(r2,r3);
r0=shuffle(r0,68);
r2.lo=mul(r2.lo,r7.lo);
r0=band(r0,load_pair(0x6bde20u));
r2=mul(r2,r5);
r1=band(r1,r4);
r3=shuffle(r4,68);
r4.lo=sub(r4.lo,r1.lo);
r3.lo=add(r3.lo,r3.lo);
r1.lo=mul(r1.lo,r1.lo);
r3.lo=sub(r3.lo,r4.lo);
r5.lo=sub(r5.lo,r1.lo);
r4.lo=mul(r4.lo,r3.lo);
r3=shuffle(r3,238);
r5.lo=sub(r5.lo,r4.lo);
r5.lo=div(r5.lo,r3.lo);
r3=add(r3,r3);
r2=mul(r2,r3);
r4=shuffle(r2,238);
r2.lo=add(r2.lo,r0.lo);
eax&=0x8000u;
r6=insert_word(r6,eax,3);
r0=shuffle(r0,238);
r2.lo=add(r2.lo,r4.lo);
r2.lo=add(r2.lo,r5.lo);
r2.lo=add(r2.lo,r3.lo);
r0.lo=add(r0.lo,r2.lo);
r0=bxor(r0,r6);
return r0.lo;
L_4fce73:
return add(load(0x6bde18),load(0x6bde10));
}
}
inline std::uint64_t omniverse_acos_bits(std::uint64_t argument,OmniverseAcosStatus* status=nullptr){
 const auto before=errno;const auto magnitude=argument&0x7fffffffffffffffull;std::uint64_t result;std::uint32_t code=0;
 if(magnitude>0x3ff0000000000000ull){code=magnitude>0x7ff0000000000000ull?1008u:58u;result=code==1008?(argument|0x8000000000000ull):0xfff8000000000000ull;errno=33;}
 else if(magnitude==0x3ff0000000000000ull)result=argument>>63?omniverse_acos_detail::add(omniverse_acos_detail::load(0x6bde38),omniverse_acos_detail::load(0x6bde30)):0;
 else result=omniverse_acos_detail::finite_core(argument);
 if(status)*status={code,before,errno};return result;
}
}
