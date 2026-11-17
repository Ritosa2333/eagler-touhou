#include "../ui/ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/AnmManager.hpp"

using namespace th13;
using namespace th13::test;

TEST(distortion_uses_the_shared_game_rng_from_0046b7c4) {
    GameRandom visual(12), game(34), expected(34);
    AnmEnvironment environment;
    environment.shared_game_rng = &game;
    AnmManager manager(visual, environment);
    CHECK(load_bank(manager, 1, synthetic_anm(1, {{AnmIns{302,0,{10}},wait(),stop()}})));
    const auto before_visual = visual.index();
    const auto before_game = game.index();
    const auto handle = manager.create(1,0);
    CHECK(handle != 0);
    CHECK_EQ(visual.index(), before_visual);
    CHECK_EQ(game.index() - before_game, 130u);
    for (unsigned n=0;n<65;++n) expected.next();
    CHECK_EQ(game.seed(), expected.seed());
}

int main() { return th13test::run_all("distortion shared RNG"); }
