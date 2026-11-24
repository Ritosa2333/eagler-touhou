#include "encounter_scene.hpp"
#include "shot_input.hpp"
#include <iostream>
#include <source_location>
unsigned checks=0;void check(bool v,const std::source_location where=std::source_location::current()){++checks;if(!v)throw std::runtime_error("Rika scene failed="+std::to_string(checks)+" line="+std::to_string(where.line()));}
soew::EncounterScene setup(const char* root,unsigned shot,unsigned rank){auto s=soew::EncounterScene::load(root,0,int(shot),int(rank),2,3);s.combat_enabled=s.survival_enabled=s.pickup_enabled=true;s.enable_effects();s.enable_bombing();s.bind_rika_battle({0,0,0,0,0,0});return s;}
int main(int argc,char**argv){try{if(argc<2)throw std::runtime_error("need assets");std::vector<std::uint32_t> pixels(640*400),repeat(640*400);
 for(unsigned shot=0;shot<3;++shot)for(unsigned rank=0;rank<4;++rank){auto s=setup(argv[1],shot,rank);s.rika.damage_taken=1400;unsigned stages=0;
  for(unsigned f=0;f<360;++f){s.life.invincibility=200;s.tick(0);check(!s.runtime.blocked&&!s.life.game_over&&s.runtime.scroll.display_pixels==0);s.render(pixels);s.render(repeat);check(pixels==repeat);if(f>=100){check(s.lasers.spawned==2);++stages;}}
  check(stages==260&&s.lasers.spawned==2&&s.rika.pattern==153&&s.bullet_core.spawned>0);std::cout<<"rika_scene shot="<<shot<<" rank="<<rank<<" bullets="<<s.bullet_core.spawned<<" lasers="<<s.lasers.spawned<<'\n';
 }
 // Bomb blocks ordinary pellet generation, NOT the dedicated laser API.
 {auto s=setup(argv[1],1,1);s.rika.damage_taken=1400;s.rika.state=2;s.rika.attack_frame=99;s.tick(soew::Bomb);check(s.bomb.active&&s.lasers.spawned==2&&s.lasers.slots[0].delay==63);const auto stock=s.life.bombs;for(unsigned f=0;f<90;++f){s.life.invincibility=200;s.tick(0);}check(s.bomb.active&&s.life.bombs==stock);bool active=false;for(const auto&l:s.lasers.slots)active|=l.state==4;check(active);}
 // Defeat returns only after real cumulative threshold and80death updates.
 for(unsigned shot=0;shot<3;++shot){auto s=setup(argv[1],shot,1);s.rika.damage_taken=2260;s.tick(soew::Bomb);check(s.rika.dying&&s.rika.damage_taken==2261&&s.life.invincibility==199&&s.score_core.score+s.score_core.delta==20001);for(unsigned f=0;f<79;++f){s.tick(0);s.render(pixels);check(!s.rika.done);}s.tick(0);check(s.rika.done&&s.rika.death_frame==0);auto end=s.ticks;s.tick(soew::Shot);check(s.ticks==end);}
 // Live high-power shots, rather than forced HP or Bomb, cross all bands.
 for(unsigned shot=0;shot<3;++shot){auto s=setup(argv[1],shot,1);s.pickups.power=80;unsigned f=0;for(;f<12000&&!s.rika.done;++f){s.life.invincibility=200;s.tick(soew::Shot);if(f%31==0)s.render(pixels);}check(s.rika.done&&s.rika.damage_taken>2260&&s.shots.fired>0);check(s.lasers.spawned==2&&s.score_core.score+s.score_core.delta>=22261);std::cout<<"rika_live_shots type="<<shot<<" frames="<<f<<" damage="<<s.rika.damage_taken<<'\n';}
 // Same initial scene and no Bomb or misses: compare sustained hold, the
 // old 12 Hz Z waveform, and the four-tick Z waveform at 60 Power (level 7).
 for(unsigned shot=0;shot<3;++shot){
    constexpr unsigned power=60;
    unsigned clear_frames[3]={};
    for(unsigned mode=0;mode<3;++mode){
        auto s=setup(argv[1],shot,1);s.pickups.power=power;
        check(s.pickups.level()==7);
        using enum soew::ShotInputMode;
        const auto input_mode=std::array{Manual,Autofire12Hz,Fast}[mode];
        soew::ShotInput z_input;z_input.event(true,input_mode);
        unsigned f=0,first_hit=0,phase_two=0,phase_three=0,lethal=0;
        for(;f<12000&&!s.rika.done;++f){
            s.life.invincibility=200;
            const bool z=z_input.tick(input_mode);
            s.tick(z?soew::Shot:0);
            const auto d=s.rika.damage_taken;
            if(d&&!first_hit)first_hit=f+1;
            if(d>=700&&!phase_two)phase_two=f+1;
            if(d>=1400&&!phase_three)phase_three=f+1;
            if(d>2260&&!lethal)lethal=f+1;
        }
        check(s.rika.done&&s.rika.damage_taken>2260&&s.shots.fired>0&&s.life.stage_misses==0&&s.bomb.stage_used==0);
        check(first_hit>0&&first_hit<phase_two&&phase_two<phase_three&&phase_three<lethal&&lethal+80==f);
        clear_frames[mode]=lethal;
        std::cout<<"rika_power="<<power<<" level="<<s.pickups.level()<<" shot="<<shot<<" mode="<<mode<<" phases="<<first_hit<<','<<phase_two<<','<<phase_three<<','<<lethal<<" damage="<<s.rika.damage_taken<<'\n';
    }
    check(clear_frames[0]>clear_frames[1]&&clear_frames[1]>clear_frames[2]);
 }
 {auto s=setup(argv[1],0,1);s.runtime.scroll.line=377;s.scroll=s.runtime.scroll;s.bind_rika_battle({0,0,0,0,0,377});s.rika.damage_taken=1400;for(unsigned f=0;f<190;++f){s.life.invincibility=200;s.tick(0);s.render(pixels);}check(s.rika.render_y==25&&s.lasers.spawned==2&&s.runtime.scroll.line==377);}
 // A fresh map-end scene still freezes: diagnostic binding is not an automatic
 // substitute for the missing preboss dialogue/normal handoff.
 {auto s=soew::EncounterScene::load(argv[1],0,0,1,2,3);s.runtime.scroll.done=true;s.tick(0);check(!s.rika_enabled&&s.ticks==0);}
 // Laser's late collision latches a hit; death begins on next player phase.
 {auto s=setup(argv[1],0,1);auto &l=s.lasers.slots[0];l.flag=1;l.state=4;l.charge_age=5;l.x=s.player.x;l.y=112;l.active_frames=100;l.base_pattern=111;s.tick(0);check(s.life.hit&&!s.life.miss_active);s.tick(0);check(s.life.miss_active&&s.life.miss_frame==1);}
 std::cout<<"rika_scene_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
