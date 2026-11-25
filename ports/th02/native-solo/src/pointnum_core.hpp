#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace soew {
// th02/main/pointnum/pointnum.cpp: 20 first-free slots, pixel Y on2 pages.
struct PointnumCore {
    struct Number {unsigned flag=0;int left=0;std::array<int,2> top{};std::uint16_t points=0;std::uint8_t age=0;};
    std::array<Number,20> slots{};
    unsigned rank=1;
    std::uint64_t generated=0;
    void begin_frame(unsigned back){if(back>1)throw std::runtime_error("Invalid popup page");for(auto &n:slots){if(n.flag==2)n.flag=0;else if(n.flag==1)n.top[back]=n.top[back^1];}}
    bool add(int left,int top,std::uint16_t points){for(auto &n:slots)if(n.flag==0){n.flag=1;n.age=0;n.left=left;n.top={top,top};n.points=points;++generated;return true;}return false;}
    void update(unsigned back){if(back>1)throw std::runtime_error("Invalid popup page");for(auto &n:slots)if(n.flag==1){if(n.age>6){--n.top[back];if(n.age>24){n.flag=2;continue;}}++n.age;}}
    // 0..9 numerals,10 multiplication,12 empty. Leading spaces retain width.
    std::array<unsigned,7> glyphs(std::uint16_t points)const{
        constexpr std::array<unsigned,4> powers={1000,100,10,1};
        std::array<unsigned,7> out={12,12,12,12,0,12,12};bool visible=false;
        for(unsigned i=0;i<4;++i){unsigned value=points/powers[i];points=static_cast<std::uint16_t>(points%powers[i]);if(value||visible){visible=true;out[i]=value;}}
        if(rank>=2&&rank<=4){out[5]=10;out[6]=1u<<(rank-1);}return out;
    }
};
}
