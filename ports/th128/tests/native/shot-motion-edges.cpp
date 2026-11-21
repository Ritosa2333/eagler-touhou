#include "game/PlayerShotTickRules.hpp"
#include <bit>
#include <cassert>
#include <iostream>
#include <string>
unsigned word(const auto& bytes,unsigned offset){return unsigned(bytes[offset])|unsigned(bytes[offset+1])<<8|unsigned(bytes[offset+2])<<16|unsigned(bytes[offset+3])<<24;}
int main(){
    std::string encoded;unsigned horizontal,vertical,depth;
    while(std::cin>>encoded>>horizontal>>vertical>>depth){
        th128::playershot::ShotTickPlan plan;plan.continuation=th128::playershot::ShotTickContinuation::NeedsMotion;
        assert(encoded.size()==240);for(unsigned index=0;index<120;++index)plan.next[index]=std::uint8_t(std::stoul(encoded.substr(index*2,2),nullptr,16));
        const auto next=th128::playershot::completeOrdinaryShotMotion(plan,{std::bit_cast<float>(horizontal),std::bit_cast<float>(vertical),std::bit_cast<float>(depth)});
        if(!next){std::cerr<<"false unsupported motion at X bits="<<word(plan.next,0x14)<<'\n';return 1;}
        std::cout<<word(next->next,0x14)<<' '<<word(next->next,0x18)<<' '<<word(next->next,0x1c)<<'\n';
    }
}
