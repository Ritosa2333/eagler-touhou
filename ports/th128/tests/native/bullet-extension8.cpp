#include "game/BulletExtension8.hpp"
#include <bit>
#include <cassert>
#include <iostream>
int main(){int time=0,period=0;float acceleration=0,rotation=0,speed=0,angle=0;while(std::cin>>time>>period>>acceleration>>rotation>>speed>>angle){
 th128::bullet::ExtensionRaw record;record.type=8;record.parameterA=period;record.parameterR=acceleration;record.parameterS=rotation;
 auto state=th128::bullet::initializeExtension8(record);assert(state);state->time=time;state->previous=time-1;state->fraction=float(time);const auto result=th128::bullet::stepExtension8(*state,speed,angle,{.5f,1.5f,0});assert(result);
 std::cout<<int(result->finished)<<' '<<(result->finished?0:8)<<' '<<result->next.time;for(const float value:{result->speed,result->angle,result->velocity.x,result->velocity.y,result->velocity.z})std::cout<<' '<<std::bit_cast<std::uint32_t>(value);std::cout<<'\n';
}}
