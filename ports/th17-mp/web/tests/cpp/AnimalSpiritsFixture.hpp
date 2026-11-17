#pragma once
#include "../../cpp/game/AnimalSpirits.hpp"
struct AnimalSpiritsFixture:AnimalSpiritHost {
    AnmManagerFixture& animations;AnimalSpirits spirits;std::vector<u32> events;
    AnimalSpiritsFixture(AnmManagerFixture& a,i32 id):animations(a),spirits(a.manager,a.environment.game_rng,*this,id){}
    bool sound(i32 id)override{events.insert(events.end(),{1,u32(id)});return true;}
    bool trophy(i32 id)override{events.insert(events.end(),{2,u32(id)});return true;}
    bool collect_spirit(i32 kind)override{events.insert(events.end(),{3,u32(kind)});return true;}
};
API(spirits_create) AnimalSpiritsFixture* spirits_create(AnmManagerFixture* a,i32 id){return new AnimalSpiritsFixture(*a,id);}
API(spirits_delete) void spirits_delete(AnimalSpiritsFixture* p){delete p;}
API(spirits_spawn) u32 spirits_spawn(AnimalSpiritsFixture* p,i32 kind,float x,float y,float z,float angle,u32 practice){return p->spirits.spawn(kind,{x,y,z},angle,practice!=0);}
API(spirits_update) u32 spirits_update(AnimalSpiritsFixture* p,float x,float y,float z,float rate){p->events.clear();return p->spirits.update({x,y,z},rate);}
API(spirits_count) u32 spirits_count(AnimalSpiritsFixture* p){return p->spirits.count();}
API(spirits_clear) u32 spirits_clear(AnimalSpiritsFixture* p){return p->spirits.clear();}
API(spirits_order) u32 spirits_order(AnimalSpiritsFixture* p,u32 index){return p->spirits.order(index);}
API(spirits_state) AnimalSpiritState* spirits_state(AnimalSpiritsFixture* p,u32 id){return p->spirits.find(id);}
API(spirits_events) const u32* spirits_events(AnimalSpiritsFixture* p){return p->events.data();}
API(spirits_event_count) u32 spirits_event_count(AnimalSpiritsFixture* p){return p->events.size();}
API(spirits_error) const char* spirits_error(AnimalSpiritsFixture* p){return p->spirits.error.c_str();}
API(spirits_rng) Rng* spirits_rng(AnimalSpiritsFixture* p,u32 visual){return visual?&p->animations.random:&p->animations.environment.game_rng;}
