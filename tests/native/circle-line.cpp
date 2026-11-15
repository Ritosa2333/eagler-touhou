#include "game/CircleFan.hpp"
#include <bit>
#include <cassert>
#include <iostream>
int main(){float x=0,y=0,radius=0,angle=0;int count=0;while(std::cin>>x>>y>>radius>>angle>>count){const auto result=th128::geometry::circleLine(x,y,radius,angle,count,0x8040c0ffu);assert(result);bool comma=false;for(const auto& vertex:*result){if(comma)std::cout<<' ';comma=true;std::cout<<std::bit_cast<std::uint32_t>(vertex.x)<<' '<<std::bit_cast<std::uint32_t>(vertex.y)<<' '<<std::bit_cast<std::uint32_t>(vertex.z)<<' '<<std::bit_cast<std::uint32_t>(vertex.rhw)<<' '<<vertex.color;}std::cout<<'\n';}}
