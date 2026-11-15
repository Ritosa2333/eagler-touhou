#include "game/Geometry526.hpp"
#include <bit>
#include <cassert>
#include <iostream>
#include <limits>

void vertex(const th128::geometry526::Vertex& value){
    for(const float scalar:{value.x,value.y,value.z,value.rhw})std::cout<<std::bit_cast<std::uint32_t>(scalar)<<' ';
    std::cout<<value.color<<' '<<std::bit_cast<std::uint32_t>(value.u)<<' '<<std::bit_cast<std::uint32_t>(value.v)<<' ';
}
int main(){
    unsigned columns=0,rows=0;
    while(std::cin>>columns>>rows){
        std::array<std::uint32_t,4> bits{};for(auto& value:bits)std::cin>>value;
        const auto grid=th128::geometry526::initializeGrid(columns,rows,std::bit_cast<float>(bits[0]),std::bit_cast<float>(bits[1]),
            std::bit_cast<float>(bits[2]),std::bit_cast<float>(bits[3]));assert(grid);
        for(const auto& value:grid->vertices)vertex(value);
        for(const auto& position:grid->positions)for(const float value:position)std::cout<<std::bit_cast<std::uint32_t>(value)<<' ';
        const auto strips=th128::geometry526::rowStrips(*grid);assert(strips);
        for(const auto& value:*strips)vertex(value);
        std::cout<<'\n';
        const auto preserved=grid->vertices[0];auto invalid=*grid;invalid.vertices.pop_back();
        assert(!th128::geometry526::rowStrips(invalid));assert(grid->vertices[0].color==preserved.color);
    }
    for(const unsigned invalid:{0u,1u,18u,std::numeric_limits<unsigned>::max()})
        assert(!th128::geometry526::initializeGrid(invalid,17,0,0,384,448));
    assert(!th128::geometry526::initializeGrid(17,17,std::numeric_limits<float>::infinity(),0,384,448));
    assert(!th128::geometry526::initializeGrid(17,17,0,0,std::numeric_limits<float>::max(),448));
}
