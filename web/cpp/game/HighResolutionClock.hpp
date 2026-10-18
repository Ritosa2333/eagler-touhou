#pragma once
#include "Arithmetic.hpp"
namespace th10 {
struct ClockEnvironment {
    u8* lock_depth;
    virtual void enter_lock()=0;
    virtual void leave_lock()=0;
    virtual i64 counter()=0;
    virtual void begin_period(u32 period)=0;
    virtual void end_period(u32 period)=0;
    virtual u32 milliseconds()=0;
};
#pragma pack(push,4)
struct HighResolutionClock {
    i64 frequency,origin;
    u8 reserved_010[0x28];
    double baseline;
    Extended locked_time(ClockEnvironment& environment);
    Extended time(ClockEnvironment& environment);
private:
    Extended counter_time(ClockEnvironment& environment);
    double tick_time(ClockEnvironment& environment);
};
#pragma pack(pop)
static_assert(sizeof(HighResolutionClock)==0x40&&offsetof(HighResolutionClock,baseline)==0x38);
}
