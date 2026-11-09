#pragma once
#include <cstdint>
#include "../Original/OriginalFeatures.h"

namespace th5w { namespace coop {
// The shared world advances once. Only player-owned objects and resources
// change when CStage binds another seat. Scores and rank remain team state.
struct Stock {
 int lives=3,bombs=3,power=1,overflow=0,fraction=0,deaths=0;
 bool ghost=false,offline=false;
};
struct Input { unsigned short buttons=0; float x=0,y=0; int motionMode=0; };
struct Session {
 int count=1,local=0,characters[3]{0,1,2},seed=0,difficulty=1;
 int initialSpares=2,initialBombs=3;
 uint64_t random=0;
 Input input[3]{};
 Stock carry[3]{};
 unsigned int combatRevision=original::OriginalBombAndRage;
 bool continuing=false,finished=false,challenge=false,localVisibility=false;
};
inline constexpr int MenuLeft=123,MenuTop=178,MenuWidth=215,MenuRowHeight=35,MenuRowStep=39,MenuRows=3;
Session& Run();
void CaptureSpectatorStage();
void Reset();
} }
