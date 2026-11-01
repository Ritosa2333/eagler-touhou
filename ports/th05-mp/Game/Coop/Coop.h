#pragma once
#include <cstdint>

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
 Input input[3]{};
 Stock carry[3]{};
 bool continuing=false,finished=false,challenge=false,localVisibility=false;
};
Session& Run();
void Reset();
} }
