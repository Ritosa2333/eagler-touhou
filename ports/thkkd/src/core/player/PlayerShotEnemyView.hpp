// 原追踪目标地址按32位回绕；-1落在敌池前置槽，80落在玩家结构。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include <stdexcept>
namespace kaikeidou {
struct PlayerShotEnemyView {
    const EnemyPool& enemies;
    std::uint32_t address;
    const Enemy* prefix;
    const ObjectRecord<240>* player;
    PlayerShotEnemyView(const EnemyPool& pool,std::uint32_t index,
        const Enemy* before=nullptr,const ObjectRecord<240>* after=nullptr)
        :enemies(pool),address(0xa87fe8u+index*248u),prefix(before),player(after){}
    const std::uint8_t& byte(std::size_t offset) const {
        const auto at=address+static_cast<std::uint32_t>(offset);
        if(at>=0xa87ef0u&&at<0xa87fe8u&&prefix)return prefix->data[at-0xa87ef0u];
        if(at>=0xa87fe8u&&at<0xa8cd68u){const auto i=at-0xa87fe8u;return enemies.enemies[i/248].data[i%248];}
        if(at>=0xa8cd68u&&at<0xa8ce58u&&player)return player->data[at-0xa8cd68u];
        throw std::out_of_range("Player shot target outside supplied original memory regions");
    }
    std::uint32_t word(std::size_t offset) const {std::uint32_t value=0;for(unsigned i=0;i<4;++i)value|=std::uint32_t(byte(offset+i))<<(8*i);return value;}
    std::uint64_t quad(std::size_t offset) const {std::uint64_t value=0;for(unsigned i=0;i<8;++i)value|=std::uint64_t(byte(offset+i))<<(8*i);return value;}
};
}
