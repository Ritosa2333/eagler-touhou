#include "RunPlayTime.hpp"
#include <cmath>
namespace th15 {
bool RunPlayTime::fail(const char* why){if(error.empty())error=why;return false;}
bool RunPlayTime::rebase(){if(!error.empty())return false;return clock.seconds(origin)||fail("Run clock unavailable");}
bool RunPlayTime::account(i32 destination){
 if(!error.empty())return false;if(progress.transition||destination==-1||destination==3)return true;
 double now;if(!clock.seconds(now))return fail("Run elapsed clock unavailable");const double duration=now-origin;
 if(duration>=0){const double units=duration*100.;if(!std::isfinite(units)||units>=18446744073709551616.)return fail("Run elapsed clock outside unsigned counter range");if(!records.add_play_time(progress.character+progress.subcharacter,(player.mode_flags&0x300)==0,u64(units)))return fail("Run elapsed record update failed");}
 return rebase();
}
}
