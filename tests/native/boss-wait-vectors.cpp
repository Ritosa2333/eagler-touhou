#include "game/EnemyRules.hpp"
#include <iostream>
int main(){
    th128::enemy::BossSlots manager;
    while(std::cin>>manager.owners[0]){
        for(std::size_t index=1;index<manager.owners.size();++index)std::cin>>manager.owners[index];
        std::cout<<th128::enemy::bossWaitPending(manager)<<'\n';
    }
}
