#include "game/CircleFan.hpp"
#include <bit>
#include <cassert>
#include <iostream>
int main(){
    float horizontal,vertical,radius,angle;int count;
    while(std::cin>>horizontal>>vertical>>radius>>angle>>count){
        const auto vertices=th128::geometry::circleFan(horizontal,vertical,radius,angle,count,0x80ff00ff,0x01ff50ff);assert(vertices);
        bool comma=false;for(const auto& vertex:*vertices)for(const auto bits:{std::bit_cast<unsigned>(vertex.x),std::bit_cast<unsigned>(vertex.y),std::bit_cast<unsigned>(vertex.z),std::bit_cast<unsigned>(vertex.rhw),vertex.color}){
            if(comma)std::cout<<' ';comma=true;std::cout<<bits;
        }
        std::cout<<'\n';
    }
}
