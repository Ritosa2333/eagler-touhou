#pragma once

#include <eagler/netplay/NetplayProtocol.hpp>

#include <array>
#include <cstdint>

namespace th13 {

class StageBattle;
struct GameInput;

namespace multiplayer {

class InputLanes {
public:
    static constexpr std::uint16_t Pause = 0x8000;
    static constexpr std::uint16_t Confirm = 0x1000, Dialogue = 0x200;
    static constexpr std::uint16_t Restart = 0x2000, Quit = 0x4000;
    static constexpr std::uint16_t HeldMask = 0x8ff;
    static constexpr std::uint32_t TranceBit = 0x800;
    static constexpr std::uint32_t DialogueAdvanceBit = 0x200;

    static bool Capture(std::uint32_t held, bool pause, Netplay::FrameInput& out);
    static std::uint32_t Controls(const Netplay::FrameInput& input) {
        return (input.buttons & (HeldMask | Dialogue)) |
            ((input.buttons & Pause) ? 0x100u : 0) |
            ((input.buttons & Confirm) ? 0x80000u : 0) |
            ((input.buttons & Restart) ? 0x200000u : 0) |
            ((input.buttons & Quit) ? 0x10000u : 0);
    }
    static void ApplyTouch(StageBattle& battle, unsigned seat, const Netplay::FrameInput& input);
    static GameInput AdvanceHistory(StageBattle& battle,
                                    const Netplay::FrameInput* frames, unsigned count);
    static bool Decode(const Netplay::FrameInput* frames, unsigned count,
                       std::array<std::uint32_t, 3>& out);
};

}
}
