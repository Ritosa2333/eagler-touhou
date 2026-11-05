#include "../../cpp/game/LocalCharge.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);std::abort();}}while(0)
using namespace th09;
struct Output final:ShotControlActions {
    std::vector<std::string> events;
    void play_sound(i32 id,i32 side)override{events.push_back("sound:"+std::to_string(id)+":"+std::to_string(side));}
    void begin_charge()override{events.push_back("begin");}
    void end_charge()override{events.push_back("end");}
    bool opponent_boss_available()override{return true;}
    void attack(i32 level,const std::string&)override{events.push_back("attack:"+std::to_string(level));}
    void effect(i32 id,const Vec3&)override{events.push_back("effect:"+std::to_string(id));}
    void fire(u32 set,i32 frame)override{events.push_back("fire:"+std::to_string(set)+":"+std::to_string(frame));}
};
bool timer_equal(const Timer& a,const Timer& b){return a.previous==b.previous&&a.time==b.time&&a.current==b.current;}
void compare(const ShotControlState& a,const ShotControlState& b){
    CHECK(a.charge==b.charge&&a.available==b.available&&a.flags==b.flags&&a.player_state==b.player_state);
    CHECK(timer_equal(a.full_charge,b.full_charge)&&timer_equal(a.charging_time,b.charging_time));
    CHECK(timer_equal(a.cooldown,b.cooldown)&&timer_equal(a.charged_fire,b.charged_fire)&&timer_equal(a.normal_fire,b.normal_fire));
    CHECK(timer_equal(a.shock,b.shock)&&timer_equal(a.protection,b.protection));
}
int main(){
    ShotResource resource;resource.charge_speed=3;resource.charge_duration=24;
    for(bool automatic:{false,true}){
        ShotControlState original,priority;original.available=priority.available=400;
        AttackAreas first,second;Output a,b;LocalChargePlanner planner;PendingChargeRelease pending;GameInput input;
        for(u32 f=0;f<2400;++f){
            const bool stopped=f%240>=110&&f%240<140;
            u16 keys=f%200<160?1:0;if(automatic&&f%200>=12&&f%200<160)keys|=4;
            input.advance(keys);input.update_auto_focus(automatic);
            if(!stopped)ShotControl(original,first,a).update(input,automatic,0,resource,{},{});
            const auto sample=planner.plan(priority,input,automatic,0,resource,{},!stopped,true,1);
            CHECK(sample.valid());CHECK(ShotControl(priority,second,b).update_priority(input,automatic,0,resource,{},{},sample,pending,!stopped));
            compare(original,priority);CHECK(a.events==b.events);CHECK(!pending.count);
            original.available=priority.available=400;
        }
    }
    // A release accepted before learning of a remote cut-in is retained once,
    // then executed when gameplay resumes. Both Replay and resimulation apply
    // the same samples without running the local planner again.
    ShotControlState live,replay;live.available=replay.available=400;
    AttackAreas areas,playback_areas;Output output,playback;LocalChargePlanner planner;
    PendingChargeRelease pending,replay_pending;GameInput input;std::vector<ChargeSample> samples;
    for(u32 f=0;f<260;++f){
        input.advance(f<100?1:0);
        const bool stopped=f>=90&&f<150;
        const auto sample=planner.plan(live,input,false,0,resource,{},true,true,1);samples.push_back(sample);
        CHECK(ShotControl(live,areas,output).update_priority(input,false,0,resource,{},{},sample,pending,!stopped));
        CHECK(live.charge==sample.charge);
        if(f==100)CHECK(pending.count==1&&live.charge==0);
        if(f<150)for(const auto& event:output.events)CHECK(event.rfind("attack:",0)!=0);
        if(f==150)CHECK(!pending.count);
    }
    input={};
    for(u32 f=0;f<samples.size();++f){
        input.advance(f<100?1:0);
        CHECK(ShotControl(replay,playback_areas,playback).update_priority(input,false,0,resource,{},{},samples[f],replay_pending,!(f>=90&&f<150)));
    }
    compare(live,replay);CHECK(output.events==playback.events);
    u32 attacks=0;for(const auto& event:output.events)if(event.rfind("attack:",0)==0)++attacks;CHECK(attacks==1);
    auto invalid=samples[0];invalid.charge=401;CHECK(!invalid.valid());invalid=samples[0];invalid.events=8;CHECK(!invalid.valid());
    std::puts("Local charge | native-equivalent normal steps | frozen owner clock | deferred release once | exact sampled Replay: PASS");
}
