#include "game/PlayerRespawnRules.hpp"
#include <bit>
#include <cassert>
#include <iostream>

int main(){
    char command=0;
    while(std::cin>>command){
        if(command=='E'){
            int elapsed=0;std::cin>>elapsed;const auto plan=th128::player::respawn::entryPlan(elapsed);assert(plan);
            std::cout<<plan->rawVertical<<' '<<std::bit_cast<std::uint32_t>(plan->pixelVertical)<<' '<<std::bit_cast<std::uint32_t>(plan->laserRadius)<<'\n';
        }else if(command=='C'){
            th128::player::respawn::ControlState state;std::cin>>state.mode04>>state.value08;
            const auto plan=th128::player::respawn::postFieldControl(state);assert(plan&&!plan->requiredSlot);
            std::cout<<plan->next.mode04;for(auto irq:plan->next.pending)std::cout<<' '<<irq;std::cout<<'\n';
        }else if(command=='X'){
            th128::player::respawn::ControlState state;state.targets[3]=0x123456;
            const auto plan=th128::player::respawn::postFieldControl(state);assert(plan&&plan->requiredSlot==3&&plan->requiredInterrupt==3);
            assert(plan->next.pending[2]==7&&plan->next.pending[3]==0);std::cout<<"boundary\n";
        }else return 2;
    }
}
