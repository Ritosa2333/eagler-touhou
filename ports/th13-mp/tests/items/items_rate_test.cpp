#include "../harness/test_framework.hpp"
#include "game/ItemManager.hpp"
using namespace th13;
TEST(slow_items_use_scaled_motion_gravity_and_fractional_clock) {
    SessionState session;session.rate=.5f;HudPlayerState hud;HudScoreState score;
    ItemCollector collector;collector.progress=&session;collector.player=&hud;
    collector.score=&score;collector.x=-100;collector.y=400;collector.state=2;
    ItemManager manager;manager.spawn_item(0,200,0,1,0,4);
    manager.update(collector);
    const auto& item=manager.items()[599];
    CHECK_EQ(item.x,2.f);CHECK_EQ(item.y,200.f);CHECK_EQ(item.vy,.015f);
    CHECK_EQ(item.anm_frame,0);CHECK_EQ(item.anm_time,.5f);
    manager.update(collector);
    CHECK_EQ(item.anm_frame,1);CHECK_EQ(item.anm_time,1.f);
    CHECK_EQ(item.y,float(200.f+.0075f));
}
TEST(slow_spirits_keep_fractional_lifetime_and_original_unscaled_acceleration) {
    SessionState session;session.rate=.25f;HudPlayerState hud;HudScoreState score;
    ItemCollector collector;collector.progress=&session;collector.player=&hud;
    collector.score=&score;collector.x=-100;collector.y=400;collector.state=2;
    GameRandom random{123};ItemManager manager{&random};
    manager.bind_session(session);manager.spawn_value_item(0,0,100,0,.5f);
    auto& spirit=manager.spirits()[0];spirit.angle=0;spirit.move_speed=4;
    manager.update_spirits(collector);
    CHECK_EQ(spirit.move_speed,float(4.f-.12666667f));
    CHECK_EQ(spirit.x,.96f);CHECK_EQ(spirit.y,100.f);
    CHECK_EQ(spirit.lifetime_clock.fractional,.25f);
    CHECK_EQ(spirit.anm_time,.25f);CHECK_EQ(spirit.anm_frame,0);
    for(int i=0;i<3;++i)manager.update_spirits(collector);
    CHECK_EQ(spirit.lifetime_clock.current,1);CHECK_EQ(spirit.lifetime_clock.fractional,1.f);
    CHECK_EQ(spirit.anm_frame,1);
}
TEST(cancel_item_activation_does_not_tick_or_move_until_the_next_update) {
    SessionState session;session.rate=.5f;
    ItemCollector collector;collector.progress=&session;collector.state=2;collector.y=400;
    ItemManager manager;manager.spawn_item(0,200,0,9,-1.5707964f,2);
    auto& item=manager.items().back();
    CHECK_EQ(item.state,5);CHECK_EQ(item.sprite,0);
    manager.update(collector);
    CHECK_EQ(item.state,2);CHECK_EQ(item.anm_frame_prev,-1);
    CHECK_EQ(item.anm_time,0.f);CHECK_EQ(item.y,200.f);
    manager.update(collector);
    CHECK_EQ(item.anm_frame_prev,0);CHECK_EQ(item.anm_time,.5f);
    CHECK_EQ(item.y,199.f);
}
int main(){return th13test::run_all("items-rate");}
