#pragma once
#include "EnemyRuntime.hpp"
#include <list>
namespace th15 {
namespace multiplayer {class EnemyFrameState;}
struct EnemyManagerHost:EnemyRuntimeHost,EnemyCommandHost {
    virtual BulletEmissionHost* bullets()=0;
    virtual bool destroy(EnemyState&)=0;
    virtual int scene_death(EnemyRuntime&,float){return -2;}
};
class EnemyManager final:public EnemySpawnHost {
    friend class EnemyCheckpoint;
    friend class multiplayer::EnemyFrameState;
    EclProgram& program;EnemyWorldState& world;Rng& random;Rng& visual_random;EnemyManagerHost& host;
    // Frame snapshots retain allocation identity while keeping their own copy
    // of mutable state. Logical cleanup still runs at the original erase point.
    using ActiveEnemies=std::list<std::shared_ptr<EnemyRuntime>>;
    ActiveEnemies active;
    bool erase(ActiveEnemies::iterator);
public:
    Timer timer;u32 next_identifier=1;float rate=1;Vec3 background_delta{};
    std::string error;
    EnemyManager(EclProgram& program,EnemyWorldState& world,Rng& random,Rng& visual_random,EnemyManagerHost& host):program(program),world(world),random(random),visual_random(visual_random),host(host){if(!world.frame_rate)world.frame_rate=&rate;world.enemy_control=99999;timer.set(0);}
    bool spawn(const EnemySpawnRequest& request)override{return spawn_at_rate(request,rate);}
    bool spawn_at_rate(const EnemySpawnRequest&,float initial_rate)override;
    bool boss_exists(i32 slot)const noexcept override;
    bool clear_field(bool suppress_death_script)override;
    bool switch_enemy_routine(u32,const std::string&)override;
    const std::string* failure()const noexcept override{return error.empty()?nullptr:&error;}
    bool update();bool clear();
    EnemyRuntime* find(u32 identifier)noexcept;
    EnemyRuntime* at(u32 index)noexcept;
    u32 count()const noexcept{return active.size();}
};
}
