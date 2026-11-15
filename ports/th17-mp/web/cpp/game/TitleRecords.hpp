#pragma once
#include "Types.hpp"
#include <array>
namespace th17 {
// Original score records contain one completion value per character, animal
// and difficulty. Menu logic consumes values, independently of storage layout.
struct TitleRecords {
    std::array<std::array<i32,5>,9> cleared{};
    std::array<std::array<std::array<u8,8>,5>,9> stage_available{};
    bool extra(i32 character,i32 animal=-1)const noexcept{
        if(character<0||character>=3||animal>=3)return false;
        for(i32 a=0;a<3;a++)if(animal<0||animal==a)for(i32 d=0;d<4;d++)if(cleared[character*3+a][d])return true;
        return false;
    }
    bool all_cleared(i32 difficulty,i32 character=-1)const noexcept{
        if(difficulty<0||difficulty>=5||character>=3)return false;
        for(i32 c=0;c<3;c++)if(character<0||character==c)for(i32 a=0;a<3;a++)if(!cleared[c*3+a][difficulty])return false;
        return true;
    }
};
}
