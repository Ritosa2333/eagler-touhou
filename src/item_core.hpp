#pragma once
#include "core_contract.hpp"
#include "enemy_math.hpp"
#include <array>
namespace soew {
// Generation + movement only from th02/main/item/item.cpp. Pickup is an
// explicit callback; false must leave item alive, never silently eat power.
struct ItemCore {
    enum Type {Power,Point,Bomb,BigPower,OneUp};
    struct Item {unsigned flag=0,type=0;std::array<int,2> left{};std::array<std::uint16_t,2> top{};int vy=0,vx=0,age=0;};
    std::array<Item,20> slots{};
    std::uint8_t ring_index=0,cycle=0,drop_cycle=0;
    unsigned bigpower_override=0;
    int item_skill=0;
    std::uint64_t spawned=0;
    void initialize(RandomRing &rng){ring_index=rng.masked(7);for(auto &s:slots)s.flag=0;}
    static int pixel(std::uint16_t raw){const auto v=enemy_s16(raw);return v>=0?v/16:-((-v+15)/16);}
    void begin_frame(unsigned back){if(back>1)throw std::runtime_error("Invalid item page");for(auto &s:slots)if(s.flag){s.left[back]=s.left[back^1];s.top[back]=s.top[back^1];if(s.flag==2)s.flag=0;}}
    bool add(int left,int top,unsigned type,unsigned lives){
        if(type>OneUp)throw std::runtime_error("Unknown original item type");
        if(bigpower_override&&type<Bomb){--bigpower_override;type=BigPower;}
        if(lives==5&&type==OneUp)type=Bomb;
        for(auto &s:slots)if(s.flag==0){s.flag=1;s.type=type;s.left={left,left};const auto raw=static_cast<std::uint16_t>(top*16);s.top={raw,raw};s.vy=-48;s.vx=0;s.age=0;++spawned;return true;}
        return false;
    }
    void semirandom(int left,int top,RandomRing &rng,unsigned lives){
        constexpr std::array<unsigned,10> ring={Power,Point,Power,Point,Point,Power,Power,Point,Point,Power};
        if(bigpower_override){--bigpower_override;add(left,top,BigPower,lives);return;}
        const auto old=cycle++;if(old%3)return;
        add(left,top,(rng.next16()&511)==18?unsigned(Bomb):ring.at(ring_index),lives);
        if(++ring_index>=10)ring_index=0;
    }
    void miss_add(int left,int top,int cached_player_x,bool gameover,RandomRing &rng){
        constexpr std::array<int,5> sides={-80,-68,-56,-44,-32},center_y={-56,-68,-80,-68,-56},center_x={-2,-1,0,1,2};
        const int region=cached_player_x<=128?-1:cached_player_x<=256?0:1;
        unsigned added=0;bool big_missing=true;
        for(auto &s:slots){if(s.flag)continue;s.flag=1;s.left={left,left};const auto raw=static_cast<std::uint16_t>(top*16);s.top={raw,raw};s.age=0;
            if(gameover)s.type=BigPower;
            else if(big_missing){s.type=rng.next8()%(5-added)==0?BigPower:rng.next8()%2;if(s.type==BigPower)big_missing=false;}
            else s.type=rng.next8()%2;
            s.vy=region?sides[added]:center_y[added];s.vx=region?region*-2:center_x[added];++spawned;if(++added==5)break;
        }
    }
    template<class Collect> void update(unsigned back,int player_x,int player_y,bool hit,Collect &&collect){
        if(back>1)throw std::runtime_error("Invalid item page");
        for(auto &s:slots){if(s.flag!=1)continue;if(s.vy<=0)s.left[back]=enemy_s16(unsigned(s.left[back]+s.vx));s.top[back]=static_cast<std::uint16_t>(s.top[back]+s.vy);
            const int left=s.left[back];int y=pixel(s.top[back]);if(y<0){y=0;s.top[back]=0;}else if(y>=384){s.flag=2;++drop_cycle;if(!(drop_cycle&15))item_skill=enemy_s16(unsigned(item_skill-1));continue;}
            if(left<=16||left>=416){s.flag=2;continue;}s.age=enemy_s16(unsigned(s.age+1));s.vy=enemy_s16(unsigned(s.vy+1));
            if(!hit&&left>=player_x-8&&left<=player_x+24&&y>=player_y&&y<=player_y+24&&collect(s.type,left,y))s.flag=2;
        }
    }
};
}
