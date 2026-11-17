#include "../harness/test_framework.hpp"
#include "game/ItemManager.hpp"
using namespace th13;
TEST(original_pools_descend_independently_and_cancel_birth_is_not_clamped) {
    ItemManager manager;
    manager.spawn_item(210,100,.1f,9,-1.5707964f,2.2f);
    manager.spawn_item(-210,100,.1f,9,-1.5707964f,2.2f);
    manager.spawn_item(210,100,.1f,1,-1.5707964f,2.2f);
    const auto& items=manager.items();
    CHECK(items[2647].alive);CHECK(items[2646].alive);CHECK(items[599].alive);
    CHECK_EQ(items[2647].x,210.f);CHECK_EQ(items[2646].x,-210.f);
    CHECK_EQ(items[599].x,192.f);
    CHECK_EQ(items[2647].sprite,0);CHECK_EQ(items[2646].sprite,0);
}
TEST(ordinary_pool_exhaustion_does_not_borrow_cancel_slots) {
    ItemManager manager;
    for(int n=0;n<601;++n)manager.spawn_item(0,200,0,1,0,0);
    size_t count=0;for(const auto&i:manager.items())count+=i.alive;
    CHECK_EQ(count,size_t(600));manager.spawn_item(0,200,0,9,0,0);
    CHECK(manager.items()[2647].alive);
}
TEST(retired_slots_return_to_their_original_free_list_head) {
    ItemManager manager;SessionState progress;HudPlayerState player;HudScoreState score;
    ItemCollector collector;collector.state=1;collector.x=0;collector.y=200;
    collector.box={-10,190,10,210};collector.progress=&progress;collector.player=&player;collector.score=&score;
    manager.spawn_item(0,200,0,1,0,0);manager.update(collector);
    CHECK(!manager.items()[599].alive);
    manager.spawn_item(0,400,0,2,0,0);CHECK(manager.items()[599].alive);
    manager.spawn_item(0,200,0,9,0,0);manager.update(collector);manager.update(collector);
    CHECK(!manager.items()[2647].alive);
    manager.spawn_item(0,400,0,9,0,0);CHECK(manager.items()[2647].alive);
}
int main(){return th13test::run_all("item-pools");}
