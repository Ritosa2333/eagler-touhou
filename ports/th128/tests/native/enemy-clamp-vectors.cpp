#include "game/EnemyRules.hpp"
#include <bit>
#include <cassert>
#include <iostream>
#include <limits>
void outputPosition(const th128::enemy::Position& value){
    for(const float scalar:{value.x,value.y,value.z})std::cout<<std::bit_cast<std::uint32_t>(scalar)<<' ';
}
int main(){
    using namespace th128::enemy;
    std::uint32_t flags=0;
    while(std::cin>>flags){
        std::array<std::uint32_t,13> words{};for(auto& word:words)std::cin>>word;
        Position absolute{std::bit_cast<float>(words[0]),std::bit_cast<float>(words[1]),std::bit_cast<float>(words[2])};
        const Position relative{std::bit_cast<float>(words[3]),std::bit_cast<float>(words[4]),std::bit_cast<float>(words[5])};
        Position combined{std::bit_cast<float>(words[6]),std::bit_cast<float>(words[7]),std::bit_cast<float>(words[8])},velocity;
        const auto area=prepareMovementArea(std::bit_cast<float>(words[9]),std::bit_cast<float>(words[10]),
            std::bit_cast<float>(words[11]),std::bit_cast<float>(words[12]));assert(area);
        if(flags&0x40000u)assert(combinePositionWithArea(combined,velocity,absolute,relative,*area));
        else assert(combinePosition(combined,velocity,absolute,relative,flags));
        outputPosition(combined);outputPosition(velocity);outputPosition(absolute);std::cout<<'\n';
        const auto saved=combined,oldAbsolute=absolute;
        auto invalid=*area;invalid.width=std::numeric_limits<float>::quiet_NaN();
        assert(!combinePositionWithArea(combined,velocity,absolute,relative,invalid));
        assert(combined.x==saved.x&&absolute.x==oldAbsolute.x);
        assert(!combinePositionWithArea(combined,combined,absolute,relative,*area));
    }
    assert(!prepareMovementArea(0,0,std::numeric_limits<float>::infinity(),64));
    assert(!prepareMovementArea(0,std::numeric_limits<float>::quiet_NaN(),288,64));
}
