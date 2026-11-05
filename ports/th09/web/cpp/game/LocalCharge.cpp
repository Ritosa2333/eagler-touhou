#include "LocalCharge.hpp"
#include <cmath>
namespace th09 {
namespace {
struct PlannedActions final:ShotControlActions {
    ShotControlState& state;ChargeSample& sample;bool boss;
    PlannedActions(ShotControlState& s,ChargeSample& p,bool b):state(s),sample(p),boss(b){}
    void play_sound(i32 id,i32)override{if(id==50)sample.events|=ChargeSample::LevelSound;}
    void begin_charge()override{sample.events|=ChargeSample::Begin;}
    void end_charge()override{sample.events|=ChargeSample::End;sample.released=state.charge;}
    bool opponent_boss_available()override{return boss;}
    void attack(i32,const std::string&)override{}
    void effect(i32,const Vec3&)override{}
    void fire(u32,i32)override{}
};
}
ChargeSample LocalChargePlanner::plan(const ShotControlState& live,const GameInput& input,bool automatic_focus,i32 side,
                                     const ShotResource& resource,const FrameTiming& timing,bool advance,bool boss_available,u32 instance){
    if(!initialized||generation!=instance){shadow=live;generation=instance;initialized=true;}
    // Capacity and life belong to gameplay. The accepted charging clock stays
    // with its local owner through corrections of the world around it.
    if((live.player_state==4||live.player_state==5)&&live.player_state!=shadow.player_state)accept(live);
    shadow.available=live.available;shadow.player_state=live.player_state;
    ChargeSample sample;sample.active=1;
    if(advance){
        AttackAreas scratch;PlannedActions actions(shadow,sample,boss_available);
        ShotControl(shadow,scratch,actions).update(input,automatic_focus,side,resource,{},timing);
    }
    sample.charge=shadow.charge;sample.charging_time=shadow.charging_time;sample.full_charge=shadow.full_charge;
    return sample;
}
}
