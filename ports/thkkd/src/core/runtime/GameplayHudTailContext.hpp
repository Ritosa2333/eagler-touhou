// HUD只新增白闪24字节；其他图形/玩家/成绩/cookie均读取共同地址视图。
#pragma once
#include "core/runtime/GameStartEntryMemory.hpp"
#include "core/frontend/MenuScenePlatform.hpp"
namespace kaikeidou {
struct GameplayHudTailContext {ObjectRecord<24> flash;};
enum class GameplayHudTailKind {completed,returned_target,memory_fault,fetch_transfer,security_cookie_failure,arithmetic_fault,execution_limit};
struct GameplayHudTailPolicy {std::vector<std::uint32_t> registered_return_targets;std::uint32_t maximum_instructions=200000;};
struct GameplayHudTailOutcome {GameplayHudTailKind kind=GameplayHudTailKind::fetch_transfer;std::uint32_t source=0,target=0,fault_address=0,fault_bytes=0,exact_registers=0xff,known_eflags=0,steps=0;bool writing=false;};
template<class Memory>void bind_gameplay_hud_tail(Memory&m,GameplayHudTailContext&c){m.bind(0xa77540,c.flash.data);}
}
