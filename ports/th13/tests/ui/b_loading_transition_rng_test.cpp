// B2/C visual-RNG contracts for the loading gate (round-16 agent B).
//
// Evidence (retail v1.00c, read-only Unicorn traces):
//  - artifacts/round-s12/A5/native-trace-boot/battle-visual-rng-trace.jsonl
//    boot lineage: 0x45ac90 font noise 256 step16 (index 256, seed 57045),
//    f0 caption-creation variable reads 2x next32 (index 260, seed 30195),
//    f1 loader frame update 12x next32 from ascii script16's signed/unit
//    variable reads (index 284, seed 40152), f2/f3 no draws, battle frame 0
//    (f4) adds 10 next32 -> index 304, seed 33690. battle-replay-core-v2
//    frame 0 records exactly 304/33690.
//  - native continuous th13_01 (artifacts/continuous-native-20261031):
//    S2->S3 door input 27673 draws no scenery values; the extra 6 visual
//    draws (12 index units) land at input 27793 when start_transition fires.
//    Stage 3's scenery ANM (st03wl.anm) is the only stage whose first
//    primitive ticks consume visual RNG (scripts 11-16 read one value each),
//    which is why S1->S2 never showed the shift and S2->S3 did.
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/AnmManager.hpp"
#include "game/StageScenery.hpp"

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
}  // namespace

// The debug replay entry must not compress the loader's 24-unit caption
// update into a synchronous call: three ordinary loader frames (main+secondary,
// as ApplicationState::advance_loading does) reproduce the native f0..f3
// lineage, and the battle-side draws then land on 304/33690.
TEST(loading_gate_boot_lineage_three_loader_frames_match_native_284_304) {
    GameRandom visual{0};
    for (int i = 0; i < 256; ++i) (void)visual.step16();  // 0x45ac90 noise
    CHECK_EQ(visual.index(), 256u);
    CHECK_EQ(visual.seed(), 57045u);
    (void)visual.next32();
    (void)visual.next32();  // f0 caption-creation variable 10022 reads
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
    CHECK_EQ(visual.index(), 260u);  // create performs only the root initial tick

    // Loader frame f1: one main + secondary pass spawns script16 and its 12
    // signed/unit reads consume 24 index units. f2/f3 (the rest of the
    // debug entry's three-frame handoff) add nothing.
    // ApplicationState::kDebugLoadingFrames == 3 (f1/f2/f3 loader frames).
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

    // Battle frame 0 ANM side: 10 next32 (native trace f4).
    for (int i = 0; i < 10; ++i) (void)visual.next32();
    CHECK_EQ(visual.index(), 304u);
    CHECK_EQ(visual.seed(), 33690u);
}

// Stage 3's STD scenery binds/tick-instances its primitive scripts once during
// initialize; st03wl scripts 11-16 each read one visual value (12 index
// units). The original does this at the loader gate's transition phase
// (27793), so prepare_next_stage must not initialize the next scenery at the
// door (27673). All other stages consume nothing here, which is why S1->S2
// stayed aligned.
TEST(stage_scenery_initial_tick_rng_is_stage3_only_and_12_units) {
    struct Case {
        const char* std_name;
        const char* anm_name;
        u32 expected_units;
    };
    const Case cases[] = {
        {"st01.std", "st01wl.anm", 0},
        {"st02.std", "st02wl.anm", 0},
        {"st03.std", "st03wl.anm", 12},
        {"st04.std", "st04wl.anm", 0},
        {"st05.std", "st05wl.anm", 0},
        {"st06.std", "st06wl.anm", 0},
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
        // Afterwards the primitives have already executed their immediate
        // random reads; ordinary updates add nothing (so the whole stage-3
        // shift is the initial tick phase alone).
        const u32 after_init = random.index();
        CHECK(scenery.update(manager, environment, 1.f));
        CHECK_EQ(random.index(), after_init);
    }
}

int main() { return th13test::run_all("b_loading transition rng contracts"); }
