#include "../harness/test_framework.hpp"
#include "game/AnmManager.hpp"
#include "game/StageScenery.hpp"
#include "ui_test_fixtures.hpp"

#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th13;

namespace {
std::vector<u8> asset(const char* name) {
    std::ifstream file(std::string("artifacts/reference/raw/") + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}
}

TEST(loading_gate_boot_lineage_three_loader_frames_match_native_284_304) {
    GameRandom visual{0};
    for (int i = 0; i < 256; ++i)
        (void)visual.step16();
    CHECK_EQ(visual.index(), 256u);
    CHECK_EQ(visual.seed(), 57045u);
    (void)visual.next32();
    (void)visual.next32();
    CHECK_EQ(visual.index(), 260u);
    CHECK_EQ(visual.seed(), 30195u);

    AnmEnvironment environment;
    environment.resolution_scale = 1;
    environment.raster_scale = 1;
    AnmManager manager{visual, environment};
    std::vector<u8> bytes = asset("ascii.anm");
    CHECK(!bytes.empty());
    CHECK(manager.load(2, bytes.data(), u32(bytes.size())));
    CHECK(manager.sprite_fallback(2));
    const u32 prayer = manager.create(2, 17, -1, 0, {480, 392, 0});
    CHECK(prayer != 0);
    CHECK_EQ(visual.index(), 260u);

    for (unsigned frame = 0; frame < 3; ++frame) {
        CHECK(manager.update(false));
        CHECK(manager.update(true));
        if (frame == 0) {
            CHECK_EQ(visual.index(), 284u);
            CHECK_EQ(visual.seed(), 40152u);
        }
    }
    CHECK_EQ(visual.index(), 284u);
    CHECK_EQ(visual.seed(), 40152u);

    for (int i = 0; i < 10; ++i)
        (void)visual.next32();
    CHECK_EQ(visual.index(), 304u);
    CHECK_EQ(visual.seed(), 33690u);
}

TEST(stage_scenery_initial_tick_rng_is_stage3_only_and_12_units) {
    struct Case {
        const char* std_name;
        const char* anm_name;
        u32 expected_units;
    };
    const Case cases[] = {
        {"st01.std", "st01wl.anm", 0}, {"st02.std", "st02wl.anm", 0}, {"st03.std", "st03wl.anm", 12},
        {"st04.std", "st04wl.anm", 0}, {"st05.std", "st05wl.anm", 0}, {"st06.std", "st06wl.anm", 0},
    };
    for (const auto& item : cases) {
        GameRandom random;
        AnmEnvironment environment;
        environment.resolution_scale = 1;
        environment.raster_scale = 1;
        AnmManager manager{random, environment};
        const auto anm = asset(item.anm_name);
        CHECK(!anm.empty());
        CHECK(manager.load(7, anm.data(), u32(anm.size())));
        CHECK(manager.sprite_fallback(7));
        StageScenery scenery;
        const auto std = asset(item.std_name);
        CHECK(scenery.file.open(std.data(), u32(std.size())));
        const u32 before = random.index();
        CHECK(scenery.initialize(manager, 7));
        CHECK_EQ(random.index() - before, item.expected_units);
        const u32 after_init = random.index();
        CHECK(scenery.update(manager, environment, 1.f));
        CHECK_EQ(random.index(), after_init);
    }
}

int main() { return th13test::run_all("b_loading transition rng contracts"); }
