#include "anm_test_fixtures.hpp"
#include "game/AnmEnvironment.hpp"

using namespace th13;

TEST(loading_caption_boot_sequence_matches_original_visual_rng_window) {
    GameRandom visual{0};
    for (int i = 0; i < 256; ++i)
        (void)visual.step16();
    CHECK_EQ(visual.index(), 256u);
    CHECK_EQ(visual.seed(), 57045u);

    AnmEnvironment environment;
    environment.resolution_scale = 1;
    environment.raster_scale = 1;
    AnmManager manager{visual, environment};
    th13test::TestFileReader reader;
    std::vector<u8> bytes;
    CHECK(reader.read("artifacts/reference/raw/ascii.anm", bytes));
    CHECK(manager.load(2, bytes.data(), u32(bytes.size())));
    CHECK(manager.sprite_fallback(2));

    const u32 prayer = manager.create(2, 17, -1, 0, {480, 392, 0});
    CHECK(prayer != 0);
    CHECK_EQ(visual.index(), 256u);

    CHECK(manager.update());
    CHECK_EQ(visual.index(), 280u);
    CHECK_EQ(visual.seed(), 47358u);
}

int main() { return th13test::run_all("anm loading caption boot rng window"); }
