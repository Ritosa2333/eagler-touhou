// 统一宿主只接平台边界；场景主体、输入计数、资源与音量计算由恢复模块推进。
#pragma once
#include "core/frontend/SaveData.hpp"
#include "core/frontend/MenuReplayServices.hpp"
#include "core/frontend/MenuReplayRuntime.hpp"
#include "core/runtime/GameStartEntryContext.hpp"
#include <string_view>
#include <vector>
namespace kaikeidou {
enum class MenuSceneApi:std::uint32_t {
 delete_graph=0x403e20,blend=0x404830,brightness=0x4048a0,color=0x4049a0,screen_flip=0x40e6b0,
 clear_draw_screen=0x41b8e0,draw_graph=0x41bb70,rotated_sprite=0x41c910,rectangle=0x421760,
 prepare_default_font=0x432c20,set_font_size=0x4331b0,text_width_default=0x437bd0,text_width_font=0x437c20,
 draw_font_text=0x437c90,draw_formatted=0x437f00,now_count=0x437fd0,input_error_log=0x438f70,
 load_graph=0x43c090,keyboard_state=0x43d9f0,pad_state=0x43da40,set_sound_type=0x43e0d0,
 delete_sound=0x43e010,set_volume=0x43fd40,stop_sound=0x440240,check_sound=0x441b20,play_sound=0x4425f0,
 load_sound=0x4434d0,shutdown=0x443db0,process_message=0x443e20,format_x86=0x455740,platform_random=0x466760,
 movie_seek=0x406770,movie_play=0x406690,
 write_file=0x697040,create_file=0x697044,set_file_pointer=0x697048,read_file=0x6970e0,
 virtual_alloc=0x697128,virtual_free=0x69712c,close_handle=0x6971fc,sleep_ms=0x69722c
};
struct MenuSceneRequest {
 MenuSceneApi api;
 // 数值按原32位参数位型展开，binary64为低/高两字；文字参数独立保留原字节。
 std::vector<std::uint32_t> words;
 std::vector<std::string_view> texts;
 std::vector<std::uint8_t> bytes;
};
struct MenuScenePlatformResult {
 std::uint32_t eax=0,ecx=0,edx=0;
 std::vector<MenuReplayWrite> writes;
 std::vector<std::uint8_t> bytes; // 键盘状态等输出缓冲的实际已提供字节。
};
struct MenuScenePlatform {
 virtual ~MenuScenePlatform()=default;
 virtual void present_gameplay(){}
 virtual void finish_gameplay(){}
 virtual MenuScenePlatformResult invoke(const MenuSceneRequest& request,MenuReplayFormatMemory* local_stack=nullptr)=0;
 virtual SaveIO& savedata_io()=0;
};
enum class MenuSceneInputRestoreRule {shifted_popad};
struct MenuSceneInputRegisterTransfer {
 std::uint32_t entry=0xb4241d,return_instruction=0xb424a5,scene=0,phase=0,binding=0,mode=0,bitmap=0,leaf_value=0;
 MenuSceneInputRestoreRule restore_rule=MenuSceneInputRestoreRule::shifted_popad;
};
enum class MenuSceneOutcomeKind {completed,gameplay_ready,exit_requested,unhandled_scene,startup_transfer,memory_fault,input_transfer,platform_output_fault,format_fault,input_register_transfer};
struct MenuSceneOutcome {
 MenuSceneOutcomeKind kind=MenuSceneOutcomeKind::completed;
 std::uint32_t entry_scene=0,scene=0,outer_frame=0,last_ecx=0,target=0,fault_address=0,fault_bytes=0;
 bool write_fault=false;
 GameStartEntryOutcome startup;
 MenuReplaySaveOutcome replay_save;
 MenuSceneInputRegisterTransfer input_register_transfer;
 std::vector<MenuReplayWrite> partial_stack_writes;
};
struct MenuSceneInputTransfer {std::uint32_t target=0,binding=0,source=0;};
struct MenuScenePlatformOutputFault {MenuSceneApi api;std::uint32_t index=0,required=0;};
}
