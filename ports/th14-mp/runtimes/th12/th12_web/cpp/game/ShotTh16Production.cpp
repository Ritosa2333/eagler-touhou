#include "ShotManager.hpp"
#include "AnmGeometry.hpp"
#include "EnemyTh16Targets.hpp"
#include <cmath>
#if PORT_TITLE == 16
namespace th11 {
i32 th16_collect_failure=0;u32 th16_collect_source=0;
namespace {
void retire(AnmManager& manager,u32 id){auto* root=manager.find(id);if(!root)return;const auto mark=[](AnmVm& vm){if(!(vm.flags&0x4000000))vm.flags2=(vm.flags2&~0x40u)|0x20u;};mark(*root);if(!root->child.previous)for(auto* node=root->child.next;node;node=node->next)mark(*node->value);}
void mirror(ShotState& state){const auto& own=state.th16;state.state=own.state;state.animation=own.animation;state.timer=own.timer;state.movement=own.movement;state.damage=own.damage;}
struct Factory final:Th16ShotDelivery {
 AnmManager& animations;AnmResource& main;AnmResource& season;ShotPlayer& player;ShotWorld& world;Th16DamageSources& sources;u16 file;
 Factory(AnmManager& a,AnmResource& m,AnmResource& s,ShotPlayer& p,ShotWorld& w,Th16DamageSources& d,u16 f):animations(a),main(m),season(s),player(p),world(w),sources(d),file(f){}
 u32 create(bool sub,i32 script)override{const Vec3 zero{};const float rotation=0;auto* vm=animations.create(sub?season:main,script,sub?30:file,0xffffffff,false,false,&zero,nullptr,nullptr,false,&rotation,0x2400);return vm?vm->id:0;}
 bool orient(u32 id,float angle)override{auto* vm=animations.find(id);if(!vm)return false;if(vm->flags2&0x80){vm->rotation.z=angle;vm->flags|=4;}return true;}
 bool position(u32 id,Vec3 p)override{auto* vm=animations.find(id);if(!vm)return false;vm->position=p;return true;}
 void erase(u32 id)override{retire(animations,id);}
 i32 callback(Th16ShotSpawn& shot,const ShotSpec& spec,i32)override{
  if(u32(spec.spawn)==2&&!world.sound(20,player.position.x))return -1;
  return shot_th16_spawn_callback(shot,spec,sources)?0:-1;
 }
 bool sound(i32 sound,float x)override{return world.sound(sound,x);}
};
struct Targets final:Th16ShotTargets {
 ShotWorld& world;bool manager;
 Targets(ShotWorld& w,bool m):world(w),manager(m){}
 bool present()const override{return manager;}
 Enemy* lookup(u32 id){return enemy_th16_lookup(world.enemies,id);}
 u32 nearest(Vec3 p,float radius)override{return enemy_th16_nearest(world.enemies,p,radius);}
 bool valid(u32 id)override{return enemy_th16_valid(world.enemies,id);}
 Th16ShotTarget target(u32 id)override{auto* enemy=lookup(id);return enemy?Th16ShotTarget{enemy->state.current.position,enemy->state.flags}:Th16ShotTarget{{},0xc000021};}
};
struct Update final:Th16ShotUpdateDelivery {
 AnmManager& animations;Targets& targets;ShotManager& manager;ShotWorld& world;
 Update(AnmManager& a,Targets& t,ShotManager& m,ShotWorld& w):animations(a),targets(t),manager(m),world(w){play_origin={a.render_space.play_origin.x-192.f,a.render_space.play_origin.y};}
 i32 update_callback(Th16ShotSpawn& shot,const ShotSpec& spec)override{switch(u32(spec.update)){case 0:return 0;case 1:shot_th16_homing(shot,targets);return 0;case 2:{
  if(!shot.damage_source)return -1;const i32 raw=i32(spec.option)-1,index=raw>=100?raw-100:raw;if(index<0||index>=8)return -1;Th16BeamEffects effects;
  if(!shot_th16_beam_update(shot,spec,manager.player,manager.th16_sources.records[shot.damage_source],animations.find(shot.animation),manager.th16_schedule.regular.current>=0,manager.th16_lasers[index+((shot.packed&0xf0000)?8:0)],effects))return -1;
  if(effects.pan_active&&!world.th16_beam_pan(effects.pan))return -1;for(u32 i=0;i<effects.count;++i){auto* root=animations.find(shot.animation);if(root){root->interrupt(i16(effects.interrupts[i]));if(!root->child.previous)for(auto* node=root->child.next;node;node=node->next)node->value->interrupt(i16(effects.interrupts[i]));}}
  return effects.stop&&!world.th16_beam_stop()?-1:0;
 }case 5:if(shot.state==1)shot.movement.speed+=.5f;return 0;default:return -1;}}
 bool find(u32 id,u32& flags)override{auto* vm=animations.find(id);if(!vm)return false;flags=vm->flags2;return true;}
 bool quad(u32 id,std::array<Vec3,4>& out)override{auto* vm=animations.find(id);Vec3 corners[4];if(!vm||!anm_quad_positions(*vm,corners,animations.render_space))return false;for(u32 i=0;i<4;++i)out[i]=corners[i];return true;}
 void erase(u32 id)override{retire(animations,id);}
 void position(u32 id,Vec3 p)override{if(auto* vm=animations.find(id))vm->position=p;}
 void orient(u32 id,float angle)override{if(auto* vm=animations.find(id)){vm->rotation.z=angle;vm->flags|=4;}}
};
struct Collect final:Th16DamageDelivery {
 ShotManager& shots;ShotWorld& world;AnmManager& animations;AnmResource& main;u16 file;
 Collect(ShotManager& s,ShotWorld& w,AnmManager& a,AnmResource& m,u16 f):shots(s),world(w),animations(a),main(m),file(f){}
 bool bomb(const Th16DamageQuery&,i32& out)override{out=0;if(shots.player.special_active){th16_collect_failure=1;return false;}return true;}
 bool overlaps(const Th16DamageSource& source,const Th16DamageQuery& query)override{return shot_th16_overlaps(source,query);}
 bool hit(u32,Th16DamageSource& source,const Th16DamageQuery& query,i32& damage)override{
  th16_collect_source=source.words[34];th16_collect_failure=2;
  const u32 index=source.words[34];if(index>=256)return false;auto& state=shots.at(index);auto& shot=state.th16;if(!state.spec)return false;
  if(source.words[36]==2){damage=shot.damage;shot.damage=1;mirror(state);return true;}
  if(source.words[36]!=1)return false;
  if(state.spec->extra_callback==2){
   Th16BeamHit result;if(!shot_th16_beam_hit(shot,source,query,result))return false;
   if(result.interrupt){auto* root=animations.find(shot.animation);if(root){root->interrupt(2);if(!root->child.previous)for(auto* node=root->child.next;node;node=node->next)node->value->interrupt(2);}}
   if(result.effect){const bool sub=shot.packed&0xf0000;if(sub&&!shots.th16_season_animation)return false;const float rotation=0;
    auto* vm=animations.create(sub?*shots.th16_season_animation:main,sub?2:8,sub?30:file,0xffffffff,false,false,&result.position,nullptr,nullptr,false,&rotation,0x2400);
    if(!vm||!world.th16_register_hit_effect(vm->id))return false;shot_th16_beam_effect_setup(*vm,result,animations.default_tangent,&animations.rate);
   }
   damage=result.damage;mirror(state);return true;
  }
  if(state.spec->extra_callback==5){
   const float angle=shot_th16_freeze_angle(shot,animations.script_rng);auto* vm=world.th16_freeze_effect(shot.movement.position,angle);
   if(!vm)return false;shot_th16_freeze_finish(*vm,animations.script_rng,&animations.rate);
  }else if(state.spec->extra_callback)return false;
  auto* vm=animations.find(shot.animation);if(!vm)return false;
  th16_collect_failure=3;const bool ok=shot_th16_default_hit(shot,shots.th16_sources,*vm,damage);mirror(state);return ok;
 }
 bool score(i32 amount)override{th16_collect_failure=4;return world.th16_shot_score(amount);}
};
}
i32 ShotManager::spawn_th16(const ShotSpec& spec,u32 packed,i32 frame,Vec3 origin){
 if(!th16_season_animation){last_error=1694;return -2;}
 if(spec.type==2){const i32 raw=i32(spec.option)-1,index=raw>=100?raw-100:raw;if(index<0||index>=8){last_error=1695;return -2;}if(th16_lasers[index+((packed&0xf0000)?8:0)])return -1;}
 u32 index=0;while(index<shots.size()&&shots[index].state)++index;if(index==shots.size())return -1;
 auto& state=shots[index];state.spec=&spec;Factory delivery(animations,animation_resource,*th16_season_animation,player,world,th16_sources,file_id);
 const i32 result=shot_th16_spawn(state.th16,spec,packed,index,frame,player,origin,animations.visual_rng,th16_lasers,th16_sources,&animations.rate,delivery);mirror(state);
 if(result==-2){last_error=1696;return -2;}return result==-1?-1:i32(index);
}
bool ShotManager::th16_fire_frames(i32 frame,i32 long_frame){
 if(!th16_season_resource||!th16_season){last_error=1694;return false;}
 struct Requests final:Th16ShotRequests {
  ShotManager& manager;const ShtResource& main;const ShtResource& season;
  Requests(ShotManager& m,const ShtResource& a,const ShtResource& b):manager(m),main(a),season(b){}
  bool spawn(u32 packed,i32 frame)override{const u32 group=(packed>>8)&255,index=packed&255;const auto& resource=packed&0xf0000?season:main;if(group>=resource.groups.size()||index>=resource.groups[group].shots.size())return false;return manager.spawn_th16(resource.groups[group].shots[index],packed,frame,manager.player.position)!=-2;}
 } requests(*this,resource,*th16_season_resource);
 return shot_th16_select(resource,*th16_season_resource,player.power,player.power_step,player.focused,*th16_season,frame,long_frame,requests);
}
bool ShotManager::update_th16(){
 Targets targets(world,player.th16_enemy_manager);Update delivery(animations,targets,*this,world);
 for(auto& state:shots){if(!state.state)continue;if(!state.spec){last_error=6;return false;}if(!shot_th16_update(state.th16,*state.spec,th16_sources,&animations.rate,delivery)){last_error=1697;return false;}mirror(state);}return true;
}
bool ShotManager::damage_th16(const Th16DamageQuery& input,i32& out){
 th16_collect_failure=0;th16_collect_source=0;
 auto query=input;query.cap=signed_bits(resource.header.native_extra[0]);Collect delivery(*this,world,animations,animation_resource,file_id);bool bomb=false;
 if(!shot_th16_collect(th16_sources,query,delivery,out,bomb,nullptr)){last_error=1699;return false;}return true;
}
}
#endif
