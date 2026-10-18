#pragma once
#include "../../cpp/game/ItemManager.hpp"
#include "../../cpp/game/AnmManager.hpp"
#include "../../cpp/game/ItemRewards.hpp"
#include "render-fixture.hpp"
#include <vector>
namespace th11::test {
struct ItemManagerFixture:ItemWorld,ItemRewardEffects {
    struct Event{i32 kind,id;float x,y;u32 color=0;};
    AnmResource resource;AnmManager animations;std::unique_ptr<ItemManager> manager;
    GameEconomy economy;ItemRewards rewards{economy,*this};bool real_rewards=false;
    RecordingGraphics graphics;AnmRenderer renderer{graphics};
    std::vector<Event> events;
    bool load(const u8* p,u32 n){if(!resource.open(p,n))return false;manager=std::make_unique<ItemManager>(resource,animations,*this,0);return true;}
    bool sound(i32 id,float x)override{events.push_back({0,id,x,0,1});return true;}
    bool effect(Vec3 p,i32 id)override{events.push_back({1,id,p.x,p.y});return true;}
    bool collect(ItemState& item,bool& convert)override{if(real_rewards){const bool ok=rewards.collect(item,convert);manager->player.power=economy.power;return ok;}events.push_back({2,item.type,item.position.x,item.position.y});return true;}
    bool rank_delta(i32 n)override{if(real_rewards)economy.add_rank(n);else events.push_back({3,n,0,0});return true;}
    bool sound(i32 id,float x,bool positional)override{events.push_back({0,id,x,0,u32(positional)});return true;}
    bool popup(Vec3 p,i32 n,u32 color)override{events.push_back({2,n,p.x,p.y,color});return true;}
    bool notify(i32 id)override{events.push_back({4,id,0,0});return true;}
    bool power_changed()override{events.push_back({5,0,0,0});return true;}
    bool lives_changed(i32 lives,i32 pieces)override{events.push_back({6,lives,float(pieces),0});return true;}
};
}
