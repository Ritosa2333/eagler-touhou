#pragma once
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
namespace th128::ending {
struct FadeControls {std::uint32_t pauseWord=0;bool timeControlPresent=false;std::uint32_t flags60=0;float clock=0;};
struct Kind5State {bool active=false;int duration=0;int previous=-1;int current=0;float fraction=0;int alpha=0;int returnCode=1;};
class Kind5Fade {
    Kind5State state_;
    bool initialized_=false,failed_=false;
    std::string failure_;
    bool fail(std::string reason,std::string& error){if(failure_.empty())failure_=std::move(reason);failed_=true;error=failure_;return false;}
    void retire(){state_.active=false;state_.returnCode=7;}
public:
    bool start(int duration,std::string& error){
        error.clear();
        if(initialized_||failed_||duration!=60)return fail("ending selectedkind5 requires fresh originalduration60",error);
        state_=Kind5State{};state_.duration=duration;state_.active=true;initialized_=true;return true;
    }
    bool startTerminal(std::string& error){
        error.clear();
        if(initialized_||failed_)return fail("terminal kind5 requires a fresh originalduration200 task",error);
        state_=Kind5State{};state_.duration=200;state_.active=true;initialized_=true;return true;
    }
    bool tick(std::optional<FadeControls> controls,std::string& error){
        error.clear();
        if(failed_)return fail(failure_,error);
        if(!initialized_||!state_.active)return fail("ending kind5 owner is notactive",error);
        if(!controls)return fail("originalendingcontrolsnapshotUNKNOWN",error);
        const auto& control=*controls;
        if(!std::isfinite(control.clock)||control.clock<0)return fail("originalendingclockdomainUNKNOWN",error);
        if(control.pauseWord!=0){retire();return true;}
        state_.alpha=state_.current>=state_.duration?255:int(std::trunc(double(state_.fraction)*255./double(state_.duration)));
        if(state_.alpha<0)state_.alpha=0;
        if(state_.current>=state_.duration+2){retire();return true;}
        if(control.timeControlPresent&&(control.flags60&5u)!=0)return true;
        state_.previous=state_.current;
        if(double(control.clock)>0.9900000095367432&&control.clock<1.0099999904632568){
            if(state_.current==std::numeric_limits<int>::max())return fail("originalendingcounteroverflowUNKNOWN",error);
            ++state_.current;state_.fraction+=1.f;
        }else{
            const float next=state_.fraction+control.clock;
            if(!std::isfinite(next)||double(next)>=2147483648.)return fail("originalendingcounterconversionUNKNOWN",error);
            state_.fraction=next;state_.current=int(std::trunc(double(next)));
        }
        state_.returnCode=1;return true;
    }
    const Kind5State& state()const{return state_;}
};
}
