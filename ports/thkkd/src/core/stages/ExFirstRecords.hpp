// 将原32位有效地址映射回已有普通/副弹池；保留两池交界别名。
#pragma once
#include "core/entities/ProjectilePool.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/BossStageContext.hpp"
namespace kaikeidou::ex_first_detail {
struct PoolRecord {
 ProjectilePools& pool;std::uint32_t address;
 std::uint8_t& byte(std::size_t offset){const auto at=address+static_cast<std::uint32_t>(offset);if(at>=0xa8ce60u&&at<0xa8e660u){const auto x=at-0xa8ce60u;return pool.secondary[x/192].data[x%192];}if(at>=0xa8e660u&&at<0xa8e660u+720000u){const auto x=at-0xa8e660u;return pool.bullets[x/240].data[x%240];}throw std::out_of_range("EX first pool address outside recorded pools");}
 std::uint32_t word(std::size_t offset){std::uint32_t value=0;for(unsigned i=0;i<4;++i)value|=std::uint32_t(byte(offset+i))<<(8*i);return value;}
 void word(std::size_t offset,std::uint32_t value){for(unsigned i=0;i<4;++i)byte(offset+i)=static_cast<std::uint8_t>(value>>(8*i));}
 std::uint64_t quad(std::size_t offset){std::uint64_t value=0;for(unsigned i=0;i<8;++i)value|=std::uint64_t(byte(offset+i))<<(8*i);return value;}
 void quad(std::size_t offset,std::uint64_t value){for(unsigned i=0;i<8;++i)byte(offset+i)=static_cast<std::uint8_t>(value>>(8*i));}
};
inline Enemy& enemy_record(EnemyPool& enemies,BossStageContext& context,std::uint32_t index){const auto address=0xa87fe8u+index*248u;if(address==0xa87ef0u)return context.enemy_prefix;if(address>=0xa87fe8u&&address<0xa87fe8u+19840u&&(address-0xa87fe8u)%248u==0)return enemies.enemies[(address-0xa87fe8u)/248u];throw std::out_of_range("EX first enemy address outside recorded pool");}
}
