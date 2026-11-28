#pragma once
#include <array>
#include <cstdint>
#include <optional>

namespace th128::player::respawn {
struct EntryPlan {int rawVertical=0;float pixelVertical=0,laserRadius=0;bool cancelAll=false,normal=false;};
std::optional<EntryPlan> entryPlan(int elapsed) noexcept;
struct ControlState {
    int mode04=0,value08=0;
    std::array<std::uint32_t,8> targets{};
    std::array<std::uint16_t,8> pending{};
};
struct ControlPlan {ControlState next;std::optional<std::uint32_t> requiredSlot;int requiredInterrupt=0;};
std::optional<ControlPlan> postFieldControl(ControlState state) noexcept;
}
