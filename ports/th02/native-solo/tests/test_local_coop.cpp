#include "encounter_scene.hpp"
#include "shot_input.hpp"
#include <iostream>
#include <source_location>

using namespace soew;

void check(bool ok,std::source_location at=std::source_location::current()){
    if(!ok)throw std::runtime_error("Local co-op assertion at line "+std::to_string(at.line()));
}

void check_shots(){
    for(unsigned type=0;type<3;++type){
        const auto run=[&](unsigned capacity){
            PlayerShots shots;shots.slots.resize(capacity);shots.level=9;shots.shot_type=type;
            IRand seed(1234);RandomRing random;random.fill(seed);shots.random=&random;
            ShotInput fire;fire.event(true,ShotInputMode::Fast);
            std::size_t peak=0;
            for(unsigned f=0;f<2400;++f){
                const unsigned page=f&1;
                shots.target_x=f%360<180?385:40;shots.target_y=f%360<180?120:380;
                shots.begin_frame(page);
                shots.input(fire.tick(ShotInputMode::Fast),page,208,344,Down);
                peak=std::max(peak,std::size_t(std::count_if(shots.slots.begin(),shots.slots.end(),[](const auto &s){return s.flag==1;})));
                shots.update(page,f);
            }
            return std::pair{shots.fired,peak};
        };
        const auto expanded=run(PlayerShots::coop_capacity);
        check(expanded==run(1024)&&expanded.second<PlayerShots::coop_capacity);
        std::cout<<"Fast full power "<<char('A'+type)<<": peak "<<expanded.second<<" slots, "<<expanded.first<<" shots\n";
    }
    PlayerShots solo,coop;
    coop.slots.resize(PlayerShots::coop_capacity);
    PlayerShots::Slot shot;
    shot.flag=1;shot.position[0]={1600,1600};
    solo.slots[35]=coop.slots.back()=shot;
    check(solo.damage(0,96,96)==0&&coop.damage(0,96,96)>0);
}

void check_boss_damage(){
    BossDamage total;
    total.add(15,3);check(total==5);
    total.add(10,2);check(total==10);
    total.add(9,1);check(total==19);

    MarisaCore marisa;
    marisa.phase=MarisaCore::Phase::DeployOrbs;
    marisa.phase_frame=140;
    marisa.orbs[0].active=marisa.orbs[1].active=true;
    marisa.orbs[0].hits=109;
    MarisaCore::Hooks h;
    h.damage_divisor=3;
    h.orb_damage=[](int,int,int,int){return 120u;};
    h.random8=[](){return 0u;};
    for(unsigned i=1;i<=3;++i){
        marisa.update(h);
        check(marisa.orbs[0].hits.sixths()==109*6+2*i);
        check(marisa.orbs[1].hits.sixths()==2*i);
        check(marisa.orbs[0].dying==(i==3)&&!marisa.orbs[1].dying);
    }
}

void check_midboss_damage(const std::filesystem::path &assets){
    auto scene=EncounterScene::load(assets,1,0,1,2,3);
    scene.combat_enabled=scene.survival_enabled=scene.midboss2_enabled=true;
    scene.enable_coop(LocalCoopConfig{3,1,1,2,3,{0,1,2}});
    auto &boss=scene.midboss2;
    boss.active=true;boss.phase_frame=2;boss.top.fill(80);
    const auto shoot=[&]{
        PlayerShots::Slot shot;
        shot.flag=1;shot.position.fill({200*16,90*16});
        scene.seat_shots(0).slots[0]=shot; // Six raw damage at power level zero.
        scene.tick(0);
        check(boss.hit_flash);
    };
    shoot();check(boss.damage_taken.sixths()==2);
    scene.seat_life(2).game_over=true;
    shoot();check(boss.damage_taken.sixths()==5);
    scene.seat_life(1).game_over=true;
    shoot();check(boss.damage_taken.sixths()==11);
    scene.coop->revive(scene.seat_motion(2),scene.seat_life(2),2);
    shoot();check(boss.damage_taken.sixths()==14);
    boss.phase_frame=319; // After the entrance, accept the full hit before scaling.
    shoot();check(boss.damage_taken.sixths()==32);
}

void check_run(const std::filesystem::path &assets){
    auto scene=EncounterScene::load(assets,0,0,1,2,3);
    scene.combat_enabled=scene.survival_enabled=scene.pickup_enabled=true;
    scene.enable_effects(1234);scene.enable_bombing();
    scene.enable_coop(LocalCoopConfig{3,0,1,2,3,{0,1,2}});
    auto &c=*scene.coop;
    check(scene.alive_count()==3);
    check(scene.seat_motion(0).x==128&&scene.seat_motion(1).x==288&&scene.seat_motion(2).x==208);
    LocalCoopState duo;duo.config.count=2;
    check(duo.spawn(0).x==176&&duo.spawn(1).x==240);
    check(scene.seat_shots(0).slots.size()==128&&scene.seat_shots(2).slots.size()==128);

    const auto tick=[&]{scene.tick(c.input[0]);};
    c.input={Right|Shot,Down|Shot,Left|Shot};
    tick();
    check(scene.seat_motion(0).x>128&&scene.seat_motion(1).y>336&&scene.seat_motion(2).x<208);
    check(scene.seat_shots(0).fired&&scene.seat_shots(1).fired&&scene.seat_shots(2).fired);

    c.input={0,Bomb,Bomb};
    tick();
    check(scene.bomb.active&&scene.bomb.shot_type==1&&c.bomb_owner==1);
    check(scene.seat_life(0).bombs==3&&scene.seat_life(1).bombs==2&&scene.seat_life(2).bombs==3);
    for(unsigned i=0;i<3;++i)check(scene.seat_life(i).bomb_invincible);
    c.input={};
    for(unsigned i=0;scene.bomb.active&&i<700;++i)tick();
    check(!scene.bomb.active&&!scene.seat_life(2).bomb_invincible);
    auto &receiver=scene.seat_life(0);auto &donor=scene.seat_life(1);
    receiver.invincibility=0;receiver.lives=0;scene.seat_pickups(0).power=40;
    const auto &p=scene.seat_motion(0);
    auto &bullet=scene.bullet_core.bullets[0];bullet.flag=1;
    const std::array<std::uint16_t,2> bullet_pos{std::uint16_t((p.x+12)*16),std::uint16_t((p.y+16)*16)};
    bullet.position={bullet_pos,bullet_pos};
    scene.lasers.add(32,24,120,60);
    tick();
    check(receiver.hit&&!scene.lasers.slots[0].flag&&bullet.flag!=1);
    for(unsigned i=0;i<26;++i)tick();
    check(receiver.game_over&&!scene.run_over()&&scene.alive_count()==2);

    const auto before=donor.lives;
    scene.score_core.score=99999;
    scene.score_core.add(1);
    tick();
    check(receiver.game_over&&receiver.lives==1&&donor.lives==before+1);
    auto &third=scene.seat_life(2);
    const auto drop_at_donor=[&](unsigned type){
        const auto &p=scene.seat_motion(1);
        check(scene.items.add(p.x+8,p.y+8,type,scene.item_spare_stock()));
        tick();
    };
    donor.bombs=3;third.bombs=5;
    const auto ghost_bombs=receiver.bombs;
    drop_at_donor(ItemCore::Bomb);
    check(donor.bombs==4&&third.bombs==5&&receiver.bombs==ghost_bombs);
    third.bombs=3;
    drop_at_donor(ItemCore::Bomb);
    check(donor.bombs==5&&third.bombs==4&&receiver.bombs==ghost_bombs);
    third.lives=5;
    drop_at_donor(ItemCore::OneUp);
    check(receiver.game_over&&receiver.lives==2&&donor.lives==before+2&&third.lives==5);
    scene.score_core.score=199999;
    scene.score_core.add(1);
    tick();
    check(receiver.game_over&&receiver.lives==3&&receiver.bombs==ghost_bombs&&
          donor.lives==before+3&&third.lives==5&&third.bombs==5);
    receiver.lives=2; // Keep the rescue test's original reserve count.
    const auto spare=donor.lives,stock=donor.bombs;
    c.spawn_at(scene.seat_motion(1),donor,0);
    // A physical held Shot disqualifies rescue even on an autofire off tick.
    c.fire_held[1]=true;
    c.input[1]=Focus;scene.step_rescue();
    check(c.rescue[1].presses==0);
    c.fire_held[1]=false;
    for(unsigned i=0;i<5;++i){
        c.input[1]=0;scene.step_rescue();
        c.input[1]=Focus;scene.step_rescue();
        if(i<4)check(receiver.game_over);
    }
    check(!receiver.game_over&&receiver.lives==2&&receiver.bombs==1&&receiver.invincibility==150);
    check(scene.seat_pickups(0).power==40&&donor.lives==spare-1&&donor.bombs==stock/2);

    for(unsigned i=0;i<3;++i)scene.seat_life(i).lives=5;
    drop_at_donor(ItemCore::OneUp);
    check(receiver.lives==5&&receiver.bombs==2&&donor.bombs==stock/2+1&&third.bombs==5);

    receiver.game_over=true;
    receiver.lives=2;
    scene.transition.phase=StageTransition::Phase::Ready;
    const auto next=scene.next_stage();
    check(next.alive_count()==3&&!next.seat_life(0).game_over&&next.seat_life(0).lives==2);
    for(unsigned i=0;i<3;++i)scene.seat_life(i).game_over=true;
    check(scene.run_over());
    scene.update_continue(0,false);scene.update_continue(0,false);scene.update_continue(OK,false);
    check(scene.alive_count()==3&&scene.continues_used==1);
}

int main(int argc,char **argv){
    try{
        check_shots();
        check_boss_damage();
        if(argc>1&&argv[1][0]){
            check_midboss_damage(argv[1]);
            check_run(argv[1]);
        }
        return 0;
    }catch(const std::exception &error){std::cerr<<error.what()<<'\n';return 1;}
}
