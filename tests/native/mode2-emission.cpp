#include "game/BulletRules.hpp"
#include <array>
#include <bit>
#include <cassert>
#include <iostream>

int main(){
    int count=0,layers=0,mode=0,index=0,layer=0;
    while(std::cin>>count>>layers>>mode>>index>>layer){
        std::array<float,5> values{};
        for(auto& value:values){std::uint32_t bits=0;std::cin>>bits;value=std::bit_cast<float>(bits);}
        auto emitter=th128::bullet::resetEmitter();emitter.count=std::int16_t(count);emitter.layers=std::int16_t(layers);
        emitter.aimMode=std::uint16_t(mode);emitter.angle=values[0];emitter.spacing=values[1];emitter.speed=values[2];emitter.endSpeed=values[3];
        const auto geometry=th128::bullet::emissionPrefix(emitter,index,layer,values[4]);
        if(!geometry){std::cout<<"unsupported\n";continue;}
        for(const auto value:{geometry->rawAngle,geometry->angle,geometry->speed,geometry->velocityX,geometry->velocityY})
            std::cout<<std::bit_cast<std::uint32_t>(value)<<' ';
        std::cout<<'\n';
    }
}
