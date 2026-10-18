#pragma once
#include "Types.hpp"
#include <array>
namespace th15 {
// Logical completion counts shared by title unlocks and clear emblems. Save
// storage supplies these values; the menu never reads executable memory.
struct TitleRecords {
    std::array<std::array<std::array<i32,5>,2>,4> clears{};
    std::array<std::array<std::array<bool,6>,5>,4> practice_stages{};
    bool extra_character(i32 character)const noexcept{for(const auto& mode:clears[character])for(i32 difficulty=0;difficulty<4;difficulty++)if(mode[difficulty])return true;return false;}
    bool extra_available()const noexcept{for(i32 character=0;character<4;character++)if(extra_character(character))return true;return false;}
    bool cleared(i32 character,i32 difficulty,u32 mode_flags)const noexcept{return clears[character][(mode_flags&0x300)==0][difficulty]!=0;}
    std::array<bool,5> all_characters(u32 mode_flags)const noexcept{std::array<bool,5> result{};for(i32 difficulty=0;difficulty<5;difficulty++){result[difficulty]=true;for(i32 character=0;character<4;character++)if(!cleared(character,difficulty,mode_flags))result[difficulty]=false;}return result;}
};
}
