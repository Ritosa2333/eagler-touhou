#include "mima_core.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using soew::EnemyBulletRequest;
using soew::MimaCore;
using Phase=MimaCore::Phase;
using Event=MimaCore::Event;
using Attack=MimaCore::Attack;

static void check(bool b,int line){
    if(!b){std::cerr<<"Mima assertion failed at line "<<line<<'\n';std::exit(1);}
}
#define CHECK(condition) check(bool(condition),__LINE__)

struct Fixture {
    MimaCore m;
    MimaCore::Hooks h;
    MimaCore::Player p;
    std::vector<EnemyBulletRequest> bullets;
    std::vector<std::tuple<int,int,int,int>> hitboxes;
    std::vector<unsigned> items,sounds,stacks,turns,palettes;
    std::string presentation;
    unsigned sample=0,random_calls=0,damage=0,score=0,invincibility=0,clears=0,hits=0,lines=0,circles=0;
    Fixture(){
        h.random8=[&]{++random_calls;return sample;};
        h.damage=[&](int x,int y,int w,int he){hitboxes.emplace_back(x,y,w,he);return damage;};
        h.bullet=[&](const auto &b){bullets.push_back(b);};
        h.score=[&](unsigned n){score+=n;};
        h.sound=[&](unsigned n){sounds.push_back(n);};
        h.invincibility=[&](unsigned n){invincibility=n;};
        h.player_hit=[&]{++hits;};
        h.clear_bullets=[&]{++clears;};
        h.item=[&](int,int,unsigned n){items.push_back(n);};
        h.stack_multiplier=[&](unsigned n){stacks.push_back(n);};
        h.turns_max=[&](unsigned n){turns.push_back(n);};
        h.line=[&](int,int,int,int,unsigned color){CHECK(color==13);++lines;};
        h.circle=[&](int,int,unsigned,unsigned){++circles;};
        h.palette0=[&](unsigned r,unsigned g,unsigned b){palettes.push_back((r<<16)|(g<<8)|b);};
        h.dot=[&](int,int,unsigned,unsigned){presentation+='.';};
        h.fill_circle=[&](int,int,unsigned,unsigned){presentation+='o';};
        p.reduce_effects=true;
    }
    void tick(){++p.stage_frame;p.page^=1;m.update(h,p);}
    void opening(unsigned byte){sample=byte;for(int i=0;i<101;++i)tick();CHECK(m.phase==Phase::First&&m.phase_frame==0);}
    void seek_phase(Phase wanted){for(int i=0;i<20000&&m.phase!=wanted;++i)tick();CHECK(m.phase==wanted);}
    void seek_event(Event wanted){
        for(int i=0;i<60000&&m.event==Event::None;++i)tick();
        if(m.event!=wanted)std::cerr<<"Expected event "<<int(wanted)<<", found "<<int(m.event)<<" in phase "<<int(m.phase)<<", rounds "<<m.rounds<<'\n';
        CHECK(m.event==wanted);
    }
    void seek_frame(unsigned f){for(unsigned i=0;i<2000&&m.phase_frame<f;++i)tick();CHECK(m.phase_frame==f);}
};

int main(){
    {
        Fixture f;
        f.m.render_background(f.h,true);
        CHECK(f.presentation.size()==193&&f.presentation.find('o')==97);
        f.p.front_x=160;f.p.front_y=65;f.damage=250;
        for(int i=0;i<4;++i)f.tick();
        CHECK(f.hits==0&&f.m.body_flash&&f.m.damage_taken==0);
        CHECK(f.m.target_x==216&&f.m.target_y==120);
        CHECK((f.hitboxes.front()==std::tuple<int,int,int,int>{192,96,64,64}));
        f.p.front_x=161;f.tick();CHECK(f.hits==1&&!f.m.body_flash);
        for(int i=0;i<96;++i)f.tick();
        CHECK(f.m.phase==Phase::First&&f.m.bg_radius==50&&f.m.bg_fill_base==33);
        CHECK(f.random_calls==1&&f.palettes.back()==0x320032);
        CHECK(f.m.selected_attack()==Attack::Sweep);
        f.seek_frame(32);
        CHECK(f.stacks==std::vector<unsigned>{0}&&f.bullets.size()==2);
        CHECK(f.bullets[0].left==140&&f.bullets[0].top==80&&f.bullets[0].angle==56);
        CHECK(f.bullets[0].group==0x25&&f.bullets[0].speed==60&&f.bullets[1].group==0x17);
        while(f.m.rounds<2)f.tick();
        CHECK(f.m.body_damage_enabled&&f.m.damage_taken==250);
        f.seek_phase(Phase::Shift1);
        CHECK(!f.m.body_damage_enabled);
        f.seek_frame(80);
        CHECK((f.items==std::vector<unsigned>{3,3,4}));
        CHECK(f.m.pattern==128);
        f.seek_phase(Phase::Mixed);
        CHECK((f.items==std::vector<unsigned>{3,3,4,3,3,3,2,3,3}));
        CHECK(f.m.bg_shadow_color==2&&f.m.selected_attack()==Attack::Bounce);
        f.seek_event(Event::Form1Defeated);
        CHECK(f.clears==1&&f.m.phase==Phase::Form1Dialogue);
        const unsigned time=f.m.phase_frame;f.tick();
        CHECK(f.m.phase_frame==time&&f.clears==1);
        auto result=f.m.resolve_form1(f.h,1);
        CHECK(result.ending&&!result.good_ending&&!result.enter_form2&&result.bonus==50000);
        CHECK(f.m.finished&&!f.m.form2&&f.m.resolve_form1(f.h,0).bonus==0);
    }
    {
        Fixture f;f.opening(1);f.seek_frame(71);
        CHECK(f.lines==9&&f.m.pattern==131);
        f.seek_frame(170);CHECK(f.m.pattern==134&&f.lines==99*9);
        f.p.x=f.m.left+17;f.tick();CHECK(f.hits==1&&f.m.top==92&&f.lines==99*9+1);
    }
    {
        Fixture f;f.opening(2);f.seek_frame(80);
        for(unsigned i=0;i<4;++i){CHECK(f.m.orbs[i].active&&f.m.orbs[i].pattern==137+i);}
        const auto orb=f.m.orbs[0];
        CHECK((f.hitboxes[f.hitboxes.size()-4]==std::tuple<int,int,int,int>{orb.x+4,orb.y,32,24}));
        f.p.front_x=orb.x;f.p.front_y=orb.y;f.tick();CHECK(f.hits>=1);
    }
    for(unsigned rank:{0u,1u}){
        Fixture f;f.p.rank=rank;f.opening(0);
        f.seek_frame(32);CHECK(f.bullets[1].group==(rank?0x17:0x19));
        f.damage=20;f.seek_phase(Phase::Second);
        f.bullets.clear();f.seek_frame(rank?105:104);
        CHECK(f.bullets.size()==1&&f.bullets[0].group==(rank?0x21:0x22));
        CHECK(f.bullets[0].speed==20&&f.bullets[0].angle==(rank?251:252));
        while(f.m.rounds<1)f.tick();
        CHECK(f.m.selected_attack()==Attack::Symmetric);
        f.bullets.clear();f.seek_frame(101);
        CHECK(f.bullets.size()==1&&f.bullets[0].group==0x23&&f.bullets[0].angle==216&&f.bullets[0].speed==90);
        while(f.m.rounds<2)f.tick();
        CHECK(f.m.selected_attack()==Attack::BurstOrbit);
        f.bullets.clear();f.seek_frame(80);
        CHECK(f.m.orbs[0].active&&f.bullets.empty());
        f.seek_frame(141);
        CHECK(f.bullets.size()==16&&f.bullets[0].speed==35&&f.bullets[3].speed==71);
        f.seek_phase(Phase::Third);
        f.bullets.clear();f.seek_frame(104);
        CHECK(f.turns==std::vector<unsigned>{0});
        CHECK(f.bullets.size()==(rank?4u:2u)&&f.bullets[0].group==0x88);
        CHECK(f.bullets[0].pattern==85&&f.bullets[0].speed==80);
        CHECK(std::uint8_t(f.bullets[0].angle-f.bullets[1].angle)==(rank?20:30));
        while(f.m.rounds<1)f.tick();
        CHECK(f.m.selected_attack()==Attack::Fan);
        f.bullets.clear();f.seek_frame(120);
        CHECK(f.bullets.size()==2&&f.bullets[0].angle==12&&f.bullets[1].angle==116);
        CHECK(f.bullets[0].group==7&&f.bullets[0].speed==80);
        while(f.m.rounds<2)f.tick();
        CHECK(f.m.selected_attack()==Attack::Satellites);
        f.seek_frame(80);CHECK(f.m.orbs[0].active&&f.m.orbs[0].x<40);
        f.tick();CHECK(f.m.orbs[0].active&&f.m.orbs[0].x>80);
        f.bullets.clear();f.seek_phase(Phase::Shift3);
        CHECK(!f.m.orbs[0].active&&!f.bullets.empty());
        for(const auto &b:f.bullets){
            CHECK(b.kind==EnemyBulletRequest::Pellet&&b.pattern==0&&b.group==0x20&&b.speed==128);
        }
    }
    {
        Fixture f;f.opening(0);f.seek_frame(31);f.h.bullet={};f.tick();
        CHECK(f.m.blocked==MimaCore::Blocked::MissingProjectileSink&&f.m.blocked_reason());
        CHECK(f.m.event==Event::None&&f.bullets.empty());
        const auto frame=f.m.phase_frame;f.tick();CHECK(f.m.phase_frame==frame);
    }
    {
        Fixture f;f.h.random8={};for(int i=0;i<102;++i)f.tick();
        CHECK(f.m.blocked==MimaCore::Blocked::MissingRng&&f.m.blocked_reason());
        CHECK(f.m.event==Event::None&&f.bullets.empty());
    }
    for(unsigned final_selection=0;final_selection<3;++final_selection){
        Fixture f;f.opening(0);f.damage=20;f.seek_event(Event::Form1Defeated);
        f.sample=final_selection;
        const auto entered=f.m.resolve_form1(f.h,0);
        CHECK(!entered.ending&&entered.enter_form2&&f.m.form2&&f.m.phase==Phase::Final);
        CHECK(f.m.pattern==128&&f.m.bg_radius==200&&f.m.bg_fill_radius==150&&f.m.damage_taken==0);
        f.bullets.clear();f.tick();CHECK(f.m.top==64);
        f.seek_frame(final_selection==0?51:final_selection==1?84:64);
        CHECK(!f.bullets.empty()&&f.bullets[0].kind==EnemyBulletRequest::Bullet16&&f.bullets[0].pattern==86);
        if(final_selection==0){CHECK(f.bullets[0].group==0x23&&f.bullets[0].speed==50);}
        if(final_selection==1){CHECK(f.circles==90&&f.bullets.size()==2&&f.bullets[0].group==0x1c&&f.bullets[1].kind==EnemyBulletRequest::Pellet);}
        if(final_selection==2){CHECK(f.bullets[0].group==0x17&&f.bullets[0].speed==90);}
        f.seek_event(Event::FinalDefeated);
        const auto result=f.m.resolve_final(final_selection==0?0:1);
        CHECK(result.ending&&result.good_ending==(final_selection==0)&&result.bonus==100000);
        CHECK(f.m.finished&&f.m.form2&&f.clears==1&&f.m.resolve_final(0).bonus==0);
    }
    {
        Fixture f;f.opening(0);f.seek_phase(Phase::Second);
        CHECK(f.m.phase==Phase::Second); // Source timeout uses rounds > 10, not >= 10.
    }
    {
        Fixture f;f.opening(0);f.damage=6000;
        for(int i=0;i<1200&&f.score==0;++i)f.tick();
        CHECK(f.score==500000&&f.invincibility==200);
        for(int i=0;i<200;++i)f.tick();
        CHECK(f.score==500000);
    }
    std::cout<<"Mima phase, collision, attack and ending checks passed\n";
}
