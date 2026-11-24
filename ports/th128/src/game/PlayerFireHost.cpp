#include "PlayerFireHost.hpp"
#include "FreezeRules.hpp"
#include <cmath>
namespace th128::player::fire {
namespace {
bool valid(hit::Timer timer){return timer.clockSource==0x4b6f58&&(timer.flags&1u)&&timer.current>=-1000000&&timer.current<1000000
    &&timer.fractional==float(timer.current);}
void set(hit::Timer& timer,int value){timer={value-1,value,float(value),0x4b6f58,1};}
void increment(hit::Timer& timer){timer.previous=timer.current;++timer.current;timer.fractional=float(timer.current);}
struct Guard{bool& value;explicit Guard(bool& flag):value(flag){value=true;}~Guard(){value=false;}};
}
bool Host::fail(std::string reason,std::string& error){if(failure_.empty())failure_=std::move(reason);error=failure_;return false;}
bool Host::configure(int shot,hit::Timer held,std::string& error){
    error.clear();if(configured_||executing_||shot< -1||shot>14||!valid(held)){error="invalid fresh fire host state";return false;}
    state_={shot,0,held};configured_=true;return true;
}
bool Host::heldTimer(hit::Timer timer,std::string& error){
    error.clear();if(!configured_||executing_||!failure_.empty()||!valid(timer))return fail("invalid live held timer",error);
    state_.held=timer;return true;
}
bool Host::suspend(const Sound& sound,std::string& error){
    error.clear();if(!configured_||executing_||!failure_.empty()||!sound)return fail(failure_.empty()?"unavailable dialogue fire reset":failure_,error);
    Guard guard(executing_);state_.shot=-1;set(state_.held,0);
    try{for(const auto identifier:{31,70})if(!sound(1,identifier,error)||!error.empty()||!failure_.empty())return fail(error.empty()?"dialogue sound stop rejected":error,error);}
    catch(...){return fail("dialogue fire reset callback threw",error);}
    return true;
}
bool Host::tick(int playerState,Input input,int busy,int mode,int& power,const Emit& emit,const Sound& sound,const Freeze& freeze,std::string& error,
                 FreezePreflight preflight,bool keepPower){
    error.clear();
    if(!configured_||executing_||!failure_.empty()||playerState<0||playerState>4||power<0||power>10000||!valid(state_.held)
        ||!emit||!sound||!freeze)return fail(failure_.empty()?"invalid owning fire host context":failure_,error);
    Guard guard(executing_);
    const auto callSound=[&](int operation,int identifier){
        if(!sound(operation,identifier,error)||!error.empty()||!failure_.empty())return fail(error.empty()?"required fire sound rejected":error,error);
        return true;
    };
    const auto addPower=[&](int amount){
        const int old=power;power=freeze::addPowerRaw(power,amount);
        if(old<10000&&power>=10000)return callSound(0,74);
        if(old<3000&&power>=3000)return callSound(0,73);
        return true;
    };
    try{
        if(playerState!=1)return true;
        if(state_.shot<0&&startsShotCycle(input,state_.shot))state_.shot=0;
        if(state_.shot>=0){
            if(!emit(state_.shot,error)||!error.empty()||!failure_.empty())return fail(error.empty()?"required player emission rejected":error,error);
            const auto cycle=advanceShotCycle(input,state_.shot);
            if(!cycle)return fail("unsupported original shot cycle",error);
            state_.shot=cycle->current;
        }
        auto& timer=state_.held;
        if(timer.current<0){increment(timer);return true;}
        if((input.holding&ShootFreezeKey)&&busy==0&&mode==0){
            increment(timer);const int count=timer.current;
            if(count==15&&!callSound(0,31))return false;
            if(count==45&&!callSound(0,70))return false;
            if(count>=20){const auto amount=freeze::heldRechargeRaw(count);if(!amount||!addPower(*amount))return false;}
            if(count>=45){
                if(state_.savedPower<3000&&power>=3000){if(!callSound(1,31)||!callSound(0,32))return false;}
                state_.savedPower=power;
            }
            return callSound(2,31);
        }
        if(timer.current>0){
            if(timer.current>=45&&power>=3000&&mode==0){
                if(preflight){
                    try{
                        const auto decision=preflight(power,error);
                        if(!error.empty())return fail(error,error);
                        if(decision==FreezePreflightDecision::Deny)return true;
                    }catch(...){return fail("freeze preflight callback threw",error);}
                }
                const int saved=power;set(timer,-40);
                if(!freeze(saved,error)||!error.empty()||!failure_.empty())return fail(error.empty()?"required freeze activation rejected":error,error);
                if(!keepPower)power=0;
            }else{set(timer,0);if(!callSound(1,31))return false;}
            state_.savedPower=0;return true;
        }
        state_.savedPower=0;return addPower(5);
    }catch(...){return fail("fire host callback threw",error);}
}
}
