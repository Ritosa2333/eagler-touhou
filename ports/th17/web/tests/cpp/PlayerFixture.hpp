#pragma once
#include "../../cpp/game/Player.hpp"
struct PlayerFixture final:PlayerHost {
    AnmManager& animations;Rng& game_random;Rng& visual_random;EffectManager effects;PlayerLifeSession session;PlayerSpellStatus spell;Player player;
    float rate=1;bool can_bomb=false;std::vector<u32> events,sounds;std::vector<i32> life_events;std::array<i32,12> audio_ids{},audio_values{};
    PlayerFixture(ShtResource& s,AnmManager& a,Rng& game,Rng& visual,i32 character):animations(a),game_random(game),visual_random(visual),effects(a,8),player(s,a,effects,game,visual,session,spell,*this,character,9+character,8){audio_ids.fill(-1);audio_values.fill(-1);}
    void event(std::initializer_list<u32> v){events.insert(events.end(),v);}
    bool allow_bomb()override{event({1});return can_bomb;}bool begin_bomb()override{event({2});return true;}bool end_roaring(bool death)override{event({12,u32(death)});return true;}
    bool cancel_lasers_near(const Vec3& p,float r,i32 kind,bool flag)override{event({4,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(r),u32(kind),u32(flag)});return true;}
    bool cancel_bullets_near(const Vec3& p,float r,i32 kind,bool all)override{event({5,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(r),u32(kind),u32(all)});return true;}
    bool cancel_lasers(i32 kind,bool flag)override{event({6,u32(kind),u32(flag)});return true;}
    bool item(i32 kind,const Vec3& p,float angle,float speed)override{event({7,u32(kind),float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(angle),float_to_bits(speed)});return true;}
    bool options_changed()override{event({8});return true;}bool game_over()override{event({9});return true;}
    bool respawn_damage(const Vec3& p,float radius,float growth,i32 frames,i32 damage)override{event({10,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(radius),float_to_bits(growth),u32(frames),u32(damage)});return true;}
    bool hit_sound(i32 id)override{life_events.insert(life_events.end(),{1,id,0});return true;}
    bool life_hud(i32 a,i32 b)override{life_events.insert(life_events.end(),{2,a,b});return true;}
    bool bomb_hud(i32 a,i32 b)override{life_events.insert(life_events.end(),{3,a,b});return true;}
    bool shot_audio(i32 id,ShotSoundAction action,float x)override{return sound(id,x,PlayerShots::SoundAction(action));}
    bool sound(i32 id,float x,PlayerShots::SoundAction action)override{if(action==PlayerShots::SoundAction::stop){for(u32 i=0;i<12;i++)if(audio_ids[i]<0||audio_ids[i]==id){audio_ids[i]=id;audio_values[i]=-1;break;}}else sounds.insert(sounds.end(),{u32(id),u32(action),id==20&&action==PlayerShots::SoundAction::pan?u32(truncate_int(float(float(x*1000.f)/192.f))):float_to_bits(x)});return true;}
    bool refresh(PlayerFrameContext&)override{return true;}
};
API(player_create) PlayerFixture* player_create(ShtResource* s,AnmManagerFixture* a,i32 character){return new PlayerFixture(*s,a->manager,a->environment.game_rng,a->random,character);}
API(player_delete) void player_delete(PlayerFixture* p){delete p;}
API(player_initialize) i32 player_initialize(PlayerFixture* p){return p->player.initialize();}
API(player_reset_for_stage) i32 player_reset_for_stage(PlayerFixture* p){return p->player.reset_for_stage();}
API(player_finish_stage_options) i32 player_finish_stage_options(PlayerFixture* p){return p->player.finish_stage_options();}
API(player_stage_field) void* player_stage_field(PlayerFixture* p,u32 kind){return kind?static_cast<void*>(p->player.shots.reward_auxiliary.data()):static_cast<void*>(&p->player.frame.bounds.damage_multiplier);}
API(player_error) const char* player_error(PlayerFixture* p){return p->player.error.c_str();}
API(player_update) i32 player_update(PlayerFixture* p,u32 held,u32 pressed,u32 flags,u32 game_flags,float rate,float dx,float dy,float dz,i32 roaring_kind){p->rate=rate;PlayerFrameContext c;c.held=held;c.pressed=pressed;c.focus_allowed=flags&1;c.enemy_present=flags&2;c.hud_available=flags&4;c.hud_bomb_active=flags&8;c.game_flags=game_flags;c.background_delta={dx,dy,dz};c.roaring_kind=roaring_kind;return p->player.update(c,p->rate);}
API(player_options_update) i32 player_options_update(PlayerFixture* p){return p->player.configure_options();}
API(player_hit) void player_hit(PlayerFixture* p){p->player.hit();}
API(player_root) AnmVm* player_root(PlayerFixture* p){return &p->player.visuals.root;}
API(player_field) void* player_field(PlayerFixture* p,i32 f){auto& v=p->player;switch(f){case 0:return &p->session;case 1:return &v.life.state;case 2:return &v.life.age;case 3:return &v.life.invulnerability;case 4:return &v.frame.input_age;case 5:return &v.frame.ready_age;case 6:return &v.frame.shooting.shot;case 7:return &v.frame.shooting.continuous;case 8:return &v.motion.behavior_flags;case 9:return &v.motion.position;case 10:return &v.motion.position_fixed;case 11:return &v.motion.last_step;case 12:return &v.motion.step;case 13:return &v.motion.velocity;case 14:return &v.motion.last_direction;case 15:return &v.motion.normal_speed;case 16:return &v.resource.header;case 17:return &v.motion.focus;case 18:return v.damage.sources.data();case 19:return v.shots.shots.data();case 20:return &v.damage.cursor;case 21:return &v.motion.option_count;case 22:return v.shot_context.laser_power.data();case 23:return &p->game_random;case 24:return &p->visual_random;case 25:return &p->rate;case 26:return &v.life.last_death_position;case 27:return &v.motion.direction;case 28:return &v.motion.option_follow_percentage;case 29:return &v.motion.collapse_frame;case 30:return &v.frame.bounds.hit;case 31:return &v.frame.bounds.item_near;case 32:return &v.frame.bounds.graze;case 33:return &v.frame.bounds.item_full;case 34:return &v.frame.bounds.enlargement;case 35:return &v.motion.enlargement;case 36:return &v.motion.movement_scale;case 37:return &v.motion.external_velocity;case 38:return &v.motion.barrier_timer;case 39:return v.visuals.option_handles.data();case 40:return &v.visuals.focus_handle;case 41:return &v.visuals.barrier_handle;case 42:return &v.frame.bounds.hit_half_size;case 43:return &v.frame.bounds.item_half_size;case 44:return &v.frame.bounds.graze_half_size;case 45:return &v.options.power_level;case 46:return &v.frame.shooting.charge;case 47:return &v.shots.reward_counter;case 48:return p->effects.handles.data();case 49:return &p->effects.cursor;case 50:return &v.frame.shooting.charge_origin;case 51:return &v.frame.shooting.charge_handle;case 52:return &v.frame.shooting.auxiliary;case 53:return &v.frame.shooting.auxiliary_active;case 54:return v.motion.history.data();case 55:return v.shot_context.option_angles.data();case 56:return &p->spell;default:return nullptr;}}
API(player_option_field) void* player_option_field(PlayerFixture* p,i32 i,i32 f){auto& o=p->player.motion.options[u32(i)];switch(f){case 0:return &o.active;case 1:return &o.target;case 2:return &o.position;case 3:return &o.normal_offset;case 4:return &o.focus_offset;case 5:return &o.snap;case 6:return &o.index;case 7:return &o.previous_focus;default:return nullptr;}}
API(player_service_count) u32 player_service_count(PlayerFixture* p){return p->events.size();}
API(player_services) u32* player_services(PlayerFixture* p){return p->events.data();}
API(player_sound_count) u32 player_sound_count(PlayerFixture* p){return p->sounds.size();}
API(player_sounds) u32* player_sounds(PlayerFixture* p){return p->sounds.data();}
API(player_life_event_count) u32 player_life_event_count(PlayerFixture* p){return p->life_events.size();}
API(player_life_events_data) i32* player_life_events_data(PlayerFixture* p){return p->life_events.data();}
API(player_audio_ids) i32* player_audio_ids(PlayerFixture* p){return p->audio_ids.data();}
API(player_audio_values) i32* player_audio_values(PlayerFixture* p){return p->audio_values.data();}
API(player_draw) i32 player_draw(PlayerFixture* p,AnmRendererFixture* r){return p->player.draw(r->renderer);}
