#pragma once
#include "MatchRules.hpp"
#include <vector>
namespace th09 {
struct StageRoute {i16 stage=0,background=0,opponent=0,opening=0,victory=0,defeat=0,music=0,weight=0;};
static_assert(sizeof(StageRoute)==16);
struct StageSelectionState {
    GameMode mode=GameMode::story;i32 difficulty=0,characters[2]{},stage=0,round=0,selector=-1,background=0,cpu_levels[2]{};
    float lives=0;u8 continued=0,all_unlocked=0;std::array<u8,16> visited{},unlocked{};
    StageRoute route;
};
struct StageSelectionActions {virtual ~StageSelectionActions()=default;virtual void encounter(i32 character)=0;};
class StageSelection {
public:
    static std::vector<StageRoute> candidates(const StageSelectionState&);
    static bool select(StageSelectionState&,Rng&,StageSelectionActions&);
    static i32 story_policy(i32 difficulty,i32 stage,i32 retry_index)noexcept;
};
}
