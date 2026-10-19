#pragma once
#include "../../cpp/game/StageSelection.hpp"
#include <vector>
namespace stage_selection_test {
using namespace th09;
struct Fixture:StageSelectionActions {StageSelectionState state;Rng random;std::vector<i32> encounters;void encounter(i32 n)override{encounters.push_back(n);}};
struct Field{u32 original,offset,size;};
#define STAGE_FIELD(address,field) {address,offsetof(StageSelectionState,field),sizeof(StageSelectionState::field)}
inline const Field fields[]={STAGE_FIELD(0x4a7ea8,mode),STAGE_FIELD(0x4a7eac,difficulty),{0x4a7db0,offsetof(StageSelectionState,characters),4},{0x4a7de8,offsetof(StageSelectionState,characters)+4,4},STAGE_FIELD(0x4a7e8c,stage),STAGE_FIELD(0x4a7e90,round),STAGE_FIELD(0x4a7ec8,selector),STAGE_FIELD(0x4a7e84,background),{0x4a7dbc,offsetof(StageSelectionState,cpu_levels),4},{0x4a7df4,offsetof(StageSelectionState,cpu_levels)+4,4},STAGE_FIELD(0x4a7e68,visited),STAGE_FIELD(0x4a81cc,unlocked)};
#undef STAGE_FIELD
}
