#pragma once
#include "BulletMotion.hpp"
namespace th08 {
struct BulletUpdateActions {
    virtual ~BulletUpdateActions()=default;
    // 0: spawn/barrier 00449ff0, 1: graze 0044a470, 2: hit 0044a230.
    virtual i32 collision(i32 kind,BulletState& bullet)=0;
    virtual void item(const Vec3& position,i32 type)=0;
};
class BulletUpdate {
public:
    BulletUpdate(BulletManagerState& state,BulletCreation& creation,Rng& rng):state(state),creation(creation),animation(rng){}
    FrameTiming timing;Vec3 player;bool paused=false;
    i32 cancel_item=-1;BulletUpdateActions* actions=nullptr;
    const i32* live_cancel_item=nullptr;
    // Bullet phase of 00431240. Item and laser phases are separate systems.
    bool update_bullets();
private:
    BulletManagerState& state;BulletCreation& creation;AnmExecutor animation;
    void cancel_reward(BulletState&);
    bool normal(BulletState&,BulletMotion&);
    i32 collision(i32 kind,BulletState& b){return actions?actions->collision(kind,b):0;}
};
}
