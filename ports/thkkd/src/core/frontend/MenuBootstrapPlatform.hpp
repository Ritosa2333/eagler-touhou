// 资源参数保留原位型；路径和字体名称保持原编码字节，平台写入显式按序提交。
#pragma once
#include "core/frontend/MenuScenePlatform.hpp"
namespace kaikeidou {
enum class MenuBootstrapApi:std::uint32_t {
 system_flag=0x443d40,now_count=0x437fd0,load_graph=0x43c090,load_div_graph=0x43c2c0,
 load_sound=0x4434d0,create_font=0x432800,delete_graph=0x403e20,platform_random=0x466760,
 set_sound_type=0x43e0d0,stop_sound=0x440240,delete_sound=0x43e010,set_volume=0x43fd40,
 play_sound=0x4425f0,blend=0x404830,color=0x4049a0,draw_formatted=0x437f00,
 movie_seek=0x406770,movie_play=0x406690
};
struct MenuBootstrapRequest {
 MenuBootstrapApi api;std::vector<std::uint32_t> words;std::vector<std::string_view> texts;
 std::uint32_t call_site=0,text_address=0,format_address=0;
 std::vector<std::uint32_t> format_words;
};
using MenuBootstrapPlatformResult=MenuScenePlatformResult;
struct MenuBootstrapPlatform {
 virtual ~MenuBootstrapPlatform()=default;
 virtual MenuBootstrapPlatformResult invoke(const MenuBootstrapRequest& request)=0;
 virtual SaveIO& savedata_io()=0;
};
}
