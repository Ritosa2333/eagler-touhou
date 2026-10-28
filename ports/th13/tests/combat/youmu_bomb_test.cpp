// 妖梦 Bomb 斩击池定向断言（native 单 TU，tests/bomb/run.sh 编译执行）。
// 期望值独立复算（本文件内镜像原式 + artifacts/verification/r4/bomb/gen_expected.py
// 的 python 字面量双保险，绝对值容差 1e-4 防 libm 差异，相对式逐位比较）。
// 断言清单（对应 r4/bomb/HANDOFF.md）：
//   A1 槽循环：261 帧生成期，帧 0..59 → 槽 0..59；帧 60..260 → slot=-1（仅视觉）；
//      age>=261 无生成且不抽随机。
//   A2 两随机顺序：每生成帧恰一次 signed_unit（GameRandom index +2）；角度随机在
//      VM 侧（ins_119），本模块不抽第二次。
//   A3 奇偶矩形：偶槽 center=(−dx, y−dy)、奇槽 (+dx, y+dy)；y/lane/jitter 与
//      python 期望逐位一致；h=rect_scale×128。
//   A4 伤害：每覆盖斩数 ×30；同矩形每帧一次；两斩重叠点计 2。
//   A5 局部消弹：矩形内且场内才清；矩形覆盖但场外不清；屏内未覆盖不清（非全屏）。
//   A6 261 终止：age 261 无生成；最后战斗斩（帧 59 生、16 帧寿）死于帧 74。
//   A7 生命周期：生成帧矩形 h=0；scale 每活帧 +1；VM 死后 scale 冻结、槽失活。
#include "game/YoumuBomb.hpp"

#include <cmath>
#include <cstdio>
#include <cstdint>

using th13::YoumuBomb;
using th13::GameRandom;

static int failures = 0;
#define CHECK(cond, msg)                                          \
    do {                                                          \
        if (!(cond)) {                                            \
            std::printf("FAIL %s:%d %s\n", __FILE__, __LINE__, msg); \
            ++failures;                                           \
        }                                                         \
    } while (0)

static bool feq(float a, float b) { return a == b; }  // 逐位（同式复算）
static bool near4(float a, float b) { return std::fabs(double(a) - double(b)) < 1e-4; }

// 测试内独立复算（与 YoumuBomb.cpp 同式；python 字面量另配 near4 双保险）
static void mirror_rect(float y, bool odd, float angle, float rs,
                        float* cx, float* cy, float* h) {
    const float ap = float(double(angle) + 1.5707963705062866);
    const float dx = float(std::cos(double(ap)) * 256.0);
    const float dy = float(std::sin(double(ap)) * 256.0);
    *cx = odd ? dx : float(0.0 - double(dx));
    *cy = odd ? float(double(y) + double(dy)) : float(double(y) - double(dy));
    *h = float(double(rs) * 128.0);
}

int main() {
    // —— 常量（0x40cb50/0x4457e0/0x4bba54） ——
    CHECK(YoumuBomb::kSlots == 60, "kSlots=60");
    CHECK(YoumuBomb::kActiveFrames == 261, "261 active frames");
    CHECK(YoumuBomb::kDamagePerRect == 30.0f, "damage 30");
    CHECK(YoumuBomb::kRectLength == 96.0f, "rect length 96");
    static_assert(sizeof(YoumuBomb::kLanes) == 11 * sizeof(float), "11 lanes");
    CHECK(YoumuBomb::kLanes[0] == 224.0f && YoumuBomb::kLanes[4] == 64.0f &&
          YoumuBomb::kLanes[10] == 224.0f, "lane table");

    YoumuBomb bomb;
    bomb.reset();
    GameRandom rng(1);

    // —— A1/A2/A3（前 4 帧精查） ——
    // gen_expected.py（seed=1）：y0=194.32656860351562 y1=217.71697998046875
    //   y2=321.91763305664062 y3=224.24783325195312
    const float exp_y[4] = {194.32656860351562f, 217.71697998046875f,
                            321.91763305664062f, 224.24783325195312f};
    const int exp_su_draw[4] = {2, 4, 6, 8};  // 每次生成 index +2
    for (int age = 0; age < 4; ++age) {
        const uint32_t idx0 = rng.index();
        const auto sp = bomb.step(age, rng);
        CHECK(sp.has_value(), "spawn each frame");
        CHECK(sp->slot == age, "slot == cursor (前 60)");
        CHECK(sp->script == ((age & 1) ? 20 : 19), "script 19 偶/20 奇");
        CHECK(feq(sp->y, exp_y[age]), "y 与 python 期望逐位一致");
        CHECK(near4(sp->y, exp_y[age]), "y python 字面量双保险");
        CHECK(rng.index() - idx0 == 2, "每生成帧恰一次 signed_unit");
        CHECK(bomb.cursor() == age + 1, "cursor 递增");
        CHECK(int(rng.index()) == exp_su_draw[age], "随机顺序（无额外抽取）");
    }
    // 前两槽以已知角度回填 VM（帧内第二次交互，模块此时不得再抽随机）
    const float a0 = -0.3f, a1 = 1.1f;
    const uint32_t idx_before_vm = rng.index();
    bomb.set_vm(0, true, a0);
    bomb.set_vm(1, true, a1);
    CHECK(rng.index() == idx_before_vm, "set_vm 不抽随机（角度随机属 VM 侧）");

    // A3：生成帧矩形 h=0（scale tick 前 0）
    CHECK(feq(bomb.rect(0).h, 0.0f), "生成帧 h=0");
    CHECK(bomb.rect_scale(0) == 0.0f && bomb.scale(0) == 1.0f, "tick 后 scale=1");

    // 帧 1：两槽再活一帧（rect_scale=1），核对奇偶矩形
    (void)bomb.step(4, rng);
    bomb.set_vm(0, true, a0);
    bomb.set_vm(1, true, a1);
    float m0x, m0y, m0h, m1x, m1y, m1h;
    mirror_rect(exp_y[0], false, a0, 1, &m0x, &m0y, &m0h);
    mirror_rect(exp_y[1], true, a1, 1, &m1x, &m1y, &m1h);
    const auto r0 = bomb.rect(0);
    const auto r1 = bomb.rect(1);
    CHECK(feq(r0.cx, m0x) && feq(r0.cy, m0y) && feq(r0.h, m0h), "偶槽矩形逐位");
    CHECK(feq(r1.cx, m1x) && feq(r1.cy, m1y) && feq(r1.h, m1h), "奇槽矩形逐位");
    // gen_expected.py 字面量：slot0 rs=1 cx=-75.65315246582031 cy=-50.23957824707031
    CHECK(near4(r0.cx, -75.65315246582031f) && near4(r0.cy, -50.23957824707031f) &&
          near4(r0.h, 128.0f), "偶槽 python 字面量");
    CHECK(near4(r1.cx, -228.1490936279297f) && near4(r1.cy, 333.83758544921875f),
          "奇槽 python 字面量");

    // —— A4 伤害（cover_count × 30，单矩形单帧一次，重叠计 2） ——
    CHECK(bomb.cover_count(r0.cx, r0.cy, 0.0f) >= 1, "中心被自身覆盖");
    // 构造两斩重叠：slot0（偶，活 0..3 → 帧 3 时 rs=3/h=384）与 slot2（偶，生于 2 →
    // 帧 3 时 rs=1/h=128），同角 0：center0=(0, y0−256)=(0,−61.67) 跨 y[−253.7,130.3]；
    // center2=(0, y2−256)=(0,65.92) 跨 y[1.9,129.9] → 探针 (0,60) 双覆盖。
    bomb.reset();
    GameRandom rng2(1);
    for (int age = 0; age <= 3; ++age) {
        (void)bomb.step(age, rng2);
        bomb.set_vm(0, true, 0.0f);
        bomb.set_vm(1, false, 0.0f);
        bomb.set_vm(2, true, 0.0f);  // 未生成帧（0,1）按原版 no-op
    }
    const auto q0 = bomb.rect(0), q2 = bomb.rect(2);
    CHECK(feq(q0.h, 384.0f) && feq(q2.h, 128.0f), "重叠构造 rs 3/1");
    CHECK(bomb.cover_count(0.0f, 60.0f, 0.0f) == 2, "两斩重叠计 2");
    CHECK(bomb.cover_count(0.0f, 60.0f, 0.0f) * YoumuBomb::kDamagePerRect == 60.0f,
          "两斩重叠伤害 60");
    CHECK(bomb.cover_count(1000.0f, 1000.0f, 0.0f) == 0, "远处无伤害");

    // —— A5 局部消弹（非全屏） ——
    CHECK(bomb.clears_bullet(0.0f, 60.0f, 0.0f), "矩形内场内 → 清");
    CHECK(!bomb.clears_bullet(0.0f, -200.0f, 0.0f), "矩形覆盖但场外 → 不清");
    CHECK(!bomb.clears_bullet(300.0f, 60.0f, 0.0f), "场内未覆盖 → 不清（非全屏）");

    // —— A6/A7 生命周期与 261 终止 ——
    bomb.reset();
    GameRandom rng3(1);
    for (int age = 0; age <= 60; ++age) {
        const auto sp = bomb.step(age, rng3);
        CHECK(sp.has_value(), "0..260 每帧生成");
        if (age < 60) {
            CHECK(sp->slot == age, "槽 0..59");
        } else {
            CHECK(sp->slot == -1, "60+ 仅视觉");
        }
    }
    // 全程模拟：VM 活 16 帧；斩生于帧 A，活 A..A+15，死于 A+16
    bomb.reset();
    GameRandom rng4(1);
    const int born = 59;  // 最后战斗斩
    int cover_at = -1;
    for (int age = 0; age <= 300; ++age) {
        (void)bomb.step(age, rng4);
        for (int s = 0; s < YoumuBomb::kSlots; ++s) {
            const bool vm_alive = s <= age && age - s < 16;
            bomb.set_vm(s, vm_alive, 0.0f);
            if (vm_alive) {
                const float rs = bomb.rect_scale(s);
                CHECK(feq(rs, float(age - s)), "rect_scale = 存活帧数");
            }
        }
        if (bomb.cover_count(0.0f, 0.0f, 8.0f) > 0) cover_at = age;
        if (age == born) CHECK(bomb.has_anm(born), "帧 59 生成战斗斩");
        if (age == born + 16) {
            CHECK(!bomb.active(born), "16 帧后 VM 死");
            CHECK(feq(bomb.scale(born), 16.0f), "死时 scale=16（冻结）");
        }
        if (age == born + 17) CHECK(feq(bomb.scale(born), 16.0f), "死后 scale 冻结");
    }
    CHECK(bomb.cover_count(0.0f, 0.0f, 8.0f) == 0, "终态无覆盖");
    CHECK(cover_at <= born + 15 + 1, "战斗覆盖止于 ~帧 75");
    const uint32_t idx_end = rng4.index();
    CHECK(!bomb.step(261, rng4).has_value(), "age 261 无生成");
    CHECK(!bomb.step(300, rng4).has_value(), "age 300 无生成");
    CHECK(rng4.index() == idx_end, "终止后不抽随机");

    if (failures == 0) std::printf("youmu_bomb_test: ALL PASS\n");
    return failures == 0 ? 0 : 1;
}
