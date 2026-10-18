#include "../th10_web/cpp/game/Item.hpp"
#include <cassert>
#include <cstdio>

using namespace th10;
// Collection returns before animation. If a fixture accidentally exercises
// another path, fail instead of silently replacing simulation with a stub.
namespace th10 {
i32 AnmVm::update(AnmEnvironment&){assert(false);return 0;}
void ItemEnvironment::initialize_animation(Item&,i32){assert(false);}
}
struct Collector final:ItemFrameEnvironment {
    Vec3 position{0,300,0};i32 state=1,collect_all=0;u32 keys=0;
    float speed=1,attraction=5;
    ItemRegion bounds{{-20,280,0},{20,320,0}};
    int refreshed=0,lives_shown=-1;
    explicit Collector(GameEconomy& game){
        economy=&game;power=&game.power;default_rate=&speed;
        player_position=&position;player_state=&state;player_attraction_speed=&attraction;
        auto_collect=&collect_all;input_keys=&keys;pickup_region=slow_region=fast_region=&bounds;
        game.faith_timer.rate=&speed;
    }
    void spawn_effect(const Vec3&,i32) override{assert(false);}
    void show_notification(i32) override{}
    void play_global_sound(i32) override{}
    void update_lives(i32 n) override{lives_shown=n;}
    void update_power_display(i32,i32) override{}
    void refresh_player_power() override{++refreshed;}
    void popup(const Vec3&,i32,u32) override{}
    void play_sound(i32,float) override{}
    ItemUpdate collect(i32 kind){
        Item item{};item.state=1;item.kind=kind;item.position=position;
        const auto result=item.update(*this);assert(item.state==0);return result;
    }
};
int main(){
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    multiplayer::TeamEconomy team{};multiplayer::PilotEconomy owners[3]{};
    GameEconomy games[3]{{team,owners[0]},{team,owners[1]},{team,owners[2]}};
    Collector p0(games[0]),p1(games[1]),p2(games[2]);
    games[0].power=20;games[1].power=80;games[2].power=100;
    games[0].lives=2;games[1].lives=3;games[2].lives=9;
    assert(p1.collect(4)==ItemUpdate::FullPower);
    assert(p1.refreshed==1&&games[1].power==100&&games[0].power==20);
    assert(p0.collect(1)==ItemUpdate::Skipped);
    assert(games[0].power==21&&games[1].power==100&&games[2].power==100);
    p1.collect(7);
    assert(games[1].lives==4&&games[0].lives==2&&games[2].lives==9);
    p2.collect(7);assert(games[2].lives==9&&p2.lives_shown==9);
    games[0].item_value=10000;const auto score=team.score;
    p2.collect(2);assert(team.score>score&&games[0].score==games[1].score);
    assert(team.faith_timer.current>0);
    // The final pilot becoming a spirit must not remove the native penalty
    // when a converted/delayed faith item falls past the playfield.
    p0.collector_available=false;p0.state=2;team.rank=100;
    Item missed{};missed.state=2;missed.kind=8;
    missed.position={0,473,0};missed.velocity={0,1,0};
    assert(missed.update(p0)==ItemUpdate::Skipped);
    assert(missed.state==0&&team.rank==96);
    std::puts("native item rewards use collector resources and shared economy: PASS");
}
