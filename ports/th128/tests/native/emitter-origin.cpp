#include "game/BulletRules.hpp"
#include <bit>
#include <iostream>
int main(){int opcode=0;float first=0,second=0;while(std::cin>>opcode>>first>>second){
    auto emitter=th128::bullet::resetEmitter();
    if(opcode==503){th128::bullet::setEmissionOffset(emitter,first,second);std::cout<<std::bit_cast<std::uint32_t>(emitter.offsetX)<<' '<<std::bit_cast<std::uint32_t>(emitter.offsetY)<<" 0\n";}
    else if(opcode==523){th128::bullet::setPolarEmissionOffset(emitter,first,second);std::cout<<std::bit_cast<std::uint32_t>(emitter.offsetX)<<' '<<std::bit_cast<std::uint32_t>(emitter.offsetY)<<" 0\n";}
    else if(opcode==525){th128::bullet::setAbsoluteEmissionOrigin(emitter,first,second);std::cout<<std::bit_cast<std::uint32_t>(emitter.absoluteX)<<' '<<std::bit_cast<std::uint32_t>(emitter.absoluteY)<<' '<<std::bit_cast<std::uint32_t>(float(emitter.absoluteOrigin))<<'\n';}
    else {th128::bullet::setEmissionOffset(emitter,32,-12);if(first)th128::bullet::setAbsoluteEmissionOrigin(emitter,-176,48);const auto position=th128::bullet::emissionOrigin(emitter,{20,100,.1f});std::cout<<std::bit_cast<std::uint32_t>(position.x)<<' '<<std::bit_cast<std::uint32_t>(position.y)<<' '<<std::bit_cast<std::uint32_t>(position.z)<<'\n';}
}}
