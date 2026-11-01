// 妖梦 Bomb 斩击池定向断言（native 单 TU，tests/bomb/run.sh 编译执行）。
// 期望值独立复算（本文件内镜像原式 + artifacts/verification/r4/bomb/gen_expected.py
// 的 python 字面量双保险，绝对值容差 1e-4 防 libm 差异，相对式逐位比较）。
//
// 生成节奏依据（oracle battle-bomb-trace，th13_udN01 stage2 行 9842..9899）：
//   0x40cb50 的生成门是 `cmp dword ptr [edi+0xd9c],0 / jg 0x40cd45`：生成尾部
//   以 +0xd94（初值 0x19=25，>8 时每次减 2）装入 +0xd9c 作为倒计时，逐帧先递减
//   再判。实测生成帧 9842 → 9865（间隔 23）→ 9886（间隔 21），倒计时读数
//   22,21,20,...；因此 261 帧生成期内不再是「每帧一斩」，而是 0,23,44,63,80,...
//   （前四间隔 23/21/19，+0xd94 降到 <=8 后固定 7 帧步进）。
// 断言清单：
//   A1 生成帧：age0（槽 0，script19）、age23（槽 1，script20）、age44（槽 2）、
//      age63（槽 3）；跳帧无生成。
//   A2 两随机顺序：生成帧恰一次 signed_unit（GameRandom index +2），跳帧不抽；
//      角度随机在 VM 侧（ins_119），本模块不抽第二次。
//   A3 奇偶矩形：偶槽 center=(−dx, y−dy)、奇槽 (+dx, y+dy)；y/lane/jitter 与
//      python 期望逐位一致；h=rect_scale×128。
//   A4 伤害：每覆盖斩数 ×30；同矩形每帧一次；两斩重叠点计 2。
//   A5 局部消弹：矩形内且场内才清；矩形覆盖但场外不清；屏内未覆盖不清（非全屏）。
//   A6 261 终止：age>=261 无生成且不抽随机；261 帧内精确 27 斩，生成帧为
//      固定完整独立期望序列 0,23,44,63,80,95,...,254（前四间隔 23/21/19，
//      +0xd94 降到 <=8 后 7 帧步进），RNG index 终值 54。
//   A7 生命周期：生成帧矩形 h=0；scale 每活帧 +1；VM 死后 scale 冻结、槽失活。
#include "game/YoumuBomb.hpp"

#include <cmath>
#include <cstdio>
#include <cstdint>
#include <vector>

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

// 推进到下一次生成；*age 为下一次 step 的实参（返回后已被推进到生成帧之后）。
static std::optional<YoumuBomb::Spawn> next_spawn(YoumuBomb& bomb, GameRandom& rng,
                                                  int* age, int limit = 300) {
    while (*age < limit) {
        const int a = *age;
        auto sp = bomb.step(a, rng);
        ++*age;
        if (sp) return sp;
    }
    return std::nullopt;
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

    // —— A1/A2（前四个生成帧精查：轨迹 9842/9865/9886 的间隔 23/21，第四个 19） ——
    // gen_expected.py（seed=1）：y0=194.32656860351562 y1=217.71697998046875
    //   y2=321.91763305664062 y3=224.24783325195312
    const float exp_y[4] = {194.32656860351562f, 217.71697998046875f,
                            321.91763305664062f, 224.24783325195312f};
    const int exp_age[4] = {0, 23, 44, 63};
    const int exp_su_draw[4] = {2, 4, 6, 8};  // 每次生成 index +2
    int age = 0;
    for (int i = 0; i < 4; ++i) {
        const uint32_t idx0 = rng.index();
        const auto sp = next_spawn(bomb, rng, &age);
        CHECK(sp.has_value(), "生成期前四斩存在");
        if (!sp) continue;
        CHECK(age - 1 == exp_age[i], "生成帧符合倒计时节奏");
        CHECK(sp->slot == i, "槽号 = 生成序（前 60）");
        CHECK(sp->script == ((i & 1) ? 20 : 19), "script 19 偶/20 奇");
        CHECK(feq(sp->y, exp_y[i]), "y 与 python 期望逐位一致");
        CHECK(near4(sp->y, exp_y[i]), "y python 字面量双保险");
        CHECK(rng.index() - idx0 == 2, "每生成帧恰一次 signed_unit");
        CHECK(bomb.cursor() == i + 1, "cursor 递增");
        CHECK(int(rng.index()) == exp_su_draw[i], "随机顺序（无额外抽取）");
    }
    // 跳帧不抽随机：生成后的一帧必然是倒计时帧
    {
        const uint32_t before = rng.index();
        const auto sp = bomb.step(age, rng);
        CHECK(!sp.has_value(), "倒计时帧无生成");
        CHECK(rng.index() == before, "倒计时帧不抽随机");
        ++age;
    }
    // 前两槽以已知角度回填 VM（帧内第二次交互，模块此时不得再抽随机）
    const float a0 = -0.3f, a1 = 1.1f;
    const uint32_t idx_before_vm = rng.index();
    bomb.set_vm(0, true, a0);
    bomb.set_vm(1, true, a1);
    CHECK(rng.index() == idx_before_vm, "set_vm 不抽随机（角度随机属 VM 侧）");

    // A3：生成帧矩形 h=0（scale tick 前 0）；再 tick 一次 → rs=1
    bomb.set_vm(0, true, a0);
    bomb.set_vm(1, true, a1);
    CHECK(bomb.rect_scale(0) == 1.0f && bomb.scale(0) == 2.0f, "第二次 tick 后 rs=1");
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
    // 构造两斩重叠：slot0（偶）tick 4 次 → rs=3/h=384；slot2（偶，第三个生成）
    // tick 2 次 → rs=1/h=128，同角 0：center0=(0, y0−256) 跨 y[−253.7,130.3]；
    // center2=(0, y2−256) 跨 y[1.9,129.9] → 探针 (0,60) 双覆盖。
    bomb.reset();
    GameRandom rng2(1);
    int age2 = 0;
    (void)next_spawn(bomb, rng2, &age2);            // slot0 @0
    bomb.set_vm(0, true, 0.0f);
    bomb.set_vm(0, true, 0.0f);
    bomb.set_vm(0, true, 0.0f);
    bomb.set_vm(0, true, 0.0f);                     // rs=3
    (void)next_spawn(bomb, rng2, &age2);            // slot1 @23
    bomb.set_vm(1, false, 0.0f);
    (void)next_spawn(bomb, rng2, &age2);            // slot2 @44
    bomb.set_vm(2, true, 0.0f);
    bomb.set_vm(2, true, 0.0f);                     // rs=1
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

    // —— A6 261 终止 + 生成计数 ——
    bomb.reset();
    GameRandom rng3(1);
    std::vector<int> spawn_ages;
    for (int a = 0; a <= 260; ++a) {
        const auto sp = bomb.step(a, rng3);
        if (sp) spawn_ages.push_back(a);
    }
    // 独立期望序列（0x40cb50 倒计时 + 0x4457e0 装入 25，>8 每次 -2）
    const std::vector<int> expected_ages = {
        0, 23, 44, 63, 80, 95, 108, 119, 128, 135, 142, 149, 156, 163,
        170, 177, 184, 191, 198, 205, 212, 219, 226, 233, 240, 247, 254};
    CHECK(int(spawn_ages.size()) == 27, "生成期恰 27 斩");
    CHECK(spawn_ages == expected_ages, "生成帧完整独立期望序列");
    CHECK(spawn_ages.size() >= 4 && spawn_ages[0] == 0 && spawn_ages[1] == 23 &&
          spawn_ages[2] == 44 && spawn_ages[3] == 63, "前四生成帧 0/23/44/63");
    CHECK(int(rng3.index()) == int(spawn_ages.size()) * 2, "抽随机次数 = 生成数");
    CHECK(int(rng3.index()) == 54, "RNG index 终值 54");
    const uint32_t idx_end = rng3.index();
    CHECK(!bomb.step(261, rng3).has_value(), "age 261 无生成");
    CHECK(!bomb.step(300, rng3).has_value(), "age 300 无生成");
    CHECK(rng3.index() == idx_end, "终止后不抽随机");

    // —— A7 生命周期：首个斩生于 0，VM 活 16 帧，死于 age16 ——
    bomb.reset();
    GameRandom rng4(1);
    std::vector<int> born;  // born[i] = 第 i 条斩的生成 age（槽号 = i）
    for (int a = 0; a <= 300; ++a) {
        const auto sp = bomb.step(a, rng4);
        if (sp && sp->slot >= 0) born.push_back(a);
        for (int s = 0; s < YoumuBomb::kSlots; ++s) {
            const bool vm_alive =
                s < int(born.size()) && a - born[size_t(s)] < 16;
            bomb.set_vm(s, vm_alive, 0.0f);
        }
        if (a == 0) {
            const auto rc = bomb.rect(0);
            CHECK(bomb.cover_count(rc.cx, rc.cy, 0.0f) >= 1, "首斩中心被自身覆盖");
        }
        if (a == 0) CHECK(bomb.has_anm(0), "帧 0 生成首斩");
        if (a == 16) {
            CHECK(!bomb.active(0), "16 帧后 VM 死");
            CHECK(feq(bomb.scale(0), 16.0f), "死时 scale=16（冻结）");
        }
        if (a == 17) CHECK(feq(bomb.scale(0), 16.0f), "死后 scale 冻结");
    }
    CHECK(bomb.cover_count(0.0f, 0.0f, 8.0f) == 0, "终态无覆盖");

    // 40ceac uses the global-rate pointer for the slot's scale timer.
    bomb.reset();
    (void)bomb.step(0, rng4);
    bomb.set_vm(0, true, 0, .5f);
    CHECK(bomb.rect_scale(0) == 0 && bomb.scale(0) == .5f, "half-rate first tick");
    bomb.set_vm(0, true, 0, .5f);
    CHECK(bomb.rect_scale(0) == .5f && bomb.scale(0) == 1.f, "half-rate second tick");

    if (failures == 0) std::printf("youmu_bomb_test: ALL PASS\n");
    return failures == 0 ? 0 : 1;
}
