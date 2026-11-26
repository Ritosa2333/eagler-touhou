// 原进程配置只新增GS cookie引用，游戏和菜单仍使用同一有界地址视图。
#pragma once
#include "core/runtime/GameStartEntryMemory.hpp"
#include <string>
namespace kaikeidou {
struct ApplicationStartupContext {std::uint32_t security_cookie=0;};
struct ApplicationStartupRequest {std::uint32_t api=0,source=0;std::vector<std::uint32_t> words;std::vector<std::string> texts;};
struct ApplicationStartupWrite {std::uint32_t address=0;std::vector<std::uint8_t> bytes;};
struct ApplicationStartupPlatformResult {std::uint32_t eax=0,ecx=0,edx=0;std::vector<ApplicationStartupWrite> writes;};
struct ApplicationStartupServices {virtual ~ApplicationStartupServices()=default;virtual ApplicationStartupPlatformResult invoke(const ApplicationStartupRequest& request)=0;};
enum class ApplicationStartupKind {entered_outer,returned_target,memory_fault,fetch_transfer,security_cookie_failure,execution_limit,platform_transfer};
struct ApplicationStartupPlatformTransfer {std::uint32_t source=0,target=0,exact_registers=0,known_eflags=0;};
struct ApplicationStartupPolicy {std::vector<std::uint32_t> registered_return_targets;std::uint32_t maximum_instructions=100000;};
struct ApplicationStartupOutcome {
 ApplicationStartupKind kind=ApplicationStartupKind::fetch_transfer;
 std::uint32_t source=0,target=0,fault_address=0,fault_bytes=0,exact_registers=0xff,known_eflags=0,steps=0;
 bool writing=false;std::vector<std::uint32_t> open_config_handles;
};
template<class Memory>void bind_application_startup(Memory& memory,ApplicationStartupContext& context){memory.bind(0x70e2b0,context.security_cookie);}
}
