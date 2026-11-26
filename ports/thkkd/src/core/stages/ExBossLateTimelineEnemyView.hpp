// 敌槽地址以32位回绕，映射前置槽、80个敌槽及紧随其后的玩家结构。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include <stdexcept>
namespace kaikeidou {
struct ExBossLateTimelineEnemyView{
    EnemyPool& pool;Enemy& prefix;ObjectRecord<240>& player;std::uint32_t address;
    ExBossLateTimelineEnemyView(EnemyPool& p,Enemy& before,ObjectRecord<240>& after,std::uint32_t index):pool(p),prefix(before),player(after),address(0xa87fe8u+index*248u){}
    std::uint8_t& byte(std::size_t offset)const{
        const auto a=address+static_cast<std::uint32_t>(offset);
        if(a>=0xa87ef0u&&a<0xa87fe8u)return prefix.data[a-0xa87ef0u];
        if(a>=0xa87fe8u&&a<0xa8cd68u){const auto i=a-0xa87fe8u;return pool.enemies[i/248u].data[i%248u];}
        if(a>=0xa8cd68u&&a<0xa8ce58u)return player.data[a-0xa8cd68u];
        throw std::out_of_range("EX late enemy address outside supplied regions");
    }
    std::uint32_t word(std::size_t offset)const{std::uint32_t v=0;for(unsigned i=0;i<4;++i)v|=std::uint32_t(byte(offset+i))<<(i*8);return v;}
    std::uint64_t quad(std::size_t offset)const{return word(offset)|(std::uint64_t(word(offset+4))<<32);}
    void word(std::size_t offset,std::uint32_t v)const{for(unsigned i=0;i<4;++i)byte(offset+i)=static_cast<std::uint8_t>(v>>(i*8));}
    void quad(std::size_t offset,std::uint64_t v)const{for(unsigned i=0;i<8;++i)byte(offset+i)=static_cast<std::uint8_t>(v>>(i*8));}
};
}
