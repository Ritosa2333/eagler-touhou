#include "game/EnemyRules.hpp"
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

int main(){
    char command=0;
    while(std::cin>>command){
        std::uint32_t flags=0;std::cin>>flags;
        if(command=='B'){
            std::int32_t retained=0,slot=0;std::cin>>retained>>slot;
            th128::enemy::BossSlots manager;std::cin>>manager.flags;
            for(auto& owner:manager.owners)std::cin>>owner;
            assert(th128::enemy::setBossSlot(manager,flags,retained,0x340000,slot));
            std::cout<<flags<<' '<<retained<<' '<<manager.flags;
            for(const auto owner:manager.owners)std::cout<<' '<<owner;
        }else if(command=='L'){
            std::int32_t life=0;std::cin>>life;
            std::array<std::uint32_t,6> words{};for(auto& word:words)std::cin>>word;
            th128::enemy::setLife(words,flags,life);
            std::cout<<flags;for(const auto word:words)std::cout<<' '<<word;
        }else return 2;
        std::cout<<'\n';
    }
    th128::enemy::BossSlots invalid;invalid.flags=123;invalid.owners[0]=456;
    std::uint32_t flags=0x1000000;std::int32_t retained=8;
    assert(!th128::enemy::setBossSlot(invalid,flags,retained,0x340000,-1));
    assert(invalid.flags==123&&invalid.owners[0]==456&&flags==0x1000000&&retained==8);
    retained=0;
    assert(!th128::enemy::setBossSlot(invalid,flags,retained,0x340000,8));
    assert(invalid.flags==123&&invalid.owners[0]==456&&flags==0x1000000&&retained==0);
}
