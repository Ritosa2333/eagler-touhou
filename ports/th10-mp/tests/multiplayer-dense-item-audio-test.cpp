#include "../th10_web/cpp/game/Item.hpp"
#include "../th10_web/cpp/multiplayer/AudioEvents.hpp"
#include <cassert>
#include <cstdio>
#include <vector>

using namespace th10;
namespace th10 {
// Faith collection returns before animation; exercise the real Item::update
// and native AudioManager commands, with no retail assets or audio backend.
i32 AnmVm::update(AnmEnvironment&){assert(false);return 0;}
void ItemEnvironment::initialize_animation(Item&,i32){assert(false);}
}
struct Collector final:ItemFrameEnvironment {
    AudioManager& audio;
    SoundDefinition definitions[128]{};
    Vec3 position{0,300,0};i32 state=1,collect_all=0;u32 keys=0;
    float speed=1,attraction=5;
    ItemRegion bounds{{-20,280,0},{20,320,0}};
    unsigned sounds=0;
    Collector(GameEconomy& game,AudioManager& output):audio(output){
        economy=&game;power=&game.power;default_rate=&speed;
        player_position=&position;player_state=&state;player_attraction_speed=&attraction;
        auto_collect=&collect_all;input_keys=&keys;pickup_region=slow_region=fast_region=&bounds;
        game.faith_timer.rate=&speed;
        definitions[20].lifetime=7;
    }
    void spawn_effect(const Vec3&,i32) override{assert(false);}
    void show_notification(i32) override{assert(false);}
    void play_global_sound(i32) override{assert(false);}
    void update_lives(i32) override{assert(false);}
    void update_power_display(i32,i32) override{assert(false);}
    void refresh_player_power() override{assert(false);}
    void popup(const Vec3&,i32,u32) override{assert(false);}
    void play_sound(i32 id,float x) override{
        assert(id==20);++sounds;audio.queue_effect_position(id,x,definitions);
    }
    void collect(){
        Item item{};item.state=1;item.kind=8;item.position=position;
        assert(item.update(*this)==ItemUpdate::Skipped&&item.state==0);
    }
};
int main(){
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    multiplayer::TeamEconomy team{};multiplayer::PilotEconomy owner{};
    GameEconomy game{team,owner};AudioManager manager{};
    for(auto& effect:manager.pending_effects)effect=-1;
    Collector collector(game,manager);
    for(unsigned i=0;i<2048;++i)collector.collect();
    assert(collector.sounds==2048&&manager.pending_effects[0]==20&&manager.pending_effects[1]==-1);
    assert(manager.pan_count[0]==128&&manager.effect_lifetimes[20]==7);
    std::puts("native: 2048 real faith pickups accepted, 128 pan samples in one native slot");
    multiplayer::AudioEvents events;manager.command_sink=&events;
    assert(events.BeginFrame(0));
    for(unsigned i=0;i<2048;++i)collector.collect();
    assert(events.IsOpen()&&!events.Failed());
    assert(events.EndFrame());
    AudioManager confirmed{};for(auto& effect:confirmed.pending_effects)effect=-1;
    assert(events.CommitThrough(0,0,confirmed,nullptr,nullptr));
    assert(events.EffectsCommitted()==2048&&events.NextCommit()==1);
    assert(std::memcmp(manager.pending_effects,confirmed.pending_effects,sizeof(manager.pending_effects))==0);
    assert(std::memcmp(manager.pan_count,confirmed.pan_count,sizeof(manager.pan_count))==0);
    assert(std::memcmp(manager.pan_values,confirmed.pan_values,sizeof(manager.pan_values))==0);
    assert(std::memcmp(manager.effect_lifetimes,confirmed.effect_lifetimes,sizeof(manager.effect_lifetimes))==0);
    std::puts("rollback: 2048 real faith pickups confirmed; original native queue bytes match");
}
