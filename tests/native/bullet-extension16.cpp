#include "game/BulletExtension16.hpp"
#include <bit>
#include <cassert>
#include <iostream>

float readFloat(){std::uint32_t bits=0;std::cin>>bits;return std::bit_cast<float>(bits);}
void output(const th128::bullet::Extension16& state,float speed,float angle,th128::bullet::Position velocity){
    std::cout<<state.active<<' '<<state.previous<<' '<<state.time<<' '<<std::bit_cast<std::uint32_t>(state.fraction)
        <<' '<<state.period<<' '<<state.limit<<' '<<state.count<<' '<<state.mode<<' '<<state.angleMode
        <<' '<<std::bit_cast<std::uint32_t>(state.targetSpeed)<<' '<<std::bit_cast<std::uint32_t>(state.angleOffset)
        <<' '<<std::bit_cast<std::uint32_t>(speed)<<' '<<std::bit_cast<std::uint32_t>(angle);
    for(const auto value:{velocity.x,velocity.y,velocity.z})std::cout<<' '<<std::bit_cast<std::uint32_t>(value);
    std::cout<<'\n';
}
int main(){
    using namespace th128::bullet;char command=0;
    while(std::cin>>command){
        if(command=='I'){
            ExtensionRaw record;record.type=16;record.parameterR=readFloat();record.parameterS=readFloat();
            std::cin>>record.parameterA>>record.parameterB>>record.parameterC;record.parameterD=0;
            const auto state=initializeExtension16(record,3.,.5f);assert(state);output(*state,3.,.5f,{});
        }else if(command=='T'){
            Extension16 state;state.active=true;
            std::cin>>state.time>>state.period>>state.limit>>state.count;state.previous=state.time-1;state.fraction=float(state.time);
            const auto speed=readFloat(),angle=readFloat();state.targetSpeed=speed;state.angleOffset=readFloat();state.mode=2;state.angleMode=0;
            int sound=-1;std::cin>>sound;
            const auto result=stepExtension16(state,speed,angle,{},{10,100,.1f},{0,400,0},sound);assert(result);
            if(result->soundRequired){std::cout<<"sound "<<sound<<'\n';continue;}
            output(result->next,result->speed,result->angle,result->velocity);
        }else return 2;
    }
}
