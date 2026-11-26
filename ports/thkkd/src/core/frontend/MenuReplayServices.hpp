// Replay UI保留原地址与32位参数；平台回调使用同一地址视图实施有序写入。
#pragma once
#include <cstdint>
#include <vector>
#include <stdexcept>
namespace kaikeidou {
struct MenuReplayRegisters {std::uint32_t eax=0,ecx=0,edx=0,ebx=0,esp=0,ebp=0,esi=0,edi=0;};
struct MenuReplayWrite {std::uint32_t address=0;std::vector<std::uint8_t> bytes;};
struct MenuReplayPlatformResult {std::uint32_t eax=0,ecx=0,edx=0;std::vector<MenuReplayWrite> writes;};
enum class MenuReplayApi:std::uint32_t {
 color=0x4049a0,prepare_default_font=0x432c20,set_font_size=0x4331b0,font_text=0x437c90,formatted_text=0x437f00,
 sound_volume=0x5e5060,music_volume=0x5e50b0,help=0x5e6f70,return_title=0x5e9800,options_body=0x5e9830,input_count=0x656c70,
 write_file=0x697040,create_file=0x697044,set_file_pointer=0x697048,read_file=0x6970e0,virtual_free=0x69712c,close_handle=0x6971fc
};
enum class MenuReplayFormatFaultKind {unavailable_argument,target_overflow,unsupported_conversion,address_fault};
struct MenuReplayFormatFault:std::runtime_error {
 MenuReplayFormatFaultKind kind;std::uint32_t address;std::size_t written;
 std::vector<MenuReplayWrite> stack_writes;
 MenuReplayFormatFault(MenuReplayFormatFaultKind k,std::uint32_t a,std::size_t n):std::runtime_error("Replay original format view fault"),kind(k),address(a),written(n){}
};
struct MenuReplayFormatMemory {
 virtual ~MenuReplayFormatMemory()=default;
 virtual std::uint8_t byte(std::uint32_t address)=0;
 virtual std::uint32_t argument(std::uint32_t address)=0;
 virtual void byte(std::uint32_t address,std::uint8_t value)=0;
};
struct MenuReplayFormatRequest {
 std::uint32_t format=0,arguments=0,target=0,capacity=2048;
};
struct MenuReplayServices {
 virtual ~MenuReplayServices()=default;
 virtual MenuReplayPlatformResult call(MenuReplayApi api,const std::vector<std::uint32_t>& arguments,MenuReplayFormatMemory* local_stack=nullptr)=0;
 // 调用方提供有界参数与目标栈视图。服务按原序写入；故障前的写入保留。
 virtual MenuReplayPlatformResult format_x86(MenuReplayFormatMemory& memory,const MenuReplayFormatRequest& request)=0;
};
}
