#include "HighResolutionClock.hpp"
namespace th10 {
Extended HighResolutionClock::counter_time(ClockEnvironment& env){
    const auto ticks=env.counter();
    const u64 difference=static_cast<u64>(ticks)-static_cast<u64>(origin);
    i64 signed_difference;std::memcpy(&signed_difference,&difference,8);
    return Extended::from_int64(signed_difference)/Extended::from_int64(frequency);
}
double HighResolutionClock::tick_time(ClockEnvironment& env){
    env.begin_period(1);const auto tick=env.milliseconds();
    auto value=Extended::from_int(static_cast<i32>(tick));
    if(tick&0x80000000u)value=value+Extended::from_double(4294967296.0);
    const double stored=value.to_double();env.end_period(1);return stored;
}
Extended HighResolutionClock::locked_time(ClockEnvironment& env){
    env.enter_lock();++*env.lock_depth;
    Extended result;
    if(frequency){
        const auto value=counter_time(env);const double stored=value.to_double();
        if(value<Extended::from_double(baseline))baseline=stored;
        env.leave_lock();result=Extended::from_double(stored)-Extended::from_double(baseline);
    }else{
        const double stored=tick_time(env);
        // The locked fallback compares the stored millisecond count directly to
        // the baseline. Preserve this original ordering and its storage rounding.
        if(Extended::from_double(stored)<Extended::from_double(baseline))baseline=stored;
        const auto value=(Extended::from_double(stored)-Extended::from_double(baseline)*Extended::from_double(1000.0))*Extended::from_double(.001);
        const double output=value.to_double();env.leave_lock();result=Extended::from_double(output);
    }
    --*env.lock_depth;return result;
}
Extended HighResolutionClock::time(ClockEnvironment& env){
    const auto value=frequency?counter_time(env):Extended::from_double(tick_time(env))*Extended::from_double(.001);
    if(value<Extended::from_double(baseline))baseline=value.to_double();
    return value-Extended::from_double(baseline);
}
}
