// 原32位地址计算对应普通弹及相邻副弹池；保存索引乘240的回绕。
#pragma once
#include "core/entities/ProjectilePool.hpp"
namespace kaikeidou::ex_boss_opening_projectile_detail {
struct Record {
 ProjectilePools& pool;std::uint32_t address;
 std::uint8_t& byte(std::size_t offset){const auto a=address+static_cast<std::uint32_t>(offset);if(a>=0xa8ce60u&&a<0xa8e660u){const auto x=a-0xa8ce60u;return pool.secondary[x/192].data[x%192];}if(a>=0xa8e660u&&a<0xb3e2e0u){const auto x=a-0xa8e660u;return pool.bullets[x/240].data[x%240];}throw std::out_of_range("EX opening projectile address outside recorded pools");}
 std::uint32_t word(std::size_t off){std::uint32_t v=0;for(unsigned i=0;i<4;++i)v|=std::uint32_t(byte(off+i))<<(8*i);return v;}
 void word(std::size_t off,std::uint32_t v){for(unsigned i=0;i<4;++i)byte(off+i)=static_cast<std::uint8_t>(v>>(8*i));}
 std::uint64_t quad(std::size_t off){std::uint64_t v=0;for(unsigned i=0;i<8;++i)v|=std::uint64_t(byte(off+i))<<(8*i);return v;}
 void quad(std::size_t off,std::uint64_t v){for(unsigned i=0;i<8;++i)byte(off+i)=static_cast<std::uint8_t>(v>>(8*i));}
};
inline std::int64_t signed_word(std::uint32_t n){return n>>31?std::int64_t(n)-0x100000000ll:n;}
}
