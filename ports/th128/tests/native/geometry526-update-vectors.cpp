#include "game/Geometry526.hpp"
#include <bit>
#include <cassert>
#include <iostream>
#include <limits>

void outputVertex(const th128::geometry526::Vertex& value){
    for(const float scalar:{value.x,value.y,value.z,value.rhw})std::cout<<std::bit_cast<std::uint32_t>(scalar)<<' ';
    std::cout<<value.color<<' '<<std::bit_cast<std::uint32_t>(value.u)<<' '<<std::bit_cast<std::uint32_t>(value.v)<<' ';
}
int main(){
    using namespace th128::geometry526;
    unsigned columns=0,rows=0;
    while(std::cin>>columns>>rows){
        std::array<std::uint32_t,9> words{};for(auto& value:words)std::cin>>value;
        EffectState state{std::bit_cast<float>(words[0]),std::bit_cast<float>(words[1]),words[2],
            {std::bit_cast<float>(words[6]),std::bit_cast<float>(words[7])}};
        const std::array position{std::bit_cast<float>(words[3]),std::bit_cast<float>(words[4]),std::bit_cast<float>(words[5])};
        const auto result=stepEffect(state,position,std::bit_cast<float>(words[8]),columns,rows);assert(result);
        std::cout<<std::bit_cast<std::uint32_t>(result->next.scale)<<' ';
        for(const float angle:result->next.angles)std::cout<<std::bit_cast<std::uint32_t>(angle)<<' ';
        for(const auto& value:result->grid.vertices)outputVertex(value);
        for(const auto& value:result->grid.positions)for(const float scalar:value)std::cout<<std::bit_cast<std::uint32_t>(scalar)<<' ';
        const auto strips=rowStrips(result->grid);assert(strips);
        for(const auto& value:*strips)outputVertex(value);
        std::cout<<'\n';
        const auto original=state;state.scale=0;assert(!stepEffect(state,position,1,columns,rows));
        state=original;assert(!stepEffect(state,position,-1,columns,rows));
        assert(!stepEffect(state,position,std::numeric_limits<float>::infinity(),columns,rows));
        state.scale=std::numeric_limits<float>::quiet_NaN();assert(!stepEffect(state,position,1,columns,rows));
        state=original;state.angles[0]=std::numeric_limits<float>::infinity();assert(!stepEffect(state,position,1,columns,rows));
        state=original;assert(!stepEffect(state,{std::numeric_limits<float>::quiet_NaN(),0,0},1,columns,rows));
        assert(!stepEffect(state,position,1,18,rows));assert(state.scale==original.scale);
    }
}
