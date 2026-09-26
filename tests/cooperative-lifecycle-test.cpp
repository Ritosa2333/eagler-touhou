#include "../th08_web/cpp/multiplayer/CooperativeLifecycle.hpp"
#include <cstdio>
using namespace th08::multiplayer;
namespace {
bool check(bool condition,const char* message){if(!condition)std::fprintf(stderr,"FAIL: %s\n",message);return condition;}
bool allocate(void* raw,std::uint8_t,std::uint8_t target) noexcept {
    auto& accepted=*static_cast<bool*>(raw);
    return accepted&&target==1;
}
bool allocate_power(void* raw,std::uint8_t,std::uint8_t target) noexcept {
    return *static_cast<bool*>(raw)&&target==2;
}
bool allocate_any(void*,std::uint8_t,std::uint8_t) noexcept {return true;}
CooperativeFrameInput adjacent(){
    CooperativeFrameInput input{};
    for(int i=0;i<3;++i){input.seats[i].x=i*800;input.seats[i].y=38000;input.seats[i].lives=i;input.seats[i].available=true;input.seats[i].can_give=true;input.seats[i].can_receive=true;}
    return input;
}
bool rescue_priority(){
    CooperativeState state;reset(state,3);auto input=adjacent();
    if(!enter_spirit(state,2,1,-1))return false;
    input.seats[2].available=false;input.seats[0].lives=2;
    input.seats[0].focus=true;
    for(int tick=1;tick<90;++tick){auto result=advance(state,input);if(result.count)return check(false,"rescue before 90 ticks");}
    const auto result=advance(state,input);
    if(!check(result.count==1&&result.events[0].kind==CooperativeEventKind::Revive&&result.events[0].target==2,"spirit takes priority over live low-life seat"))return false;
    if(!check(!state.seats[2].spirit&&state.seats[0].waiting_for_focus_release,"spirit revived once"))return false;
    for(int tick=0;tick<100;++tick)if(advance(state,input).count)return check(false,"holding focus repeated transfer");
    input.seats[0].focus=false;advance(state,input);
    return check(!state.seats[0].waiting_for_focus_release,"focus release rearms transfer");
}
bool item_allocation(){
    CooperativeState state;reset(state,2);auto input=adjacent();input.seats[0].lives=2;input.seats[1].lives=1;input.seats[0].focus=true;
    bool accepted=false;
    for(int tick=0;tick<90;++tick)if(advance(state,input,allocate,nullptr,&accepted).count)return check(false,"failed item allocation debited life");
    accepted=true;
    for(int tick=1;tick<90;++tick)if(advance(state,input,allocate,nullptr,&accepted).count)return check(false,"retry transferred before renewed hold");
    const auto result=advance(state,input,allocate,nullptr,&accepted);
    return check(result.count==1&&result.events[0].kind==CooperativeEventKind::LifeItem&&result.events[0].target==1,"targeted item commits after allocation");
}
bool power_gift(){
    CooperativeState state;reset(state,3);auto input=adjacent();
    input.seats[0].power=20;input.seats[1].power=50;input.seats[2].power=0;
    bool accepted=true;
    for(int tap=1;tap<8;++tap){
        input.seats[0].shoot=input.seats[0].shoot_pressed=true;
        if(advance(state,input,nullptr,allocate_power,&accepted).count)return check(false,"power given before eighth tap");
        input.seats[0].shoot=input.seats[0].shoot_pressed=false;
        advance(state,input,nullptr,allocate_power,&accepted);
    }
    input.seats[0].shoot=input.seats[0].shoot_pressed=true;
    const auto result=advance(state,input,nullptr,allocate_power,&accepted);
    return check(result.count==1&&result.events[0].kind==CooperativeEventKind::PowerItems&&
                 result.events[0].target==2,"eight taps choose least-powered 3P recipient");
}
bool lower_seat_breaks_equal_resource_ties(){
    CooperativeState state;reset(state,3);auto input=adjacent();
    enter_spirit(state,0,1,-1);enter_spirit(state,1,-1,1);
    input.seats[0].available=input.seats[1].available=false;
    input.seats[0].lives=input.seats[1].lives=0;input.seats[2].lives=3;
    input.seats[2].focus=true;
    for(int tick=0;tick<89;++tick)advance(state,input);
    const auto life=advance(state,input);
    if(!check(life.count==1&&life.events[0].target==0,"equal Spirit lives choose lower seat"))return false;

    reset(state,3);input=adjacent();
    input.seats[2].power=20;input.seats[0].power=input.seats[1].power=0;
    bool accepted=true;
    for(int tap=0;tap<8;++tap){
        input.seats[2].shoot=input.seats[2].shoot_pressed=true;
        const auto result=advance(state,input,nullptr,allocate_any,&accepted);
        if(tap==7)return check(result.count==1&&result.events[0].target==0,"equal Power chooses lower seat");
        input.seats[2].shoot=input.seats[2].shoot_pressed=false;
        advance(state,input,nullptr,allocate_any,&accepted);
    }
    return false;
}
bool stage_interactions(){
    CooperativeState state;reset(state,2);auto input=adjacent();
    input.seats[0].power=20;input.seats[1].power=0;
    input.seats[0].shoot=input.seats[0].shoot_pressed=true;
    advance(state,input);
    if(!check(state.seats[0].power_taps==1,"power tap recorded"))return false;
    enter_spirit(state,1,1,-1);
    begin_stage(state);
    return check(state.seats[0].power_taps==0&&state.seats[0].power_window==0&&
                 !state.seats[1].spirit&&state.wipe_progress==0&&!state.retry_pending,
                 "stage clears interaction and revives spirits");
}
bool power_gift_retry(){
    CooperativeState state;reset(state,3);auto input=adjacent();
    input.seats[0].power=20;input.seats[1].power=50;input.seats[2].power=0;
    bool accepted=false;
    for(int tap=0;tap<8;++tap){
        input.seats[0].shoot=input.seats[0].shoot_pressed=true;
        if(advance(state,input,nullptr,allocate_power,&accepted).count)return check(false,"rejected item allocation emitted power gift");
        input.seats[0].shoot=input.seats[0].shoot_pressed=false;
        advance(state,input,nullptr,allocate_power,&accepted);
    }
    if(!check(state.seats[0].power_taps==0,"rejected gift resets gesture"))return false;
    accepted=true;
    input.seats[0].shoot=input.seats[0].shoot_pressed=true;
    advance(state,input,nullptr,allocate_power,&accepted);
    input.seats[0].shoot=input.seats[0].shoot_pressed=false;
    for(int tick=0;tick<24;++tick)advance(state,input,nullptr,allocate_power,&accepted);
    return check(state.seats[0].power_taps==0&&state.seats[0].power_window==0,
                 "expired power gesture cannot cross the 24-tick window");
}
bool wipe(){
    CooperativeState state;reset(state,2);auto input=adjacent();
    enter_spirit(state,0,1,1);enter_spirit(state,1,-1,1);
    input.seats[0].available=input.seats[1].available=false;
    for(int tick=1;tick<180;++tick)if(advance(state,input).count)return check(false,"retry before 180 ticks");
    const auto result=advance(state,input);
    if(!check(result.count==1&&result.events[0].kind==CooperativeEventKind::Retry,"full wipe requests retry at 180"))return false;
    if(!check(advance(state,input).count==0,"retry requested only once"))return false;
    revive(state,0);
    return check(state.wipe_progress==0&&!state.retry_pending,"revival clears wipe countdown");
}
}
int main(){return rescue_priority()&&item_allocation()&&power_gift()&&lower_seat_breaks_equal_resource_ties()&&power_gift_retry()&&stage_interactions()&&wipe()?0:1;}
