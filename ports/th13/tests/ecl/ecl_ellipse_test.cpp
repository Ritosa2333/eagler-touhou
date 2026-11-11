#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;

namespace {
struct Rig {
    StageSim sim;
    StageEnemy* enemy = nullptr;
    EclContext context;
    Rig() {
        std::vector<u8> root;
        th13test::emit(root, 0, 23, 0, 255, 0, {600});
        th13test::emit(root, 0, 10, 0, 255, 0, {});
        const auto data = th13test::build_scpt({{"main", root}}, {});
        sim.program.attach(data.data(), u32(data.size()));
        sim.commands.sim = &sim;
        sim.gvars.sim = &sim;
        enemy = sim.spawn_enemy("main", 12, 45, 10, nullptr, false);
        enemy->tick_gate = false;
        sim.current = enemy;
    }
    int ellipse(u16 opcode, std::initializer_list<float> args) {
        std::vector<u8> data;
        th13test::emit(data, 0, opcode, 0, 255, 0,
                      {0, 0, 0, 0, 0, 0});
        size_t offset = 16;
        for (float value : args) {
            std::memcpy(data.data() + offset, &value, 4);
            offset += 4;
        }
        context.instruction = reinterpret_cast<const EclInstruction*>(data.data());
        return sim.commands.execute(context, opcode);
    }
};

TEST(ellipse_scales_local_x_and_updates_position_without_advancing_clock) {
    Rig r;
    CHECK_EQ(r.ellipse(420, {0, 0, 60, 4, 0, .5f}), 0);
    CHECK_EQ(r.enemy->vel.pos[0], 42.f);
    CHECK_EQ(r.enemy->vel.pos[1], 45.f);
    CHECK_EQ(r.enemy->position[0], 42.f);
    CHECK_EQ(r.enemy->vel.radius, 60.f);
    CHECK(r.sim.step_frame());
    CHECK_EQ(r.enemy->vel.radius, 64.f);
    CHECK_EQ(r.enemy->position[0], 44.f);
    CHECK_EQ(r.enemy->position[1], 45.f);
}

TEST(relative_ellipse_composes_with_absolute_mover_and_preserves_z) {
    Rig r;
    r.enemy->pos.pos[0] = 100;
    r.enemy->pos.pos[1] = 70;
    r.enemy->pos.pos[2] = 4;
    CHECK_EQ(r.ellipse(422, {0, 0, 60, 0, 0, .5f}), 0);
    CHECK_EQ(r.enemy->vel.pos[0], 12.f);
    CHECK_EQ(r.enemy->pos.pos[0], 130.f);
    CHECK_EQ(r.enemy->position[0], 142.f);
    CHECK_EQ(r.enemy->position[1], 115.f);
    CHECK_EQ(r.enemy->position[2], 4.f);
}

TEST(ellipse_keeps_circle_center_but_reanchors_an_existing_ellipse) {
    Rig r;
    auto& mover = r.enemy->vel;
    mover.mode = 2;
    mover.aux[0] = 20;
    mover.aux[1] = 80;
    CHECK_EQ(r.ellipse(420, {0, 0, 60, 0, 0, .5f}), 0);
    CHECK_EQ(mover.pos[0], 50.f);
    CHECK_EQ(mover.pos[1], 80.f);
    // 41c990's center-copy gate is != 2, not != 3. Keep sentinels
    // preserve the ellipse parameters but do not suppress that copy.
    CHECK_EQ(r.ellipse(420, {-999999, -999999, -999999,
                             -999999, -999999, -999999}), 0);
    CHECK_EQ(mover.pos[0], 80.f);
    CHECK_EQ(mover.pos[1], 80.f);
    CHECK_EQ(mover.radius, 60.f);
    CHECK_EQ(mover.ellipse_ratio, .5f);
}
}

int main() { return th13test::run_all("ellipse-movement"); }
