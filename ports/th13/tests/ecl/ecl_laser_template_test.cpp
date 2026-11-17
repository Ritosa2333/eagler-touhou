// H-LASER-TEMPLATE: 0041b260 cases 0x87/0x88 configure an existing
// enemy et slot, rather than allocate a global laser or reset its state.
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"

using namespace th13;
namespace {
struct Rig {
    StageSim sim;
    StageEnemy enemy;
    EclContext context;
    Rig() { sim.commands.sim = &sim; sim.current = &enemy; }
    int run(u16 opcode, std::initializer_list<u32> args) {
        std::vector<u8> bytes;
        th13test::emit(bytes, 0, opcode, 0, 0xff, 0, args);
        context.instruction = reinterpret_cast<const EclInstruction*>(bytes.data());
        return sim.commands.execute(context, opcode);
    }
};

TEST(laser_template_retains_emitter_and_other_enemy_state) {
    Rig rig;
    auto& et = rig.enemy.et[3];
    et.sprite = 8; et.subtype = 6; et.angle1 = 1.25f; et.speed1 = 8;
    et.offset[0] = 19; et.ex[0].op = 0x100000; et.ex_next = 1;
    CHECK_EQ(rig.run(701, {3, 20, 8, 140, 10, 9}), 0);
    CHECK_EQ(rig.run(700, {3, float_to_bits(480), float_to_bits(240),
                            float_to_bits(-2), float_to_bits(16)}), 0);
    CHECK_EQ(et.spawn[0], 480.0f); CHECK_EQ(et.spawn[1], 240.0f);
    CHECK_EQ(et.spawn[2], -2.0f); CHECK_EQ(et.laser_width, 16.0f);
    const i32 timing[] = {20, 8, 140, 10, 9};
    for (int n = 0; n < 5; ++n) CHECK_EQ(et.laser_timing[n], timing[n]);
    CHECK_EQ(et.sprite, 8u); CHECK_EQ(et.subtype, 6u);
    CHECK_EQ(et.angle1, 1.25f); CHECK_EQ(et.speed1, 8.0f);
    CHECK_EQ(et.offset[0], 19.0f); CHECK_EQ(et.ex[0].op, 0x100000u);
    CHECK_EQ(et.ex_next, 1u);
    CHECK_EQ(rig.enemy.et[2].laser_width, 0.0f);
    StageEnemy other;
    CHECK_EQ(other.et[3].laser_timing[2], 0);
    // No clipping or replacement of zero/negative durations or signed flags.
    CHECK_EQ(rig.run(701, {3, 0, u32(-2), 0, u32(-4), 0x80000009u}), 0);
    CHECK_EQ(et.laser_timing[0], 0); CHECK_EQ(et.laser_timing[1], -2);
    CHECK_EQ(et.laser_timing[2], 0); CHECK_EQ(et.laser_timing[3], -4);
    CHECK_EQ(u32(et.laser_timing[4]), 0x80000009u);
}

TEST(laser_template_copy_reset_and_bullet_position_alias) {
    Rig rig;
    CHECK_EQ(rig.run(700, {1, float_to_bits(480), float_to_bits(480), 0,
                            float_to_bits(16)}), 0);
    CHECK_EQ(rig.run(701, {1, 20, 8, 140, 10, 9}), 0);
    CHECK_EQ(rig.run(614, {2, 1}), 0);
    CHECK_EQ(rig.enemy.et[2].laser_width, 16.0f);
    CHECK_EQ(rig.enemy.et[2].laser_timing[2], 140);
    CHECK_EQ(rig.run(600, {1}), 0);
    CHECK_EQ(rig.enemy.et[1].laser_width, 0.0f);
    for (auto value : rig.enemy.et[1].laser_timing) CHECK_EQ(value, 0);
    CHECK_EQ(rig.enemy.et[2].laser_timing[2], 140);
    rig.enemy.position[0] = 12; rig.enemy.position[1] = 34; rig.enemy.position[2] = 3;
    rig.enemy.et[2].offset[0] = 5;
    CHECK_EQ(rig.run(601, {2}), 0);
    CHECK_EQ(rig.enemy.et[2].spawn[0], 17.0f);
    CHECK_EQ(rig.enemy.et[2].spawn[1], 34.0f);
    CHECK_EQ(rig.enemy.et[2].spawn[2], 3.0f);
    rig.enemy.et[2].absolute[0] = -40;
    rig.enemy.et[2].absolute[1] = 50;
    rig.enemy.et[2].absolute[2] = 1;
    CHECK_EQ(rig.run(601, {2}), 0);
    CHECK_EQ(rig.enemy.et[2].spawn[0], -35.0f);
    CHECK_EQ(rig.enemy.et[2].spawn[1], 50.0f);
    CHECK_EQ(rig.enemy.et[2].spawn[2], 0.0f);
    CHECK_EQ(rig.enemy.et[2].laser_width, 16.0f);
}

TEST(laser_template_rejects_out_of_range_slots) {
    for (u16 opcode : {700, 701}) {
        for (i32 slot : {-1, 8}) {
            Rig rig;
            CHECK_EQ(rig.run(opcode, {u32(slot), 0, 0, 0, 0, 0}), -2);
            CHECK(!rig.sim.error().empty());
        }
    }
}

TEST(absolute_emitter_instruction_preserves_offset_and_supports_disable_sentinel) {
    for(float x:{-991.f,-990.f,0.f,128.f}) {
        Rig rig;rig.enemy.position[0]=12;rig.enemy.position[1]=34;
        rig.enemy.et[2].offset[0]=5;rig.enemy.et[2].offset[1]=7;
        CHECK_EQ(rig.run(628,{2,float_to_bits(x),float_to_bits(100)}),0);
        CHECK_EQ(rig.enemy.et[2].absolute[0],x);
        CHECK_EQ(rig.enemy.et[2].absolute[1],100.f);
        CHECK_EQ(rig.enemy.et[2].absolute[2],x>=-990?1.f:0.f);
        CHECK_EQ(rig.run(601,{2}),0);
        CHECK_EQ(rig.enemy.et[2].spawn[0],x>=-990?x+5:17.f);
        CHECK_EQ(rig.enemy.et[2].spawn[1],x>=-990?107.f:41.f);
    }
}

}
int main() { return th13test::run_all("laser-template"); }
