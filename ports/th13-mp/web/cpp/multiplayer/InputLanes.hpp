// TH13 MP 输入线路（M7）：本作 GameInput 32 位掩码 ↔ 共用 FrameInput。
// 归一化规则（docs/MP-DEVELOPMENT.md §M7 前冻结）：
//   - C 键在键盘层同时置 0x800（灵界）与 0x200（对话快进）——网线路
//     只携带 gameplay 位：0x200 剥离，0x800 保留；
//   - 0x100（Esc 暂停菜单）不作为 gameplay 位传输（暂停菜单是本地 UI
//     事件，原版单人暂停语义在 MP 下由宿主层另行仲裁）；
//   - Q/S/Home/P/D/R/F10 是菜单/回放键，不进 gameplay 线路。
// GameInput gameplay 面（keyboard_keys 的 gameplay 子集）：
//   0x001 Z 射击 | 0x002 X 炸弹 | 0x008 Shift 低速 | 0x010..0x0a0 方向 |
//   0x800 灵界（C）。
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
    // 网线路暂停位（FrameInput.buttons 高位区，gameplay 位之外）。
    static constexpr std::uint16_t Pause = 0x8000;
    static constexpr std::uint16_t Confirm = 0x1000, Dialogue = 0x200;
    static constexpr std::uint16_t Restart = 0x2000, Quit = 0x4000;
    // gameplay 位全集（GameInput.held 的网线路投影；0x200/0x100 剥离）。
    // bits 0-7 = Z/X/低速/↑↓←→/斜向（0x50/0x60/0x90/0xa0 组合在 bits 4-7），
    // bit 11 = 0x800 灵界（C）。0x200（对话快进）与 0x100（Esc）不在内。
    static constexpr std::uint16_t HeldMask = 0x8ff;
    // 本作 GameInput 的灵界位（键盘层 C 同时置 0x800+0x200）。
    static constexpr std::uint32_t TranceBit = 0x800;
    static constexpr std::uint32_t DialogueAdvanceBit = 0x200;

    // 采集：物理 held（32 位）→ FrameInput。非法位/非法模式拒绝。
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
    // 解码：FrameInput[seat] → 每席 held（网线路 → GameInput.held 的
    // gameplay 面；灵界位原样回 0x800）。count = 席位数（2..3）。
    static bool Decode(const Netplay::FrameInput* frames, unsigned count,
                       std::array<std::uint32_t, 3>& out);
};

}  // namespace th13::multiplayer
}  // namespace th13
