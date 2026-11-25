#include "encounter_scene.hpp"
#include <iostream>
#include <source_location>
unsigned checks=0;void check(bool v,const std::source_location p=std::source_location::current()){++checks;if(!v)throw std::runtime_error("Preboss failed="+std::to_string(checks)+" line="+std::to_string(p.line()));}
soew::EncounterScene setup(const char*root,unsigned shot,unsigned rank){auto s=soew::EncounterScene::load(root,0,int(shot),int(rank),2,3);s.combat_enabled=s.survival_enabled=s.pickup_enabled=true;s.enable_effects();s.enable_bombing();s.enable_midboss();s.enable_preboss();return s;}
int main(int argc,char**argv){try{if(argc<2)throw std::runtime_error("need assets");std::vector<std::uint32_t>a(640*400),b(640*400);
 for(unsigned shot=0;shot<3;++shot){auto s=setup(argv[1],shot,1);for(unsigned i=0;i<20000&&!s.dialogue.active();++i){s.life.invincibility=200;s.tick(0);check(!s.runtime.blocked);}check(s.runtime.scroll.done&&s.preboss_started&&!s.rika_enabled&&s.dialogue.phase==soew::StageDialogue::Phase::SlideIn);const auto ticks=s.ticks,rt=s.runtime.ticks;const auto rng=s.runtime.random.cursor,stock=s.life.bombs,inv=s.life.invincibility;const int x=s.player.x,y=s.player.y;const auto score=s.score_core.score,delta=s.score_core.delta;
  s.paused=true;s.tick(soew::Shot,true);check(s.dialogue.updates==0);s.paused=false;
  for(unsigned i=0;i<23;++i){s.tick(soew::Shot|soew::Bomb|soew::Left,true);s.render(a);}check(s.dialogue.box_left==32&&s.dialogue.phase==soew::StageDialogue::Phase::Text);
  for(unsigned box=0;box<16;++box){check(s.dialogue.current==box);while(s.dialogue.phase==soew::StageDialogue::Phase::Text)s.tick(soew::Shot,true);s.render(a);s.render(b);check(a==b);for(unsigned i=0;i<10;++i)s.tick(soew::Shot,true);check(s.dialogue.current==box);for(unsigned i=0;i<4;++i)s.tick(0,false);check(s.dialogue.phase==soew::StageDialogue::Phase::KeyDelay);for(unsigned i=0;i<2;++i)s.tick(soew::OK,true);check(s.dialogue.current==box+1);check(s.ticks==ticks&&s.runtime.ticks==rt&&s.runtime.random.cursor==rng&&s.life.bombs==stock&&s.life.invincibility==inv&&s.player.x==x&&s.player.y==y&&s.score_core.score==score&&s.score_core.delta==delta&&!s.rika_enabled);}
  check(s.dialogue.phase==soew::StageDialogue::Phase::SlideOut);for(unsigned i=0;i<23;++i){s.tick(0,false);check(!s.rika_enabled);s.render(a);}s.tick(0,false);check(s.rika_enabled&&s.normal_rika_handoff&&s.rika_stage_frame==0&&s.rika.phase_frame==s.midboss.phase_frame&&s.dialogue.current==16&&s.dialogue.phase==soew::StageDialogue::Phase::Complete);check(!s.midboss_enabled&&s.rika.render_y==int((48+s.scroll.line)%400));s.tick(0,false);check(s.rika_stage_frame==1&&s.ticks==ticks+1);s.render(a);
  s.pickups.power=80;for(unsigned f=0;f<12000&&!s.rika.done;++f){s.life.invincibility=200;s.tick(soew::Shot);if(f%47==0)s.render(a);}check(s.rika.done&&s.rika.damage_taken>2260&&s.dialogue.current==16&&s.stage==0);const auto end=s.ticks;s.tick(soew::OK,true);check(s.ticks==end&&s.stage==0);
  // Same natural map/dialogue/live-shot defeat, then all8 postboss boxes
  // and two independent acknowledgments, without forcing boss damage.
  s.transitions_enabled=true;s.tick(0);check(s.dialogue.postboss&&s.dialogue.current==16);
  for(unsigned box=16;box<24;++box){while(s.dialogue.phase==soew::StageDialogue::Phase::SlideIn||s.dialogue.phase==soew::StageDialogue::Phase::Text)s.tick(soew::Shot,true);check(s.dialogue.current==box);for(unsigned i=0;i<4;++i)s.tick(0,false);for(unsigned i=0;i<2;++i)s.tick(soew::OK,true);}
  check(s.transition.phase==soew::StageTransition::Phase::BonusAck);for(unsigned ack=0;ack<2;++ack){for(unsigned i=0;i<4;++i)s.tick(0,false);for(unsigned i=0;i<2;++i)s.tick(soew::OK,true);}for(unsigned i=0;i<142;++i)s.tick(0);check(s.transition.phase==soew::StageTransition::Phase::Ready);auto next=s.next_stage();for(unsigned i=0;i<42;++i)next.tick(0);for(unsigned i=0;i<300;++i){next.life.invincibility=200;next.tick(0);}check(next.stage==1&&next.ticks==300&&!next.runtime.blocked&&next.pickups.power==80);
  std::cout<<"preboss shot="<<shot<<" map_ticks="<<ticks<<" dialogue_updates="<<s.dialogue.updates<<" scroll="<<s.scroll.line<<'\n';
 }
 // Terminal map-end frames must not enter dialogue and falsely bind a frozen Rika.
 for(unsigned terminal=0;terminal<3;++terminal){auto s=setup(argv[1],0,1);s.runtime.scroll.done=true;if(terminal==0)s.life.game_over=true;else if(terminal==1)s.weapon_blocked=true;else s.runtime.blocked=soew::EnemyPathRuntime::Block{};s.tick(0);check(!s.dialogue.active()&&!s.preboss_started&&!s.rika_enabled);}
 // Acceleration has no original frame_delay:finish text on that update, but a
 // held key cannot satisfy the separate release-and-new-press handshake.
 {auto d=soew::StageDialogue::decode(soew::StageScene::read(std::filesystem::path(argv[1])/"STAGE0.TXT"));d.start_preboss();for(unsigned i=0;i<23;++i)d.tick(false,false);d.tick(true,true);check(d.phase==soew::StageDialogue::Phase::KeyDelay&&d.reveal.waiting);for(unsigned i=0;i<20;++i)d.tick(true,true);check(d.current==0);}
 // Original bonus grant intentionally retains delta; capped lives don't add bombs.
 {soew::ScoreCore s;s.score=99990;s.delta=20;unsigned lives=5;s.grant_dialogue_bonus(lives);check(s.score==100010&&s.delta==20&&lives==5&&s.extends==1&&s.sound8==0);s.score=199990;lives=2;s.grant_dialogue_bonus(lives);check(lives==3&&s.extends==2&&s.delta==20&&s.sound8==1);}
 std::cout<<"preboss_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
