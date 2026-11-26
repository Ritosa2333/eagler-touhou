// 关底弹的记录视图与原改色/fabs边界，-1槽映射既有副弹池末240字节。
#pragma once
#include "core/entities/ProjectilePool.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/shared/FinalProjectileConstants.hpp"
namespace kaikeidou {
namespace final_projectile_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
struct Record {
    ProjectilePools& pools;std::uint32_t index;
    std::uint8_t& byte(std::size_t offset){if(index!=0xffffffffu)return pools.bullets[index].data[offset];const auto at=5904+offset;return pools.secondary[at/192].data[at%192];}
    std::uint32_t word(std::size_t offset){std::uint32_t v=0;for(unsigned i=0;i<4;++i)v|=std::uint32_t(byte(offset+i))<<(8*i);return v;}
    void word(std::size_t offset,std::uint32_t v){for(unsigned i=0;i<4;++i)byte(offset+i)=static_cast<std::uint8_t>(v>>(8*i));}
    std::uint64_t quad(std::size_t offset){std::uint64_t v=0;for(unsigned i=0;i<8;++i)v|=std::uint64_t(byte(offset+i))<<(8*i);return v;}
    void quad(std::size_t offset,std::uint64_t v){for(unsigned i=0;i<8;++i)byte(offset+i)=static_cast<std::uint8_t>(v>>(8*i));}
};
}
inline void final_projectile_recolor(ProjectilePools& pools,std::uint32_t index,std::uint32_t r,std::uint32_t g,std::uint32_t b){final_projectile_detail::Record p{pools,index};p.word(0x18,r);p.word(0x1c,g);p.word(0x20,b);}
inline std::uint64_t final_projectile_degrees(std::uint64_t angle,unsigned precision){using namespace binary_float;using final_projectile_detail::constant;return to_binary64(divide(multiply(binary64(angle),binary64(constant(0x6d1860)),precision),binary64(constant(0x6d1858)),precision));}
inline std::uint64_t final_projectile_pick(std::uint32_t difficulty,const std::array<std::uint64_t,6>& values){return values[difficulty<6?difficulty:3];}
inline std::uint64_t final_projectile_absolute(std::uint64_t value){return value&0x7fffffffffffffffull;}
}
