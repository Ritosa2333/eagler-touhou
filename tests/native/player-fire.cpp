#include "game/PlayerFireRules.hpp"
#include <bit>
#include <cstdlib>
#include <iostream>
#include <limits>

using namespace th128::player::fire;
void check(bool condition){if(!condition)std::exit(2);}
void print(const Plan& plan){
    std::cout<<std::hex<<plan.boundary<<std::dec<<' '<<plan.target<<' '<<plan.argument<<' '
        <<plan.next.previous<<' '<<plan.next.current<<' '<<std::bit_cast<std::uint32_t>(plan.next.fractional)<<' '
        <<plan.next.clockSource<<' '<<plan.next.flags<<' '<<plan.powerRaw<<' '<<plan.savedPower<<' '
        <<std::bit_cast<std::uint32_t>(plan.scale)<<'\n';
}
int main(){
    check(!head(1,{}, {-1,std::numeric_limits<int>::min(),0,0x4b6f58,1}));
    check(!head(1,{}, {-1,0,std::numeric_limits<float>::quiet_NaN(),0x4b6f58,1}));
    check(!charge({-1,0,0,0x4b6f58,1},65536,0,0,10000,0,1));
    char operation=0;
    while(std::cin>>operation){
        if(operation=='H'){
            int state=0;unsigned holding=0,pressed=0,flags=0;int previous=0,current=0;
            std::cin>>state>>holding>>pressed>>previous>>current>>flags;
            const auto plan=head(state,{std::uint16_t(holding),std::uint16_t(pressed),0},
                {previous,current,float(current),0x4b6f58,flags});
            check(bool(plan));print(*plan);
        }else{
            int held=0,busy=0,mode=0,power=0,saved=0;unsigned holding=0;
            std::cin>>held>>holding>>busy>>mode>>power>>saved;
            const auto plan=charge({held-1,held,float(held),0x4b6f58,1},holding,busy,mode,power,saved,.5f);
            check(bool(plan));print(*plan);
        }
    }
}
