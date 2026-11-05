#include "GameHud.hpp"
#include <cmath>
namespace th15 {
bool GameHud::cue(u32 handle,i32 label,bool immediate){auto* vm=animations.registry.find(handle);if(!vm)return check(false);if(vm->object_host&&!vm->object_host->interrupt_effect(*vm,label,animations.rate))return check(false);vm->pending_interrupt=label;return !immediate||check(animations.tick_instance(*vm)>=0);}
bool GameHud::retire_boss(u32 slot,bool clear){
 auto& ring=boss_animations[slot];auto& track=boss.health[slot];if(clear){track.segments[0].fraction=0;for(u32 i=2;i<6;i++)track.segments[i].fraction=0;}
 if(ring.created){for(auto& handle:ring.handles)if(!check(animations.retire(handle)))return false;ring.created=0;}
 if(clear&&slot==0&&!check(animations.retire(boss_banner)))return false;return true;
}
bool GameHud::update_before_dialogue(const HudFrameContext& context,HudFrameServices& services){
 animations.rate=progress.rate;const auto sound=[&](i32 id){if(services.hud_sound(id))return true;error="HUD sound service failed";return false;};
 if(flags&0x100)intro_age.tick(&progress.rate);
 if(flags&0x1800){intro_age.tick(&progress.rate);if((flags&0x1800)==0x800&&intro_age.current>89){
  if(result.remaining<=0){if(intro_age.current!=90&&!sound(47))return false;result.remaining=result.initial;result.current=result.total;result.increment=0;flags=(flags&~0x800u)|0x1000;}
  else{if(!(u32(intro_age.current)&3)&&!sound(39))return false;result.current=wrapping_add(result.current,result.increment);result.remaining=float(result.remaining-1.f);}
 }if(intro_age.current>=result.limit){if(!check(animations.interrupt(result_notice,1)))return false;flags=(flags&~0x1000u)|0x800;intro_age.set(0);flags&=~0x1800u;}}
 if(tutorial_state&&!animations.registry.find(background_notice)){background_notice=0;tutorial_state=0;}
 if(context.player_position){const auto& p=*context.player_position;if(!(flags&1)){if(p.y>400&&p.x<-64)flags|=1;}else if(p.y<384||p.x>-64)flags&=~1u;}
 const auto* world=context.enemies;const auto* primary=world?world->boss_at(0):nullptr;
 const bool hide=!world||world->boss_seconds<0||!primary||(world->manager_flags&1)||context.dialogue||(progress.scene_flags&0x10000);
 if(hide){for(auto handle:boss_icons)if(!check(animations.pause(handle,true)))return false;flags=(flags&~0x200u)|0x400;}
 else{
  if(!context.player_position){error="Boss HUD player position unavailable";return false;}for(auto handle:boss_icons)if(!check(animations.pause(handle,false)))return false;
  const bool lower=context.spell_flags&0x100;const float y=context.player_position->y;const u32 mode=(flags>>9)&3;
  if(!mode){if((!lower&&y<128)||(lower&&y>320)){flags=(flags&~0x400u)|0x200;for(auto handle:boss_icons)if(!cue(handle,5))return false;}}
  else if(mode==1){if((!lower&&y<160)||(lower&&y>288)){for(auto handle:boss_icons)if(!cue(handle,4))return false;flags&=~0x600u;}}
  else{for(auto handle:boss_icons)if(!cue(handle,(context.spell_flags&1)?2:3,true)||!cue(handle,4,true))return false;flags&=~0x600u;}
  const i32 seconds=world->boss_seconds;if(seconds<last_countdown){if(seconds<5){for(auto handle:boss_icons)if(!cue(handle,9))return false;if(!sound(12))return false;}else if(seconds<10){for(auto handle:boss_icons)if(!cue(handle,8))return false;if(!sound(11))return false;}}
  else if(seconds>last_countdown){for(auto handle:boss_icons)if(!cue(handle,7))return false;}
  if(seconds!=last_countdown){for(u32 i=0;i<2;i++){auto* vm=animations.registry.find(boss_icons[i]);if(!vm||!vm->select_sprite((i?seconds%10:seconds/10)+239)){error="Boss countdown sprite unavailable";return false;}}}last_countdown=seconds;
 }
 if(world&&!(world->manager_flags&1))for(u32 slot=0;slot<2;slot++){
  const auto* enemy=world->boss_at(i32(slot));if(!enemy){if(!retire_boss(slot,true))return false;continue;}
  if(enemy->life>=100000||(enemy->flags&0x31)||enemy->collision_timer.current>0||context.dialogue){if(!retire_boss(slot,false))return false;continue;}
  auto& track=boss.health[slot];const float target=float(float(enemy->life)/float(enemy->initial_life));auto& displayed=track.segments[0].fraction;track.segments[0].color=float_to_bits(target);track.segments[1].fraction=float_from_bits(u32(enemy->life));if(displayed<target)displayed=float(displayed+.025f);if(target<displayed)displayed=target;
  auto& ring=boss_animations[slot];if(!ring.created){for(u32 i=0;i<ring.handles.size();i++){ring.handles[i]=create(front,i<3?i32(i)+237:240);if(!ring.handles[i])return false;}ring.created=1;}
  if(!name_banner(context.boss_banners))return false;
  const Vec3 center{float(enemy->motion.position.x*2.f),float(enemy->motion.position.y*2.f),enemy->motion.position.z};
  for(u32 i=0;i<3;i++){auto* vm=animations.registry.find(ring.handles[i]);if(!vm){error="Boss ring animation unavailable";return false;}vm->visual.translation=center;if(!i){vm->visual.flags|=4;vm->variables.rotation.x=float(displayed*-6.2831854820251465f);}}
  float z=0;for(u32 i=0;i<4;i++){const float fraction=track.segments[i+2].fraction;auto* vm=animations.registry.find(ring.handles[i+3]);if(!vm){error="Boss health marker unavailable";return false;}
   if(fraction==0||displayed<=fraction){if(!check(animations.pause(ring.handles[i+3],true)))return false;}
   else{if(!check(animations.pause(ring.handles[i+3],false)))return false;const float angle=normalize_angle(float(-3.1415927410125732f-float(fraction*6.2831854820251465f)));vm->visual.flags|=4;vm->variables.rotation.z=angle;const float sine=float(std::sin(double(angle))),cosine=float(std::cos(double(angle)));z=float(z+enemy->motion.position.z);vm->visual.translation={float(float(float(cosine*0.f)-float(sine*112.f))+center.x),float(float(float(sine*0.f)+float(cosine*112.f))+center.y),z};}
  }
  if(!context.player_position){error="Boss health proximity player unavailable";return false;}
  const float dx=float(enemy->motion.position.x-context.player_position->x),dy=float(enemy->motion.position.y-context.player_position->y),distance=float(float(dx*dx)+float(dy*dy));
  if(!ring.dimmed&&distance<6400){for(auto handle:ring.handles)if(!check(animations.interrupt(handle,3)))return false;ring.dimmed=1;}
  else if(ring.dimmed&&distance>=9216){for(auto handle:ring.handles)if(!check(animations.interrupt(handle,2)))return false;ring.dimmed=0;}
 }
 for(u32 i=0;i<boss_stars.size();i++)if(i32(i)<boss.displayed_segments){if(!boss_stars[i]&&(boss_stars[i]=create(front,i32(i)+69))==0)return false;}else if(boss_stars[i]){if(!check(animations.interrupt(boss_stars[i],1)))return false;boss_stars[i]=0;}
 return true;
}
bool GameHud::update_after_dialogue(const HudFrameContext& context){
 const auto* world=context.enemies;if(world){const auto* enemy=world->boss_at(0);if(enemy&&!(enemy->flags&0x21)){
  auto* vm=animations.registry.find(pointdevice);if(!vm){error="Boss position indicator unavailable";return false;}if(!check(animations.pause(pointdevice,false)))return false;const bool spell=context.spell_flags&1;const i32 life=enemy->phase_life;const u32 state=(flags>>1)&3;
  if(state==0&&life<(spell?2000:700)){if(!cue(pointdevice,7))return false;flags=(flags&~4u)|2;}
  else if(state==1&&life<(spell?1000:400)){if(!cue(pointdevice,8))return false;flags=(flags&~2u)|4;}
  else if(state==2&&life<(spell?400:200)){if(!cue(pointdevice,9))return false;flags|=6;}
  else if(state==3&&life>(spell?400:200)){if(!cue(pointdevice,10))return false;flags&=~6u;}
  vm->visual.translation.x=float(float(float(enemy->motion.position.x+32.f)+192.f)*2.f);vm->visual.translation.y=960;
  if(!context.player_position){error="Boss indicator player unavailable";return false;}const float distance=std::fabs(float(enemy->motion.position.x-context.player_position->x));const u32 alpha=distance<64?u8(wrapping_add(truncate_int(float(float(distance*191.f)*.015625f)),64)):255;vm->visual.color=(vm->visual.color&0xffffffu)|(alpha<<24);if(enemy->motion.position.x<-192||enemy->motion.position.x>192)vm->visual.color&=0xffffffu;
 }else if(!check(animations.pause(pointdevice,true)))return false;}
 frame_age.tick(&progress.rate);return true;
}
}
