// Win32保留原cdecl/stdcall参数位型和实际输出字节，资源服务沿既有接口。
#pragma once
#include "core/runtime/GameStartEntryContext.hpp"
namespace kaikeidou {
enum class GameStartEntryPlatformApi:std::uint32_t {virtual_alloc=0x697128,create_file=0x697044,set_file_pointer=0x697048,read_file=0x6970e0,close_handle=0x6971fc};
struct GameStartEntryPlatformWrite {std::uint32_t address=0;std::vector<std::uint8_t> bytes;};
struct GameStartEntryPlatformResult {
 std::uint32_t eax=0,ecx=0,edx=0;
 std::vector<GameStartEntryPlatformWrite> writes;
};
struct GameStartEntryServices:StageResourceServices {
 virtual GameStartEntryPlatformResult platform(GameStartEntryPlatformApi api,const std::vector<std::uint32_t>& arguments)=0;
};
}
