#pragma once
#include "../../cpp/game/RoaringSystem.hpp"
struct RoaringFixture:RoaringHost {
    AnmManagerFixture& animations;PlayerMotion player;PlayerLifeSession session;Timer invulnerability{0,0,0,0,0};PlayerSpellStatus spell;DamageSources damage;
    RoaringSystem system;std::vector<u32> events;bool active_shot=false;
    RoaringFixture(AnmManagerFixture& a,i32 id):animations(a),system(a.manager,a.environment.game_rng,damage,player,session,invulnerability,spell,*this,id){}
    bool sound(i32 id,bool queued)override{events.insert(events.end(),{queued?2u:1u,u32(id)});return true;}
    bool item(i32 kind,const Vec3& p,float angle,float speed,i32 delay,i32 sound)override{events.insert(events.end(),{3,u32(kind),float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(angle),float_to_bits(speed),u32(delay),u32(sound)});return true;}
    bool spirit(i32 kind,const Vec3& p,float angle)override{events.insert(events.end(),{4,u32(kind),float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(angle)});return true;}
    bool cancel_bullets(const Vec3& p,float radius,i32 reward)override{events.insert(events.end(),{5,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(radius),u32(reward)});return true;}
    bool cancel_lasers(const Vec3& p,float radius,i32 reward,bool honor)override{events.insert(events.end(),{6,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(radius),u32(reward),u32(honor)});return true;}
    bool active_player_shot()const override{return active_shot;}
};
API(roaring_create) RoaringFixture* roaring_create(AnmManagerFixture* a,i32 id){return new RoaringFixture(*a,id);}
API(roaring_delete) void roaring_delete(RoaringFixture* p){delete p;}
API(roaring_hud) u32 roaring_hud(RoaringFixture* p){return p->system.initialize_hud();}
API(roaring_enter_stage) u32 roaring_enter_stage(RoaringFixture* p){return p->system.enter_stage();}
API(roaring_field) void* roaring_field(RoaringFixture* p,u32 field){switch(field){case 0:return &p->system.state;case 1:return &p->system.visuals;case 2:return &p->player.position;case 3:return &p->session.animal;case 4:return &p->player.behavior_flags;case 5:return &p->invulnerability;case 6:return &p->spell;case 7:return &p->session.bomb_state;case 8:return p->damage.sources.data();case 9:return &p->damage.cursor;case 10:return &p->system.break_count;case 11:return &p->system.break_chain;case 12:return &p->active_shot;default:return nullptr;}}
API(roaring_collect) u32 roaring_collect(RoaringFixture* p,i32 kind){p->events.clear();return p->system.collect(kind);}
API(roaring_break) u32 roaring_break(RoaringFixture* p,u32 death){p->events.clear();return p->system.break_roaring(death!=0);}
API(roaring_rewards) u32 roaring_rewards(RoaringFixture* p){p->events.clear();return p->system.reward_items();}
API(roaring_update) u32 roaring_update(RoaringFixture* p,float rate,u32 hud_collect,u32 held){p->events.clear();return p->system.update(rate,hud_collect!=0,held);}
API(roaring_damage_tick) void roaring_damage_tick(RoaringFixture* p,float rate){p->damage.tick(rate);}
API(roaring_events) const u32* roaring_events(RoaringFixture* p){return p->events.data();}
API(roaring_event_count) u32 roaring_event_count(RoaringFixture* p){return p->events.size();}
API(roaring_error) const char* roaring_error(RoaringFixture* p){return p->system.error.c_str();}
API(roaring_rng) Rng* roaring_rng(RoaringFixture* p,u32 visual){return visual?&p->animations.random:&p->animations.environment.game_rng;}
API(roaring_damage_pool) DamageSources* roaring_damage_pool(RoaringFixture* p){return &p->damage;}
API(roaring_damage_route) void roaring_damage_route(RoaringFixture* p){p->damage.hit_callback=[p](DamageSource& source,const DamageQuery&)->i32{if(source.callback_kind==4)return p->system.otter_hit();p->damage.error="Unconnected roaring damage callback";return -1;};}
