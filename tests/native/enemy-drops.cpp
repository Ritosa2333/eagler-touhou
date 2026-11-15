#include "game/EnemyDropRules.hpp"
#include <bit>
#include <cassert>
#include <iostream>
#include <limits>

int main(){
    using namespace th128::enemy;
    int opcode=0;
    while(std::cin>>opcode){
        DropState drops;
        for(auto& value:drops.words){std::uint32_t input=0;std::cin>>input;value=std::bit_cast<std::int32_t>(input);}
        std::uint32_t width=0,height=0;std::cin>>width>>height;
        drops.width=std::bit_cast<float>(width);drops.height=std::bit_cast<float>(height);
        if(opcode==406)resetAdditionalDrops(drops);
        else if(opcode==407){int index=0,value=0;std::cin>>index>>value;assert(setDropWord(drops,index,value));}
        else if(opcode==408){std::cin>>width>>height;assert(setDropSpread(drops,std::bit_cast<float>(width),std::bit_cast<float>(height)));}
        else return 2;
        for(auto value:drops.words)std::cout<<std::bit_cast<std::uint32_t>(value)<<' ';
        std::cout<<std::bit_cast<std::uint32_t>(drops.width)<<' '<<std::bit_cast<std::uint32_t>(drops.height)<<'\n';
    }
    DropState guarded;guarded.words[0]=1;
    assert(!setDropWord(guarded,-1,2)&&!setDropWord(guarded,11,2)&&guarded.words[0]==1);
    assert(!setDropSpread(guarded,std::numeric_limits<float>::infinity(),24)&&guarded.width==0);
}
