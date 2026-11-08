#include "../harness/test_framework.hpp"
#include "game/ItemManager.hpp"

using namespace th13;

namespace {
struct PointFixture {
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    ItemCollector collector;
    GameRandom random{60189};
    ItemManager manager{&random};
    PointFixture() {
        progress.difficulty = 1;
        progress.max_point = 1000000;
        progress.max_point_cap = 25000000;
        collector.x = 0;
        collector.y = 238.0546875f;
        collector.state = 1;
        collector.box = {-15, collector.y - 15, 15, collector.y + 15};
        collector.progress = &progress;
        collector.player = &player;
        collector.score = &score;
    }
    void spirit(u32 flags = 0) {
        manager.spawn_value_item(1, 0, collector.y, 0, 0.5f);
        for (auto& spirit : manager.spirits()) {
            if (!spirit.alive) continue;
            spirit.x = 0;
            spirit.y = collector.y;
            spirit.flags = flags;
        }
        manager.update_spirits(collector);
    }
    void item(i32 type) {
        manager.spawn_item(0, collector.y, 0, type, 0, 0);
        for (auto& item : manager.items()) {
            if (!item.alive) continue;
            item.state = 1;
            item.vx = item.vy = 0;
        }
        manager.update(collector);
    }
};
}

TEST(ordinary_point_spirit_adds_ten_displayed_points_as_one_thousand_raw) {
    PointFixture fixture;
    fixture.spirit();
    CHECK_EQ(fixture.progress.max_point, 1001000);
    fixture.spirit();
    fixture.spirit();
    CHECK_EQ(fixture.progress.max_point, 1003000);
    CHECK_EQ(fixture.score.score, 0);
    fixture.item(ItemPoint);
    // Retail f409: 00438bc3 supplied 10.0 three times; 0042eaf2 then
    // supplied 5680 to 00413bf0. Point collection does not increment max_point.
    CHECK_EQ(fixture.progress.max_point, 1003000);
    CHECK_EQ(fixture.score.score, 568);
}

TEST(trance_spirit_and_full_power_variant_use_one_hundred_displayed_points) {
    PointFixture fixture;
    fixture.spirit(1);
    CHECK_EQ(fixture.progress.max_point, 1010000);
    fixture.player.power = fixture.player.max_power;
    fixture.item(ItemBigPowerAlt);
    CHECK_EQ(fixture.progress.max_point, 1020000);
}

TEST(cancel_item_uses_tenth_displayed_point_and_all_increments_cap) {
    PointFixture fixture;
    fixture.item(ItemSpiritDrop);
    CHECK_EQ(fixture.progress.max_point, 1000010);
    CHECK_EQ(fixture.score.score, 1);
    fixture.progress.max_point = fixture.progress.max_point_cap - 1;
    fixture.spirit();
    CHECK_EQ(fixture.progress.max_point, fixture.progress.max_point_cap);
}

int main() { return th13test::run_all("original maximum point increments"); }
