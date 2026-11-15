#include "game/ItemRules.hpp"
#include <bit>
#include <cstdint>
#include <iostream>
float readFloat(){std::uint32_t value=0;std::cin>>value;return std::bit_cast<float>(value);}
int main(){
    char command=0;
    while(std::cin>>command){
        if(command=='P'){
            int value=0,state=0;std::cin>>value;const auto y=readFloat();std::cin>>state;
            const auto reward=th128::item::pointReward(value,y,state,2);if(!reward)return 4;
            std::cout<<*reward<<'\n';
        }else if(command=='M'){
            th128::item::MotionInput input;
            input.position={readFloat(),readFloat(),readFloat()};input.velocity={readFloat(),readFloat(),readFloat()};
            std::cin>>input.playerState;input.playerY=readFloat();
            const auto result=th128::item::ordinaryMotion(input);if(!result)return 2;
            std::cout<<int(result->continuation);
            for(float value:{result->position.x,result->position.y,result->position.z,result->velocity.x,result->velocity.y,result->velocity.z})
                std::cout<<' '<<std::bit_cast<std::uint32_t>(value);
            std::cout<<'\n';
        }else{
            const th128::item::Position position{readFloat(),readFloat(),readFloat()};
            int focused=0;std::cin>>focused;
            const auto result=th128::item::contactDecision(position,{0,400,0},30,50,focused);
            if(!result)return 3;std::cout<<int(*result)<<'\n';
        }
    }
}
