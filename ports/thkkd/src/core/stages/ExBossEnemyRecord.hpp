// 原索引按32位地址回绕，前置槽和玩家邻槽与各自共同状态共享字节。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/entities/BossStageContext.hpp"
namespace kaikeidou::ex_boss_enemies_detail {
struct EnemyRecord {
    EnemyPool& enemies;BossStageContext& context;ObjectRecord<240>& player;std::uint32_t address;
    std::uint8_t& byte(std::size_t offset){
        const auto at=address+static_cast<std::uint32_t>(offset);
        if(at>=0xa87ef0u&&at<0xa87fe8u)return context.enemy_prefix.data[at-0xa87ef0u];
        if(at>=0xa87fe8u&&at<0xa8cd68u){const auto index=at-0xa87fe8u;return enemies.enemies[index/248].data[index%248];}
        if(at>=0xa8cd68u&&at<0xa8ce58u)return player.data[at-0xa8cd68u];
        throw std::out_of_range("EX boss enemy address outside recorded neighbors");
    }
    std::uint32_t word(std::size_t offset){std::uint32_t value=0;for(unsigned i=0;i<4;++i)value|=std::uint32_t(byte(offset+i))<<(8*i);return value;}
    void word(std::size_t offset,std::uint32_t value){for(unsigned i=0;i<4;++i)byte(offset+i)=static_cast<std::uint8_t>(value>>(8*i));}
    std::uint64_t quad(std::size_t offset){std::uint64_t value=0;for(unsigned i=0;i<8;++i)value|=std::uint64_t(byte(offset+i))<<(8*i);return value;}
    void quad(std::size_t offset,std::uint64_t value){for(unsigned i=0;i<8;++i)byte(offset+i)=static_cast<std::uint8_t>(value>>(8*i));}
};
}
