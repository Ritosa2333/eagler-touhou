#pragma once
#include "BulletMotion.hpp"
namespace th08 {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
namespace multiplayer {class PoolsJournal;}
#endif
struct LaserActions {
    virtual ~LaserActions()=default;
    virtual void collision(const Vec2& center,const Vec2& size,const Vec3& origin,float angle,bool graze)=0;
};
class LaserRuntime {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    friend class multiplayer::PoolsJournal;
#endif
public:
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    Netplay::RollbackJournal* rollback_journal=nullptr;
#endif
    LaserRuntime(BulletManagerState& state,Rng& rng):state(state),animation(rng){}
    FrameTiming timing;Vec3 player;LaserActions* actions=nullptr;bool invalid=false;
    LaserState* create(const BulletEmission&);
    bool update();
private:
    BulletManagerState& state;AnmExecutor animation;
    bool step(LaserState&);
};
}
