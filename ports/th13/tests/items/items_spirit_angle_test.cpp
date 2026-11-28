#include "../harness/test_framework.hpp"
#include "game/ItemManager.hpp"
#include <cstring>
using namespace th13;

static u32 bits_of(float value) {
    u32 bits = 0;
    std::memcpy(&bits, &value, sizeof bits);
    return bits;
}

static float tracking_angle(float player_x, float player_y, float spirit_x, float spirit_y) {
    SessionState session;
    session.rate = 1.f;
    ItemCollector collector;
    collector.progress = &session;
    collector.x = player_x;
    collector.y = player_y;
    collector.state = 1;
    GameRandom random{123};
    ItemManager manager{&random};
    manager.spawn_value_item(0, 0, 0, 0, 0.f);
    auto& spirit = manager.spirits()[0];
    spirit.state = 2;
    spirit.x = spirit_x;
    spirit.y = spirit_y;
    spirit.speed = 1.f;
    spirit.move_speed = 1.f;
    manager.update_spirits(collector);
    return spirit.angle;
}

TEST(spirit_tracking_angle_matches_native_s3_f8187_bits) {
    const float angle = tracking_angle(145.265625f, 260.6328125f, 132.00999450683594f, 226.8300018310547f);
    CHECK_EQ(bits_of(angle), 0x3f9939e3u);
}

TEST(spirit_tracking_angle_selects_double_atan2_overload) {
    const float angle = tracking_angle(0x1.1aff78p+4f, 0x1.7b861p+4f, 0.f, 0.f);
    CHECK_EQ(bits_of(angle), 0x3f6e1960u);
}

int main() { return th13test::run_all("items-spirit-angle"); }
