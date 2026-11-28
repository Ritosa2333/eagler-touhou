#include "PlayerFireRules.hpp"
#include "FreezeRules.hpp"
#include <bit>
#include <cmath>

namespace th128::player::fire {
namespace {
bool known(hit::Timer timer) noexcept {
    return timer.clockSource==0x4b6f58&&std::isfinite(timer.fractional)&&
           timer.current>=-1000000&&timer.current<=1000000&&timer.fractional==float(timer.current);
}
hit::Timer set(hit::Timer timer,int count) noexcept {
    timer.previous=count-1;timer.current=count;timer.fractional=float(count);
    if(!(timer.flags&1u)){timer.flags|=1u;timer.clockSource=0x4b6f58;}
    return timer;
}
hit::Timer increment(hit::Timer timer) noexcept {
    timer.previous=timer.current;++timer.current;timer.fractional+=1;return timer;
}
void request(Plan& plan,std::uint32_t site,std::uint32_t target,int argument) noexcept {
    plan.continuation=Continuation::RequiredCall;plan.boundary=site;plan.target=target;plan.argument=argument;
}
void addPower(Plan& plan,int increment) noexcept {
    const int old=plan.powerRaw;plan.powerRaw=freeze::addPowerRaw(old,increment);
    if(old<10000&&plan.powerRaw>=10000)request(plan,0x41f448,0x458410,74);
    else if(old<3000&&plan.powerRaw>=3000)request(plan,0x41f467,0x458410,73);
}
}
std::optional<Plan> head(int state,Input input,hit::Timer timer) noexcept {
    if(state<0||state>4||!known(timer))return std::nullopt;
    Plan result;result.next=timer;
    if(state!=1){result.boundary=0x43f48b;return result;}
    if(timer.current<0){
        if(!startsShotCycle(input,timer.current)){
            result.continuation=Continuation::Charge;result.boundary=0x43f2e1;return result;
        }
        result.next=set(timer,0);
    }
    if(result.next.current!=result.next.previous){
        result.continuation=Continuation::Emit;result.boundary=0x43f25d;result.target=0x43f160;
    }else{
        result.continuation=Continuation::Cycle;result.boundary=0x43f264;
    }
    return result;
}
std::optional<Plan> charge(hit::Timer held,unsigned holding,int busy,int mode,int power,int saved,float scale) noexcept {
    if(!known(held)||!(held.flags&1u)||holding>65535||power<0||power>10000||!std::isfinite(scale))return std::nullopt;
    Plan result;result.next=held;result.powerRaw=power;result.savedPower=saved;result.scale=scale;result.boundary=0x43f48b;
    if(held.current<0){result.next=increment(held);return result;}
    result.scale=1;
    if((holding&ShootFreezeKey)&&busy==0&&mode==0){
        result.next=increment(held);
        const int count=result.next.current;
        if(count==15){request(result,0x43f343,0x458410,31);return result;}
        if(count==45){request(result,0x43f358,0x458410,70);return result;}
        if(count>=20){
            const auto amount=freeze::heldRechargeRaw(count);
            if(!amount)return std::nullopt;
            addPower(result,*amount);
            if(result.target)return result;
        }
        if(count>=45){
            if(saved<3000&&result.powerRaw>=3000){request(result,0x43f3c1,0x458590,31);return result;}
            result.savedPower=result.powerRaw;
        }
        result.boundary=0x43f3dc;
        return result;
    }
    if(held.current>0){
        if(held.current>=45&&power>=3000&&mode==0){
            result.next=set(held,-40);request(result,0x43f439,0x4101d0,0);return result;
        }
        result.next=set(held,0);request(result,0x43f453,0x458590,31);return result;
    }
    addPower(result,5);return result;
}
}
