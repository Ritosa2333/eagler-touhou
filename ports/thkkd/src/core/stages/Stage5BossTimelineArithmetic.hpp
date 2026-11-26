// 五面关底局部整数选择与逐指令精度算术；共享数值库保持原字节。
#pragma once
#include "core/stages/Stage5BossTimelineConstants.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/math/FiniteSqrt.hpp"
#include "core/entities/ProjectilePool.hpp"
#include "core/player/OptionsMotion.hpp"
#include <cstring>
namespace kaikeidou {
inline std::uint32_t stage5_boss_timeline_pick_integer(const std::uint32_t& difficulty,const std::array<std::uint32_t,6>& values){return values[difficulty<6?difficulty:3];}
inline std::uint64_t stage5_boss_timeline_pick_real(const std::uint32_t& difficulty,const std::array<std::uint64_t,6>& values){return values[difficulty<6?difficulty:3];}
namespace stage5_boss_timeline_detail {
inline std::int64_t signed_word(std::uint32_t value){return value>>31?std::int64_t(value)-0x100000000ll:std::int64_t(value);}
inline std::int64_t remainder(std::uint32_t numerator,std::uint32_t denominator){
    if(!denominator)throw std::domain_error("Original stage5 boss timeline IDIV zero divisor");
    if(numerator==0x80000000u&&denominator==0xffffffffu)throw std::domain_error("Original stage5 boss timeline IDIV overflow");
    return signed_word(numerator)%signed_word(denominator);
}
inline std::uint32_t quotient(std::uint32_t a,std::uint32_t b){(void)remainder(a,b);return static_cast<std::uint32_t>(signed_word(a)/signed_word(b));}
struct BulletView {
 ProjectilePools& pools;std::uint32_t displacement;
 std::uint32_t address()const{return 0xa8e660u+displacement;}
 std::uint8_t* at(std::size_t offset,std::size_t size)const{
  const auto target=displacement+static_cast<std::uint32_t>(offset);
  if(target<=720000u-size)return reinterpret_cast<std::uint8_t*>(pools.bullets.data())+target;
  if(target>=0xffffe800u&&std::uint64_t(target)+size<=0x100000000ull)return reinterpret_cast<std::uint8_t*>(pools.secondary.data())+(target+6144u);
  throw std::out_of_range("Stage5 bullet address outside supplied pools");
 }
 std::uint32_t word(std::size_t offset)const{std::uint32_t v;std::memcpy(&v,at(offset,4),4);return v;}
 void word(std::size_t offset,std::uint32_t v)const{std::memcpy(at(offset,4),&v,4);}
 std::uint64_t quad(std::size_t offset)const{std::uint64_t v;std::memcpy(&v,at(offset,8),8);return v;}
 void quad(std::size_t offset,std::uint64_t v)const{std::memcpy(at(offset,8),&v,8);}
};
template<std::size_t N> inline void copy_name(std::array<std::uint8_t,64>& target,const std::array<std::uint8_t,N>& source){
    std::size_t i=0;for(;i+4<=N;i+=4)std::memcpy(target.data()+i,source.data()+i,4);
    if(i+2<=N){std::memcpy(target.data()+i,source.data()+i,2);i+=2;}if(i<N)target[i]=source[i];
}
struct Arithmetic {
    using Number=binary_float::Number;unsigned precision;
    Number q(std::uint64_t value)const{return binary_float::binary64(value);}
    Number c(std::uint32_t address)const{return q(constant(address));}
    Number i(std::uint32_t value)const{return binary_float::integer(value);}
    Number add(Number a,Number b)const{return binary_float::add(a,b,precision);}
    Number sub(Number a,Number b)const{b.negative=!b.negative;return add(a,b);}
    Number mul(Number a,Number b)const{return binary_float::multiply(a,b,precision);}
    Number div(Number a,Number b)const{return binary_float::divide(a,b,precision);}
    std::uint64_t store(Number value)const{return binary_float::to_binary64(value);}
};
}
inline std::uint64_t stage5_boss_timeline_absolute(std::uint64_t value){return value&0x7fffffffffffffffull;}
inline void stage5_boss_timeline_ease_real(std::uint64_t& value,std::uint64_t target,std::uint64_t divisor,unsigned precision){value=ease_option_value(value,target,divisor,precision);}
}
