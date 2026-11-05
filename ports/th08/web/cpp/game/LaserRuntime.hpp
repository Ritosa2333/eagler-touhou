#pragma once
#include "BulletMotion.hpp"
namespace th08 {
struct LaserActions {
    virtual ~LaserActions()=default;
    virtual void collision(const Vec2& center,const Vec2& size,const Vec3& origin,float angle,bool graze)=0;
};
class LaserRuntime {
public:
    LaserRuntime(BulletManagerState& state,Rng& rng):state(state),animation(rng){}
    FrameTiming timing;Vec3 player;LaserActions* actions=nullptr;bool invalid=false;
    LaserState* create(const BulletEmission&);
    bool update();
private:
    BulletManagerState& state;AnmExecutor animation;
    bool step(LaserState&);
};
}
