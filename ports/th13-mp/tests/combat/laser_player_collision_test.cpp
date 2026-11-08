#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
using namespace th13;
namespace {
struct Provider final:EclResourceProvider {
    std::vector<u8> data;
    std::vector<std::string>* names=nullptr;
    Provider(){std::vector<u8> root;th13test::emit(root,0,502,0,255,0,{0x20});th13test::emit(root,0,23,0,255,0,{600});th13test::emit(root,0,10,0,255,0,{});data=th13test::build_scpt({{"main",root}},{"enemy.anm","st01enm.anm"});}
    bool read(const std::string& name,std::vector<u8>& out)override{if(name!="laser.ecl")return false;out=data;return true;}
    bool animation(u32,const std::string& name)override{names->push_back(name);return true;}
};
struct Host final:StageBattleServices {
    int hits=0,grazes=0;
    bool life_hud(i32,i32)override{return true;}
    bool bomb_hud(i32,i32)override{return true;}
    bool request_game_over()override{return true;}
    bool hud_sound(i32 id)override{hits+=id==2;grazes+=id==42;return true;}
};
struct Rig {
    Provider provider;Host host;SessionState session;HudPlayerState player;HudScoreState score;StageBattle battle;
    bool init(float y=400){
        provider.names=&battle.sim.anim_list;
        if(!battle.load(provider,"laser.ecl",2,host,session,player,score))return false;
        battle.set_player_pos(48,y);battle.sim.player_x=48;battle.sim.player_y=y;
        battle.player_phase=StageBattle::PlayerPhase::Active;battle.invulnerable=0;
        StageLaser l;l.id=1;l.origin[1]=400;l.phase=2;l.length=l.target_length=96;l.width=l.target_width=16;l.timing[2]=140;l.timing[3]=10;
        battle.sim.append_laser(l);return true;
    }
};
TEST(laser_hit_enters_deathbomb_and_splits_in_the_same_manager_update) {
    Rig r;CHECK(r.init());r.battle.sim.advance_lasers();
    CHECK_EQ(int(r.battle.player_phase),int(StageBattle::PlayerPhase::Deathbomb));
    CHECK_EQ(r.host.hits,1);CHECK_EQ(r.battle.sim.lasers.size(),2u);
    CHECK_EQ(r.battle.sim.lasers[0].length,32.0f);CHECK_EQ(r.battle.sim.lasers[1].length,32.0f);
    CHECK_EQ(r.battle.sim.lasers[1].time,0);CHECK_EQ(r.session.graze,0);
}
TEST(laser_invulnerability_trance_and_cancellation_protection_are_distinct) {
    for(int gate=0;gate<3;++gate){Rig r;CHECK(r.init());
        if(gate==0)r.battle.invulnerable=10;
        if(gate==1)r.session.run_mode_flags|=kTranceActiveFlag;
        if(gate==2)r.battle.sim.lasers[0].cancel_protection=10;
        r.battle.sim.advance_lasers();
        CHECK_EQ(r.battle.sim.lasers.size(),1u);CHECK_EQ(r.battle.sim.lasers[0].length,96.0f);
        CHECK_EQ(r.host.hits,gate==2?1:0);
    }
}
TEST(infinite_laser_graze_is_unrewarded_but_line_graze_counts_every_third_query) {
    Rig r;CHECK(r.init(410));
    for(int n=0;n<5;++n)r.battle.sim.advance_lasers();
    CHECK_EQ(r.session.graze,0);CHECK_EQ(r.host.grazes,0);
    auto& l=r.battle.sim.lasers[0];l.kind=StageLaser::Kind::Line;l.cull_timer=30;
    for(int n=0;n<7;++n)r.battle.sim.advance_lasers();
    CHECK_EQ(r.session.graze,3);CHECK_EQ(r.host.grazes,3);CHECK_EQ(r.host.hits,0);
}
TEST(curve_contact_reaches_deathbomb_and_preserves_curve_tail) {
    Rig r;CHECK(r.init());r.battle.sim.lasers.clear();
    StageEt et;et.reset();et.sound=-1;et.sprite=0;et.subtype=4;
    et.speed1=4;et.laser_width=14;et.laser_timing[0]=16;
    StageEnemy emitter;emitter.position[0]=-100;emitter.position[1]=400;
    r.battle.sim.emit_curve_laser(et,emitter);
    r.battle.invulnerable=1;
    for(int i=0;i<40;++i)r.battle.sim.advance_lasers();
    CHECK_EQ(r.host.hits,0);r.battle.invulnerable=0;
    r.battle.sim.advance_lasers();CHECK_EQ(r.host.hits,1);
    CHECK(r.battle.player_phase==StageBattle::PlayerPhase::Deathbomb);
    CHECK_EQ(r.battle.sim.lasers.size(),1u);
    CHECK(r.battle.sim.lasers[0].kind==StageLaser::Kind::Curve);
    // Native4355f0 ->444260 ->4371b0: eight leading samples removed,
    // path time40 rewound to32 then advanced once after the hit query.
    CHECK_EQ(r.battle.sim.lasers[0].curve_samples.size(),8u);
    CHECK_EQ(r.battle.sim.lasers[0].time,33);
    CHECK_EQ(r.session.graze,0);
}
TEST(curve_invulnerability_trance_and_cancel_protection_are_separate) {
    for(int gate=0;gate<3;++gate) {
        Rig r;CHECK(r.init());r.battle.sim.lasers.clear();
        StageEt et;et.reset();et.sound=-1;et.speed1=4;et.laser_width=14;et.laser_timing[0]=16;
        StageEnemy emitter;emitter.position[0]=-100;emitter.position[1]=400;
        r.battle.sim.emit_curve_laser(et,emitter);r.battle.invulnerable=1;
        for(int i=0;i<40;++i)r.battle.sim.advance_lasers();
        if(gate!=0)r.battle.invulnerable=0;
        if(gate==1)r.session.run_mode_flags|=kTranceActiveFlag;
        if(gate==2)r.battle.sim.lasers[0].cancel_protection=10;
        r.battle.sim.advance_lasers();CHECK_EQ(r.host.hits,gate==2?1:0);
        CHECK_EQ(r.battle.sim.lasers[0].curve_samples.size(),16u);
    }
}
TEST(curve_graze_query_does_not_award_line_laser_points) {
    Rig r;CHECK(r.init(410));r.battle.sim.lasers.clear();
    StageEt et;et.reset();et.sound=-1;et.speed1=4;et.laser_width=14;et.laser_timing[0]=16;
    StageEnemy emitter;emitter.position[0]=-100;emitter.position[1]=400;
    r.battle.sim.emit_curve_laser(et,emitter);
    for(int i=0;i<45;++i)r.battle.sim.advance_lasers();
    CHECK_EQ(r.host.hits,0);CHECK_EQ(r.host.grazes,0);CHECK_EQ(r.session.graze,0);
    CHECK_EQ(r.battle.sim.lasers[0].graze_timer,45);
}
}
int main(){return th13test::run_all("laser-player-collision");}
