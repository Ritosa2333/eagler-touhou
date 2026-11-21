#include "game/PlayerRules.hpp"

#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

using namespace th128::player;

void fixedEdges() {
    static_assert(ShootFreezeKey == 1 && BombKey == 2 && FocusKey == 8);
    static_assert(UpKey == 16 && DownKey == 32 && LeftKey == 64 && RightKey == 128 && RapidKey == 512);
    static_assert(RawUnitsPerPixel == 128);
    constexpr std::array<std::uint8_t, 16> expected{0, 1, 2, 2, 3, 5, 7, 5, 4, 6, 8, 6, 3, 5, 7, 5};
    for (std::size_t index = 0; index < expected.size(); ++index) {
        const Input input{static_cast<std::uint16_t>(index << 4), 0, 0};
        assert(static_cast<std::uint8_t>(direction(input)) == expected[index]);
    }
    const FocusContext available{true, true, 4, 0};
    assert(isFocused({8, 0, 0}, available));
    assert(!isFocused({8, 0, 0}, {false, true, 4, 99}));
    assert(!isFocused({8, 0, 0}, {true, true, 3, 99}));
    assert(!isFocused({0, 0, 0}, {true, true, 4, 11}));
    assert(isFocused({0, 0, 0}, {true, true, 4, 12}));
    const auto diagonal = rawDelta(Direction::UpLeft, false);
    assert(diagonal && diagonal->horizontal == -407 && diagonal->vertical == -407);
    assert(!rawDelta(static_cast<Direction>(255), false));
    const auto focusedDiagonal = rawDelta(Direction::DownRight, true);
    assert(focusedDiagonal && focusedDiagonal->horizontal == 181 && focusedDiagonal->vertical == 181);
    const auto overflow = advancePosition({std::numeric_limits<std::int32_t>::max(), 4096}, {1, -1});
    assert(overflow.horizontal == -23552 && overflow.vertical == 4096);
    const auto endpoint = advancePosition({23552, 55296}, {0, 0});
    assert(endpoint.horizontal == 23552 && endpoint.vertical == 55296);
    const auto motion = moveOneTick({0, 51200}, {0x50, 0, 0}, available);
    assert(motion.direction == Direction::UpLeft && !motion.focused);
    assert(motion.position.horizontal == -407 && motion.position.vertical == 50793);
    const auto focusedMotion = moveOneTick({0, 51200}, {0xa8, 0, 0}, available);
    assert(focusedMotion.focused && focusedMotion.position.horizontal == 181);
    const auto pixels = toPixels(endpoint);
    assert(pixels.horizontal == 184.0f && pixels.vertical == 432.0f);
    assert(!startsShotCycle({1, 0, 0}, -1));
    assert(startsShotCycle({1, 1, 0}, -1));
    assert(startsShotCycle({512, 0, 0}, -1));
    assert(!startsShotCycle({513, 0, 0}, -1));
    assert(!startsShotCycle({512, 0, 0}, 0));
    assert(!advanceShotCycle({0, 0, 0}, -1));
    assert(!advanceShotCycle({0, 0, 0}, 15));
    const auto repeated = advanceShotCycle({512, 0, 0}, 14);
    assert(repeated && repeated->previous == -1 && repeated->current == 0);
    assert(!recordDue(0, 0, 0) && !recordDue(-1, 0, 0) && !recordDue(128, 0, 0));
    assert(!recordDue(3, 128, 0) && !recordDue(3, -129, 0));
    assert(recordDue(3, -1, -4) == true);
    const Input unknown{0xffff, 0xdead, 0xbeef};
    assert(unknown.holding == 0xffff && unknown.pressed == 0xdead && unknown.released == 0xbeef);
    assert(!startsShotCycle({0, 0, 1}, -1));
}

int main() {
    fixedEdges();
    std::cout << "player fixed edges passed\n" << std::setprecision(std::numeric_limits<double>::max_digits10);
    std::string operation;
    while (std::cin >> operation) {
        if (operation == "focus") {
            unsigned holding;
            int present, active, counter, held;
            std::cin >> holding >> present >> active >> counter >> held;
            const Input input{static_cast<std::uint16_t>(holding), 0, 0};
            std::cout << static_cast<int>(direction(input)) << ' '
                      << isFocused(input, {present != 0, active != 0, counter, held}) << '\n';
        } else if (operation == "velocity") {
            int value, focused;
            std::cin >> value >> focused;
            const auto result = rawDelta(static_cast<Direction>(value), focused != 0);
            assert(result);
            std::cout << result->horizontal << ' ' << result->vertical << '\n';
        } else if (operation == "position") {
            RawPosition start;
            RawDelta delta;
            std::cin >> start.horizontal >> start.vertical >> delta.horizontal >> delta.vertical;
            const auto result = advancePosition(start, delta);
            const auto pixels = toPixels(result);
            std::cout << result.horizontal << ' ' << result.vertical << ' '
                      << pixels.horizontal << ' ' << pixels.vertical << '\n';
        } else if (operation == "start" || operation == "cycle") {
            unsigned holding, pressed;
            int current;
            std::cin >> holding >> pressed >> current;
            const Input input{static_cast<std::uint16_t>(holding), static_cast<std::uint16_t>(pressed), 0};
            if (operation == "start") {
                std::cout << startsShotCycle(input, current) << '\n';
            } else {
                const auto result = advanceShotCycle(input, current);
                assert(result);
                std::cout << result->previous << ' ' << result->current << '\n';
            }
        } else if (operation == "record") {
            int period, phase, count;
            std::cin >> period >> phase >> count;
            const auto result = recordDue(period, phase, count);
            assert(result);
            std::cout << *result << '\n';
        } else {
            return 2;
        }
        if (!std::cin) return 3;
    }
    return 0;
}
