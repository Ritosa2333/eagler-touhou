#pragma once
#include "pointnum_core.hpp"
#include "enemy_math.hpp"
#include <array>
#include <cstdint>
#include <vector>
namespace soew {
// Reward half of item_hittest_y. Caller owns exact movement/contact test and
// aggregates this.frame_score once per item frame, applying rank multiplier.
struct PickupCore {
    unsigned power=1,overflow=0;
    std::uint8_t collect_skill=0;
    std::uint16_t points_collected=0;
    int item_skill=0;
    std::uint32_t frame_score=0;
    std::vector<unsigned> sounds;
    std::uint64_t collected=0;
    constexpr static std::array<unsigned,43> bonuses={1,1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900,950,1000,1050,1100,1200,1250,1280};
    constexpr static std::array<unsigned,21> levels={0,1,2,2,3,3,4,4,5,5,6,6,6,7,7,7,8,8,8,8,9};
    unsigned level()const{if(power>80)throw std::runtime_error("Invalid original power");return levels.at(power>>2);}
    void begin_frame(){frame_score=0;}
    void collect(unsigned type,int left,int top,unsigned &lives,unsigned &bombs,PointnumCore &numbers){
        if(power>80||overflow>=bonuses.size()||type>4)throw std::runtime_error("Invalid pickup state");
        auto point=[&](unsigned value){numbers.add(left-12,top,static_cast<std::uint16_t>(value));};
        switch(type){
        case 0:
            if(power<80){++power;overflow=0;++frame_score;}
            else{if(overflow<42)++overflow;frame_score+=bonuses[overflow];++collect_skill;}
            ++collect_skill;point(bonuses[overflow]);break;
        case 1:{++points_collected;unsigned value;
            if(top<=64){collect_skill=static_cast<std::uint8_t>(collect_skill+8);value=5120;}
            else{value=static_cast<std::uint16_t>(2800-top*7);collect_skill=static_cast<std::uint8_t>(collect_skill+(value>=2000?4:value>=1000?2:1));}
            point(value);frame_score+=value;break;}
        case 2:
            if(bombs<5){++bombs;frame_score+=1000;point(1000);}
            else{frame_score+=6553;point(6553);collect_skill=static_cast<std::uint8_t>(collect_skill+16);}break;
        case 3:
            if(power<70){power+=10;++frame_score;overflow=0;}
            else{overflow=overflow<38?overflow+5:42;power=80;frame_score+=bonuses[overflow];}
            point(bonuses[overflow]);break;
        case 4:if(lives<5){++lives;sounds.push_back(8);}break;
        }
        ++collected;sounds.push_back(13);
        if(collect_skill>=32){item_skill=enemy_s16(unsigned(item_skill+1));collect_skill=static_cast<std::uint8_t>(collect_skill-32);}
    }
    std::uint32_t score_for_rank(unsigned rank)const{if(rank>4)throw std::runtime_error("Invalid pickup rank");return frame_score<<(rank>=2?rank-1:0);}
};
}
