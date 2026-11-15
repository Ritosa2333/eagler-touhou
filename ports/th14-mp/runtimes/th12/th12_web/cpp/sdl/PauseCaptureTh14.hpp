#pragma once
#include "../game/GameSession.hpp"

namespace th11 {
struct PauseCaptureSourceTh14 {
    bool text_intermediate=false;
    i32 rectangle[4]{};
};

inline PauseCaptureSourceTh14 th14_pause_capture_source(const GameSession& session,float scale=1.f){
    // 4321d6 publishes GUI+194 after MSG construction; 430df4 clears it
    // at MSG completion. The port retains its Dialogue object between entries.
    const bool dialogue=session.state.phase==GameSessionPhase::paused&&session.battle
        &&session.battle->dialogue&&session.battle->dialogue->active;
    // Immediate Pause448bf0 selects text texture3/+1b0 during dialogue;
    // otherwise it copies the backbuffer/+1b4. Deferred end capture is separate.
    PauseCaptureSourceTh14 result;
    result.text_intermediate=dialogue;
    result.rectangle[0]=i32((dialogue?128.f:32.f)*scale);
    result.rectangle[1]=i32(16.f*scale);
    result.rectangle[2]=i32((dialogue?512.f:416.f)*scale);
    result.rectangle[3]=i32(464.f*scale);
    return result;
}
}
