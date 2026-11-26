// 标题三块原记录及Replay返回字段；保留空洞与未对齐字段的原字节。
#pragma once
#include "core/shared/ObjectRecord.hpp"
#include <array>
namespace kaikeidou {
struct MenuTitleReturnContext {
 std::array<ObjectRecord<32>,5> strips{}; // A773E0，160字节。
 std::array<ObjectRecord<40>,50> particles{}; // A785F8，2000字节。
 std::array<ObjectRecord<56>,5> objects{}; // A73118，280字节。
 std::uint32_t replay_mode=0,save_disabled=0,save_choice=0; // B4310C、B43A00、未对齐B4303E。
 std::uint32_t eof_flags=0,accumulator=0; // B43C00、B43C04，完整dword。
 ObjectRecord<8> prompt{}; // 未对齐B43056，两次dword写入，不补NUL。
};
}
