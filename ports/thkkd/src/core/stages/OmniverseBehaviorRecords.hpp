// 原32位地址视图共用已有池与前置字段，不复制跨池别名状态。
#pragma once
#include "core/stages/OmniverseBehaviorContext.hpp"
namespace kaikeidou::omniverse_behavior_detail {
struct Record {
 EnemyPool& enemies;ProjectilePools& pool;OmniverseBehaviorContext& context;std::uint32_t address;std::uint32_t index_hint=0;
 std::uint8_t& byte(std::uint32_t offset)const{
  const auto a=address+offset;
  if(a>=0xa87ef0u&&a<0xa87fe8u)return context.boss.enemy_prefix.data[a-0xa87ef0u];
  if(a>=0xa87fe8u&&a<0xa8cd68u){const auto n=a-0xa87fe8u;return enemies.enemies[n/248].data[n%248];}
  if(a>=0xa8cd68u&&a<0xa8ce58u)return context.player.data[a-0xa8cd68u];
  if(a>=0xa8ce60u&&a<0xa8e660u){const auto n=a-0xa8ce60u;return pool.secondary[n/192].data[n%192];}
  if(a>=0xa8e660u&&a<0xb3e2e0u){const auto n=a-0xa8e660u;return pool.bullets[n/240].data[n%240];}
  if(a>=0xa73550u&&a<0xa735c0u)return context.effect_prefix.data[a-0xa73550u];
  if(a>=0xa735c0u&&a<0xa74ba0u){const auto n=a-0xa735c0u;return context.effects.effects[n/112].data[n%112];}
  throw std::out_of_range("Omniverse behavior address outside supplied pools");
 }
 std::uint32_t word(unsigned o)const{std::uint32_t v=0;for(unsigned i=0;i<4;++i)v|=std::uint32_t(byte(o+i))<<(8*i);return v;}
 std::uint64_t quad(unsigned o)const{std::uint64_t v=0;for(unsigned i=0;i<8;++i)v|=std::uint64_t(byte(o+i))<<(8*i);return v;}
 void word(unsigned o,std::uint32_t v)const{for(unsigned i=0;i<4;++i)byte(o+i)=static_cast<std::uint8_t>(v>>(8*i));}
 void quad(unsigned o,std::uint64_t v)const{for(unsigned i=0;i<8;++i)byte(o+i)=static_cast<std::uint8_t>(v>>(8*i));}
};
}
namespace kaikeidou {
inline void omniverse_behavior_recolor(omniverse_behavior_detail::Record b,std::uint32_t red,std::uint32_t green,std::uint32_t blue){b.word(24,red);b.word(28,green);b.word(32,blue);}
inline std::uint32_t omniverse_behavior_remove(omniverse_behavior_detail::Record b,std::uint32_t mode){if(mode>99||!b.word(52)){if(b.word(0)==1||b.word(0)==2){b.word(0,3);b.word(16,0);b.word(8,0);if(!b.word(44))b.word(44,1);}}return b.address-0xa8e660u;}
}
