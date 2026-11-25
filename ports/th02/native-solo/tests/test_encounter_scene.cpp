#include "encounter_scene.hpp"
#include "shot_input.hpp"
#include <iostream>
#include <source_location>
unsigned checks=0;void check(bool b,std::source_location at=std::source_location::current()){++checks;if(!b)throw std::runtime_error("Visible encounter contract failed: check " + std::to_string(checks)+" at line "+std::to_string(at.line()));}
int wmain(int argc,wchar_t**argv){try{
 if(argc!=2)throw std::runtime_error("Usage: encounter-tests <assets>");std::vector<std::uint32_t> pixels(640*400);
 {
  soew::StageAmbient ambient(2);
  unsigned previous=100;
  for(unsigned age=1;age<=200;++age){
   ambient.tick(false,false);
   const auto tone=ambient.opening_tone(false);
   check(ambient.opening_tone(true)==100&&tone>=100&&tone<=126);
   if(age<=168||age>=180)check(tone==100);
   if(age==171)check(tone==126);
   check(age<=171?tone>=previous:tone<=previous);
   previous=tone;
  }
 }
 for(auto mode:{soew::ShotInputMode::Autofire12Hz,soew::ShotInputMode::Fast}){
  auto manual=soew::EncounterScene::load(argv[1],0,0,1,2,3);
  manual.combat_enabled=true;
  auto autofire=manual;
  autofire.sound.shot_cadence.set_autofire(true);
  std::vector<unsigned> manual_cues,autofire_cues,expected_manual,expected_autofire;
  unsigned frame=0,volleys=0;
  manual.sound_output=[&](unsigned id){if(id==1)manual_cues.push_back(frame);};
  autofire.sound_output=[&](unsigned id){if(id==1)autofire_cues.push_back(frame);};
  soew::ShotInput input;input.event(true,mode);
  for(;frame<50;++frame){
   const auto fired=manual.shots.fired,requests=manual.shots.sound_requests;
   const auto keys=input.tick(mode)?soew::Shot:0;
   manual.tick(keys);autofire.tick(keys);
   check(autofire.shots.fired==manual.shots.fired&&autofire.shots.sound_requests==manual.shots.sound_requests);
   if(manual.shots.sound_requests!=requests)expected_manual.push_back(frame+1);
   if(manual.shots.fired!=fired&&volleys++%3==0)expected_autofire.push_back(frame+1);
  }
  manual.tick(0);autofire.tick(0); // Dispatch the last tick's queued sound.
  check(volleys>=7&&autofire_cues==expected_autofire&&manual_cues==expected_manual&&autofire_cues.size()<manual_cues.size());
 }
 for(bool reduced:{false,true}){
  auto s=soew::EncounterScene::load(argv[1],2,0,1,2,3);
  s.reduced_effects=reduced;
  std::vector<unsigned> sounds;s.sound_output=[&](unsigned id){sounds.push_back(id);};
  for(unsigned i=0;i<167;++i)s.tick(0);
  check(s.presentation_slowdown()==1&&std::ranges::count(sounds,14)==0);
  s.tick(0);check(s.presentation_slowdown()==1);
  s.tick(0);check(s.presentation_slowdown()==3&&std::ranges::count(sounds,14)==1);
  s.render(pixels);
  if(!reduced){
   auto sharp=pixels; s.reduced_effects=true;s.render(sharp);s.reduced_effects=false;
   check(pixels[16*640+32]!=sharp[16*640+32]);
  }
  while(s.ticks<200)s.tick(0);
  check(s.presentation_slowdown()==1&&std::ranges::count(sounds,14)==1);
 }
 for(int stage:{0,1}){
  auto s=soew::EncounterScene::load(argv[1],stage,0,1,2,3);
  if(stage==0){s.bind_rika_battle({0,0,0,0,0,0});s.rika.dying=true;}
  else{s.bind_meira_battle();s.meira.dying=true;}
  std::vector<std::pair<std::uint64_t,unsigned>> cues;
  s.sound_output=[&](unsigned id){if(id==18||id==14)cues.emplace_back(s.ticks,id);};
  for(unsigned i=0;i<70;++i)s.tick(0);
  using Cues=decltype(cues);
  check(cues==(stage==0?Cues{{1,18},{32,14}}:Cues{{2,18},{26,18},{33,14},{57,14}}));
 }
 for(int stage=0;stage<6;++stage){auto scene=soew::EncounterScene::load(argv[1],stage,0,1,2,3);auto repeat=scene;check(scene.bank.next>=128);scene.render(pixels);for(unsigned i=0;i<10000&&!scene.runtime.blocked&&!scene.runtime.scroll.done;++i){scene.tick(0);repeat.tick(0);check(scene.runtime.random.cursor==repeat.runtime.random.cursor&&scene.runtime.spawns==repeat.runtime.spawns&&scene.ticks==repeat.ticks);if(i%16==0){scene.render(pixels);check(pixels[0]==0&&pixels[639]==0);}}
  scene.render(pixels);check(scene.runtime.spawns>0);if(stage==3){
      check(scene.runtime.blocked&&scene.runtime.blocked->result.reason=="declared formation row unavailable");
      check(scene.runtime.dispatch.cursor==266&&scene.runtime.scroll.progress==2175);
      check(!scene.runtime.scroll.done&&!scene.preboss_started);
      check(scene.midboss4.entries==2&&!scene.midboss4.active);
      using Art=soew::StageSprites::Stage4Art;
      const auto mima=scene.bank.stage4_art(argv[1],Art::MimaIntro);
      const auto marisa=scene.bank.stage4_art(argv[1],Art::Marisa);
      check(scene.bank.next==152&&scene.bank.sprites[150].has_value());
      check(mima.next==192&&mima.sprites[191]->height==32);
      check(marisa.next==142&&marisa.sprites[128]->width==48&&marisa.sprites[129]->height==96);
      check(marisa.sprites[138]->width==32&&!marisa.sprites[150]);
      check(marisa.sprites[10]->pixels==scene.bank.sprites[10]->pixels);
      check(scene.runtime.dispatch.suppressed>0);
  }if(stage==5)check(scene.runtime.blocked&&scene.runtime.blocked->result.reason=="effect consumer not connected");scene.paused=true;auto old=scene.runtime.ticks;scene.tick(soew::Left);check(old==scene.runtime.ticks);
  std::cout<<"encounter_stage="<<stage<<" sprites="<<scene.bank.next<<" ticks="<<scene.ticks<<" enemies_generated="<<scene.runtime.spawns<<" bullets_generated="<<scene.bullet_core.spawned<<" blocked="<<bool(scene.runtime.blocked)<<" reason="<<(scene.runtime.blocked?scene.runtime.blocked->result.reason:"none")<<'\n';
 }
 for(int stage=0;stage<6;++stage){auto scene=soew::EncounterScene::load(argv[1],stage,0,1,2,3);scene.combat_enabled=true;for(unsigned i=0;i<4000&&!scene.runtime.blocked&&!scene.runtime.scroll.done;++i){scene.tick(soew::Shot);if(i%16==0)scene.render(pixels);}scene.render(pixels);check(scene.shots.fired>0);if(stage==0)check(scene.damage.kills>0);std::cout<<"combat_stage="<<stage<<" ticks="<<scene.ticks<<" shots="<<scene.shots.fired<<" hit_damage="<<scene.shots.damage_events<<" kills_recorded="<<scene.damage.kills<<" blocked="<<bool(scene.runtime.blocked)<<'\n';}
 for(int stage=0;stage<6;++stage){auto scene=soew::EncounterScene::load(argv[1],stage,0,1,2,3);scene.combat_enabled=true;scene.survival_enabled=true;for(unsigned i=0;i<6000&&!scene.life.game_over&&!scene.runtime.blocked&&!scene.runtime.scroll.done;++i){scene.tick(soew::Shot);if(i%16==0)scene.render(pixels);}scene.render(pixels);check(scene.life.lives<=2);if(scene.life.game_over){const auto before=scene.ticks;scene.tick(soew::Right|soew::Shot);check(scene.ticks==before);}
  std::cout<<"survival_stage="<<stage<<" ticks="<<scene.ticks<<" lives="<<unsigned(scene.life.lives)<<" misses="<<scene.life.stage_misses<<" gameover="<<scene.life.game_over<<" blocked="<<bool(scene.runtime.blocked)<<'\n';
 }
 // Compare full frames at the same scroll positions, changing only the overlay gate.
 {auto s=soew::EncounterScene::load(argv[1],3,0,1,2,3);
  std::vector<std::uint32_t> normal(pixels.size()),off(pixels.size());
  // A controlled image distinguishes source bits from terrain bits even if
  // the original palette maps some intensity pairs to the same RGB.
  for(unsigned i=0;i<16;++i)s.background.palette[i]={std::uint8_t(i),0,0};
  for(auto &tile:s.background.tiles)tile.fill(0);
  s.background.tiles.at(0)[0]=8;
  check(s.background.stage4_overlay_color(5,0,0)==13);
  check(s.background.stage4_overlay_color(5,1,0)==5);
  for(unsigned scroll:{2u,144u,412u}){
   s.scroll.display_pixels=scroll;
   s.rendered_page=0;s.reduced_effects=true;s.render(off);
   s.reduced_effects=false;s.render(normal);
   unsigned changes=0;
   for(unsigned y=0;y<400;++y)for(unsigned x=0;x<640;++x)if(normal[y*640+x]!=off[y*640+x]){
    if(x<176||x>=272||y<154||y>=250)throw std::runtime_error("Stage 4 overlay outside its original rectangle");
    ++changes;
   }
   if(!changes)throw std::runtime_error("No Stage 4 overlay pixels at scroll " + std::to_string(scroll));
   // The visible interval uses the unmodified palette color on both pages.
   check(normal[154*640+176]==0x080000);
   s.rendered_page=1;s.render(off);check(normal==off);
   s.rendered_page=0;s.reduced_effects=true;s.render(off);
   s.reduced_effects=false;s.runtime.scroll.done=true;s.render(normal);check(normal==off);s.runtime.scroll.done=false;
  }
  check(!soew::EncounterScene::stage4_overlay_enabled(2,false,false));
  check(soew::StageBackground::stage4_overlay_index(95,95)==85);
 }
 // Isolate the two midboss4 reward paths from ordinary enemy scoring.
 {soew::Midboss4Core boss;soew::Midboss4Core::Hooks hooks;
  unsigned damage=400,score=0;std::vector<unsigned> drops;
  hooks.random8=[](){return 0u;};
  hooks.damage=[&](int x,int y,int w,int h){check(x==boss.x+8&&y==boss.y&&w==48&&h==48);return damage;};
  hooks.score=[&](unsigned value){score+=value;};
  hooks.item=[&](int x,int y,unsigned type){check(x==boss.x+24&&y==boss.y);drops.push_back(type);};
  for(unsigned entry=0;entry<2;++entry){
   const auto trigger=soew::Midboss4Core::triggers[entry];
   boss.trigger(trigger-1);check(!boss.active);
   boss.trigger(trigger);boss.trigger(trigger);check(boss.active&&boss.entries==entry+1);
   boss.begin_frame(0);boss.update(0,110,1,hooks);
   check(!boss.dying&&boss.damage_taken==400&&boss.y==-14&&boss.x==(entry==0?72:312)); // VRAM Y=96 is excluded.
   damage=1;boss.begin_frame(1);boss.update(1,110,1,hooks);
   check(boss.dying&&boss.damage_taken==401&&boss.y==-12&&boss.x==(entry==0?80:304));
   for(unsigned i=0;i<64;++i){const unsigned page=i&1;boss.begin_frame(page);boss.update(page,110,1,hooks);check(boss.active==(i<63));}
   boss.trigger(trigger);check(!boss.active&&boss.target_x==-1&&boss.target_y==-1);
   damage=400;
  }
  check(score==100000&&drops==std::vector<unsigned>{2,4});
 }
 // Bullet collision occurs after player phase: it is pending until NEXT frame.
 {auto scene=soew::EncounterScene::load(argv[1],0,0,1,1,3);scene.combat_enabled=true;scene.survival_enabled=true;auto &b=scene.bullet_core.bullets[0];b.flag=1;b.position[1]={static_cast<std::uint16_t>((208+10)*16),static_cast<std::uint16_t>((336+15)*16)};scene.tick(0);check(scene.life.hit&&!scene.life.miss_active&&scene.life.miss_frame==0);scene.tick(soew::Right|soew::Shot);check(scene.life.miss_active&&scene.life.miss_frame==1&&scene.player.x==208);scene.paused=true;auto frozen=scene.ticks;scene.tick(soew::Shot);check(scene.ticks==frozen&&scene.life.miss_frame==1);scene.paused=false;for(unsigned i=1;i<68;++i)scene.tick(soew::Shot);check(!scene.life.miss_active&&scene.life.lives==0&&!scene.life.game_over);scene.life.invincibility=0;scene.life.register_hit();for(unsigned i=0;i<24;++i)scene.tick(0);check(scene.life.game_over);}
 // Enemy contact uses FRONT-page X but cached current Y, and happens before
 // player phase. Invincibility swallows player hit, not enemy's contact death.
 for(bool invincible:{false,true}){auto scene=soew::EncounterScene::load(argv[1],0,0,1,2,3);scene.combat_enabled=scene.survival_enabled=true;scene.life.invincibility=invincible?10:0;scene.script.enemies[0].raw[22]=100;scene.script.enemies[0].raw[23]=0;scene.script.enemies[0].raw[24]=0;auto path_id=scene.script.enemies[0].path_slot;scene.script.paths[path_id].bytes={0,255};auto &enemy=scene.runtime.enemies.slots[0];enemy.raw[0xe]=1;enemy.word(0xc,0);enemy.word(4,208);enemy.word(6,336);scene.tick(0);check(enemy.raw[0x10]==1);check(scene.life.stage_misses==(invincible?0u:1u));if(!invincible)check(scene.life.miss_frame==1);}
 for(int stage=0;stage<6;++stage){auto scene=soew::EncounterScene::load(argv[1],stage,0,1,2,3);scene.combat_enabled=scene.survival_enabled=true;scene.enable_effects();auto repeat=scene;for(unsigned f=0;f<6000&&!scene.life.game_over&&!scene.runtime.blocked&&!scene.runtime.scroll.done;++f){scene.tick(soew::Shot);repeat.tick(soew::Shot);check(scene.runtime.random.cursor==repeat.runtime.random.cursor&&scene.score_core.delta==repeat.score_core.delta&&scene.score_core.score==repeat.score_core.score&&scene.items.spawned==repeat.items.spawned&&scene.sparks.generated==repeat.sparks.generated);if(f%32==0)scene.render(pixels);}scene.render(pixels);if(stage==0)check(scene.sparks.generated>0&&scene.items.spawned>0&&scene.score_core.score>0);std::cout<<"effects_stage="<<stage<<" ticks="<<scene.ticks<<" items="<<scene.items.spawned<<" sparks="<<scene.sparks.generated<<" score="<<scene.score_core.score<<" delta="<<scene.score_core.delta<<" rng="<<unsigned(scene.runtime.random.cursor)<<'\n';}
 for(unsigned type=0;type<3;++type){auto scene=soew::EncounterScene::load(argv[1],0,int(type),1,2,3);scene.combat_enabled=scene.survival_enabled=true;scene.enable_effects();scene.pickup_enabled=true;scene.items.add(210,340,0,2);scene.tick(0);check(scene.pickups.collected==1&&scene.pickups.power==2&&scene.items.slots[0].flag==2&&scene.numbers.generated==1);scene.items.add(210,340,0,2);scene.tick(0);scene.items.add(210,340,0,2);scene.tick(0);check(scene.pickups.power==4&&scene.pickups.level()==1&&!scene.weapon_blocked);scene.tick(soew::Shot);check(scene.shots.level==1&&scene.shots.fired>0);scene.items.add(210,340,3,2);scene.tick(0);check(!scene.weapon_blocked&&scene.pickups.power>=8);const auto old=scene.ticks;scene.tick(soew::Shot);check(scene.ticks==old+1);scene.render(pixels);}
 for(int stage=0;stage<6;++stage){auto scene=soew::EncounterScene::load(argv[1],stage,0,1,2,3);scene.combat_enabled=scene.survival_enabled=true;scene.enable_effects();scene.pickup_enabled=true;for(unsigned i=0;i<6000&&!scene.life.game_over&&!scene.runtime.blocked&&!scene.runtime.scroll.done&&!scene.weapon_blocked;++i){scene.tick(soew::Shot);if(i%32==0)scene.render(pixels);}scene.render(pixels);std::cout<<"pickup_stage="<<stage<<" ticks="<<scene.ticks<<" collected="<<scene.pickups.collected<<" power="<<scene.pickups.power<<" level="<<scene.pickups.level()<<" unsupported_weapon="<<scene.weapon_blocked<<'\n';}
 for(unsigned type=0;type<3;++type)for(unsigned level=0;level<10;++level){auto scene=soew::EncounterScene::load(argv[1],0,int(type),1,2,3);scene.combat_enabled=scene.survival_enabled=true;scene.enable_effects();scene.pickup_enabled=true;constexpr unsigned power[]={1,4,8,16,24,32,40,52,64,80};scene.pickups.power=power[level];for(unsigned f=0;f<180;++f){scene.tick(soew::Shot);if(f%8==0)scene.render(pixels);}check(scene.shots.fired>0&&!scene.weapon_blocked);scene.render(pixels);}
 // Ordinary enemy hit feedback4 and lethal feedback3 are requested after
 // current frame's sound dispatch, then dispatched at next game boundary.
 for(unsigned hp:{1u,100u,65534u}){auto s=soew::EncounterScene::load(argv[1],0,0,1,2,3);s.combat_enabled=true;s.enable_effects();s.script.enemies[0].raw[22]=std::uint8_t(hp);s.script.enemies[0].raw[23]=std::uint8_t(hp>>8);s.script.enemies[0].raw[24]=1;s.script.paths[s.script.enemies[0].path_slot].bytes={0,255};s.runtime.dispatch.cursor=1;auto &e=s.runtime.enemies.slots[0];e.raw[0xe]=1;e.word(0xc,0);e.word(4,208);e.word(6,100);s.runtime.enemies.high_water=1;auto &p=s.shots.slots[0];p.flag=1;p.type=0;p.pattern=52;p.position[1]={std::uint16_t(208*16),std::uint16_t(100*16)};unsigned last=255;s.sound_output=[&](unsigned id){last=id;};s.tick(0);check(s.sound.current==(hp==1?3u:4u));check(last==255);check(s.sparks.slots[0].flag==1&&s.sparks.slots[0].shot_hit);s.tick(0);check(last==(hp==1?3u:4u));}
 std::cout<<"encounter_scene_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
