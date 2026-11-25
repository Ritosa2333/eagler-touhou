#include "anm_test_fixtures.hpp"
#include "game/AnmEnvironment.hpp"

using namespace th13;

namespace {

AnmVariables make_variables() {
    AnmVariables variables{};
    constexpr i32 integers[] = {11, -22, 33, 44};
    constexpr float floats[] = {1.5f, -2.25f, 3.125f, 4.5f};
    constexpr i32 extra_integers[] = {55, 66};
    constexpr float position[] = {7.5f, 8.5f, 9.5f};
    constexpr float rotation[] = {0.25f, 0.5f, 0.75f};
    std::memcpy(variables.integers, integers, sizeof integers);
    std::memcpy(variables.floats, floats, sizeof floats);
    std::memcpy(variables.extra_integers, extra_integers, sizeof extra_integers);
    std::memcpy(&variables.position, position, sizeof position);
    std::memcpy(&variables.rotation, rotation, sizeof rotation);
    variables.random_scale = 1.75f;
    variables.random_angle = 2.5f;
    variables.random_bound = 1000;
    return variables;
}

float read_floating(AnmVariables& variables, i32 id) {
    GameRandom visual(0x1234), game(0x5678);
    const bool game_stream = id >= 10030 && id <= 10032;
    const bool random_draw = (id >= 10010 && id <= 10012) || id == 10022 || game_stream;
    const u32 expected_index = random_draw ? 2u : 0u;
    const u32 draw_seed = game_stream ? 25162u : 7814u;
    const u32 idle_seed = game_stream ? 22136u : 4660u;
    const u32 other_seed = game_stream ? 4660u : 22136u;
    GameRandom& drawn = game_stream ? game : visual;
    GameRandom& idle = game_stream ? visual : game;
    const float result = variables.floating(float(id), game_stream ? game : visual);
    CHECK_EQ(drawn.index(), expected_index);
    CHECK_EQ(drawn.seed(), random_draw ? draw_seed : idle_seed);
    CHECK_EQ(idle.index(), 0u);
    CHECK_EQ(idle.seed(), other_seed);
    return result;
}

Vec3 run_position_variable(i32 id) {
    std::vector<u8> bytes;
    const i32 argument = i32(float_to_bits(float(id)));
    th13test::push_insn(bytes, 400, 0, 7, {argument, argument, argument});
    th13test::push_insn(bytes, 2, 0);
    th13test::push_terminator(bytes);
    auto script = th13test::make_script(std::move(bytes));
    AnmEnvironment environment;
    environment.camera_origin = {100.f, 200.f, 1200.f};
    environment.screen_translation = {10.f, 20.f, 100.f};
    environment.reference_position = {0.1f, 0.2f, 0.3f};
    AnmVm parent, vm;
    CHECK(parent.bind(script));
    CHECK(vm.bind(script));
    parent.variables = make_variables();
    parent.variables.rotation.z = 1.25f;
    vm.variables = make_variables();
    if (id == 10026) vm.rotation_parent = &parent;
    GameRandom visual(0x1234), game(0x5678);
    vm.environment = &environment;
    environment.shared_game_rng = &game;
    CHECK_EQ(vm.tick(visual), 0);
    return vm.variables.position;
}

}

TEST(floating_variables_match_retail_oracle) {
    AnmVariables variables = make_variables();
    CHECK_EQ(read_floating(variables, 10000), 11.f);
    CHECK_EQ(read_floating(variables, 10001), -22.f);
    CHECK_EQ(read_floating(variables, 10002), 33.f);
    CHECK_EQ(read_floating(variables, 10003), 44.f);
    CHECK_EQ(read_floating(variables, 10004), 1.5f);
    CHECK_EQ(read_floating(variables, 10005), -2.25f);
    CHECK_EQ(read_floating(variables, 10006), 3.125f);
    CHECK_EQ(read_floating(variables, 10007), 4.5f);
    CHECK_EQ(read_floating(variables, 10008), 55.f);
    CHECK_EQ(read_floating(variables, 10009), 66.f);
    CHECK_EQ(read_floating(variables, 10010), -1.9005180597305298f);
    CHECK_EQ(read_floating(variables, 10011), 0.20981867611408234f);
    CHECK_EQ(read_floating(variables, 10012), -1.3303626775741577f);
    CHECK_EQ(read_floating(variables, 10013), 7.5f);
    CHECK_EQ(read_floating(variables, 10014), 8.5f);
    CHECK_EQ(read_floating(variables, 10015), 9.5f);
    CHECK_EQ(read_floating(variables, 10022), 514951072.f);
    CHECK_EQ(read_floating(variables, 10023), 0.25f);
    CHECK_EQ(read_floating(variables, 10024), 0.5f);
    CHECK_EQ(read_floating(variables, 10025), 0.75f);
    CHECK_EQ(read_floating(variables, 10027), 1.75f);
    CHECK_EQ(read_floating(variables, 10028), 2.5f);
    CHECK_EQ(read_floating(variables, 10029), 1000.f);
    CHECK_EQ(read_floating(variables, 10030), -0.7234500646591187f);
    CHECK_EQ(read_floating(variables, 10031), 0.6217924952507019f);
    CHECK_EQ(read_floating(variables, 10032), -0.5064150094985962f);
}

TEST(integer_variables_match_retail_oracle) {
    AnmVariables variables = make_variables();
    GameRandom visual(0x1234), game(0x5678);
    CHECK_EQ(variables.integer(10000, visual), 11);
    CHECK_EQ(variables.integer(10001, visual), -22);
    CHECK_EQ(variables.integer(10002, visual), 33);
    CHECK_EQ(variables.integer(10003, visual), 44);
    CHECK_EQ(variables.integer(10004, visual), 1);
    CHECK_EQ(variables.integer(10005, visual), -2);
    CHECK_EQ(variables.integer(10006, visual), 3);
    CHECK_EQ(variables.integer(10007, visual), 4);
    CHECK_EQ(variables.integer(10008, visual), 55);
    CHECK_EQ(variables.integer(10009, visual), 66);
    CHECK_EQ(variables.integer(10010, visual), 10010);
    CHECK_EQ(variables.integer(10011, visual), 10011);
    CHECK_EQ(variables.integer(10012, visual), 10012);
    CHECK_EQ(variables.integer(10013, visual), 10013);
    CHECK_EQ(variables.integer(10014, visual), 10014);
    CHECK_EQ(variables.integer(10015, visual), 10015);
    CHECK_EQ(variables.integer(10016, visual), 10016);
    CHECK_EQ(variables.integer(10017, visual), 10017);
    CHECK_EQ(variables.integer(10018, visual), 10018);
    CHECK_EQ(variables.integer(10019, visual), 10019);
    CHECK_EQ(variables.integer(10020, visual), 10020);
    CHECK_EQ(variables.integer(10021, visual), 10021);
    CHECK_EQ(variables.integer(10022, visual), 73);
    CHECK_EQ(variables.integer(10023, visual), 10023);
    CHECK_EQ(variables.integer(10024, visual), 10024);
    CHECK_EQ(variables.integer(10025, visual), 10025);
    CHECK_EQ(variables.integer(10026, visual), 10026);
    CHECK_EQ(variables.integer(10027, visual), 1);
    CHECK_EQ(variables.integer(10028, visual), 2);
    CHECK_EQ(variables.integer(10029, visual), 1000);
    CHECK_EQ(variables.integer(10030, visual), 10030);
    CHECK_EQ(variables.integer(10031, visual), 10031);
    CHECK_EQ(variables.integer(10032, visual), 10032);
}

TEST(interpreter_reads_script_variables) {
    Vec3 value = run_position_variable(10016);
    CHECK_EQ(float_to_bits(value.x), float_to_bits(110.f));
    CHECK_EQ(float_to_bits(value.y), float_to_bits(110.f));
    CHECK_EQ(float_to_bits(value.z), float_to_bits(110.f));
    value = run_position_variable(10017);
    CHECK_EQ(float_to_bits(value.x), float_to_bits(220.f));
    CHECK_EQ(float_to_bits(value.y), float_to_bits(220.f));
    CHECK_EQ(float_to_bits(value.z), float_to_bits(220.f));
    value = run_position_variable(10018);
    CHECK_EQ(float_to_bits(value.x), float_to_bits(1300.f));
    CHECK_EQ(float_to_bits(value.y), float_to_bits(1300.f));
    CHECK_EQ(float_to_bits(value.z), float_to_bits(1300.f));
    value = run_position_variable(10019);
    CHECK_EQ(float_to_bits(value.x), float_to_bits(0.1f));
    CHECK_EQ(float_to_bits(value.y), float_to_bits(0.1f));
    CHECK_EQ(float_to_bits(value.z), float_to_bits(0.1f));
    value = run_position_variable(10020);
    CHECK_EQ(float_to_bits(value.x), float_to_bits(0.2f));
    CHECK_EQ(float_to_bits(value.y), float_to_bits(0.2f));
    CHECK_EQ(float_to_bits(value.z), float_to_bits(0.2f));
    value = run_position_variable(10021);
    CHECK_EQ(float_to_bits(value.x), float_to_bits(0.3f));
    CHECK_EQ(float_to_bits(value.y), float_to_bits(0.3f));
    CHECK_EQ(float_to_bits(value.z), float_to_bits(0.3f));
    value = run_position_variable(10026);
    CHECK_EQ(float_to_bits(value.x), float_to_bits(2.f));
    CHECK_EQ(float_to_bits(value.y), float_to_bits(2.f));
    CHECK_EQ(float_to_bits(value.z), float_to_bits(2.f));
}

TEST(variable_destinations_match_retail_oracle) {
    AnmVariables variables = make_variables();
    float argument = 0;
    auto mapped_float = [&](i32 id, float* destination) {
        argument = float(id);
        CHECK_EQ(variables.float_destination(&argument), destination);
    };
    auto draft_float = [&](i32 id) {
        argument = float(id);
        CHECK_EQ(variables.float_destination(&argument), &argument);
    };
    mapped_float(10004, &variables.floats[0]);
    mapped_float(10005, &variables.floats[1]);
    mapped_float(10006, &variables.floats[2]);
    mapped_float(10007, &variables.floats[3]);
    mapped_float(10013, &variables.position.x);
    mapped_float(10014, &variables.position.y);
    mapped_float(10015, &variables.position.z);
    mapped_float(10023, &variables.rotation.x);
    mapped_float(10024, &variables.rotation.y);
    mapped_float(10025, &variables.rotation.z);
    mapped_float(10027, &variables.random_scale);
    mapped_float(10028, &variables.random_angle);
    argument = 10004.5f;
    CHECK_EQ(variables.float_destination(&argument), &variables.floats[0]);
    for (i32 id : {10000, 10001, 10002, 10003, 10008, 10009, 10010, 10011, 10012, 10016, 10017, 10018, 10019,
                   10020, 10021, 10022, 10026, 10029, 10030, 10031, 10032, 9999,  10033, 5,     -3})
        draft_float(id);
    draft_float(-3);
    i32 integer_argument = 0;
    auto mapped_integer = [&](i32 id, i32* destination) {
        integer_argument = id;
        CHECK_EQ(variables.integer_destination(&integer_argument), destination);
    };
    auto draft_integer = [&](i32 id) {
        integer_argument = id;
        CHECK_EQ(variables.integer_destination(&integer_argument), &integer_argument);
    };
    mapped_integer(10000, &variables.integers[0]);
    mapped_integer(10001, &variables.integers[1]);
    mapped_integer(10002, &variables.integers[2]);
    mapped_integer(10003, &variables.integers[3]);
    mapped_integer(10008, &variables.extra_integers[0]);
    mapped_integer(10009, &variables.extra_integers[1]);
    mapped_integer(10029, reinterpret_cast<i32*>(&variables.random_bound));
    for (i32 id :
         {10004, 10005, 10006, 10007, 10010, 10011, 10012, 10013, 10014, 10015, 10016, 10017, 10018, 10019, 10020,
          10021, 10022, 10023, 10024, 10025, 10026, 10027, 10028, 10030, 10031, 10032, 9999,  10033, 5,     -3})
        draft_integer(id);
}

int main() { return th13test::run_all("anm variables retail oracle"); }
