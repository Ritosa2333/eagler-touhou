#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
TEST(move_reset_folds_sub_positions_and_keeps_angles_and_radii) {
    StageSim sim;
    StageEnemy e;
    EclContext ctx;
    sim.current = &e;
    sim.commands.sim = &sim;
    e.vel.pos[0] = 10.0f;
    e.vel.pos[1] = 20.0f;
    e.vel.pos[2] = 30.0f;
    e.pos.pos[0] = 0.5f;
    e.pos.pos[1] = 0.25f;
    e.pos.pos[2] = 0.125f;
    e.vel.speed = 0.6f;
    e.vel.angle = 2.5f;
    e.vel.radius = 1.25f;
    e.vel.growth = 0.5f;
    e.vel.aux[0] = 1.0f;
    e.vel.aux[1] = 2.0f;
    e.vel.aux[2] = 3.0f;
    e.vel.mode = 0x1234u;
    e.pos.speed = 0.3f;
    e.pos.angle = -1.1f;
    e.pos.radius = 2.0f;
    e.pos.growth = -0.25f;
    e.pos.aux[0] = 4.0f;
    e.pos.aux[1] = 5.0f;
    e.pos.aux[2] = 6.0f;
    e.pos.mode = 0xABCDu;
    e.vel.polar.duration = 10.0f;
    e.pos.polar.duration = 20.0f;
    e.vel.postime.duration = 30.0f;
    e.pos.postime.duration = 40.0f;
    e.vel.radial.duration = 50;
    e.pos.radial.duration = 60;
    std::vector<u8> code;
    th13test::emit(code, 0, 427, 0, 255, 0, {});
    ctx.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(ctx, 427), 0);
    CHECK_EQ(th13test::bits_of(e.vel.pos[0]), th13test::bits_of(10.5f));
    CHECK_EQ(th13test::bits_of(e.vel.pos[1]), th13test::bits_of(20.25f));
    CHECK_EQ(th13test::bits_of(e.vel.pos[2]), th13test::bits_of(30.125f));
    CHECK_EQ(e.pos.pos[0], 0.0f);
    CHECK_EQ(e.pos.pos[1], 0.0f);
    CHECK_EQ(e.pos.pos[2], 0.0f);
    CHECK_EQ(e.vel.speed, 0.0f);
    CHECK_EQ(e.pos.speed, 0.0f);
    CHECK_EQ(th13test::bits_of(e.vel.angle), th13test::bits_of(2.5f));
    CHECK_EQ(th13test::bits_of(e.pos.angle), th13test::bits_of(-1.1f));
    CHECK_EQ(th13test::bits_of(e.vel.radius), th13test::bits_of(1.25f));
    CHECK_EQ(th13test::bits_of(e.vel.growth), th13test::bits_of(0.5f));
    CHECK_EQ(th13test::bits_of(e.pos.radius), th13test::bits_of(2.0f));
    CHECK_EQ(th13test::bits_of(e.pos.growth), th13test::bits_of(-0.25f));
    for (int i = 0; i < 3; ++i) {
        CHECK_EQ(e.vel.aux[i], 0.0f);
        CHECK_EQ(e.pos.aux[i], 0.0f);
    }
    CHECK_EQ(e.vel.mode, 0x1230u);
    CHECK_EQ(e.pos.mode, 0xABC0u);
    CHECK_EQ(e.vel.polar.duration, 0.0f);
    CHECK_EQ(e.pos.polar.duration, 0.0f);
    CHECK_EQ(e.vel.postime.duration, 0.0f);
    CHECK_EQ(e.pos.postime.duration, 0.0f);
    CHECK_EQ(e.vel.radial.duration, 0);
    CHECK_EQ(e.pos.radial.duration, 0);
}
TEST(move_reset_keeps_main_position_and_residual_rounding_delta) {
    StageSim sim;
    StageEnemy e;
    EclContext ctx;
    sim.current = &e;
    sim.commands.sim = &sim;
    e.position[0] = 3.0f;
    e.position[1] = 4.0f;
    e.position[2] = 0.0f;
    e.vel.pos[0] = 3.004f;
    e.vel.pos[1] = 3.996f;
    e.vel.pos[2] = 0.0f;
    e.pos.pos[0] = 0.0f;
    e.pos.pos[1] = 0.0f;
    e.pos.pos[2] = 0.0f;
    std::vector<u8> code;
    th13test::emit(code, 0, 427, 0, 255, 0, {});
    ctx.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(ctx, 427), 0);
    CHECK_EQ(th13test::bits_of(e.vel.pos[0]), th13test::bits_of(3.004f));
    CHECK_EQ(th13test::bits_of(e.vel.pos[1]), th13test::bits_of(3.996f));
    CHECK_EQ(th13test::bits_of(e.position[0]), th13test::bits_of(3.0f));
    CHECK_EQ(th13test::bits_of(e.position[1]), th13test::bits_of(4.0f));
}
int main() { return th13test::run_all("move-reset"); }
