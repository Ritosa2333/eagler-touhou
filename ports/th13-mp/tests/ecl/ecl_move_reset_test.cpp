// moveReset（ECL op 427）原生语义回归。
// 证据：0x41ba18 反汇编（op427 经 0x41f9d8[427-300=127]=34 → 0x41f760[34] 分派）：
//   - 41ba18-41ba48：vel 子位置 += pos 子位置（flds 0xcc/fadds 0x88/fstps 0x88）
//   - 41ba56/41ba61/41ba79：pos 子位置清零
//   - 41ba73/41ba6d：vel/pos 速度清零（fstps 0xa0 / fsts 0xe4）
//   - 41ba7f-41bac4：两 mover 的 aux（0xbc..0xc4 / 0x100..0x108）清零
//   - 41baca/41bad0：两 mover 的 mode 仅清低 4 位（and 0xfffffff0）
//   - 41bad8/41bade/41bae4/41baea/41baf0/41baf6：postime/polar/radial 的
//     duration 门（0x340/0x3a8/0x3fc/0x44c/0x49c/0x4e0）清零
//   - 角度 0xa4/0xe8、半径 0xa8/0xec、成长 0xac/0xf0、椭圆字段均无写入：
//     原版保留它们。S4 f11261（BossMiyakoCard2_move 的 moveVelTime 起始角）
//     即依赖此保留语义。
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
TEST(move_reset_folds_sub_positions_and_keeps_angles_and_radii) {
    StageSim sim; StageEnemy e; EclContext ctx;
    sim.current = &e; sim.commands.sim = &sim;
    e.vel.pos[0] = 10.0f; e.vel.pos[1] = 20.0f; e.vel.pos[2] = 30.0f;
    e.pos.pos[0] = 0.5f;  e.pos.pos[1] = 0.25f; e.pos.pos[2] = 0.125f;
    e.vel.speed = 0.6f;  e.vel.angle = 2.5f;
    e.vel.radius = 1.25f; e.vel.growth = 0.5f;
    e.vel.aux[0] = 1.0f; e.vel.aux[1] = 2.0f; e.vel.aux[2] = 3.0f;
    e.vel.mode = 0x1234u;
    e.pos.speed = 0.3f;  e.pos.angle = -1.1f;
    e.pos.radius = 2.0f; e.pos.growth = -0.25f;
    e.pos.aux[0] = 4.0f; e.pos.aux[1] = 5.0f; e.pos.aux[2] = 6.0f;
    e.pos.mode = 0xABCDu;
    e.vel.polar.duration = 10.0f; e.pos.polar.duration = 20.0f;
    e.vel.postime.duration = 30.0f; e.pos.postime.duration = 40.0f;
    e.vel.radial.duration = 50; e.pos.radial.duration = 60;
    std::vector<u8> code; th13test::emit(code, 0, 427, 0, 255, 0, {});
    ctx.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(ctx, 427), 0);
    // 子位置折叠：vel += pos（41ba18），pos 清零（41ba56）。
    CHECK_EQ(th13test::bits_of(e.vel.pos[0]), th13test::bits_of(10.5f));
    CHECK_EQ(th13test::bits_of(e.vel.pos[1]), th13test::bits_of(20.25f));
    CHECK_EQ(th13test::bits_of(e.vel.pos[2]), th13test::bits_of(30.125f));
    CHECK_EQ(e.pos.pos[0], 0.0f); CHECK_EQ(e.pos.pos[1], 0.0f); CHECK_EQ(e.pos.pos[2], 0.0f);
    // 速度清零（41ba73/41ba6d），角度/半径/成长保留（无写入）。
    CHECK_EQ(e.vel.speed, 0.0f); CHECK_EQ(e.pos.speed, 0.0f);
    CHECK_EQ(th13test::bits_of(e.vel.angle), th13test::bits_of(2.5f));
    CHECK_EQ(th13test::bits_of(e.pos.angle), th13test::bits_of(-1.1f));
    CHECK_EQ(th13test::bits_of(e.vel.radius), th13test::bits_of(1.25f));
    CHECK_EQ(th13test::bits_of(e.vel.growth), th13test::bits_of(0.5f));
    CHECK_EQ(th13test::bits_of(e.pos.radius), th13test::bits_of(2.0f));
    CHECK_EQ(th13test::bits_of(e.pos.growth), th13test::bits_of(-0.25f));
    // aux 清零（41ba7f-41bac4）。
    for (int i = 0; i < 3; ++i) {
        CHECK_EQ(e.vel.aux[i], 0.0f);
        CHECK_EQ(e.pos.aux[i], 0.0f);
    }
    // mode 仅清低 4 位（41baca/41bad0）。
    CHECK_EQ(e.vel.mode, 0x1230u);
    CHECK_EQ(e.pos.mode, 0xABC0u);
    // 插值挂接门清零（41bad8/41bade/41bae4/41baea/41baf0/41baf6）。
    CHECK_EQ(e.vel.polar.duration, 0.0f); CHECK_EQ(e.pos.polar.duration, 0.0f);
    CHECK_EQ(e.vel.postime.duration, 0.0f); CHECK_EQ(e.pos.postime.duration, 0.0f);
    CHECK_EQ(e.vel.radial.duration, 0); CHECK_EQ(e.pos.radial.duration, 0);
}
TEST(move_reset_keeps_main_position_and_residual_rounding_delta) {
    // 0x419950 把主位置量化为 0.01；41ba18 的折叠保留 vel+pos 与主位置的
    // 量化残差，下一帧 delta 仍按 (vel+pos)-main 计算（不得把 vel 直接设为
    // 主位置而抹掉残差）。
    StageSim sim; StageEnemy e; EclContext ctx;
    sim.current = &e; sim.commands.sim = &sim;
    e.position[0] = 3.0f; e.position[1] = 4.0f; e.position[2] = 0.0f;
    e.vel.pos[0] = 3.004f; e.vel.pos[1] = 3.996f; e.vel.pos[2] = 0.0f;
    e.pos.pos[0] = 0.0f; e.pos.pos[1] = 0.0f; e.pos.pos[2] = 0.0f;
    std::vector<u8> code; th13test::emit(code, 0, 427, 0, 255, 0, {});
    ctx.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(ctx, 427), 0);
    CHECK_EQ(th13test::bits_of(e.vel.pos[0]), th13test::bits_of(3.004f));
    CHECK_EQ(th13test::bits_of(e.vel.pos[1]), th13test::bits_of(3.996f));
    CHECK_EQ(th13test::bits_of(e.position[0]), th13test::bits_of(3.0f));
    CHECK_EQ(th13test::bits_of(e.position[1]), th13test::bits_of(4.0f));
}
int main() { return th13test::run_all("move-reset"); }
