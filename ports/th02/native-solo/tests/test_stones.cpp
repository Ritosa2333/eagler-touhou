#include "stones_core.hpp"
#include <iostream>
#include <vector>
using Core=soew::StonesCore;
void check(bool ok,const char *message){if(!ok)throw std::runtime_error(message);}
int main(){try{
    Core s; Core::Hooks h;
    unsigned score=0,damage=140,calls=0;
    std::vector<unsigned> items;std::vector<soew::EnemyBulletRequest> bullets;
    h.damage=[&](int,int,int w,int height){check(w==32&&height==40,"hitbox");++calls;return damage;};
    h.score=[&](unsigned n){score+=n;};
    h.item=[&](int,int,unsigned type){items.push_back(type);};
    h.bullet=[&](const auto &b){bullets.push_back(b);};
    for(unsigned i=0;i<63;++i)s.update(1,200,h);
    check(calls==315&&score==0&&s.stones[0].damage==0,"dormant hit checks without damage");
    check(s.phase==Core::Phase::Opening&&s.stones[0].pattern==154,"opening frame 63");
    s.update(1,200,h);
    check(s.phase==Core::Phase::Inner&&s.stones[0].pattern==155&&s.stones[1].flag==Core::Flag::Active,"inner activation");
    damage=0;
    for(unsigned i=0;i<47;++i)s.update(1,200,h);
    check(bullets.empty(),"no early inner shot");s.update(1,200,h);
    check(bullets.size()==2&&bullets[0].left==140&&bullets[0].top==40&&bullets[0].group==0x18&&bullets[0].speed==38,"inner pellet parameters");
    damage=140;s.update(1,200,h);damage=0;
    check(score==60000&&items==std::vector<unsigned>{2,3}&&s.stones[0].death_frame==1,"inner kill rewards and first death tick");
    for(unsigned i=0;i<100&&s.phase==Core::Phase::Inner;++i)s.update(1,200,h);
    check(s.phase==Core::Phase::Outer&&s.stones[2].pattern==159&&s.stones[3].flag==Core::Flag::Active,"outer activation after inner removal");
    bullets.clear();for(unsigned i=0;i<70;++i)s.update(1,200,h);
    check(bullets.size()==12&&bullets[5].angle==210&&bullets[5].group==0x82&&bullets[5].pattern==82,"outer ring parameters");
    for(unsigned i=0;i<1230;++i)s.update(1,200,h);
    check(s.elapsed==1300&&s.stones[2].flag==Core::Flag::Active,"outer timer boundary");
    s.update(1,200,h);check(s.stones[2].flag==Core::Flag::Dying&&score==60000&&items.size()==2,"timeout without kill rewards");
    for(unsigned i=0;i<70&&s.phase==Core::Phase::Outer;++i)s.update(1,200,h);
    check(s.phase==Core::Phase::NorthOpening&&s.stones[4].flag==Core::Flag::Dormant,"north opening after pair removal");
    bool rejected=false;
    try{s.update(1,200,h);}catch(const std::runtime_error &){rejected=true;}
    check(rejected&&s.phase_frame==0,"missing stripe hook rejected before advancing");
    std::vector<std::array<int,4>> tiles;
    h.stripe_tile=[&](int x,int y,unsigned tile){tiles.push_back({int(s.phase_frame),x,y,int(tile)});};
    for(unsigned i=0;i<50;++i)s.update(1,200,h);
    check(tiles.empty(),"no stripe request before frame 51");
    // ASM 8551-8569 tests for the 152 -> 160 jump between two increments.
    // The cels at frames 123, 126, 129, 132, 135, 138 are 150, 152, 160, 161, 162, 163.
    for(unsigned i=50;i<137;++i)s.update(1,200,h);
    check(tiles.size()==72&&!s.done()&&s.stones[4].pattern==162&&s.stones[4].flag==Core::Flag::Dormant,"three stripe passes before north activation");
    for(unsigned i=0;i<72;++i){const unsigned pair=i/2,col=pair%12;
        const int x=32+int(i%2?23-col:col)*16;
        check(tiles[i]==std::array<int,4>{51+2*int(pair),x,96,41+int(pair/12)},"stripe timing, column and tile");
    }
    s.update(1,200,h);
    check(s.phase==Core::Phase::NorthFirstAttacks&&s.stones[4].flag==Core::Flag::Active&&s.stones[4].pattern==163,"north activation");
    std::vector<std::array<int,5>> lasers;
    h.laser=[&](int x,int y,int duration,unsigned pat,unsigned delay){lasers.push_back({x,y,duration,int(pat),int(delay)});};
    damage=400;s.update(1,200,h);s.update(1,200,h);damage=0;
    check(s.stones[4].flag==Core::Flag::Active&&s.stones[4].damage==500,"north phase 4 damage clamp");
    for(unsigned i=2;i<80;++i)s.update(1,200,h);
    check(lasers.size()==8&&lasers[0]==std::array<int,5>{48,96,36,103,16}&&lasers[7][0]==352,"phase 4 laser waves");
    bullets.clear();for(unsigned i=0;i<64;++i)s.update(1,200,h);
    check(!s.done()&&s.north_attack==2&&bullets.size()==16&&bullets.front().angle==0&&bullets.back().angle==120&&bullets.back().group==0x23&&bullets.back().speed==70,"phase 4 sweep");
    for(unsigned i=0;i<24;++i)s.update(1,200,h);
    rejected=false;try{s.update(1,200,h);}catch(const std::runtime_error &){rejected=true;}
    check(rejected&&s.phase_frame==24,"player X required before aimed laser");
    lasers.clear();s.update(1,123,200,h);
    check(lasers.size()==1&&lasers[0]==std::array<int,5>{128,96,30,103,0},"aimed laser aligned to player center");
    while(s.north_attack==2)s.update(1,123,200,h);
    h.random8=[](){return 17u;};bullets.clear();
    while(s.north_attack==3)s.update(1,123,200,h);
    check(bullets.size()==24&&bullets[0].left==128&&bullets[0].group==0x83&&bullets[0].pattern==82,"gravity attack");
    bullets.clear();while(s.north_attack==4)s.update(1,123,200,h);
    check(bullets.size()==32&&bullets[0].speed==32&&bullets.back().speed==94&&bullets[0].group==0x21,"random speed attack");
    lasers.clear();bullets.clear();while(s.phase==Core::Phase::NorthFirstAttacks)s.update(1,123,200,h);
    check(lasers.size()==5&&lasers[0]==std::array<int,5>{304,96,30,111,12}&&bullets.size()==1,"final phase 4 attacks");
    tiles.clear();while(s.phase==Core::Phase::NorthClosing)s.update(1,123,200,h);
    check(tiles.size()==72&&tiles.front()[3]==42&&tiles.back()[3]==40&&s.stones[4].flag==Core::Flag::Dormant&&s.stones[4].pattern==148&&s.phase==Core::Phase::NorthSecondAttacks,"phase 5 closing and phase 6 entry");
    std::vector<std::array<int,3>> events;std::vector<std::array<int,3>> multipliers;
    h.bullet=[&](const auto &b){bullets.push_back(b);events.push_back({int(s.north_attack),int(s.phase_frame),int(b.angle)});};
    h.bullet_stack_multiplier=[&](unsigned n){multipliers.push_back({int(s.north_attack),int(s.phase_frame),int(n)});};
    bullets.clear();
    for(unsigned attack=0;attack<3;++attack){
        const unsigned duration=attack==1?420:168;
        for(unsigned frame=1;frame<=duration;++frame){
            const auto before=bullets.size();s.update(1,123,200,h);
            const bool fires=attack==1?frame>=30&&frame%30==0:frame%8==0;
            check(bullets.size()-before==(fires?(attack==1?8u:2u):0u),"phase 6 shot schedule");
        }
        check(s.north_attack==attack+1&&s.phase_frame==0,"phase 6 attack transition");
    }
    check(events.front()==std::array<int,3>{0,8,194}&&bullets.front().group==0x24&&bullets.front().speed==64,"phase 6 initial pellet pair");
    check(events[42]==std::array<int,3>{1,30,41}&&bullets[42].pattern==82&&bullets[42].speed==60,"phase 6 stone wave");
    bullets.clear();events.clear();
    for(unsigned frame=1;frame<=447;++frame){
        const auto before=bullets.size();s.update(1,123,200,h);
        const bool fires=frame>=60&&frame%4==0&&(frame>=384||(frame%64)/4<=8);
        check(bullets.size()-before==(fires?5u:0u),"phase 6 aimed and ring schedule");
        if(frame==383)check(multipliers.empty(),"no early multiplier change");
    }
    check(multipliers.size()==17&&multipliers.front()==std::array<int,3>{3,384,0}&&multipliers.back()==std::array<int,3>{3,447,1},"phase 6 multiplier timing");
    check(!s.done()&&s.phase==Core::Phase::NorthReopening&&s.phase_frame==0,"phase 7 entry");
    tiles.clear();for(unsigned frame=1;frame<=137;++frame)s.update(1,123,200,h);
    check(!s.done()&&tiles.size()==72&&tiles.front()[3]==41&&tiles.back()[3]==43&&s.stones[4].pattern==162,"phase 7 stripe and north activation timing");
    s.update(1,123,200,h);
    check(!s.done()&&s.phase==Core::Phase::NorthFinalAttacks&&s.stones[4].flag==Core::Flag::Active&&s.stones[4].pattern==163&&s.elapsed==0,"phase 8 entry");
    auto death=s; // Both routes start from the naturally reached final phase.
    lasers.clear();while(s.north_attack==0)s.update(1,123,200,h);
    check(lasers.size()==40&&lasers.front()==std::array<int,5>{32,96,16,111,3}&&lasers[19][0]==256&&lasers[20]==std::array<int,5>{208,96,16,111,1}&&lasers.back()[0]==368,"phase 8 laser sequence");
    bullets.clear();while(s.north_attack==1)s.update(1,123,200,h);
    check(bullets.size()==16&&bullets.front().angle==0&&bullets.back().angle==120,"phase 8 sweep");
    bullets.clear();while(s.north_attack==2)s.update(1,123,200,h);
    check(bullets.size()==32&&bullets[0].speed==80&&bullets[0].group==0x20,"phase 8 aimed pair sequence");
    while(s.north_attack!=1)s.update(1,123,200,h);
    check(s.elapsed<2000&&s.phase==Core::Phase::NorthFinalAttacks,"phase 8 loops attacks one through five");
    unsigned invincibility=0,death_items=0,death_score=0;
    std::vector<unsigned> sounds;
    h.invincibility=[&](unsigned n){invincibility=n;};
    const auto before_timeout_score=score;
    const auto before_timeout_items=items.size();
    while(s.elapsed<2000)s.update(1,123,200,h);
    check(s.phase==Core::Phase::NorthFinalAttacks,"phase 8 strict timeout threshold");
    s.update(1,123,200,h);
    check(s.phase==Core::Phase::NorthFinalAttacks,"phase 8 timeout waits for attack end");
    for(unsigned i=0;i<200&&s.phase==Core::Phase::NorthFinalAttacks;++i)s.update(1,123,200,h);
    check(s.phase==Core::Phase::NorthDeath&&s.phase_frame==0&&s.stones[4].death_frame==0,"timeout begins death on attack boundary");
    for(unsigned i=0;i<96;++i)s.update(1,123,200,h);
    check(!s.done()&&s.stones[4].flag==Core::Flag::Remove,"timeout still needs final invalidation step");
    s.update(1,123,200,h);
    check(s.done()&&score==before_timeout_score&&items.size()==before_timeout_items&&invincibility==0,"timeout completes without kill rewards");

    // The natural route carries damage forward from phase 4 (capped at 500).
    check(death.stones[4].damage==500,"phase 4 damage carries into the final phase");
    Core::Hooks dh;unsigned damage_this_tick=929-death.stones[4].damage;
    dh.damage=[&](int x,int,int,int){return x==208?damage_this_tick:0u;};
    dh.invincibility=[&](unsigned n){invincibility=n;};
    dh.sound=[&](unsigned id){sounds.push_back(id);};dh.item=[&](int,int,unsigned){++death_items;};dh.score=[&](unsigned n){death_score+=n;};
    death.update(1,123,200,dh);
    check(death.stones[4].flag==Core::Flag::Active&&death_score==0,"below north damage threshold");
    damage_this_tick=1;
    for(unsigned t=0;t<96;++t){
        death.update(1,123,200,dh);
        check(!death.done()&&death.phase==Core::Phase::NorthDeath,"north death does not end early");
        check(death.death_effect_count==(t<24?1u:2u),"two death requests with 24-tick offset");
        for(unsigned i=0;i<death.death_effect_count;++i){const auto &e=death.death_effects[i];
            check(e.x==216&&e.y==40&&e.frame==t-i*24&&e.angle==(t<=56?0x20:0xe0),"ordered death effect arguments");
        }
        check(death.draw[4].dying&&death.draw[4].visible==(t<95)&&death.draw[4].pattern==(t<32?163u:10+(t-32)/8),"north death cel and final visibility");
        check(death.terminal_death_frame()==(t==95),"only the final effect request marks a terminal frame");
    }
    check(death.stones[4].flag==Core::Flag::Remove,"north enters Remove on tick 96");
    death.update(1,123,200,dh);
    check(death.done()&&death.stones[4].flag==Core::Flag::Removed&&death.death_effect_count==0&&death.target_x==-1,"final invalidation completes battle");
    death.update(1,123,200,dh);
    check(invincibility==200&&death_score==30000&&sounds==std::vector<unsigned>{4,18,18,14,14}&&death_items==0,"north death rewards and effect sounds");
    std::cout<<"stones_first_half_contract passed\n";return 0;
}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
