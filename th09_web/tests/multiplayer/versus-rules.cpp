// Asset-free applicability guard for the cross-title multiplayer rule audit.
// These are regression assertions for TH09's existing competitive rules, not
// newly designed balance values or an original-executable oracle.
#include "../../cpp/game/PlayerLife.hpp"
#include "../../cpp/game/PlayerItems.hpp"
#include "../../cpp/game/MatchRules.hpp"
#include "../cpp/attack-controller-fixture.hpp"
#include "../cpp/match-scene-fixture.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace th09;
namespace {
int checks=0;
void check(bool condition,const char* message){
    ++checks;
    if(!condition){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}
}
struct LifeActions final:PlayerLifeActions {
    bool boss_available=true;int attacks=0,last_attack=-1,winner=-1,wins=0;
    float recovery_charge=0;
    void play_sound(i32,i32)override{}
    void begin_charge()override{}
    void end_charge()override{}
    bool opponent_boss_available()override{return boss_available;}
    void attack(i32 level,const std::string&)override{++attacks;last_attack=level;}
    void effect(i32,const Vec3&)override{}
    void fire(u32,i32)override{}
    void play_positioned_sound(i32,float)override{}
    void opponent_wins(i32 side)override{winner=side;++wins;}
    void slotted_effect(i32,const Vec3&,i32,u32)override{}
    void critical_health()override{}
    void damage_flash(i32,i32,u32)override{}
    void end_focus()override{}
    void remove_shield()override{}
    void flush_combo()override{}
    void reset_ai()override{}
    void charge(float value)override{recovery_charge+=value;}
};
void quick_charge(){
    ShotResource resource;
    for(float available:{0.f,100.f,199.f,200.f,299.f,300.f,399.f,400.f}){
        for(bool boss_available:{false,true}){
            ShotControlState state;state.available=available;
            AttackAreas areas;LifeActions actions;actions.boss_available=boss_available;
            ShotControl control(state,areas,actions);GameInput input;input.held=2;
            const bool used=control.bomb(input,false,resource,{});
            check(used==(available>=200),"quick charge requires at least 200 gauge");
            check(actions.attacks==(available>=200?1:0),"quick charge makes exactly one attack");
            if(!used){check(state.available==available,"insufficient gauge remains unchanged");continue;}
            const int expected=available>=400&&boss_available?2:available>=300?1:0;
            check(actions.last_attack==expected,"charge threshold keeps the native attack level");
            const float remainder=available>=400&&!boss_available?100.f:0.f;
            check(state.available==remainder&&state.charge==remainder,"quick charge consumes gauge, not bomb stock");
            check(state.player_state==3&&areas.count==2,"quick charge creates native protection and cancellation");
            check(!control.bomb(input,false,resource,{}),"held quick-charge input cannot reuse spent gauge");
        }
    }
    for(int gate=0;gate<3;++gate){
        ShotControlState state;state.available=400;state.player_state=gate==1?4:0;
        AttackAreas areas;LifeActions actions;ShotControl control(state,areas,actions);
        GameInput input;input.held=gate==2?0:2;
        check(!control.bomb(input,gate==0,resource,{}),"scene/recovery/input gate blocks quick charge");
        check(state.available==400&&actions.attacks==0,"blocked quick charge preserves gauge");
    }
}
void health_and_round_loss(){
    ShotResource resource;
    for(u32 side=0;side<2;++side){
        PlayerMotion motion;motion.player=side;motion.health=1;
        ShotControlState control;control.available=287;
        AnmVm body{};AttackAreas areas;Rng random;LifeActions actions;
        PlayerLife life(motion,control,body,areas,random,actions);
        DamageRules damage;PlayerHazards hazards;hazards.circle({},4,nullptr);
        check(life.collide(hazards,1,damage,resource),"fatal collision is accepted once");
        check(motion.health==0&&actions.winner==i32(1-side)&&actions.wins==1,"fatal hit awards the opposing side the round");
        check(control.player_state==4&&control.available==287,"fatal hit does not synthesize a Bomb1 stock");
        check(!life.collide(hazards,1,damage,resource)&&actions.wins==1,"repeated collision during recovery cannot duplicate a round win");
    }
    for(int health:{2,3,8,10}){
        PlayerMotion motion;motion.health=health;ShotControlState control;control.available=287;
        AnmVm body{};AttackAreas areas;Rng random;LifeActions actions;
        PlayerLife life(motion,control,body,areas,random,actions);DamageRules damage;damage.damage=5;
        life.damage(damage,resource);
        const int remaining=health-5<2?1:health-5;
        check(motion.health==remaining&&actions.wins==0,"nonfatal damage clamps at critical health instead of spending a life stock");
        check(control.available==287,"nonfatal damage does not reset charge");
        control.protection.reset(60);life.recover({{-136,16},{272,416}});
        check(actions.recovery_charge==(remaining==1?400.f:130.f-float(remaining)*10.f),"recovery preserves the native health-dependent charge reward");
    }
    for(float available:{199.f,200.f,400.f}){
        PlayerMotion motion;motion.character=8;motion.health=1;
        ShotControlState control;control.available=available;
        AnmVm body{};AttackAreas areas;Rng random;LifeActions actions;
        PlayerLife life(motion,control,body,areas,random,actions);DamageRules damage;
        life.damage(damage,resource);
        check((motion.health==1)==(available>=200),"Tewi's existing automatic defence preserves the native gauge threshold");
        check(actions.wins==(available<200?1:0),"automatic defence does not become a gift revival");
    }
}
struct RewardActions final:MatchRuleActions {
    std::vector<std::pair<int,int>> rewards;
    void reward_enemy(i32 side,i32 reward)override{rewards.emplace_back(side,reward);}
    void play_sound(i32,i32)override{}
    void reward_notification(i32)override{}
};
struct ItemActions final:PlayerItemActions {
    float charge_value=0;int normal=0,character=0,score=0,transfers=0;
    void start_animation(AnmVm&,i32)override{}
    void draw_animation(AnmVm&)override{}
    void charge(float value)override{charge_value+=value;}
    TransferParameters* create_transfer(i32,const Vec3&,const Vec3&,float)override{++transfers;return nullptr;}
    void combo(const Vec3&,i32 n,i32,i32 c,i32 s)override{normal+=n;character+=c;score+=s;}
    void play_sound(i32,i32)override{}
};
void symmetric_rewards(){
    // Iterate existing native settings only; no multiplayer difficulty tiers.
    for(int difficulty=0;difficulty<4;++difficulty){
        EclWorldState world;RewardActions actions;MatchRules rules(world,actions);
        rules.initialize(difficulty,GameMode::versus,9);
        rules.scores[0].lives=rules.scores[1].lives=0;
        rules.scores[0].points=rules.scores[0].displayed=999999999;
        rules.progress.reward_accumulator=10001;rules.update(false,0,0);
        check(actions.rewards.size()==2,"reward threshold emits exactly two field-local rewards");
        check(actions.rewards[0].first==0&&actions.rewards[1].first==1&&actions.rewards[0].second==actions.rewards[1].second,"each opposing field receives one copy of the same reward");
        check(!rules.scores[0].eligible_for_extend(GameMode::versus)&&rules.scores[0].lives==0,"versus score does not create cooperative spare lives");
        rules.update(false,0,0);check(actions.rewards.size()==2,"threshold consumption does not duplicate rewards next tick");
        for(int side=0;side<2;++side)for(int type=0;type<4;++type){
            Rng random;ItemActions item_actions;PlayerItems items(random,item_actions);
            ItemContext c;c.side=side;c.rank=3;c.difficulty=difficulty;c.pickup_size=16;
            c.pickup_bounds={{-20,-20,0},{20,20,0}};c.geometry[0].width=c.geometry[1].width=288;
            items.spawn(type,{},false);items.update(c);
            check(items.items[0].active==0,"native reward is collected once");
            check(item_actions.charge_value==(type==0?400.f:0.f),"charge item remains 400 gauge");
            check(item_actions.transfers==(type==1?3+10+difficulty*2:0),"attack item preserves native transfer count");
            check(item_actions.character==(type==2?400:0)&&item_actions.normal==(type==2?1:0),"character item retains its native combo reward");
            check(item_actions.score==(type==3?70000:0),"score item retains its native value");
            items.update(c);check(item_actions.score==(type==3?70000:0)&&item_actions.charge_value==(type==0?400.f:0.f),"consumed item cannot grant another reward");
        }
    }
}
void opponent_boss_and_match(){
    for(int side=0;side<2;++side){
        attack_controller_test::Fixture boss(side);
        check(boss.controller.begin(2,2,0,"test boss"),"opponent boss attack starts on its receiving field");
        const auto spawn=std::find_if(boss.events.begin(),boss.events.end(),[](const auto& event){return event[0]==1;});
        check(spawn!=boss.events.end()&&(*spawn)[1]==side&&(*spawn)[2]==2&&(*spawn)[3]==1400,"boss spawn preserves native 1400 HP instead of applying team scaling");
        check(boss.players[1-side].boss_count==1,"boss attack belongs to the opposing player");
        check(!boss.controller.begin(2,2,0,"test boss"),"occupied boss slot refuses a duplicate");
        match_scene_test::Fixture match;match.rules.mode=GameMode::versus;match.wins_required=2;
        match.rules.scores[0].lives=5;match.rules.scores[1].lives=6;
        match.scene.end_round(side);match.scene.ending_frames=60;match.scene.update();
        check(match.wins[side]==1&&match.scene.retry==1,"first competitive round loss schedules a new round");
        check(match.rules.scores[0].lives==5&&match.rules.scores[1].lives==6,"competitive rounds do not transfer or spend campaign life stock");
        match.scene.end_round(side);match.scene.ending_frames=60;match.scene.update();
        check(match.wins[side]==2&&match.scene.retry==0,"winning threshold ends the competitive match");
        match.scene.show_results();match.dialogue_id=-1;match.scene.update();
        check(match.match_complete==1&&!match.game_over,"versus results use match_complete, not a cooperative revive/game-over path");
    }
}
}
int main(){
    quick_charge();health_and_round_loss();symmetric_rewards();opponent_boss_and_match();
    std::printf("TH09 versus applicability: PASS (%d assertions)\n",checks);
    return 0;
}
