#include "../harness/test_framework.hpp"
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"

using namespace th13;

TEST(enemy_manager_reserves_zero_and_assigns_birth_serials) {
    StageSim sim;
    std::vector<u8> code;
    th13test::emit(code, 0, 40, 0, 255, 0, {0});
    th13test::emit(code, 10000, 1, 0, 255, 0, {});
    auto bytes = th13test::build_scpt({{"main", code}});
    CHECK_EQ(sim.program.attach(bytes.data(), u32(bytes.size())), 0);
    sim.commands.sim = &sim; sim.gvars.sim = &sim;
    auto* root = sim.spawn_enemy("main", 0, 0, 100, nullptr, false);
    auto* child = sim.spawn_enemy("main", 0, 0, 100, root, false);
    CHECK(root != nullptr); CHECK(child != nullptr);
    CHECK_EQ(root->serial, 1u); CHECK_EQ(root->script_id, 1);
    CHECK_EQ(child->serial, 2u); CHECK_EQ(child->trace_id, 1);
    CHECK(sim.find_by_script_id(1) == root);
    CHECK(sim.find_by_script_id(0) == nullptr);
}

int main() { return th13test::run_all("enemy serial"); }
