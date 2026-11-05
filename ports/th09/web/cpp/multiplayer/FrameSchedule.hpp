#pragma once
#include "../../../portable/sdl/FrameCadence.hpp"

namespace th09::multiplayer {
// Wall-clock scheduling only; never an input queue or a simulation clock.
// A blocked rollback tick was already due. Retry that ONE tick at the next
// presentation opportunity without waiting for another 1/60 second credit,
// and without accumulating wall-clock debt during a network/recovery stall.
class FrameSchedule {
    touhou::sdl::FrameCadence cadence;
    bool retry=false;
public:
    void reset(){cadence.reset();retry=false;}
    bool retry_pending()const{return retry;}
    unsigned advance(double seconds,bool rollback=false){
        if(!rollback)retry=false;
        return retry?1:cadence.advance(seconds);
    }
    void blocked(bool rollback){
        if(!rollback){reset();return;}
        retry=true;
        cadence.debt=std::fmod(cadence.debt,touhou::sdl::FrameCadence::interval);
    }
    void complete(){retry=false;}
    void exhausted(unsigned completed,unsigned due){if(completed<due)reset();}
};
}
