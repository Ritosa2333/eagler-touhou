#include "encounter_scene.hpp"
#include "music_room.hpp"
#include <cmath>
#include <iostream>
#include <numbers>
#include <stdexcept>
unsigned checks=0;void check(bool v){++checks;if(!v)throw std::runtime_error("midboss3 scene check="+std::to_string(checks));}

static void north_pixels(soew::EncounterScene &s){
 using Core=soew::StonesCore;
 // Keep the loaded third-stage terrain and cels. Isolate boss drawing from
 // player/bullet artifacts, and force one VRAM seam through the affected area.
 const int actual_line=s.scroll.line;
 s.life.pattern.reset();s.stones.draw={};
 std::vector<std::uint32_t> terrain(640*400),expected(terrain.size()),actual(terrain.size());
 s.render(terrain);
 const auto rgb=[&](unsigned i){const auto c=s.background.palette[i];return (unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];};
 const auto sample=[](int radius,int angle){
     return int(std::floor(radius*std::round(256*std::sin(angle*std::numbers::pi/128))/256));
 };
 for(const int line:{actual_line,357}){
     s.scroll.line=line;
     s.stones.init();
     for(auto &stone:s.stones.stones)stone.flag=Core::Flag::Removed;
     auto &north=s.stones.stones.back();north.flag=Core::Flag::Dying;north.pattern=163;
     // Other stones are absent on a normal clear. Deliberate overlap exercises
     // their earlier draw pass and North's later draw around t=31/32.
     if(line==357)s.stones.stones[2]={201,37,159,Core::Flag::Dormant};
     s.stones.phase=Core::Phase::NorthDeath;
     for(unsigned t=0;t<=96;++t){
         s.stones.update(1,220,300,{});
         const auto pace=t<32||t>=80?1u:(t>=53&&t<=55)||(t>=77&&t<=79)?2u:3u;
         check(s.presentation_slowdown()==pace&&s.boss_terminal_frame()==(t==95));
         if(t!=0&&t!=24&&t!=31&&t!=32&&t!=40&&t!=54&&t!=56&&t!=57&&t!=74&&t!=79&&t!=94&&t!=95&&t!=96)continue;
         for(bool reduced:{false,true}){
             s.reduced_effects=reduced;expected=terrain;
             const bool relocate=!reduced&&t%2&&((t>=32&&t<=52)||(t>=56&&t<=76));
             const bool brighten=!reduced&&t%2&&((t>=32&&t<=40)||(t>=56&&t<=64));
             const auto dot=[&](int x,int top,int dy,unsigned i,bool rolling){
                 if(x<32||x>=416+(relocate?16:0)||top+dy<16||top+dy>=384)return;
                 if(!rolling&&(top+line)%400+dy>=400)return;
                 expected[(top+dy)*640+x]=rgb(i);
             };
             const auto cel=[&](unsigned id,int left,int top,bool rolling){
                 const auto &g=*s.bank.sprites.at(id);
                 for(unsigned y=0;y<g.height;++y)for(unsigned x=0;x<g.width;++x){
                     const auto i=g.pixels[y*g.width+x];
                     if(i!=g.transparent)dot(left+int(x),top,int(y),i,rolling);
                 }
             };
             if(line==357)cel(159,201,37,true);
             for(unsigned lag:{0u,24u})if(t>=lag&&t<=95){
                 const auto f=t-lag;
                 if(f<30){
                     const int r=240-8*int(f),w=r/64+1;
                     for(int a=0;a<256;a+=2){
                         const int x=223+sample(r,a+64),y=40+sample(r,a);
                         if(x<=24||x>=416||y<=8||y>=384)continue;
                         for(int dy=0;dy<w;++dy)for(int dx=0;dx<w;++dx)dot(x+dx,y,dy,f%2?15:4,false);
                     }
                 }else if(f>=32&&f<56){
                     const int age=int(f)-32,r=16*age;
                     for(int a=0;a<256;a+=4){
                         const int x=216+sample(r,a+age+64),y=40+sample(r,a+age+(t<=56?32:-32));
                         if(x>16&&x<416&&y>0&&y<384)cel(88+unsigned(age/6),x,y,true);
                     }
                 }
             }
             if(t<95)cel(t<32?163:10+(t-32)/8,208,32,t<32);
             if(relocate)for(int y=16;y<384;++y){
                 auto first=expected.begin()+y*640+32;
                 std::copy(first+16,first+400,first);std::fill_n(first+384,16,0u);
             }
             if(brighten)for(auto &c:expected){
                 unsigned bright=0;
                 for(unsigned b:{0u,8u,16u})bright|=(17*(15-(15-((c>>(b+4))&15))*3/10))<<b;
                 c=bright;
             }
             s.render(actual);
             if(actual!=expected)throw std::runtime_error("North compositing mismatch at age "+std::to_string(t)+", scroll "+std::to_string(line)+", reduced "+std::to_string(reduced));
             if(t==95){s.render(actual,false);check(actual==expected);}
         }
     }
 }
}
int main(int argc,char **argv){try{if(argc!=2)throw std::runtime_error("Need asset root");auto previous=soew::EncounterScene::load(argv[1],1,0,1,2,3);
 previous.combat_enabled=true;previous.survival_enabled=true;previous.enable_effects();
 // Start at the Stage 2 settlement boundary; Stage 3 uses the normal loader.
 previous.transition.phase=soew::StageTransition::Phase::Ready;
 auto s=previous.next_stage();
 check(s.stage==2&&s.midboss3_enabled&&s.preboss_enabled&&s.transitions_enabled);
 check(s.transition.phase==soew::StageTransition::Phase::Entry);
 for(unsigned i=0;i<5000&&!s.midboss3.active&&!s.runtime.blocked;++i){s.life.invincibility=200;s.tick(0);}check(!s.runtime.blocked);check(s.midboss3.active);check(!s.stage3_preview_boundary);std::vector<std::uint32_t> pixels(640*400);unsigned frames=0;for(;frames<2100&&s.midboss3.active&&!s.runtime.blocked;++frames){s.life.invincibility=200;s.tick(0);if(frames%8==0)s.render(pixels);}check(!s.runtime.blocked);check(!s.midboss3.active);check(frames>0);const auto before=s.ticks;const auto progress=s.runtime.scroll.progress;for(unsigned i=0;i<12000&&!s.runtime.scroll.done&&!s.runtime.blocked;++i){s.life.invincibility=200;s.tick(0);if(i%64==0)s.render(pixels);}check(!s.runtime.blocked);check(s.runtime.scroll.done&&s.runtime.scroll.progress>progress&&s.ticks>before);check(!s.midboss3.active);
 check(s.preboss_started&&s.dialogue.active()); // Map end already started the dialogue.
 const auto advance_dialogue=[&]{for(unsigned i=0;i<2000&&s.dialogue.active();++i){
     const bool press=s.dialogue.phase==soew::StageDialogue::Phase::KeyDelay&&s.dialogue.key_delay.waiting_press;
     s.tick(0,press);if(i%16==0)s.render(pixels);
 }check(!s.dialogue.active());};
 advance_dialogue();check(s.stones_enabled&&s.dialogue.current==8);
 {
     auto visuals=std::make_unique<soew::EncounterScene>(soew::EncounterScene::load(argv[1],2,0,1,2,3));
     visuals->runtime.scroll=visuals->scroll=s.scroll;visuals->survival_enabled=true;
     visuals->bind_stones_battle();north_pixels(*visuals);
 }
 const auto frozen=s.runtime.scroll.line;bool saw_stripe=false,saw_death=false,saw_terminal=false;
 for(unsigned i=0;i<9000&&!s.stones.done();++i){
     s.life.invincibility=200;s.tick(0);
     if(i%16==0||s.stones.death_effect_count)s.render(pixels);
     check(s.runtime.scroll.line==frozen&&!s.runtime.blocked);
     saw_stripe|=!s.stones_stripe.empty();saw_death|=s.stones.death_effect_count!=0;
     if(s.boss_terminal_frame()){
         check(!saw_terminal&&s.transition.phase==soew::StageTransition::Phase::Battle&&!s.dialogue.active());
         saw_terminal=true;
     }
 }
 check(s.stones.done()&&saw_stripe&&saw_death&&saw_terminal&&s.lasers.spawned>0&&!s.boss_terminal_frame());
 s.render(pixels,false);const auto cleared_tick=s.ticks;
 s.tick(0);check(s.ticks==cleared_tick&&s.transition.phase==soew::StageTransition::Phase::Postboss&&s.dialogue.current==8&&s.dialogue_snapshot==pixels);
 advance_dialogue();check(s.dialogue.current==11&&s.transition.phase==soew::StageTransition::Phase::BonusAck);
 for(unsigned i=0;i<200&&s.transition.phase!=soew::StageTransition::Phase::Ready;++i)s.tick(0,s.transition.gate.waiting_press);
 check(s.transition.phase==soew::StageTransition::Phase::Ready);
 auto next=s.next_stage();
 check(next.stage==3&&next.transition.phase==soew::StageTransition::Phase::Entry&&!next.stones_enabled&&next.life.lives==s.life.lives&&next.life.bombs==s.life.bombs);
 // First walk the ordinary handoff with the authentic script and no map-end override.
 // Protect the player only, so the known resource boundary is reachable deterministically.
 {
     auto route=std::make_unique<soew::EncounterScene>(s.next_stage(true));
     check(!route->script.stage4_compatibility);
     for(unsigned i=0;i<10000&&!route->runtime.blocked;++i){
         route->life.invincibility=200;route->tick(0);
     }
     check(route->midboss4.entries==2&&route->runtime.dispatch.cursor==266);
     check(route->runtime.scroll.progress==2175&&route->runtime.blocked.has_value());
     check(route->runtime.blocked->result.reason=="declared formation row unavailable");
     check(!route->runtime.scroll.done&&!route->dialogue.active()&&!route->marisa_enabled&&!route->marisa_music);
     const auto stopped=route->ticks;route->tick(soew::Shot,true);
     check(route->ticks==stopped&&!route->preboss_started);
 }
 // Exercise the normal Stage 3 settlement handoff, not a diagnostic override.
 check(next.script.declared_formations==270&&next.script.formations.size()==266);
 check(next.script.stage4_compatibility);
 check(!next.script.complete()&&next.script.declared_formations==270);
 for(unsigned i=0;i<10000&&!next.dialogue.active()&&!next.runtime.blocked;++i){
     next.life.invincibility=200;next.tick(0);
 }
 check(!next.runtime.blocked&&next.runtime.dispatch.cursor==266&&next.midboss4.entries==2);
 check(next.runtime.scroll.done&&next.runtime.scroll.display_pixels==17920&&next.runtime.scroll.progress==2240);
 check(next.preboss_started&&next.dialogue.active()&&!next.marisa_enabled&&!next.marisa_music);
 const auto before_dialogue=next.ticks;
 const auto play=[&](soew::EncounterScene &route){
     for(unsigned i=0;i<1500&&route.dialogue.active();++i){
         const bool press=route.dialogue.phase==soew::StageDialogue::Phase::KeyDelay&&route.dialogue.key_delay.waiting_press;
         if(!route.dialogue.postboss&&route.dialogue.phase==soew::StageDialogue::Phase::Text){
             const bool boss_line=route.dialogue.current>=soew::StageDialogue::stage4_generic_end;
             check(route.marisa_music==boss_line);
             const auto track=soew::gameplay_music_track(route.stage,route.preboss_started,false,route.marisa_music);
             check(soew::music_files.at(track)==(boss_line?"BOSS3.M":"STAGE3.M"));
         }
         route.tick(soew::Shot,press);
         if(i%16==0)route.render(pixels);
     }
     check(!route.dialogue.active());
 };
 const auto settle=[&](soew::EncounterScene &route){
     route.tick(0);check(route.dialogue.postboss&&route.dialogue.active());
     play(route);check(route.dialogue.current==52&&route.transition.phase==soew::StageTransition::Phase::BonusAck);
     for(unsigned i=0;i<200&&route.transition.phase!=soew::StageTransition::Phase::Ready;++i)
         route.tick(0,route.transition.gate.waiting_press);
     auto fifth=std::make_unique<soew::EncounterScene>(route.next_stage());
     check(fifth->stage==4&&fifth->stage5_entry_only&&!fifth->marisa_enabled&&!fifth->marisa_music);
     check(fifth->life.lives==route.life.lives&&fifth->life.bombs==route.life.bombs);
     for(unsigned i=0;i<100;++i)fifth->tick(0);
     check(fifth->ticks==0&&fifth->runtime.scroll.progress==0);
     // Open the same gate as the isolated mainline candidate: no scroll or boss injection.
     fifth->enable_stage5_preview();
     for(unsigned i=0;i<3000&&!fifth->dialogue.active()&&!fifth->runtime.blocked;++i){
         fifth->life.invincibility=200;fifth->tick(0);
     }
     check(!fifth->runtime.blocked&&fifth->runtime.scroll.done&&fifth->runtime.scroll.progress==176);
     check(fifth->runtime.scroll.display_pixels==1408&&fifth->runtime.dispatch.cursor==76);
     check(fifth->preboss_started&&fifth->dialogue.active()&&!fifth->mima_enabled&&!fifth->mima_music);
 };
 play(next);check(next.marisa_enabled&&next.marisa_music&&next.ticks==before_dialogue);
 // Independent input-driven branch: no injected projectiles or core damage hooks.
 {
     auto fight=std::make_unique<soew::EncounterScene>(next);
     fight->enable_bombing();fight->pickup_enabled=true;fight->pickups.power=80;
     const auto stock=fight->life.bombs;
     check(stock>0);
     bool saw_bomb=false,saw_recovery=false;
     for(unsigned i=0;i<30000&&!fight->marisa.cleared;++i){
         fight->life.invincibility=200; // deterministic input test, not a survival claim
         const int dx=fight->marisa.left+24-fight->player.x;
         const auto move=dx>4?soew::Right:dx<-4?soew::Left:0;
         fight->tick(soew::Shot|move|(i==0?soew::Bomb:0));
         saw_bomb|=fight->bomb.active;
         saw_recovery|=saw_bomb&&!fight->bomb.active&&!fight->life.bomb_invincible;
         if(i%128==0)fight->render(pixels);
     }
     std::cout<<"marisa_input shots="<<fight->shots.fired<<" shot_damage="<<fight->shots.damage_events
              <<" body_damage="<<fight->marisa.damage_taken<<" rounds="<<fight->marisa.rounds
              <<" bombs_used="<<unsigned(fight->bomb.stage_used)<<'\n';
     check(saw_bomb&&saw_recovery&&fight->bomb.stage_used==1&&fight->life.bombs==stock-1);
     check(fight->shots.fired>0&&fight->shots.damage_events>0&&fight->marisa.cleared&&!fight->runtime.blocked);
     check(fight->marisa.damage_taken>=900&&fight->marisa.rounds<7);
     settle(*fight);
 }
 for(unsigned i=0;i<30000&&!next.marisa.cleared;++i){
     next.life.invincibility=200;next.tick(0);
     if(i%64==0||next.marisa.defeated)next.render(pixels);
 }
 check(next.marisa.cleared&&next.marisa.rounds==7&&!next.runtime.blocked);
 settle(next);
 std::cout<<"midboss3_scene_checks="<<checks<<" frames="<<frames<<" failures=0\n";return 0;}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
