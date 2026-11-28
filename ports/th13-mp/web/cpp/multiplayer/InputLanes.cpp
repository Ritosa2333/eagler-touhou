#include "InputLanes.hpp"

#include "../game/StageBattle.hpp"

namespace th13::multiplayer {

bool InputLanes::Capture(std::uint32_t held, bool pause, Netplay::FrameInput& out) {
    constexpr std::uint32_t kGameplay = 0x8ffu;
    constexpr std::uint32_t kUiBits = DialogueAdvanceBit | 0x100u | 0x10000u |
                                      0x20000u | 0x40000u | 0x80000u |
                                      0x100000u | 0x200000u | 0x800000u;
    if (held & ~(kGameplay | kUiBits))
        return false;
    const std::uint32_t gameplay = held & kGameplay;
    Netplay::FrameInput next(std::uint16_t(gameplay) | (pause ? Pause : 0));
    if (!Netplay::IsValidFrameInput(next)) return false;
    out = next;
    return true;
}

void InputLanes::ApplyTouch(StageBattle& battle, unsigned seat,
                            const Netplay::FrameInput& input) {
    auto& touch = battle.seats[seat].touch;
    touch = {};
    if (input.analogMode == Netplay::AnalogMode::DirectTouch)
        touch = {input.unlimited ? 2 : 1, input.x, input.y};
}

GameInput InputLanes::AdvanceHistory(StageBattle& battle,
        const Netplay::FrameInput* frames, unsigned count) {
    GameInput menu;
    for (unsigned seat = 0; seat < count; ++seat) {
        auto& input = battle.seat_inputs[seat];
        input.update(Controls(frames[seat]));
        ApplyTouch(battle, seat, frames[seat]);
        menu.held |= input.held;
        menu.pressed |= input.pressed;
        menu.released |= input.released;
        menu.repeated |= input.repeated;
        for (unsigned bit = 0; bit < 32; ++bit)
            menu.duration[bit] = std::max(menu.duration[bit], input.duration[bit]);
    }
    return menu;
}

bool InputLanes::Decode(const Netplay::FrameInput* frames, unsigned count,
                        std::array<std::uint32_t, 3>& out) {
    if (!frames || count < 2 || count > 3) return false;
    std::array<std::uint32_t, 3> next{};
    for (unsigned seat = 0; seat < count; ++seat) {
        const auto& frame = frames[seat];
        if (!Netplay::IsValidFrameInput(frame) || frame.touchBomb ||
            (frame.buttons & ~(HeldMask | Pause | Confirm | Dialogue | Restart | Quit)))
            return false;
        next[seat] = frame.buttons & HeldMask;
    }
    out = next;
    return true;
}

}
