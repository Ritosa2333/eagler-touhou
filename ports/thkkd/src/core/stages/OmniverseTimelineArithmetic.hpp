#pragma once
#include "core/stages/Stage6BossTimelineArithmetic.hpp"
#include "core/stages/OmniverseTimelineConstants.hpp"
#include <cerrno>
namespace kaikeidou::omniverse_timeline_detail {
struct Number{
 binary_float::Number finite{};std::uint64_t special=0;
 Number()=default;Number(binary_float::Number n):finite(n){};
 bool negative()const{return special?(special>>63)!=0:finite.negative;}
 bool nan()const{return special&&(special&0xfffffffffffffull);}
 bool infinite()const{return special&&!nan();}
 bool zero()const{return !special&&!finite.mantissa;}
};
inline std::int64_t signed_word(std::uint32_t w){return w>>31?std::int64_t(w)-0x100000000ll:w;}
inline Number exceptional(std::uint64_t bits){Number n;n.special=bits;return n;}
inline Number quiet(Number n){if(n.nan())n.special|=0x8000000000000ull;return n;}
inline Number negated(Number n){if(n.special)n.special^=0x8000000000000000ull;else n.finite.negative=!n.finite.negative;return n;}
struct Arithmetic{
 unsigned precision;Number q(std::uint64_t b)const{return (b&0x7ff0000000000000ull)==0x7ff0000000000000ull?exceptional(b):Number(binary_float::binary64(b));}
 Number c(std::uint32_t a)const{return q(constant(a));}Number i(std::uint32_t w)const{return binary_float::integer(w);}
 Number add(Number a,Number b)const{if(a.nan())return quiet(a);if(b.nan())return quiet(b);if(a.infinite()&&b.infinite()&&a.negative()!=b.negative())return exceptional(0xfff8000000000000ull);if(a.infinite())return a;if(b.infinite())return b;return binary_float::add(a.finite,b.finite,precision);}
 Number sub(Number a,Number b)const{if(a.nan())return quiet(a);if(b.nan())return quiet(b);return add(a,negated(b));}
 Number mul(Number a,Number b)const{if(a.nan())return quiet(a);if(b.nan())return quiet(b);if((a.infinite()&&b.zero())||(b.infinite()&&a.zero()))return exceptional(0xfff8000000000000ull);if(a.infinite()||b.infinite())return exceptional(((std::uint64_t(a.negative()!=b.negative()))<<63)|0x7ff0000000000000ull);return binary_float::multiply(a.finite,b.finite,precision);}
 Number div(Number a,Number b)const{if(a.nan())return quiet(a);if(b.nan())return quiet(b);if((a.zero()&&b.zero())||(a.infinite()&&b.infinite()))return exceptional(0xfff8000000000000ull);const bool sign=a.negative()!=b.negative();if(b.zero()||a.infinite())return exceptional((std::uint64_t(sign)<<63)|0x7ff0000000000000ull);if(b.infinite())return Number(binary_float::Number{0,0,sign});return binary_float::divide(a.finite,b.finite,precision);}
 std::uint64_t store(Number n)const{return n.special?n.special:binary_float::to_binary64(n.finite);}
};
inline int compare(Number a,Number b){if(a.nan()||b.nan())return 2;if(a.infinite()&&b.infinite())return a.negative()==b.negative()?0:a.negative()?-1:1;if(a.infinite())return a.negative()?-1:1;if(b.infinite())return b.negative()?1:-1;if(stage6_boss_timeline_detail::greater(a.finite,b.finite))return 1;if(stage6_boss_timeline_detail::greater(b.finite,a.finite))return -1;return 0;}
inline bool lt(Number a,Number b){return compare(a,b)==-1;}inline bool le(Number a,Number b){auto c=compare(a,b);return c==-1||c==0;}
inline bool gt(Number a,Number b){return compare(a,b)==1;}inline bool ge(Number a,Number b){auto c=compare(a,b);return c==0||c==1;}
inline std::int64_t remainder(std::uint32_t n,std::uint32_t d){if(!d||(n==0x80000000u&&d==0xffffffffu))throw std::domain_error("Original Omniverse IDIV fault");return signed_word(n)%signed_word(d);}
inline std::uint32_t convert(Number n){if(n.special)return 0x80000000u;const auto stored=binary_float::to_binary64(n.finite);if((stored&0x7ff0000000000000ull)==0x7ff0000000000000ull)return 0x80000000u;const auto v=binary_float::binary64(stored);if(!v.mantissa||v.exponent<0)return 0;if(v.exponent>31)return 0x80000000u;const auto mag=v.mantissa>>(63-v.exponent);if(mag>(v.negative?0x80000000ull:0x7fffffffull))return 0x80000000u;return v.negative?0u-static_cast<std::uint32_t>(mag):static_cast<std::uint32_t>(mag);}
}
namespace kaikeidou{
inline std::uint64_t omniverse_timeline_absolute(std::uint64_t value){return value&0x7fffffffffffffffull;}
inline std::uint32_t omniverse_timeline_convert(omniverse_timeline_detail::Number value){return omniverse_timeline_detail::convert(value);}
inline std::uint32_t omniverse_timeline_character(const std::uint32_t& character,const std::array<std::uint32_t,3>& values){return values[character<3?character:0];}
inline omniverse_timeline_detail::Number omniverse_timeline_cosine(GameMath& math,std::uint64_t arg){if((arg&0x7ff0000000000000ull)==0x7ff0000000000000ull){return omniverse_timeline_detail::exceptional((arg&0xfffffffffffffull)?arg|0x8000000000000ull:0xfff8000000000000ull);}return math.cosine(arg);}
inline omniverse_timeline_detail::Number omniverse_timeline_sine(GameMath& math,std::uint64_t arg){if((arg&0x7ff0000000000000ull)==0x7ff0000000000000ull){return omniverse_timeline_detail::exceptional((arg&0xfffffffffffffull)?arg|0x8000000000000ull:0xfff8000000000000ull);}return math.sine(arg);}
inline omniverse_timeline_detail::Number omniverse_timeline_sqrt(std::uint64_t arg,unsigned precision){if((arg&0x7ff0000000000000ull)==0x7ff0000000000000ull)return omniverse_timeline_detail::exceptional((arg&0xfffffffffffffull)?arg|0x8000000000000ull:arg>>63?0xfff8000000000000ull:arg);if(arg>>63&&(arg&0x7fffffffffffffffull))return omniverse_timeline_detail::exceptional(0xfff8000000000000ull);return finite_square_root(arg,precision);}
template<class Record> inline void omniverse_timeline_ease_object(Record object,unsigned offset,std::uint64_t target,std::uint64_t divisor,unsigned precision){const omniverse_timeline_detail::Arithmetic m{precision};object.quad(offset,m.store(m.add(m.div(m.sub(m.q(target),m.q(object.quad(offset))),m.q(divisor)),m.q(object.quad(offset)))));}
inline void omniverse_timeline_ease(std::uint64_t& value,std::uint64_t target,std::uint64_t divisor,unsigned precision){const omniverse_timeline_detail::Arithmetic m{precision};value=m.store(m.add(m.div(m.sub(m.q(target),m.q(value)),m.q(divisor)),m.q(value)));}
}
