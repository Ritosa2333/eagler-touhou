#pragma once
#include "ReplaySession.hpp"
#include <string>
namespace th09 {
namespace multiplayer { class WorldState; }
struct ReplayMetadata {
    std::array<u8,204> configuration{};
    u32 version_time_a=0,version_time_b=0;
    u8 mode=0,versus=0,difficulty=1,health[2]{10,10},alternate[2]{};
};
// A whole run, with separate streams for each story stage or the versus match.
// Only the native .rpy serialization boundary uses original binary offsets.
class ReplayArchive {
    friend class multiplayer::WorldState;
    struct Stage {
        bool present=false;std::array<std::array<u8,32>,3> headers{};
        ReplayRecording recording;
    };
    std::array<Stage,10> stages;
    i32 active=-1;
public:
    ReplayMetadata metadata;std::string error;
    void begin(const ReplayMetadata&);
    bool begin_stage(u32 stage,const ReplayRoundSettings&);
    bool record(u32 flags,bool paused,const GameInput (&inputs)[3],const bool (&cpu)[2],u8 rate);
    ReplayFile finish(Rng&,const char* name,const char* date,i32 left_character,i32 right_character);
    u32 frame()const{return active<0?0:stages[active].recording.frame();}
    bool has_stage(u32 stage)const{return stage<10&&stages[stage].present;}
};
}
