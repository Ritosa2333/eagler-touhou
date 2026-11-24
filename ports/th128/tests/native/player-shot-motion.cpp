#include "game/PlayerShotMotion.hpp"
#include <bit>
#include <cstdint>
#include <iostream>

int main(){
    std::uint32_t angle=0,speed=0;
    while(std::cin>>angle>>speed){
        const auto result=th128::playershot::computeShotMotion(std::bit_cast<float>(angle),std::bit_cast<float>(speed));
        if(!result)return 2;
        std::cout<<std::bit_cast<std::uint32_t>(result->normalizedAngle)<<' '
            <<std::bit_cast<std::uint32_t>(result->velocity.x)<<' '<<std::bit_cast<std::uint32_t>(result->velocity.y)<<' '
            <<std::bit_cast<std::uint32_t>(result->velocity.z)<<'\n';
    }
    return 0;
}
