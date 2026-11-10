#pragma once
#include "../../cpp/game/ItemSpirits.hpp"
struct ItemSpiritsFixture:ItemSpiritHost {
    AnmManagerFixture& animations;EffectManager effects;PlayerMotion player;PlayerBounds bounds;PlayerLifeSession session;Timer invulnerability{0,0,0,0,0};PlayerSpellStatus spell;DamageSources damage;ItemScoreState score;
    ItemSpirits system;std::vector<u32> events;bool active_shot=false;
    ItemSpiritsFixture(AnmManagerFixture& a,i32 ch):animations(a),effects(a.manager,8),system(a.manager,effects,a.environment.game_rng,a.random,player,bounds,session,invulnerability,spell,damage,score,*this,ch,9,30){}
    bool sound(i32 id,bool queued)override{events.insert(events.end(),{1,u32(id),u32(queued)});return true;}
    bool trophy(i32 id)override{events.insert(events.end(),{2,u32(id)});return true;}
    bool options_changed()override{events.push_back(3);return true;}
    bool notice(i32 id)override{events.insert(events.end(),{4,u32(id)});return true;}
    bool popup(const Vec3& p,i32 value,u32 color)override{events.insert(events.end(),{5,u32(value),color,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z)});return true;}
    bool life_hud(i32 lives,i32 pieces)override{events.insert(events.end(),{6,u32(lives),u32(pieces)});return true;}
    bool bomb_hud(i32 bombs,i32 pieces)override{events.insert(events.end(),{7,u32(bombs),u32(pieces)});return true;}
    bool cancel_bullets(const Vec3& p,float radius,i32 reward)override{events.insert(events.end(),{8,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(radius),u32(reward)});return true;}
    bool cancel_lasers(const Vec3& p,float radius,i32 reward,bool honor)override{events.insert(events.end(),{9,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(radius),u32(reward),u32(honor)});return true;}
    bool active_player_shot()const override{return active_shot;}
};
API(item_spirits_create) ItemSpiritsFixture* item_spirits_create(AnmManagerFixture* a,i32 ch){return new ItemSpiritsFixture(*a,ch);}
API(item_spirits_delete) void item_spirits_delete(ItemSpiritsFixture* p){delete p;}
API(item_spirits_hud) u32 item_spirits_hud(ItemSpiritsFixture* p){return p->system.initialize();}
API(item_spirits_roaring_field) void* item_spirits_roaring_field(ItemSpiritsFixture* p,u32 field){switch(field){case 0:return &p->system.roaring.state;case 1:return &p->system.roaring.visuals;case 2:return &p->player.position;case 3:return &p->session.animal;case 4:return &p->player.behavior_flags;case 5:return &p->invulnerability;case 6:return &p->spell;case 7:return &p->session.bomb_state;case 8:return p->damage.sources.data();case 9:return &p->damage.cursor;case 10:return &p->system.roaring.break_count;case 11:return &p->system.roaring.break_chain;case 12:return &p->active_shot;default:return nullptr;}}
API(item_spirits_field) void* item_spirits_field(ItemSpiritsFixture* p,u32 f){switch(f){case 0:return &p->session;case 1:return &p->score;case 2:return &p->bounds.item_near;case 3:return &p->bounds.graze;case 4:return &p->bounds.item_full;case 5:return &p->system.items.requests;case 6:return &p->system.items.cancel_density;case 7:return &p->system.items.cancel_stamp;case 8:return &p->system.items.active_count;case 9:return &p->system.items.motion_scale;default:return nullptr;}}
API(item_spirits_spawn) u32 item_spirits_spawn(ItemSpiritsFixture* p,i32 kind,float x,float y,float z,float angle,float speed,i32 delay){return p->system.spawn(kind,{x,y,z},angle,speed,delay);}
API(item_spirits_item_state) ItemState* item_spirits_item_state(ItemSpiritsFixture* p,u32 id){auto* slot=p->system.items.find(id);return slot?&slot->state:nullptr;}
API(item_spirits_item_vm) AnmVm* item_spirits_item_vm(ItemSpiritsFixture* p,u32 id,u32 arrow){auto* slot=p->system.items.find(id);return slot?(arrow?slot->arrow.get():slot->body.get()):nullptr;}
API(item_spirits_free) u32 item_spirits_free(ItemSpiritsFixture* p,u32 cancel){return p->system.items.free_head(cancel!=0);}
API(item_spirits_count) u32 item_spirits_count(ItemSpiritsFixture* p){return p->system.spirits.count();}
API(item_spirits_actor) AnimalSpiritState* item_spirits_actor(ItemSpiritsFixture* p,u32 index){return p->system.spirits.find(p->system.spirits.order(index));}
API(item_spirits_update) u32 item_spirits_update(ItemSpiritsFixture* p,float rate,u32 phase,u32 held,u32 hud,u32 practice){p->events.clear();const ItemSpiritFrame frame{rate,5,1,0,0,held,hud!=0,practice!=0};p->system.set_practice(practice!=0);return phase?p->system.update_spirits(frame):p->system.update_items(frame);}
API(item_spirits_break) u32 item_spirits_break(ItemSpiritsFixture* p,u32 death){p->events.clear();return p->system.end_roaring(death!=0);}
API(item_spirits_damage_tick) void item_spirits_damage_tick(ItemSpiritsFixture* p,float rate){p->damage.tick(rate);}
API(item_spirits_events) const u32* item_spirits_events(ItemSpiritsFixture* p){return p->events.data();}
API(item_spirits_event_count) u32 item_spirits_event_count(ItemSpiritsFixture* p){return p->events.size();}
API(item_spirits_clear_events) void item_spirits_clear_events(ItemSpiritsFixture* p){p->events.clear();}
API(item_spirits_rng) Rng* item_spirits_rng(ItemSpiritsFixture* p,u32 visual){return visual?&p->animations.random:&p->animations.environment.game_rng;}
API(item_spirits_error) const char* item_spirits_error(ItemSpiritsFixture* p){return p->system.error.c_str();}
