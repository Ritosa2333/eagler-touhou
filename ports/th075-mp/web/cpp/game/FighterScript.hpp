#pragma once
#include "Types.hpp"
namespace th075 {
enum class StoryCommand:u8 { Player=0,Opponent=1,Narrator=2,Face=10,Music=11,Wait=12,Voice=13,Route=14 };
struct StoryRecord { StoryCommand kind=StoryCommand::Player; i16 argument=-1;std::string text; };
class FighterScript {
public:
    std::array<i16,1000> labels{};
    std::vector<std::vector<StoryRecord>> blocks;std::string error;
    FighterScript(){labels.fill(-1);}
    bool load(const void*,std::size_t);
    bool parse(const void*,std::size_t);
    const StoryRecord* get(u16 label,u32 index)const;
};
}
