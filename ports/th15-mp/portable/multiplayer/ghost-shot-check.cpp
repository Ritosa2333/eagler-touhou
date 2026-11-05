#include "native-world.hpp"
int main(int argc,char** argv){
 require(argc==2,"Retail archive required");RetailAssets files(argv[1]);
 for(unsigned count:{2u,3u})for(unsigned seat=0;seat<count;++seat)for(int character:{1,0,2,3})for(unsigned focus:{0u,8u}){
  World w(files,count,seat,{character,character,character});auto& b=*w.battle;auto& p=*b.pilots[seat];
  BattleFrame frame;frame.focus_allowed=true;frame.player_enabled=frame.enemies_enabled=frame.items_enabled=false;require(b.step(frame),b.error);
  std::vector<u32> handles;unsigned lasers=0;
  p.input.held=1|focus;
  for(unsigned n=0;n<45;++n)require(p.tick(),b.error);
  for(const auto& shot:p.player->shots.shots)if(shot.state){require(shot.animation!=0,"Shot fixture has no animation");handles.push_back(shot.animation);lasers+=p.player->shots.specification(shot.specification)->type==2;}
  require(!handles.empty(),"Held fire did not create real shots");
  if(character==1&&!focus)require(lasers>0,"Marisa fixture did not create a laser");
  auto& teammate=*b.pilots[(seat+1)%count];teammate.input.held=1|focus;
  for(unsigned n=0;n<45;++n)require(teammate.tick(),b.error);
  std::vector<u32> teammate_handles;for(const auto& shot:teammate.player->shots.shots)if(shot.state)teammate_handles.push_back(shot.animation);
  require(!teammate_handles.empty(),"Teammate shot fixture is empty");
  p.session.extra_lives=0;p.player->life.state=4;p.player->life.age.set(8);
  for(unsigned n=0;n<40&&!p.ghost;++n)require(p.tick(),b.error);
  require(p.ghost&&p.session.extra_lives==-1,"Final held-fire death did not become a ghost");
  for(u32 handle:teammate_handles){const auto* vm=w.animations.registry.find(handle);require(vm&&!(vm->visual.render_flags&32),"Final death retired a teammate's shot");}
  for(u32 handle:handles){const auto* vm=w.animations.registry.find(handle);require(!vm||(vm->visual.render_flags&32),"Final death left a live shot animation in the shared registry");}
  for(const auto& shot:p.player->shots.shots)require(!shot.state&&!shot.animation,"Ghost retained shot state or animation ownership");
  for(const auto& source:p.player->damage.sources)require(!(source.flags&1),"Ghost retained active shot damage");
  require(p.player->shot_context.shoot_frame==-1,"Ghost retained its shoot frame");
  for(auto power:p.player->shot_context.laser_power)require(!power,"Ghost retained a laser reservation");
  for(unsigned n=0;n<120;++n){p.input.held=1|focus;require(p.tick(),b.error);require(w.animations.update(false),w.animations.error);}
  for(u32 handle:handles)require(!w.animations.registry.find(handle),"Ghost shot survived animation cleanup");
  require(p.revive(true),b.error);p.player->frame.ready_age.set(30);p.input.held=1|focus;
  for(unsigned n=0;n<45;++n)require(p.tick(),b.error);
  unsigned active=0;for(const auto& shot:p.player->shots.shots)active+=shot.state!=0;require(active>0,"Rescued player could not fire again");
  require(p.game_over(),b.error);require(p.reset_stage(),b.error);require(w.animations.update(false),w.animations.error);
  for(u32 handle:handles)require(!w.animations.registry.find(handle),"Old shots reappeared after stage reset");
 }
 std::puts("PASS: held-fire final death, all characters/focus modes/seats in 2P and 3P, shot animation/damage cleanup, ghost ticks, rescue firing and stage reset");
}
