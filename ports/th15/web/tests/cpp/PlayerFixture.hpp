#pragma once
#include "../../cpp/game/Player.hpp"
struct PlayerFixture:PlayerHost {
 AnmManager& animations;Rng& game_random;Rng& visual_random;EffectManager effects;PlayerLifeSession session;PlayerSpellStatus spell;PlayerLifecycleFixtureHost host;Player player;float rate=1;std::vector<i32> sounds,life_events;std::array<i32,12> audio_ids{},audio_values{};
 PlayerFixture(ShtResource& s,AnmManager& a,Rng& game,Rng& visual,i32 character):animations(a),game_random(game),visual_random(visual),effects(a,8),player(s,a,effects,game,visual,session,spell,*this,character,9+character,8){audio_ids.fill(-1);audio_values.fill(-1);}
 bool allow_bomb()override{return host.allow_bomb();}bool begin_bomb()override{return host.begin_bomb();}
 bool cancel_lasers_near(const Vec3& p,float r,i32 k,bool f)override{return host.cancel_lasers_near(p,r,k,f);}bool cancel_bullets_near(const Vec3& p,float r,i32 k)override{return host.cancel_bullets_near(p,r,k);}bool cancel_lasers(i32 k,bool f)override{return host.cancel_lasers(k,f);}
 bool item(i32 k,const Vec3& p,float a,float s)override{return host.item(k,p,a,s);}bool options_changed()override{return host.options_changed();}bool game_over()override{return host.game_over();}bool respawn_damage(const Vec3& p,float r,float g,i32 f,i32 d)override{return host.respawn_damage(p,r,g,f,d);}bool bomb_hud(i32 v,i32 p)override{return host.bomb_hud(v,p);}
 bool sound(i32 id,float,PlayerShots::SoundAction action)override{sounds.push_back(id);sounds.push_back(i32(action));return true;}bool hit_sound(i32 id)override{life_events.push_back(1);life_events.push_back(id);life_events.push_back(0);return true;}bool life_hud(i32 v,i32 p)override{life_events.push_back(2);life_events.push_back(v);life_events.push_back(p);return true;}
 bool stop_sound(i32 id)override{for(u32 i=0;i<12;i++)if(audio_ids[i]<0||audio_ids[i]==id){audio_ids[i]=id;audio_values[i]=-1;break;}return true;}bool refresh(PlayerFrameContext&)override{return true;}
};
