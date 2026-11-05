#pragma once
#include <bitset>
#include <vector>

namespace th09::multiplayer {
// Irreversible output ownership, beside the retained frame and outside world
// checkpoints: restoring game state cannot unplay a one-shot sound.
class FrameAudio {
    std::bitset<54> played_;
public:
    struct Event {int kind,id;float value;};
    void Clear(){played_.reset();}
    template<class Output> void Emit(const std::vector<Event>& events,bool confirmed,Output output){
        const auto alreadyPlayed=played_;
        for(const auto& event:events){
            if(event.kind>1||event.kind<0||event.id<0||event.id>=54)continue;
            // Hurt, critical health and match results describe confirmed
            // outcomes. Ordinary one-shot feedback can lead confirmation.
            if(!confirmed&&(event.id==4||event.id==48||event.id==49))continue;
            if(alreadyPlayed.test(event.id))continue;
            output(event);played_.set(event.id);
        }
        // Retain all requests in the first pass for original pan averaging;
        // suppress this ID only on subsequent resimulation/confirmation passes.
    }
};
}
