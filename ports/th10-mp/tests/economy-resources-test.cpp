#include "../th10_web/cpp/game/GameEconomy.hpp"
#include <cassert>
#include <cstdio>

using namespace th10;
struct Notifications final:EconomyEnvironment {
    i32 life=-1,notification=-1,sound=-1;
    void show_notification(i32 n) override {notification=n;}
    void play_global_sound(i32 n) override {sound=n;}
    void update_lives(i32 n) override {life=n;}
};

int main() {
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    float speed=1;
    Notifications hud;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    multiplayer::TeamEconomy team{};
    multiplayer::PilotEconomy owners[3]{};
    GameEconomy p0{team,owners[0]},p1{team,owners[1]},p2{team,owners[2]};
    GameEconomy* pilots[]{&p0,&p1,&p2};
    for(i32 i=0;i<3;++i){pilots[i]->power=20+i*20;pilots[i]->lives=2+i;pilots[i]->character=i%2;pilots[i]->shot_type=i;}
    p0.faith_timer.rate=&speed;
    p1.add_score(12345);
    assert(p0.score==1234&&p2.score==1234);
    p2.add_item_value(1000);
    p1.add_rank(60);
    assert(p0.item_value==100&&p2.rank==60);
    p1.add_power(25,hud);
    p1.add_lives(1,hud);
    assert(p0.power==20&&p1.power==65&&p2.power==60);
    assert(p0.lives==2&&p1.lives==4&&p2.lives==4&&hud.life==4);
    p2.power-=20; // Native TH10 bomb cost is paid by its bound pilot.
    assert(p0.power==20&&p1.power==65&&p2.power==40);
    p2.extend_faith_timer(60,&speed);
    assert(p0.faith_timer.current==60&&p1.faith_timer.current==60);
    const auto saved_team=team;
    const auto saved_pilot=owners[1];
    p1.add_score(3000);p1.power=0;p1.lives=-1;
    team=saved_team;owners[1]=saved_pilot;
    // Restoring only value owners keeps every native view correctly bound.
    p1.add_lives(1,hud);p2.add_score(100);
    assert(p0.score==1244&&p1.score==1244&&p2.score==1244);
    assert(p1.lives==5&&p1.power==65&&p0.lives==2&&p2.lives==4);
    assert(p0.character==0&&p1.character==1&&p2.shot_type==2);
    p2.lives=9;p2.add_lives(1,hud);assert(p2.lives==9&&p0.lives==2);
#else
    static_assert(sizeof(GameEconomy)==0x64);
    GameEconomy p0{};
    p0.faith_timer.rate=&speed;
    p0.add_score(12345);assert(p0.score==1234);
    p0.power=40;p0.add_power(25,hud);assert(p0.power==65);
    p0.lives=9;p0.add_lives(1,hud);assert(p0.lives==9);
    p0.extend_faith_timer(60,&speed);assert(p0.faith_timer.current==60);
#endif
    std::puts("economy owner isolation and native arithmetic: PASS");
}
