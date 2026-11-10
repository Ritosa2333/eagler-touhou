#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace Netplay {
template<class Word>
Word CooperativePressedButtons(const Word* current, const Word* previous, std::size_t count) {
    Word pressed = 0;
    for (std::size_t seat = 0; seat < count; ++seat) pressed |= Word(current[seat] & ~previous[seat]);
    return pressed;
}
// Merge each seat's edges before held keys so one player's hold cannot hide
// another player's confirm, dialogue or pause press.
template<class Word, std::size_t Seats = 3>
struct CooperativeMenuInput {
    std::array<Word, Seats> previous{};
    struct Frame { Word held = 0, pressed = 0; };
    Frame Update(const std::array<Word, Seats>& buttons, std::size_t count) {
        Frame frame;
        for (std::size_t seat = 0; seat < Seats; ++seat) {
            const Word held = seat < count ? buttons[seat] : 0;
            frame.held |= held;
            frame.pressed |= Word(held & ~previous[seat]);
            previous[seat] = held;
        }
        return frame;
    }
};
}
