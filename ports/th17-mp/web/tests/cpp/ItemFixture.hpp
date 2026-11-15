#pragma once
#include "../../cpp/game/ItemManager.hpp"
struct ItemCollectionFixture:ItemCollectionHost {
    PlayerLifeSession player;ItemScoreState score;PlayerMotion motion;ItemState item;ItemCollection collection;std::vector<u32> events;
    ItemCollectionFixture(i32 character):collection(player,score,motion,*this,character){}
    void event(u32 kind,i32 a=0,i32 b=0,const Vec3& p={}){events.insert(events.end(),{kind,u32(a),u32(b),float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z)});}
    bool options_changed()override{event(0);return true;}bool notice(i32 id)override{event(1,id);return true;}
    bool sound(i32 id,bool queued)override{event(queued?3:2,id);return true;}
    bool popup(const Vec3& p,i32 value,u32 color)override{event(4,value,i32(color),p);return true;}
    bool life_hud(i32 lives,i32 pieces)override{event(5,lives,pieces);return true;}bool bomb_hud(i32 bombs,i32 pieces)override{event(6,bombs,pieces);return true;}
};
API(item_collection_create) ItemCollectionFixture* item_collection_create(i32 character){return new ItemCollectionFixture(character);}
API(item_collection_delete) void item_collection_delete(ItemCollectionFixture* p){delete p;}
API(item_collection_field) void* item_collection_field(ItemCollectionFixture* p,u32 f){switch(f){case 0:return &p->player;case 1:return &p->score;case 2:return &p->motion.position;default:return &p->item;}}
API(item_collection_award) u32 item_collection_award(ItemCollectionFixture* p,i32 kind){p->events.clear();p->item.kind=kind;return p->collection.award(p->item);}
API(item_collection_events) const u32* item_collection_events(ItemCollectionFixture* p){return p->events.data();}
API(item_collection_event_count) u32 item_collection_event_count(ItemCollectionFixture* p){return p->events.size();}
API(item_collection_error) const char* item_collection_error(ItemCollectionFixture* p){return p->collection.error.c_str();}
struct ItemManagerFixture {
    AnmManagerFixture& animations;EffectManager effects;ItemManager manager;ItemSpawnContext context;std::vector<i32> sounds;std::vector<u32> spirits;
    ItemCollectionFixture collection;PlayerBounds bounds;ItemFrameContext frame;
    ItemManagerFixture(AnmManagerFixture& a,i32 id,i32 character=0):animations(a),effects(a.manager,8),manager(a.manager,effects,id),context{a.environment.game_rng,{},[this](i32 kind,const Vec3& p,float direction){spirits.insert(spirits.end(),{u32(kind),float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(direction)});return true;}},collection(character),frame{collection.motion,bounds,collection.collection,a.random}{frame.character=character;manager.sound=[this](i32 id){sounds.push_back(id);collection.event(3,id);return true;};manager.immediate_sound=[this](i32 id){collection.event(2,id);return true;};}
};
API(item_manager_create) ItemManagerFixture* item_manager_create(AnmManagerFixture* a,i32 id){return new ItemManagerFixture(*a,id);}
API(item_manager_delete) void item_manager_delete(ItemManagerFixture* p){delete p;}
API(item_manager_spawn) u32 item_manager_spawn(ItemManagerFixture* p,i32 kind,float x,float y,float z,float angle,float speed,i32 delay,i32 sound){return p->manager.spawn(kind,{x,y,z},angle,speed,delay,sound,&p->context);}
API(item_manager_state) ItemState* item_manager_state(ItemManagerFixture* p,u32 id){auto* s=p->manager.find(id);return s?&s->state:nullptr;}
API(item_manager_vm) AnmVm* item_manager_vm(ItemManagerFixture* p,u32 id,u32 arrow){auto* s=p->manager.find(id);return s?(arrow?s->arrow.get():s->body.get()):nullptr;}
API(item_manager_field) void* item_manager_field(ItemManagerFixture* p,u32 f){switch(f){case 0:return &p->manager.requests;case 1:return &p->manager.cancel_density;case 2:return &p->manager.cancel_stamp;case 4:return &p->manager.active_count;case 5:return &p->manager.motion_scale;case 6:return &p->animations.environment.game_rng;case 7:return &p->animations.random;case 8:return &p->context.player;case 9:return &p->collection.player;case 10:return &p->collection.score;case 11:return &p->collection.motion.position;case 12:return &p->bounds.item_near;case 13:return &p->bounds.graze;case 14:return &p->bounds.item_full;case 15:return &p->frame.player_state;case 16:return &p->frame.bomb_state;case 17:return &p->frame.bomb_frame;case 18:return &p->frame.held;case 19:return &p->frame.attraction_speed;default:return nullptr;}}
API(item_manager_error) const char* item_manager_error(ItemManagerFixture* p){return p->manager.error.c_str();}
API(item_manager_free) u32 item_manager_free(ItemManagerFixture* p,u32 cancel){return p->manager.free_head(cancel!=0);}
API(item_manager_release) u32 item_manager_release(ItemManagerFixture* p,u32 id){return p->manager.release(id);}
API(item_manager_reset) u32 item_manager_reset(ItemManagerFixture* p){return p->manager.reset();}
API(item_manager_sounds) const i32* item_manager_sounds(ItemManagerFixture* p){return p->sounds.data();}
API(item_manager_sound_count) u32 item_manager_sound_count(ItemManagerFixture* p){return p->sounds.size();}
API(item_manager_spirits) const u32* item_manager_spirits(ItemManagerFixture* p){return p->spirits.data();}
API(item_manager_spirit_count) u32 item_manager_spirit_count(ItemManagerFixture* p){return p->spirits.size();}
API(item_frame_create) ItemManagerFixture* item_frame_create(AnmManagerFixture* a,i32 resource,i32 character){return new ItemManagerFixture(*a,resource,character);}
API(item_frame_update) u32 item_frame_update(ItemManagerFixture* p,float rate,u32 hud_collect){p->frame.rate=rate;p->frame.hud_collect=hud_collect!=0;p->collection.events.clear();return p->manager.update(p->frame);}
API(item_frame_events) const u32* item_frame_events(ItemManagerFixture* p){return p->collection.events.data();}
API(item_frame_event_count) u32 item_frame_event_count(ItemManagerFixture* p){return p->collection.events.size();}
API(item_manager_draw) u32 item_manager_draw(ItemManagerFixture* p,AnmRendererFixture* r){return p->manager.draw(r->renderer);}
