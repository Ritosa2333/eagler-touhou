#include "EnemyFrameState.hpp"
namespace th15::multiplayer {
struct EnemyFrameState::Data {
    struct Entry {
        EnemyState state;
        EclThreadsSnapshot scripts;
        std::string error,script_error;
        EnemyCommandHost* command_host=nullptr;
        EnemySpawnHost* spawning=nullptr;
        BulletEmissionHost* bullets=nullptr;
        LaserEmissionHost* lasers=nullptr;
        LaserScene* laser_scene=nullptr;
        EnemyVisualHost* visuals=nullptr;
    };
    EnemyManager* owner=nullptr;
    std::vector<std::shared_ptr<EnemyRuntime>> owners;
    std::vector<Entry> entries;
    EnemyWorldState world;
    Rng game_random,visual_random;
    Timer timer;
    u32 next_identifier=1;
    float rate=1;
    Vec3 background_delta{};
    std::string error;
    bool ready=false;
};
EnemyFrameState::EnemyFrameState()=default;
EnemyFrameState::~EnemyFrameState()=default;
EnemyFrameState::EnemyFrameState(EnemyFrameState&&)noexcept=default;
EnemyFrameState& EnemyFrameState::operator=(EnemyFrameState&&)noexcept=default;
void EnemyFrameState::discard()noexcept{if(saved_){saved_->owners.clear();saved_->ready=false;}}
size_t EnemyFrameState::size()const noexcept{return saved_&&saved_->ready?saved_->owners.size():0;}
bool EnemyFrameState::capture(EnemyManager& manager){
    error.clear();if(!saved_)saved_=std::make_unique<Data>();auto& s=*saved_;s.ready=false;
    s.owner=&manager;s.owners.assign(manager.active.begin(),manager.active.end());
    if(s.entries.size()<s.owners.size())s.entries.resize(s.owners.size());
    for(size_t i=0;i<s.owners.size();++i){const auto& enemy=*s.owners[i];auto& entry=s.entries[i];
        entry.state=enemy.state;entry.scripts=enemy.scripts.snapshot();entry.error=enemy.error;entry.script_error=enemy.scripts.error;
        entry.command_host=enemy.commands.host;entry.spawning=enemy.commands.spawning;entry.bullets=enemy.commands.bullets.host;
        entry.lasers=enemy.commands.lasers.host;entry.laser_scene=enemy.commands.lasers.scene;entry.visuals=enemy.commands.visuals.host;
    }
    s.world=manager.world;s.game_random=manager.random;s.visual_random=manager.visual_random;s.timer=manager.timer;
    s.next_identifier=manager.next_identifier;s.rate=manager.rate;s.background_delta=manager.background_delta;s.error=manager.error;
    s.ready=true;return true;
}
bool EnemyFrameState::restore(EnemyManager& manager)const{
    error.clear();if(!saved_||!saved_->ready||saved_->owner!=&manager){error="Enemy rollback crossed its scene owner";return false;}
    const auto& s=*saved_;
    // Erase/restore here changes ownership only. Calling clear()/spawn() would
    // play cleanup, drops, initial ECL, animation allocation and RNG again.
    manager.active.assign(s.owners.begin(),s.owners.end());manager.world=s.world;
    manager.random=s.game_random;manager.visual_random=s.visual_random;manager.timer=s.timer;
    manager.next_identifier=s.next_identifier;manager.rate=s.rate;manager.background_delta=s.background_delta;manager.error=s.error;
    for(size_t i=0;i<s.owners.size();++i){auto& enemy=*s.owners[i];const auto& entry=s.entries[i];
        enemy.state=entry.state;
        if(!enemy.scripts.restore(entry.scripts)){error=enemy.scripts.error;return false;}
        enemy.scripts.error=entry.script_error;enemy.error=entry.error;
        enemy.commands.host=entry.command_host;enemy.commands.spawning=entry.spawning;enemy.commands.bullets.host=entry.bullets;
        enemy.commands.lasers.host=entry.lasers;enemy.commands.lasers.scene=entry.laser_scene;enemy.commands.visuals.host=entry.visuals;
    }
    return true;
}
}
