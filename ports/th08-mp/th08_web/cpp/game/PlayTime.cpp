#include "PlayTime.hpp"
namespace th08 {
void accumulate_play_time(u32 (&time)[4],u32& previous,u32 now){
    if(now<previous)previous=0;u32 elapsed=now-previous;
    time[0]+=elapsed/3600000;elapsed%=3600000;time[1]+=elapsed/60000;elapsed%=60000;time[2]+=elapsed/1000;time[3]+=elapsed%1000;
    if(time[3]>=1000){time[2]+=time[3]/1000;time[3]%=1000;}
    if(time[2]>=60){time[1]+=time[2]/60;time[2]%=60;}
    if(time[1]>=60){time[0]+=time[1]/60;time[1]%=60;}previous=now;
}
}
