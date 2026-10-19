// Prediction-quality probe using the actual common core, not a game benchmark.
// Each trace includes releases; a longer hold is not automatically better.
#include <eagler/netplay/NetplayCore.hpp>
#include <array>
#include <cstdio>
#include <cstdlib>
using namespace Netplay;
constexpr unsigned frames=2400;
unsigned keys(unsigned pattern,unsigned f){
    constexpr unsigned directions[]={64,80,16,144,128,160,32,96};
    if(pattern==0)return directions[(f/90)%8]; // long holds, all eight directions
    if(pattern==1)return f%12<2?directions[(f/12)%8]:0; // short correction taps
    if(pattern==2)return f%60<40?directions[(f/60)%8]:0; // move, then release
    return f%4<2?64:128; // frequent reversal, no extrapolated turn
}
int main(){
    const char* names[]={"long-hold","short-tap","move-release","rapid-reverse"};
    std::printf("[");bool comma=false;
    for(unsigned pattern=0;pattern<4;++pattern)for(unsigned horizon:{0u,3u,6u}){
        RollbackCore core,longCore;std::array<int,frames> received;received.fill(-1);CoreConfig c;c.sessionId=9;c.localPlayer=0;c.playerCount=2;
        c.inputDelay=0;c.maxRollbackFrames=8;c.predictableButtons=1|4|0xf0;c.directionButtons=0xf0;c.maxDirectionPredictionFrames=horizon?horizon:3;
        if(!core.Reset(c))return 1;c.maxDirectionPredictionFrames=6;if(!longCore.Reset(c))return 1;
        unsigned tested=0,mismatch=0,falseStop=0,overHold=0,wrongDirection=0,stalls=0;
        for(unsigned f=0;f<frames;++f){
            if(!core.ScheduleLocalInput(f,0)||!longCore.ScheduleLocalInput(f,0))return 2;
            // Deterministic 3-6 tick one-way delay with reorder; identical for
            // all candidates. No continuous touch samples enter this probe.
            for(unsigned lag=3;lag<=6&&lag<=f;++lag){const unsigned source=f-lag;if(lag!=3+(source*17)%4)continue;core.SubmitRemoteInput(1,source,std::uint16_t(keys(pattern,source)));longCore.SubmitRemoteInput(1,source,std::uint16_t(keys(pattern,source)));received[source]=int(keys(pattern,source));}
            auto d=core.PrepareFrame(f);
            // Candidate only: extend a direction after six consecutive actual
            // same-direction inputs. Never infer stability from predictions.
            if(horizon==0){int latest=int(f)-1;while(latest>=0&&received[latest]<0)--latest;
                bool stable=latest>=5&&received[latest]!=0;
                for(int n=1;stable&&n<6;++n)stable=received[latest-n]==received[latest];
                if(stable)d=longCore.PrepareFrame(f);
            }if(!d.canAdvance){++stalls;continue;}
            const unsigned predicted=d.inputs[1].buttons&0xf0,actual=keys(pattern,f);++tested;
            if(predicted!=actual){++mismatch;if(!predicted)++falseStop;else if(!actual)++overHold;else ++wrongDirection;}
        }
        if(comma)std::printf(",");comma=true;
        std::printf("{\"trace\":\"%s\",\"holdFrames\":%u,\"tested\":%u,\"mismatches\":%u,\"falseStop\":%u,\"overHoldAfterRelease\":%u,\"wrongDirection\":%u,\"stalls\":%u}",names[pattern],horizon,tested,mismatch,falseStop,overHold,wrongDirection,stalls);
    }
    std::puts("]");return 0;
}
