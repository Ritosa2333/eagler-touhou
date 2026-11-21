#include "game/EnemyRandomMovement.hpp"
#include <bit>
#include <cassert>
#include <iostream>
int main(){
    using namespace th128::enemy;
    char operation=0;
    while(std::cin>>operation){
        int duration=0,mode=0,elapsed=0;std::uint32_t angleBits=0,speedBits=0;
        std::cin>>duration>>mode>>elapsed>>angleBits>>speedBits;
        const float angle=std::bit_cast<float>(angleBits),speed=std::bit_cast<float>(speedBits);
        Polar polar{.5f,1,0xffffffffu};PolarInterpolation interpolation;
        assert(beginRandomMovement(polar,interpolation,angle,duration,mode,speed));
        if(operation=='I'){
            std::cout<<std::bit_cast<std::uint32_t>(interpolation.startAngle)<<' '
                <<std::bit_cast<std::uint32_t>(interpolation.startSpeed)<<' '
                <<std::bit_cast<std::uint32_t>(interpolation.targetAngle)<<' '
                <<std::bit_cast<std::uint32_t>(interpolation.targetSpeed)<<' '
                <<interpolation.previous<<' '<<interpolation.elapsed<<' '<<std::bit_cast<std::uint32_t>(interpolation.elapsedFloat)<<' '
                <<interpolation.duration<<' '<<interpolation.mode<<' '<<polar.modeFlags<<' '
                <<std::bit_cast<std::uint32_t>(polar.angle)<<' '<<std::bit_cast<std::uint32_t>(polar.speed)<<'\n';
        }else{
            interpolation.elapsed=elapsed;interpolation.elapsedFloat=float(elapsed);interpolation.previous=elapsed-1;
            assert(stepPolarInterpolation(polar,interpolation));
            std::cout<<std::bit_cast<std::uint32_t>(polar.angle)<<' '<<std::bit_cast<std::uint32_t>(polar.speed)<<' '
                <<interpolation.previous<<' '<<interpolation.elapsed<<' '<<std::bit_cast<std::uint32_t>(interpolation.elapsedFloat)<<' '
                <<interpolation.duration<<' '<<interpolation.mode<<'\n';
        }
    }
}
