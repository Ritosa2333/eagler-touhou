#include "game/EnemyRandomMovement.hpp"
#include <bit>
#include <cassert>
#include <iostream>
#include <limits>
int main(){
    unsigned seed=0;std::uint32_t calls=0;
    while(std::cin>>seed>>calls){
        std::array<std::uint32_t,7> words{};for(auto& word:words)std::cin>>word;
        const th128::enemy::Position position{std::bit_cast<float>(words[0]),std::bit_cast<float>(words[1]),0};
        const th128::enemy::MovementArea area{std::bit_cast<float>(words[3]),std::bit_cast<float>(words[4]),
            std::bit_cast<float>(words[5]),std::bit_cast<float>(words[6])};
        th128::Random random;random.seed=std::uint16_t(seed);random.calls=calls;
        const auto direction=th128::enemy::randomMovementDirection(position,area,std::bit_cast<float>(words[2]),random);assert(direction);
        std::cout<<std::bit_cast<std::uint32_t>(*direction)<<' '<<random.seed<<' '<<random.calls<<'\n';
        const auto before=random;
        assert(!th128::enemy::randomMovementDirection(position,area,std::numeric_limits<float>::quiet_NaN(),random));
        assert(random.seed==before.seed&&random.calls==before.calls);
    }
}
