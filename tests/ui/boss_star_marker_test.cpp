// 阶段星（front script243）专用回归：原版 0x4270da-0x427193 的两段隐藏判据
// 与轨道几何（证据原件 artifacts/pr159-fix-20261104/blue-marker/）。
//   1) ratio == 0 隐藏（0x4270da fldz/fld st(1)/fucompp; test ah,0x44; jnp）。
//      全 stage 的 ECL527 只写 index0，其余槽恒 0；缺此判据时 4 个零标记叠
//      在同一点（用户截图 145616 左侧蓝团）。
//   2) 位置 = boss 世界位 + R(t)·(0,56,0) = boss + (−56·sin t, 56·cos t)；
//      调用点 0x427135 push 后 0x42713c lea 0xa0(%esp) 实际读 {0.0,56.0}。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

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

}  // namespace

TEST(boss_star_ratio_zero_stays_hidden_even_when_displayed_grows) {
    // 道中/未写 ECL527 的槽：4 个 ratio 全 0。displayed 涨到 0.5 后原版仍全隐
    //（0x4270da 第一比较与 displayed 无关）。缺 angle==0 时 4 枚星全部可见。
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
    // 对照：同帧令 index0 比例非零，判据不误伤合法标记。
    input.markers[0].ratio = 0.25f;
    CHECK(h.boss.update(input, [](i32) { return true; }));
    CHECK(shown(h.vm(h.boss.star_handles()[0])));
    CHECK(!shown(h.vm(h.boss.star_handles()[1])));
}

TEST(boss_star_position_uses_rotated_zero_x_radius_vector) {
    // ratio=0.25 → t=normalize(−π−0.25·2π)=+π/2；原版输入向量 (0,56) 经
    // R(t) 得 (−56,0)，位置 = boss+(−56,0)（旧式 boss+(0,56) 为 90° 偏差）。
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
    CHECK_NEAR(double(star->visual.translation.x),
               double(10.f - 56.f * std::sin(t)), 1e-3);
    CHECK_NEAR(double(star->visual.translation.y),
               double(128.f + 56.f * std::cos(t)), 1e-3);
    CHECK_NEAR(double(star->visual.translation.x), -46.0, 1e-3);
    CHECK_NEAR(double(star->visual.translation.y), 128.0, 1e-3);
}

int main() { return th13test::run_all("boss star markers"); }
