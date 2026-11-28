#include "game/BulletExtension4.hpp"
#include <bit>
#include <cassert>
#include <iostream>

float readFloat(){std::uint32_t bits=0;std::cin>>bits;return std::bit_cast<float>(bits);}
int main(){
    using namespace th128::bullet;char command=0;
    while(std::cin>>command){
        if(command=='I'){
            ExtensionRaw record;record.type=4;record.parameterR=readFloat();record.parameterS=readFloat();std::cin>>record.parameterA;
            const auto result=initializeExtension4(record,.5f,{10,100,.1f},{0,400,0});assert(result);
            std::cout<<result->active<<' '<<result->time<<' '<<result->period;
            for(float value:{result->magnitude,result->direction,result->vector.x,result->vector.y,result->vector.z})std::cout<<' '<<std::bit_cast<std::uint32_t>(value);
            std::cout<<'\n';
        }else if(command=='T'){
            int time=0,period=0;std::cin>>time>>period;const auto accel=readFloat(),direction=readFloat(),speed=readFloat();
            Position velocity{readFloat(),readFloat(),readFloat()};ExtensionRaw record;record.type=4;record.parameterR=accel;record.parameterS=direction;record.parameterA=period;
            auto initial=initializeExtension4(record,.5f,{10,100,.1f},{0,400,0});assert(initial);initial->time=time;initial->fraction=float(time);initial->previous=time-1;
            const auto result=stepExtension4(*initial,speed,.5f,velocity);assert(result);
            std::cout<<result->finished<<' '<<result->next.active<<' '<<result->next.previous<<' '<<result->next.time<<' '<<std::bit_cast<std::uint32_t>(result->next.fraction)
                <<' '<<std::bit_cast<std::uint32_t>(result->speed)<<' '<<std::bit_cast<std::uint32_t>(result->angle);
            for(float value:{result->velocity.x,result->velocity.y,result->velocity.z})std::cout<<' '<<std::bit_cast<std::uint32_t>(value);
            std::cout<<'\n';
        }else return 2;
    }
}
