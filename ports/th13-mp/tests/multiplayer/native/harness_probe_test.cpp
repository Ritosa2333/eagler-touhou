#include "game/GameInput.hpp"
#include "game/Rng.hpp"

#include "../../harness/test_framework.hpp"

#include <cstdint>

namespace {

struct RefGameRandom {
    uint16_t seed = 0;
    uint32_t index = 0;

    explicit RefGameRandom(uint16_t initial) : seed(initial) {}
    static uint16_t twist(uint16_t value) {
        return static_cast<uint16_t>((value >> 14) + value * 4);
    }
    uint32_t next() {
        const uint16_t a = static_cast<uint16_t>((seed ^ 0x9630u) - 0x6553u);
        const uint16_t b = static_cast<uint16_t>((twist(a) ^ 0x9630u) - 0x6553u);
        seed = twist(b);
        index += 2;
        return (static_cast<uint32_t>(a) << 16) | b;
    }
    uint16_t step16() {
        const uint16_t a = static_cast<uint16_t>((seed ^ 0x9630u) - 0x6553u);
        seed = twist(a);
        index += 1;
        return seed;
    }
};

}

TEST(native_rng_reseed_restore) {
    for (const uint16_t seed : {uint16_t{0}, uint16_t{1}, uint16_t{0x1234}, uint16_t{0xFFFF}}) {
        th13::GameRandom game;
        game.reseed_restore(seed);
        RefGameRandom reference(seed);
        for (int i = 0; i < 16; i++) {
            CHECK_EQ(game.next(), reference.next());
            CHECK_EQ(game.index(), static_cast<uint32_t>(2 * (i + 1)));
        }
        game.reseed_restore(seed);
        CHECK_EQ(game.index(), 0u);
        RefGameRandom replay(seed);
        for (int i = 0; i < 4; i++) CHECK_EQ(game.next(), replay.next());
        CHECK_EQ(game.index(), 8u);
    }
    th13::GameRandom keep;
    keep.reseed_restore(7);
    RefGameRandom warm(7);
    for (int i = 0; i < 3; i++) keep.next();
    CHECK_EQ(keep.index(), 6u);
    keep.reseed(9);
    CHECK_EQ(keep.index(), 6u);
    RefGameRandom fresh(9);
    CHECK_EQ(keep.next(), fresh.next());
    CHECK_EQ(keep.index(), 8u);
}

TEST(native_rng_step16_next32) {
    th13::GameRandom game;
    game.reseed_restore(0xBEEF);
    RefGameRandom reference(0xBEEF);
    for (int i = 0; i < 16; i++) {
        CHECK_EQ(game.step16(), reference.step16());
        CHECK_EQ(game.index(), static_cast<uint32_t>(i + 1));
    }
    th13::GameRandom mixed;
    mixed.reseed_restore(0x0F0D);
    RefGameRandom mixed_reference(0x0F0D);
    for (int i = 0; i < 3; i++) CHECK_EQ(mixed.step16(), mixed_reference.step16());
    RefGameRandom from_state(mixed_reference.seed);
    CHECK_EQ(mixed.next(), from_state.next());
    CHECK_EQ(mixed.index(), 5u);
    th13::GameRandom via_next32;
    via_next32.reseed_restore(0x0042);
    th13::GameRandom via_next;
    via_next.reseed_restore(0x0042);
    for (int i = 0; i < 16; i++) {
        CHECK_EQ(via_next32.next32(), via_next.next());
        CHECK_EQ(via_next32.index(), via_next.index());
    }
    CHECK_EQ(via_next32.index(), 32u);
}

TEST(native_game_input_edges) {
    th13::GameInput input;
    input.update(0);
    CHECK_EQ(input.held, 0u);
    CHECK_EQ(input.previous, 0u);
    CHECK_EQ(input.pressed, 0u);
    CHECK_EQ(input.released, 0u);

    input.update(0x01);
    CHECK_EQ(input.previous, 0u);
    CHECK_EQ(input.held, 0x01u);
    CHECK_EQ(input.pressed, 0x01u);
    CHECK_EQ(input.released, 0u);

    input.update(0x01);
    CHECK_EQ(input.pressed, 0u);
    CHECK_EQ(input.released, 0u);
    CHECK_EQ(input.previous, 0x01u);
    CHECK_EQ(input.held, 0x01u);

    input.update(0x03);
    CHECK_EQ(input.pressed, 0x02u);
    CHECK_EQ(input.previous, 0x01u);
    CHECK_EQ(input.held, 0x03u);

    for (int i = 0; i < 5; i++) input.update(0x03);
    CHECK_EQ(input.long_held, 0x01u);
    CHECK_EQ(input.repeated, 0u);
    input.update(0x03);
    input.update(0x03);
    CHECK_EQ(input.long_held, 0x03u);
    CHECK_EQ(input.repeated, 0u);
    for (int i = 0; i < 16; i++) input.update(0x03);
    CHECK_EQ(input.repeated, 0x01u);
    input.update(0x03);
    CHECK_EQ(input.repeated, 0u);
    input.update(0x03);
    CHECK_EQ(input.repeated, 0x02u);
    input.update(0x03);
    CHECK_EQ(input.repeated, 0u);
    CHECK_EQ(input.long_held, 0x03u);

    input.update(0);
    CHECK_EQ(input.held, 0u);
    CHECK_EQ(input.previous, 0x03u);
    CHECK_EQ(input.released, 0x03u);
    CHECK_EQ(input.pressed, 0u);
    CHECK_EQ(input.long_held, 0u);

    input.update(0x01);
    CHECK_EQ(input.pressed, 0x01u);
    CHECK_EQ(input.released, 0u);
    for (int i = 0; i < 6; i++) input.update(0x01);
    CHECK_EQ(input.long_held, 0u);
    CHECK_EQ(input.repeated, 0u);
    input.update(0x01);
    CHECK_EQ(input.long_held, 0x01u);

    input.update(0x02);
    CHECK_EQ(input.released, 0x01u);
    CHECK_EQ(input.pressed, 0x02u);
    CHECK_EQ(input.held, 0x02u);
    CHECK_EQ(input.long_held, 0u);
}

int main() { return th13test::run_all("m1.unit_native_harness"); }
