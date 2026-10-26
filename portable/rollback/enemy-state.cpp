#include "multiplayer/EnemyFrameState.hpp"
#include <iostream>
#include <stdexcept>
using namespace th15;
using th15::multiplayer::EnemyFrameState;
static void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
struct Host final:EnemyManagerHost {
    int destroyed=0;
    EnemyVisualHost* animations()override{return nullptr;}
    BulletEmissionHost* bullets()override{return nullptr;}
    bool destroy(EnemyState&)override{++destroyed;return true;}
    int after_script(EnemyRuntime&,float)override{return 0;}
    int collide_and_damage(EnemyRuntime&,float)override{return 0;}
    bool update_distortion(EnemyState&,float)override{return true;}
    bool pause_animation(u32,bool)override{return true;}
    bool sound(i32,const Vec3&)override{return true;}
};
int main(){try{
    EclProgram program;auto file=std::make_unique<EclResource>();file->bytes.resize(32);
    EclInstruction wait{};wait.time=1000000;wait.length=16;wait.difficulty=255;
    std::memcpy(file->bytes.data()+16,&wait,sizeof wait);file->subroutines.push_back({"hold",0,32});
    program.definitions.push_back({file.get(),0});program.files.push_back(std::move(file));
    EnemyWorldState world;Rng game{123,0,0},visual{456,0,0};world.random=&game;Host host;
    EnemyManager manager(program,world,game,visual,host);manager.rate=.375f;
    EnemySpawnRequest request;request.routine="hold";request.position={0,100,0};request.life=100;
    for(int i=0;i<3;++i)check(manager.spawn(request),manager.error.c_str());
    auto* original=manager.find(1);check(original!=nullptr,"initial actor");
    original->state.death_script="retained allocation, copied mutable payload";
    original->state.interrupts[3].script="fractional child thread";
    original->state.age_timer.tick(&manager.rate);original->state.age_timer.tick(&manager.rate);
    auto threads=original->scripts.snapshot();threads.children.push_back(threads.main);
    threads.children.back().thread_id=71;threads.children.back().time=3.625f;threads.children.back().stack[101]=73;
    check(original->scripts.restore(threads),"install concurrent thread");
    world.boss_ids[0]=1;world.boss_ids[1]=3;world.refresh_boss();world.integer_registers[2]=1234;
    const auto saved_timer=original->state.age_timer;const auto saved_game=game,saved_visual=visual;
    const auto next_id=manager.next_identifier;EnemyFrameState snapshot;
    check(snapshot.capture(manager),snapshot.error.c_str());check(snapshot.size()==3,"retained count");
    for(int i=0;i<7;++i)check(manager.update(),manager.error.c_str());
    original->state.death_script="future";original->state.interrupts[3].script.clear();original->state.life=7;
    check(manager.clear(),"logical clear");check(host.destroyed==3,"native cleanup happens at erase");
    check(manager.spawn(request),"future allocation");game.next32();visual.next16();world.integer_registers[2]=-1;
    check(snapshot.restore(manager),snapshot.error.c_str());
    check(host.destroyed==3,"restore must not call cleanup/spawn");
    check(manager.count()==3&&manager.find(1)==original,"actor allocation identity");
    check(world.boss==&original->state&&world.boss_at(1)==&manager.find(3)->state,"world boss references");
    check(world.enemies[0]==&original->state&&world.integer_registers[2]==1234,"world order/registers");
    check(original->state.life==100&&original->state.death_script=="retained allocation, copied mutable payload","deep actor values");
    check(original->state.interrupts[3].script=="fractional child thread","interrupt payload");
    check(std::memcmp(&original->state.age_timer,&saved_timer,sizeof saved_timer)==0,"fractional timer bytes");
    check(original->scripts.count()==2&&original->scripts.at(1)->time==3.625f&&original->scripts.at(1)->stack[101]==73,"concurrent thread stack/time");
    check(original->scripts.at(1)->variables==&original->variables&&original->scripts.at(1)->commands==&original->commands,"concurrent thread owner references");
    check(game.seed==saved_game.seed&&game.calls==saved_game.calls&&visual.seed==saved_visual.seed&&visual.calls==saved_visual.calls,"both RNG streams");
    check(manager.next_identifier==next_id&&manager.rate==.375f,"manager allocation/time state");
    check(manager.spawn(request)&&manager.find(next_id)!=nullptr,"post-restore allocation identifier");
    check(snapshot.restore(manager),"immutable repeated restore");
    check(host.destroyed==3&&manager.count()==3,"discarded speculative actors have no logical cleanup");
    EnemyWorldState other_world;EnemyManager other(program,other_world,game,visual,host);
    check(!snapshot.restore(other),"cross-scene restore rejected");
    std::cout<<"TH15 enemy allocation, ECL and fractional rollback passed\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
