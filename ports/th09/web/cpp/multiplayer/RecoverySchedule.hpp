#pragma once
#include <cstdint>

namespace th09::multiplayer {
// One asynchronous recovery owner per loop epoch. An old timer cannot consume
// a replacement loop's pending work, including stop/start before timer delivery.
class RecoverySchedule {
    bool enabled_=false,pending_=false;
    std::uint32_t epoch_=0,serial_=0,ticket_=0;
public:
    void Configure(bool enabled){enabled_=enabled;Cancel();}
    bool Enabled()const{return enabled_;}
    void Cancel(){pending_=false;}
    std::uint32_t Schedule(std::uint32_t epoch){
        if(!enabled_||(pending_&&epoch_==epoch))return 0;
        ticket_=++serial_;if(!ticket_)ticket_=++serial_;
        epoch_=epoch;pending_=true;return ticket_;
    }
    bool Take(std::uint32_t ticket,std::uint32_t epoch){
        if(!pending_||epoch_!=epoch||ticket_!=ticket)return false;
        pending_=false;return true;
    }
};
}
