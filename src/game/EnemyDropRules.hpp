#pragma once
#include <array>
#include <cstdint>

namespace th128::enemy {
struct DropState {std::array<std::int32_t,11> words{};float width=0,height=0;};
void resetAdditionalDrops(DropState& drops) noexcept;
bool setDropWord(DropState& drops,int index,int value) noexcept;
bool setDropSpread(DropState& drops,float width,float height) noexcept;
}
