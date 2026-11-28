#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/BossPresentation.hpp"

#include <cmath>

using namespace th13;
using namespace th13::test;

namespace {

struct StarHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    BossPresentation boss{manager, {3, 2, 8}};
    StarHarness() {
        CHECK(load_bank(manager, 2, synthetic_anm(302, trivial_scripts(46))));
        CHECK(load_bank(manager, 3, synthetic_anm(16, trivial_scripts(244))));
    }
    AnmVm* vm(u32 handle) { return manager.registry.find(handle); }
};

bool shown(AnmVm* vm) { return vm && (vm->visual.flags & 3u) == 3u; }

}

TEST(boss_star_ratio_zero_stays_hidden_even_when_displayed_grows) {
    StarHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[0] = 0;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 60;
    input.life_initial = 1000;
    input.boss_life = 1000;
    for (int frame = 0; frame < 20; ++frame)
        CHECK(h.boss.update(input, [](i32) { return true; }));
    CHECK(h.boss.displayed_ratio() >= 0.5f);
    for (u32 handle : h.boss.star_handles()) {
        CHECK(handle != 0);
        CHECK(!shown(h.vm(handle)));
    }
    input.markers[0].ratio = 0.25f;
    CHECK(h.boss.update(input, [](i32) { return true; }));
    CHECK(shown(h.vm(h.boss.star_handles()[0])));
    CHECK(!shown(h.vm(h.boss.star_handles()[1])));
}

TEST(boss_star_position_uses_rotated_zero_x_radius_vector) {
    StarHarness h;
    CHECK(h.boss.initialize());
    StageEnemy enemy;
    enemy.position[0] = 10;
    enemy.position[1] = 128;
    BossFrameInput input;
    input.boss = &enemy;
    input.boss_slot_present = true;
    input.timer_seconds = 60;
    input.life_initial = 1000;
    input.boss_life = 1000;
    input.markers[0].ratio = 0.25f;
    for (int frame = 0; frame < 20; ++frame)
        CHECK(h.boss.update(input, [](i32) { return true; }));
    auto* star = h.vm(h.boss.star_handles()[0]);
    CHECK(star != nullptr);
    CHECK(shown(star));
    const float t = star->variables.rotation.z;
    CHECK_NEAR(double(t), 1.5707963, 1e-4);
    CHECK_NEAR(double(star->visual.translation.x), double(10.f - 56.f * std::sin(t)), 1e-3);
    CHECK_NEAR(double(star->visual.translation.y), double(128.f + 56.f * std::cos(t)), 1e-3);
    CHECK_NEAR(double(star->visual.translation.x), -46.0, 1e-3);
    CHECK_NEAR(double(star->visual.translation.y), 128.0, 1e-3);
}

int main() { return th13test::run_all("boss star markers"); }
