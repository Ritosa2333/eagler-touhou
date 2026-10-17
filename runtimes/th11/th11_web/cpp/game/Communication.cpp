#include "Communication.hpp"
#include <algorithm>
namespace th11 {
void Communication::reward(GameEconomy& economy,i32 amount,const float* rate)noexcept {
    economy.communication=std::min(wrapping_add(economy.communication,amount),13000);
    reward_delay.set(10,rate);
}
void Communication::update(GameEconomy& economy,float y)noexcept {
    auto& value=economy.communication;
    if(y<128){
        if(value<10000)value=std::min(wrapping_add(value,5000),10000);
        else value=wrapping_add(value,y<64?16:y<80?12:9);
    }else value=std::max(wrapping_add(value,-30),0);
    if(reward_delay.current<=0)secondary=std::max(wrapping_add(secondary,-30),0);
    reward_delay.advance(-1);
    value=std::min(value,13000);
    if(secondary>=value)secondary=value;
}
}
