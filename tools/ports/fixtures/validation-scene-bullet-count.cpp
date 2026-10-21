#include "BulletState.hpp"
#include "ValidationScene.hpp"
#include <cassert>
#include <memory>

int main(){
    using namespace th08;
    static_assert(BulletManagerState::bullet_slot_count==1536);
    auto pool=std::make_unique<BulletManagerState>();
    pool->reset();
    assert(pool->bullets[1536].state==6);
    assert(pool->live_bullet_count()==0);
    ValidationSceneGate empty;
    assert(empty.begin_before_first_update(true,false,false,0,pool->live_bullet_count(),0));

    pool->bullets[0].state=1;
    assert(pool->live_bullet_count()==1);
    ValidationSceneGate active;
    assert(!active.begin_before_first_update(true,false,false,0,pool->live_bullet_count(),0));

    pool->bullets[0].state=0;
    pool->bullets[1535].state=5;
    assert(pool->live_bullet_count()==1);
    ValidationSceneGate fading;
    assert(!fading.begin_before_first_update(true,false,false,0,pool->live_bullet_count(),0));
}
