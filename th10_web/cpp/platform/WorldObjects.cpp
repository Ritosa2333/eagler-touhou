#include "../game/CallbackNames.hpp"
#include "World.hpp"
#include "../game/GameObjectResources.hpp"
#include "../game/HighRefresh.hpp"
#include <cstdlib>
namespace th10::browser {
namespace {
struct Resources final:GameObjectResourceEnvironment {
    World& w;explicit Resources(World& world):w(world){items=&w.actors.items;bomb=&w.actors.bomb;effects=&w.actors.effects;chain=&w.chain;callbacks=&w.engine.callback_environment;item_update=callback_id::ItemsUpdate;item_draw=callback_id::ItemsDraw;bomb_update=callback_id::BombUpdate;bomb_draw=callback_id::BombDraw;effects_update=callback_id::EffectsUpdate;effects_draw=callback_id::EffectsDraw;}
    void* allocate(u32 size) override{return std::malloc(size);}
    void delete_object(void* p) override{std::free(p);}void release_geometry(void* p) override{w.engine.release_memory(p);}
    AnmFile* load_effect_animations() override{return w.engine.manager.load(7,"bullet.anm",w.engine.resources);}
    void report_effect_error() override{w.fail();}
};
struct Items final:ItemFrameEnvironment,ItemDrawEnvironment {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    World& w;
    i32 seat,unavailable=2,no_auto_collect=0;
    Items(World& world,i32 selected=0):w(world),seat(selected){
        collector_available=selected>=0;
        auto& pilot=w.pilots[selected<0?0:u32(selected)];
        animation_file=w.actors.bullets->animation_file;animations=&w.engine;started_animations=&w.engine.manager.started_scripts;default_rate=&w.engine.speed;power=&pilot.game.power;economy=&pilot.game;
        auto& player=*pilot.player;player_position=&player.position;player_state=selected<0?&unavailable:&player.state;player_attraction_speed=&player.profile->item_attraction_speed;auto_collect=selected<0?&no_auto_collect:&pilot.bomb->active;input_keys=&pilot.input_keys;
        pickup_region=reinterpret_cast<const ItemRegion*>(&player.pickup_bounds);slow_region=reinterpret_cast<const ItemRegion*>(&player.slow_pickup_bounds);fast_region=reinterpret_cast<const ItemRegion*>(&player.fast_pickup_bounds);
    }
#else
    World& w;explicit Items(World& world):w(world){
        animation_file=w.actors.bullets->animation_file;animations=&w.engine;started_animations=&w.engine.manager.started_scripts;default_rate=&w.engine.speed;power=&w.state.game.power;economy=&w.state.game;
#ifdef TH_ENABLE_THPRAC
        practice=&w.state.practice;
#endif
        auto& player=*w.actors.player;player_position=&player.position;player_state=&player.state;player_attraction_speed=&player.profile->item_attraction_speed;auto_collect=&w.actors.bomb->active;input_keys=reinterpret_cast<const u32*>(&w.input.player_profiles[0].input.current);
        pickup_region=reinterpret_cast<const ItemRegion*>(&player.pickup_bounds);slow_region=reinterpret_cast<const ItemRegion*>(&player.slow_pickup_bounds);fast_region=reinterpret_cast<const ItemRegion*>(&player.fast_pickup_bounds);
    }
#endif
    void spawn_effect(const Vec3& p,i32 script) override{w.effect(*animation_file,script,p);}
    void show_notification(i32 script) override{HudEconomy env(*w.hud);env.show_notification(script);}
    void play_global_sound(i32 id) override{w.sound(id);}
    void update_lives(i32 lives) override{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(seat!=0)return;
#endif
        w.actors.gui->update_lives(lives);
    }
    void update_power_display(i32 whole,i32 fraction) override{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(seat!=0)return;
#endif
        w.hud->update_power(whole,fraction);
    }
    void refresh_player_power() override{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if(seat>=0)w.configure_player(w.pilots[u32(seat)]);
#else
        w.configure_player();
#endif
    }
    void popup(const Vec3& p,i32 value,u32 color) override{w.popup(p,value,color);}
    void play_sound(i32 id,float x) override{w.sound(id,x);}
    void bind_item_sprite(AnmVm& vm,i32 sprite) override{animation_file->bind_sprite(vm,sprite);}
    void draw_animation(AnmVm& vm) override{w.engine.draw(vm);}
    bool presentation(const Item& item,Vec3& position) override{
        if(!high_refresh::render_only||!high_refresh::active)return false;const Item* base=nullptr;const World::ItemPresentation* sample=nullptr;
        if(&item>=w.actors.items->regular&&&item<w.actors.items->regular+150){const auto i=&item-w.actors.items->regular;sample=&w.item_regular_presentation[i];}
        else if(&item>=w.actors.items->faith&&&item<w.actors.items->faith+2048){const auto i=&item-w.actors.items->faith;sample=&w.item_faith_presentation[i];}else return false;
        const float dx=item.position.x-sample->position.x,dy=item.position.y-sample->position.y;if(!sample->active||sample->state!=item.state||sample->kind!=item.kind||item.timer.current<sample->age||dx*dx+dy*dy>=16384.0f)return false;
        position={high_refresh::lerp_world(sample->position.x,item.position.x),high_refresh::lerp_world(sample->position.y,item.position.y),high_refresh::lerp_world(sample->position.z,item.position.z)};return true;
    }
};
}
bool World::create_items(){Resources env(*this);return GameObjectResources{env}.create(GameObjectKind::Items)!=nullptr;}
void World::destroy_items(ItemManager* p){Resources env(*this);GameObjectResources{env}.shutdown(*p);std::free(p);}
i32 World::update_items(){for(u32 i=0;i<150;++i){const auto& v=actors.items->regular[i];auto& p=item_regular_presentation[i];p.active=v.state!=0;if(p.active)p={v.position,v.timer.current,v.state,v.kind,true};}for(u32 i=0;i<2048;++i){const auto& v=actors.items->faith[i];auto& p=item_faith_presentation[i];p.active=v.state!=0;if(p.active)p={v.position,v.timer.current,v.state,v.kind,true};}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    multiplayer::ItemCandidate candidates[3]{};
    for(u32 seat=0;seat<player_count;++seat){
        const auto& pilot=pilots[seat];const auto& p=*pilot.player;auto& c=candidates[seat];
        c.eligible=cooperation.seats[seat].lifeState==multiplayer::LifeState::Alive&&p.state!=2&&p.state!=4;
        c.position=p.position;c.auto_collect=pilot.bomb&&pilot.bomb->active;c.focused=(pilot.input_keys&4)!=0;
        c.pickup=*reinterpret_cast<const ItemRegion*>(&p.pickup_bounds);c.slow=*reinterpret_cast<const ItemRegion*>(&p.slow_pickup_bounds);c.fast=*reinterpret_cast<const ItemRegion*>(&p.fast_pickup_bounds);
    }
    actors.items->faith_count=actors.items->active_count=0;
    auto update=[&](Item& item,multiplayer::ItemOwnership& owner){
        if(!item.state){owner={};return;}
        const auto previous_claim=owner.homing;
        const int seat=multiplayer::select_item_collector(owner,item,candidates,player_count);
        if((item.state==3||item.state==4)&&(seat<0||(previous_claim>=0&&owner.homing<0))){item.state=1;item.velocity={};}
        Items env(*this,seat);const auto result=item.update(env);
        if(!item.state)owner={};else if((item.state==3||item.state==4)&&seat>=0)owner.homing=std::int8_t(seat);
        if(result==ItemUpdate::Active)++actors.items->active_count;
    };
    for(u32 i=0;i<150;++i)update(actors.items->regular[i],regular_item_owners[i]);
    for(u32 i=0;i<2048;++i)update(actors.items->faith[i],faith_item_owners[i]);
    // Losing or reviving a pilot also changes the team's full-power threshold.
    convert_power();return 1;
#else
    Items env(*this);return actors.items->update(env);
#endif
}
i32 World::draw_items(){Items env(*this);return actors.items->draw(env);}
i32 World::spawn_item(const Vec3& p,i32 kind,u32 color,float angle,float speed){Items env(*this);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    std::int16_t minimum_power=100;bool any_alive=false;
    for(u32 seat=0;seat<player_count;++seat)if(cooperation.seats[seat].lifeState==multiplayer::LifeState::Alive){any_alive=true;if(pilots[seat].game.power<minimum_power)minimum_power=pilots[seat].game.power;}
    if(!any_alive)minimum_power=0;
    env.power=&minimum_power;
    if(kind==8){const auto index=actors.items->faith_cursor;if(!actors.items->faith[index].state){
        auto& item=actors.items->faith[index];
        if(!rollback.Touch(&item,sizeof(item))){fail();return 0;}
        faith_item_owners[index]={};
    }}
    else for(u32 i=0;i<150;++i)if(!actors.items->regular[i].state){
        auto& item=actors.items->regular[i];
        if(!rollback.Touch(&item,sizeof(item))){fail();return 0;}
        regular_item_owners[i]={};break;
    }
#endif
    return actors.items->spawn(p,kind,color,angle,speed,env);
}
i32 World::convert_power(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    bool any_alive=false;
    for(u32 seat=0;seat<player_count;++seat)if(cooperation.seats[seat].lifeState==multiplayer::LifeState::Alive){any_alive=true;if(pilots[seat].game.power<100)return 0;}
    if(!any_alive)return 0;
    for(auto& item:actors.items->regular)if(item.state&&(item.kind==1||item.kind==4)){
        const auto p=item.position;item.state=0;spawn_item(p,9,0xffffffff,-1.5707963705062866f,2.2f);effect(*actors.bullets->animation_file,0x189,p);
    }
    return 1;
#else
    Items env(*this);return actors.items->convert_power(env);
#endif
}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
bool World::spawn_life_transfer(u32 donor,u32 recipient){
    if(donor>=player_count||recipient>=player_count||donor==recipient||!actors.items||!pilots[donor].player)return false;
    u32 index=0;while(index<150&&actors.items->regular[index].state)++index;
    if(index==150)return false;
    spawn_item(pilots[donor].player->position,7,0xffffffff,-1.5707963705062866f,2.2f);
    auto& item=actors.items->regular[index];if(!item.state||item.kind!=7)return false;
    regular_item_owners[index]={std::int8_t(recipient),std::int8_t(recipient)};
    return true;
}
#endif
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
bool World::create_bomb(){Resources env(*this);return GameObjectResources{env}.create(GameObjectKind::Bomb)!=nullptr;}
void World::destroy_bomb(Bomb* p){Resources env(*this);GameObjectResources{env}.shutdown(*p);std::free(p);}
#endif
bool World::create_effects(){Resources env(*this);return GameObjectResources{env}.create(GameObjectKind::Effects)!=nullptr;}
void World::destroy_effects(GameEffects* p){Resources env(*this);GameObjectResources{env}.shutdown(*p);std::free(p);}
}
