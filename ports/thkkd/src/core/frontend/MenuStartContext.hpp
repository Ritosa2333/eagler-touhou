// 启动菜单及其确认页共用原地址状态；由外层与其他菜单按字段同步。
#pragma once
#include "core/shared/ObjectRecord.hpp"
#include <array>
namespace kaikeidou {
struct MenuStartContext {
 std::uint32_t outer_scene=0,phase=0,timer=0; // A8CE58、A87F48、A87F4C。
 std::uint32_t main_selected=0,difficulty_selected=0,character_selected=0,weapon_selected=0; // A87F20..2C。
 std::uint32_t music_selected=0,music_scroll=0,key_selected=0,option_selected=0,character_phase=0; // A87F34..44。
 std::uint32_t game_mode=0,difficulty=0; // A7E618、A7E610。
 std::array<std::uint32_t,3> sound_requests{}; // A73230..38，确认、取消、移动。
 std::uint32_t font=0,title_graph=0; // A74BA0、A72FF8。
 std::array<std::uint32_t,6> card_graphs{}; // A733C4..D8。
 std::array<ObjectRecord<48>,6> cards{}; // A734A0，四难度、EX、Omniverse。
 std::array<ObjectRecord<64>,3> characters{}; // A77480。
 std::array<ObjectRecord<32>,3> weapons{}; // B3E2F0。
 std::uint64_t character_transition_bits=0,weapon_transition_bits=0; // A758A8、B3E2E0。
 std::array<std::uint64_t,17> music_x_bits{}; // A87F60。
 std::array<std::uint32_t,9> key_pending{}; // A773B4。
 std::array<std::uint32_t,5> option_pending{}; // A7A238。
 std::uint32_t replay_submode=0; // 原B41215为未对齐dword。
};
}
