// s5-drift-20261028 §2 位级验收：wrap_angle 迭代上限（0x472e90/0x472f30
// 正方向 34 次减法、比较在落盘之后）与 emit_et 出生速度向量用 raw 角
// （0x40e840 尾 0x412510(node+0xb7c, local_8, speed)）；
// fp-audit-20261028 §4/§6：0x411770 重生消弹半步三轴同式三级 f32 链
// （0x4118cf..0x41193c）。全部期望值为零售 unicorn 实测的逐位复现。
#include "../harness/test_framework.hpp"
#include "game/StageMath.hpp"
#include "game/StageSim.hpp"
using namespace th13;
u32 word(float f){u32 w;std::memcpy(&w,&f,4);return w;}

TEST(wrap_s5_raw_two_calls_each_34_subtractions) {
    // raw 合成角 f32=780.3973388671875（angle1 779.20556640625 + aim
    // 1.191794514656067，mode0 链）；零售 node+0xb8c 实测 =
    // 353.14031982421875（两条 wrap 调用各 34 次减法链，s5-drift §2.3）。
    // 33 次链的旧生产值 = 365.7066650390625（恰差 2×2π）。
    CHECK_EQ(word(wrap_angle(780.3973388671875f)), 0x440db12eu);
    CHECK_EQ(word(wrap_angle(wrap_angle(780.3973388671875f))), 0x43b091f6u);
}

TEST(wrap_34th_subtraction_lands_before_exit) {
    // 0x472e90/0x472f30：cmp eax,0x20; jg 位于第 34 次减法落盘之后。
    // 212.0 需恰好 34 次减法才入界：34 次后 = -1.6283559799194336；
    // 旧代码（减前 break，33 次）给 4.654829502105713（越界值）。
    CHECK_EQ(word(wrap_angle(212.0f)), 0xbfd06df8u);
}

TEST(wrap_sub33_and_negative_regions_unchanged) {
    // 回归锚：≤33 次减法可入界的角度与负方向 do-while（同为 34 上限）
    // 不受本次修复影响（负方向循环原本已对齐零售，勿动）。
    CHECK_EQ(word(wrap_angle(208.9f)), 0x3fc70478u);
    CHECK_EQ(word(wrap_angle(207.0f)), 0xbeb0b9e0u);
    CHECK_EQ(word(wrap_angle(-780.3973388671875f)), 0xc40db12eu);
}

TEST(wrap_identity_within_pi) {
    // |angle| ≤ π 时两条修复必须零变化：wrap 恒等（位级）且 raw==stored，
    // 出生速度向量与旧代码逐位相同。
    const float pi_f = 3.1415927f;
    const float cases[] = {0.0f, 1.0f, -1.0f, 0.5f, -2.9f, 3.0f, pi_f, -pi_f};
    for (float v : cases) CHECK_EQ(word(wrap_angle(v)), word(v));
}

TEST(emit_et_birth_velocity_uses_raw_angle) {
    // 0x40e840 s5 实测场景：mode0、way=count=1、speed1=0.1、
    // angle1=779.20556640625、aim=1.191794514656067。
    // 零售出生 vel = f32(polar(raw, 0.1)) = (0.028445275500416756,
    // 0.09586900472640991)（fsincos 内部精确归约，s5-drift §2.3）；
    // 弹体 angle 字段 = wrap 后 stored = 353.14031982421875。
    StageSim sim; sim.interactive = true;
    StageEt et; et.reset();
    et.angle1 = 779.20556640625f;
    et.speed1 = 0.1f;
    const float spawn[3] = {-100.64227294921875f, 162.38999938964844f, 0.0f};
    sim.emit_et(et, 1, 0, spawn, 1.191794514656067f);
    CHECK(sim.error().empty());
    CHECK_EQ(sim.bullets.size(), 1u);
    if (sim.bullets.size() != 1) return;
    CHECK_EQ(word(sim.bullets[0].angle), 0x43b091f6u);
    CHECK_EQ(word(sim.bullets[0].vel[0]), 0x3ce90611u);
    CHECK_EQ(word(sim.bullets[0].vel[1]), 0x3dc456f8u);
}

TEST(emit_et_raw_birth_vel_settles_bit_exact_under_op4_steer) {
    // 同一弹带 etEx op4 steer（step=0.008333333767950535、时长 120、
    // target<=-990 → 弹体 angle=stored）：零售 settled vel =
    // (0.31329917907714844, 1.0544403791427612)、speed =
    // 1.1000003814697266（0x4102b0 逐帧行为，s5-drift §2.3 实测复现）。
    StageSim sim; sim.interactive = true;
    StageEt et; et.reset();
    et.angle1 = 779.20556640625f;
    et.speed1 = 0.1f;
    et.ex[0].op = 4;
    et.ex[0].args[0] = 0.008333333767950535f;
    et.ex[0].args[1] = -999999.0f;
    et.ex[0].args[4] = 120.0f;
    const float spawn[3] = {-100.64227294921875f, 162.38999938964844f, 0.0f};
    sim.emit_et(et, 1, 0, spawn, 1.191794514656067f);
    CHECK(sim.error().empty());
    for (int f = 0; f < 120; ++f) {
        sim.advance_bullets();
        CHECK(sim.error().empty());
    }
    CHECK_EQ(sim.bullets.size(), 1u);
    if (sim.bullets.size() != 1) return;
    CHECK_EQ(word(sim.bullets[0].vel[0]), 0x3ea068c0u);
    CHECK_EQ(word(sim.bullets[0].vel[1]), 0x3f86f7e7u);
    CHECK_EQ(word(sim.bullets[0].speed), 0x3f8cccd0u);
}

TEST(respawn_cancel_half_step_uses_same_f32_chain_on_all_axes) {
    // 0x411770 半步（0x4118cf..0x41193c）：t=f32(vel·rate) → h=f32(t·0.5)
    // → pos=f32(pos+h)，三轴同式。x 轴的 double 单舍入旧式在 rate≠1 时
    // 差 1ulp（本例 0x3fa851eb vs 0x3fa851ec）；rate=1.0 时数值恒等
    //（fp-audit §4 D2）。
    StageSim sim;
    sim.set_time_rate(0.9f);
    StageBullet b;
    b.x = 1.0f; b.y = 10.0f; b.z = -3.0f;
    b.vel[0] = 0.7f; b.vel[1] = 1.1f; b.vel[2] = 2.35f;
    b.state = 1;
    sim.bullets.push_back(b);
    CHECK(sim.cancel_respawn_bullets({1.0f, 10.0f, -3.0f}, 0.0f));
    CHECK(sim.error().empty());
    CHECK_EQ(sim.bullets.size(), 1u);
    if (sim.bullets.size() != 1) return;
    CHECK_EQ(word(sim.bullets[0].x), 0x3fa851ecu);
    CHECK_EQ(word(sim.bullets[0].y), 0x4127eb85u);
    CHECK_EQ(word(sim.bullets[0].z), 0xbff8a3d8u);
    CHECK_EQ(sim.bullets[0].state, 4u);
}

TEST(respawn_cancel_half_step_rate_one_unchanged) {
    // rate=1.0 时 x 新旧两式数值恒等（回归锚：常规消弹窗口零变化）。
    StageSim sim;
    StageBullet b;
    b.x = 1.0f; b.y = 10.0f; b.z = -3.0f;
    b.vel[0] = 0.7f; b.vel[1] = 1.1f; b.vel[2] = 2.35f;
    b.state = 1;
    sim.bullets.push_back(b);
    CHECK(sim.cancel_respawn_bullets({1.0f, 10.0f, -3.0f}, 0.0f));
    CHECK(sim.error().empty());
    // t=f32(vel*1)=vel（精确）；h=f32(vel*0.5) 精确折半；pos+h 单次舍入。
    CHECK_EQ(word(sim.bullets[0].x), word(1.0f + 0.35f));
    CHECK_EQ(word(sim.bullets[0].y), word(10.0f + 0.55f));
    CHECK_EQ(word(sim.bullets[0].z), word(-3.0f + 1.175f));
}
int main(){return th13test::run_all("wrap-angle/emit-et retail bits");}
