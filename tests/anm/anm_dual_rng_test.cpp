#include "anm_test_fixtures.hpp"
#include "game/AnmEnvironment.hpp"

using namespace th13;
namespace th13 {
using AnmRandomObserver = void (*)(const AnmVm&, i32, i32, i32, u32, u32, u32, u32, bool);
extern AnmRandomObserver anm_random_observer;
}
namespace {
struct Event { i32 id; bool game; u32 before, after; };
std::vector<Event> events;
void observe(const AnmVm&, i32, i32 id, i32, u32 before, u32, u32 after, u32, bool game) {
    events.push_back({id, game, before, after});
}
i32 bits(float value) { i32 result; std::memcpy(&result, &value, 4); return result; }
}

TEST(masked_random_variables_select_independent_original_streams) {
    // Direct Machine calls to 461e80 with visual seed12345, game seed9529,
    // vm+4d4=1 and vm+4d8=3.141592741: see ignored
    // artifacts/th13-longrun-20261029/anm-dual-rng-probe/probe.py.
    std::vector<u8> bytes;
    th13test::push_insn(bytes, 400, 0, 7, {bits(10030.f), bits(10010.f), bits(10031.f)});
    th13test::push_insn(bytes, 400, 1, 7, {bits(10011.f), bits(10032.f), bits(10012.f)});
    th13test::push_insn(bytes, 2, 2);
    auto script = th13test::make_script(std::move(bytes));
    GameRandom visual(12345), game(9529);
    AnmEnvironment environment; environment.shared_game_rng = &game;
    AnmVm vm; vm.environment = &environment; CHECK(vm.bind(script));
    events.clear(); anm_random_observer = observe;
    CHECK_EQ(vm.tick(visual), 0);
    CHECK_EQ(float_to_bits(vm.variables.position.x), 3214801955u);
    CHECK_EQ(float_to_bits(vm.variables.position.y), 3217478529u);
    CHECK_EQ(float_to_bits(vm.variables.position.z), 1033415593u);
    CHECK_EQ(visual.index(), 2u); CHECK_EQ(visual.seed(), 48728u);
    CHECK_EQ(game.index(), 4u); CHECK_EQ(game.seed(), 54341u);
    CHECK_EQ(vm.tick(visual), 0);
    CHECK_EQ(float_to_bits(vm.variables.position.x), 1061360949u);
    CHECK_EQ(float_to_bits(vm.variables.position.y), 1060783355u);
    CHECK_EQ(float_to_bits(vm.variables.position.z), 1060772601u);
    CHECK_EQ(visual.index(), 6u); CHECK_EQ(visual.seed(), 62161u);
    CHECK_EQ(game.index(), 6u); CHECK_EQ(game.seed(), 62881u);
    anm_random_observer = nullptr;
    CHECK_EQ(events.size(), size_t(6));
    const i32 expected_ids[] = {10030,10010,10031,10011,10032,10012};
    for (size_t i = 0; i < events.size(); ++i) {
        CHECK_EQ(events[i].id, expected_ids[i]);
        CHECK_EQ(events[i].game, (i == 0 || i == 2 || i == 4));
        CHECK_EQ(events[i].after - events[i].before, 2u);
    }
}

TEST(game_random_variable_without_environment_fails_explicitly) {
    std::vector<u8> bytes;
    th13test::push_insn(bytes, 400, 0, 1, {bits(10030.f), bits(0.f), bits(0.f)});
    th13test::push_insn(bytes, 2, 1);
    auto script = th13test::make_script(std::move(bytes));
    GameRandom visual(12345); AnmVm vm; CHECK(vm.bind(script));
    CHECK_EQ(vm.tick(visual), -1);
    CHECK_EQ(visual.index(), 0u);
    CHECK(!vm.error.empty());
}

int main() { return th13test::run_all("anm dual random streams"); }
