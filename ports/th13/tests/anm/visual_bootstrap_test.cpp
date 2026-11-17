#include "../harness/test_framework.hpp"
#include "game/VisualBootstrap.hpp"

using namespace th13;

TEST(font_noise_advances_the_visual_rng_once_per_entry) {
    GameRandom actual(0), expected(0);
    const auto table = visual_font_noise(actual);
    for (unsigned i = 0; i < table.size(); ++i)
        CHECK_EQ(table[i], std::uint8_t(expected.step16() >> 9));
    CHECK_EQ(actual.seed(), expected.seed());
    CHECK_EQ(actual.index(), 256u);
}

TEST(font_noise_uses_current_state_and_preserves_prior_count) {
    GameRandom actual(0x1234), expected(0x1234);
    actual.next(); expected.next();
    const auto table = visual_font_noise(actual);
    for (unsigned i = 0; i < table.size(); ++i)
        CHECK_EQ(table[i], std::uint8_t(expected.step16() >> 9));
    CHECK_EQ(actual.seed(), expected.seed());
    CHECK_EQ(actual.index(), 258u);
}

int main() { return th13test::run_all("visual bootstrap"); }
