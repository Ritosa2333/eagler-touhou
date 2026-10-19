#include "../../cpp/multiplayer/RecoverySchedule.hpp"
#include <cstdio>
#include <cstdlib>
#define CHECK(x) do {if(!(x)){std::fprintf(stderr,"%d: %s\n",__LINE__,#x);std::abort();}}while(0)
int main(){
    th09::multiplayer::RecoverySchedule schedule;
    CHECK(!schedule.Enabled());CHECK(!schedule.Schedule(1));
    schedule.Configure(true);const auto first=schedule.Schedule(1);CHECK(first);
    CHECK(!schedule.Schedule(1));CHECK(!schedule.Take(first,2));CHECK(schedule.Take(first,1));
    CHECK(!schedule.Take(first,1));
    // Pause/resume within one loop epoch cannot revive a cancelled callback.
    const auto paused=schedule.Schedule(1);schedule.Cancel();const auto resumed=schedule.Schedule(1);
    CHECK(paused&&resumed&&paused!=resumed);CHECK(!schedule.Take(paused,1));CHECK(schedule.Take(resumed,1));
    // An old loop's timer cannot clear a replacement loop's pending work.
    const auto old=schedule.Schedule(1),replacement=schedule.Schedule(2);
    CHECK(!schedule.Take(old,2));CHECK(!schedule.Take(replacement,1));CHECK(schedule.Take(replacement,2));
    const auto disabled=schedule.Schedule(2);schedule.Configure(false);CHECK(!schedule.Take(disabled,2));
    CHECK(!schedule.Schedule(2));
    std::puts("After-present recovery ownership: pause/resume, stop/start, deduplication and disable PASS");
}
