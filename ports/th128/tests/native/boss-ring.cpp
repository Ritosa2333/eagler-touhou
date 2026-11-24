#include "game/BulletExtension1.hpp"
#include <bit>
#include <cassert>
#include <iostream>
float readFloat(){unsigned bits;std::cin>>bits;return std::bit_cast<float>(bits);}
int main(){
    using namespace th128::bullet;char command;
    while(std::cin>>command){
        if(command=='G'){
            auto emitter=resetEmitter();emitter.type=4;emitter.aimMode=3;emitter.angle=readFloat();assert(setSpawnRadius(emitter,readFloat()));
            emitter.positionX=10;emitter.positionY=144;emitter.speed=1.5;
            const auto geometry=emissionPrefix(emitter,0,0,0);assert(geometry);const auto position=spawnPositionPrefix(emitter,*geometry);assert(position);
            for(const float value:{position->x,position->y,position->z,geometry->velocityX,geometry->velocityY})std::cout<<std::bit_cast<unsigned>(value)<<' ';
            std::cout<<0<<'\n';
        }else{
            int time;std::cin>>time;const float angle=readFloat();Extension1 state{true,time-1,time,float(time)};
            const auto result=stepExtension1(state,1.5,angle,{.5,1.5,0});assert(result);
            std::cout<<result->next.active<<' '<<result->next.previous<<' '<<result->next.time<<' '<<std::bit_cast<unsigned>(result->next.fraction);
            for(const float value:{result->velocity.x,result->velocity.y,result->velocity.z})std::cout<<' '<<std::bit_cast<unsigned>(value);std::cout<<'\n';
        }
    }
}
