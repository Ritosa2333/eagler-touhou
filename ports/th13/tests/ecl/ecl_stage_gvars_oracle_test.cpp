#include "ecl_test_fixtures.hpp"
#include "game/AnmVm.hpp"
#include "game/SessionState.hpp"
#include "game/StageSim.hpp"

#include <cstring>

using namespace th13;

namespace {
constexpr i32 gvar_ids[] = {
    -10000, -9999, -9998, -9997, -9996, -9995, -9994, -9993, -9992, -9991, -9990, -9989, -9988, -9987, -9986, -9985,
    -9984,  -9983, -9982, -9981, -9980, -9979, -9978, -9977, -9976, -9975, -9974, -9973, -9972, -9971, -9970, -9969,
    -9968,  -9967, -9966, -9965, -9964, -9963, -9962, -9961, -9960, -9959, -9958, -9957, -9956, -9955, -9954, -9953,
    -9952,  -9951, -9950, -9949, -9948, -9947, -9946, -9945, -9944, -9943, -9942, -9941, -9940, -9939, -9938, -9937,
    -9936,  -9935, -9934, -9933, -9932, -9931, -9930, -9929, -9928, -9927, -9926, -9925, -9924, -9923, -9922, -9921,
    -9920,  -9919, -9918, -9917, -9916, -9915, -9914, -9913, -9912, -9911, -9910, -9909, -9908, -9907};

constexpr i32 gvar_ints[] = {
    1526044818, 0,     0,   11,  -22, 1,   -2,  7,   8,    192, 288,       0,  77,   0,   0,  -5, 6,   -7,  8,
    1,          -2,    3,   -4,  11,  -22, 1,   -2,  7,    8,   -7,        11, 1234, -10, 9,  13, 192, 288, 211,
    -222,       -1234, 20,  3,   -1,  1,   0,   0,   1234, 0,   0,         0,  1,    3,   -4, 5,  3,   8,   359,
    -55,        66,    -77, 88,  -11, 12,  -13, 14,  5,    -6,  7,         -8, 77,   2,   0,  0,  0,   9,   -10,
    11,         12,    13,  -14, 15,  -16, 17,  -18, 2,    -3,  287454020, 0,  0,    0,   0,  7,  1,   21};

constexpr u32 gvar_float_bits[] = {
    1320545073, 1052109617, 3211312052, 1093664768, 3249537024, 1069547520, 3222274048, 1089470464, 1091043328,
    1128300544, 1133518848, 1065701094, 1117388800, 3197381022, 0,          3231711232, 1086324736, 3235905536,
    1090519040, 1067450368, 3224371200, 1080033280, 3229876224, 1093664768, 3249537024, 1069547520, 3222274048,
    1089470464, 1091043328, 3236429824, 1094451200, 1150959616, 3240230912, 1092091904, 1096024064, 1128300544,
    1133518848, 1129512960, 3277717504, 0,          1101004800, 1077936128, 3213735693, 1065353216, 1065171615,
    1065126976, 1150959616, 0,          0,          0,          1065353216, 1077936128, 3229614080, 1084227584,
    1077936128, 1090519040, 1135851611, 3260809216, 1115947008, 3264872448, 1118830592, 3241410560, 1094975488,
    3243769856, 1097334784, 1084751872, 3234594816, 1088684032, 3238264832, 1117388800, 1073741824, 0,
    0,          0,          1091567616, 3240099840, 1093664768, 1094713344, 1095761920, 3244294144, 1098383360,
    3246522368, 1099825152, 3247702016, 1074790400, 3227516928, 1300828570, 0,          0,          3211099090,
    0,          1088421888, 1065353216, 1101529088};

void set_movement(StageMover& mover, float x, float y, float speed, float angle, float radius) {
    mover.pos[0] = x;
    mover.pos[1] = y;
    mover.speed = speed;
    mover.angle = angle;
    mover.radius = radius;
}

TEST(stage_gvars_supply_matches_retail_probe) {
    StageSim sim;
    SessionState session;
    AnmVm anm;
    sim.gvars.sim = &sim;
    sim.diff = 3;
    sim.rank = 20;
    sim.player_lives = 2;
    sim.player_x = 192.5f;
    sim.player_y = 288.25f;
    sim.boss_slot[0] = 17;
    sim.last_created_enemy_id = 77;
    sim.shared_integers = {9, -10, 11, 12};
    sim.shared_floats = {13.0f, -14.0f, 15.5f, -16.25f, 17.75f, -18.5f, 2.25f, -3.5f};
    session.stage = 5;
    session.selected_spell_id = 21;
    sim.session = &session;
    anm.source_script = -1234;
    sim.enemy_animation = [&anm](StageEnemy&, i32) { return &anm; };

    auto enemy = std::make_unique<StageEnemy>();
    StageEnemy* current = enemy.get();
    enemy->script_id = 0x11223344;
    enemy->trace_id = 7;
    enemy->position[0] = 11.0f;
    enemy->position[1] = -22.0f;
    enemy->position[2] = 33.0f;
    enemy->motion_delta[0] = 0.25f;
    enemy->motion_delta[1] = -0.5f;
    set_movement(enemy->vel, 1.5f, -2.25f, 1234.0f, -7.25f, 9.5f);
    set_movement(enemy->pos, 7.5f, 8.5f, -10.125f, 11.75f, 13.25f);
    enemy->i_slots[0] = -5;
    enemy->i_slots[1] = 6;
    enemy->i_slots[2] = -7;
    enemy->i_slots[3] = 8;
    for (i32 slot = 0; slot < 8; ++slot)
        enemy->f_slots[slot] = (slot == 0)   ? 1.25f
                               : (slot == 1) ? -2.75f
                               : (slot == 2) ? 3.5f
                               : (slot == 3) ? -4.125f
                               : (slot == 4) ? 5.25f
                               : (slot == 5) ? -6.375f
                               : (slot == 6) ? 7.125f
                                             : -8.25f;
    enemy->age = 77.0f;
    enemy->life = 1234.0f;
    enemy->flags = 0xabcd;
    sim.enemies.push_back(std::move(enemy));
    sim.current = current;

    auto boss = std::make_unique<StageEnemy>();
    boss->script_id = 17;
    boss->trace_id = 23;
    boss->position[0] = 211.0f;
    boss->position[1] = -222.0f;
    boss->motion_delta[0] = 200.25f;
    boss->motion_delta[1] = -250.5f;
    boss->i_slots[0] = -55;
    boss->i_slots[1] = 66;
    boss->i_slots[2] = -77;
    boss->i_slots[3] = 88;
    boss->f_slots[0] = -11.25f;
    boss->f_slots[1] = 12.25f;
    boss->f_slots[2] = -13.5f;
    boss->f_slots[3] = 14.5f;
    sim.enemies.push_back(std::move(boss));

    auto third = std::make_unique<StageEnemy>();
    third->flags = 0x31;
    third->invulnerable = 5;
    sim.enemies.push_back(std::move(third));

    constexpr i32 writable_ids[3] = {-9949, -9948, -9947};
    constexpr i32 writable_values[3] = {3, -4, 5};
    for (i32 slot = 0; slot < 3; ++slot) {
        i32* destination = sim.gvars.integer_destination(writable_ids[slot]);
        CHECK(destination);
        if (destination) *destination = writable_values[slot];
    }

    for (u32 index = 0; index < sizeof gvar_ids / sizeof gvar_ids[0]; ++index) {
        const i32 id = gvar_ids[index];
        const bool integer_random = id == -10000 || id == -9999 || id == -9987;
        const bool float_random = id == -10000 || id == -9999 || id == -9998 || id == -9987;
        if (integer_random) sim.random.reseed_restore(0x5678);
        i32 integer = 12345;
        CHECK(sim.gvars.integer(id, integer));
        CHECK_EQ(integer, gvar_ints[index]);
        if (integer_random) {
            CHECK_EQ(u32(sim.random.seed()), u32(0x624a));
            CHECK_EQ(sim.random.index(), u32(2));
        }
        if (float_random) sim.random.reseed_restore(0x5678);
        float floating = 12345.0f;
        CHECK(sim.gvars.floating(id, floating));
        const u32 actual_bits = [&floating] {
            u32 bits = 0;
            std::memcpy(&bits, &floating, sizeof bits);
            return bits;
        }();
        CHECK_EQ(actual_bits, gvar_float_bits[index]);
        if (float_random) {
            CHECK_EQ(u32(sim.random.seed()), u32(0x624a));
            CHECK_EQ(sim.random.index(), u32(2));
        }
    }
}
}

int main() { return th13test::run_all("stage-gvars-oracle"); }
