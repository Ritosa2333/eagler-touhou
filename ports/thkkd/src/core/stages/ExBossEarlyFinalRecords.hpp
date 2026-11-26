// 将原32位有效地址映射回已有普通/副弹池；保留两池交界别名。
#pragma once
#include "core/entities/ProjectilePool.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/entities/EnemyDeathContext.hpp"
#include "core/stages/StagePatternContext.hpp"
namespace kaikeidou::ex_boss_early_final_detail {
struct PoolRecord {
 ProjectilePools& pool;std::uint32_t address;ObjectRecord<240>* player=nullptr;
 std::uint8_t& byte(std::size_t offset){const auto at=address+static_cast<std::uint32_t>(offset);if(player&&at>=0xa8cd68u&&at<0xa8ce58u)return player->data[at-0xa8cd68u];if(at>=0xa8ce60u&&at<0xa8e660u){const auto x=at-0xa8ce60u;return pool.secondary[x/192].data[x%192];}if(at>=0xa8e660u&&at<0xa8e660u+720000u){const auto x=at-0xa8e660u;return pool.bullets[x/240].data[x%240];}throw std::out_of_range("EX early final pool address outside recorded pools");}
 std::uint32_t word(std::size_t offset){std::uint32_t value=0;for(unsigned i=0;i<4;++i)value|=std::uint32_t(byte(offset+i))<<(8*i);return value;}
 void word(std::size_t offset,std::uint32_t value){for(unsigned i=0;i<4;++i)byte(offset+i)=static_cast<std::uint8_t>(value>>(8*i));}
 std::uint64_t quad(std::size_t offset){std::uint64_t value=0;for(unsigned i=0;i<8;++i)value|=std::uint64_t(byte(offset+i))<<(8*i);return value;}
 void quad(std::size_t offset,std::uint64_t value){for(unsigned i=0;i<8;++i)byte(offset+i)=static_cast<std::uint8_t>(value>>(8*i));}
};
inline Enemy& enemy_record(EnemyPool& enemies,BossStageContext& context,std::uint32_t index){const auto address=0xa87fe8u+index*248u;if(address==0xa87ef0u)return context.enemy_prefix;if(address>=0xa87fe8u&&address<0xa87fe8u+19840u&&(address-0xa87fe8u)%248u==0)return enemies.enemies[(address-0xa87fe8u)/248u];throw std::out_of_range("EX early final enemy address outside recorded pool");}
}

namespace kaikeidou {
inline void ex_boss_early_final_recolor(ProjectilePools& pool,std::uint32_t index,std::uint32_t red,std::uint32_t green,std::uint32_t blue){ex_boss_early_final_detail::PoolRecord b{pool,0xa8e660u+index*240u};b.word(24,red);b.word(28,green);b.word(32,blue);}
inline std::int32_t ex_boss_early_final_remove100(ProjectilePools& pool,std::uint32_t index){ex_boss_early_final_detail::PoolRecord b{pool,0xa8e660u+index*240u};if(b.word(0)==1||b.word(0)==2){b.word(0,3);b.word(16,0);b.word(8,0);if(!b.word(44))b.word(44,1);}return static_cast<std::int32_t>(index*240u);}
}

namespace kaikeidou {
inline std::int32_t ex_boss_early_final_remove(ProjectilePools& pool,std::uint32_t index,std::uint32_t mode){if(mode!=0&&mode!=100)throw std::out_of_range("EX early final remove mode");ex_boss_early_final_detail::PoolRecord b{pool,0xa8e660u+index*240u};if(mode>99||!b.word(52)){if(b.word(0)==1||b.word(0)==2){b.word(0,3);b.word(16,0);b.word(8,0);if(!b.word(44))b.word(44,1);}}return static_cast<std::int32_t>(index*240u);}
}
