// 原子机记录位于 0x00a74f70，共 4 槽，每槽 0x230 字节。
#pragma once
#include "core/shared/ObjectRecord.hpp"

namespace kaikeidou {
using PlayerOption = ObjectRecord<560>;
struct OptionsPool { std::array<PlayerOption, 4> options{}; };
struct OptionsContext { std::uint32_t character = 0, weapon = 0; };
}
