// 本页新增字段；共用菜单与实际角色/stage继续由既有上下文持有。
#pragma once
#include <array>
#include <cstdint>
namespace kaikeidou {
struct MenuCharacterPracticeContext {
 std::uint32_t practice_selected=0; // A87F30，练习关卡与Omniverse Boss共用。
 std::uint32_t weapon=0; // A7E614，确认后实际机体。
 std::uint32_t name_font=0,detail_font=0; // A74BAC、A74BB0。
 std::uint32_t list_font=0; // A74BB4，在列表color调用后重读。
 std::array<std::uint32_t,2> other_character_graphs{}; // A73018、A73038；角色0复用menu.title_graph@A72FF8。
};
}
