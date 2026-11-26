// 原键盘/手柄计数以及基础、Replay输入读取；绑定地址运算保持32位回绕。
#pragma once
#include "core/frontend/MenuScenePlatform.hpp"
#include "core/frontend/MenuSceneConstants.hpp"
namespace kaikeidou {
namespace menu_scene_detail {
inline std::int64_t signed_word(std::uint32_t n){return n>>31?std::int64_t(n)-0x100000000ll:std::int64_t(n);}
}
template<class Memory,class Invoke>void update_menu_scene_keyboard(Memory& memory,Invoke&& invoke){
 const auto state=invoke(MenuSceneRequest{MenuSceneApi::keyboard_state,{},{},{}});
 for(std::uint32_t i=0;i<256;++i){if(i>=state.bytes.size())throw MenuScenePlatformOutputFault{MenuSceneApi::keyboard_state,i,1};const auto address=0xa71e08u+i*4u;memory.word(address,state.bytes[i]==1?memory.word(address)+1u:0u);}
}
template<class Memory,class Invoke>void update_menu_scene_pad(Memory& memory,Invoke&& invoke){
 const auto bitmap=invoke(MenuSceneRequest{MenuSceneApi::pad_state,{1},{},{}}).eax;
 for(std::uint32_t i=0;i<16;++i){const auto address=0xa72208u+i*4u;memory.word(address,(bitmap&(1u<<i))?memory.word(address)+1u:0u);}
 constexpr std::array<std::array<std::uint32_t,2>,9> keys{{{{0xcb,0xa733dc}},{{0xc8,0xa733e4}},{{0xcd,0xa733e0}},{{0xd0,0xa733e8}},{{0x2c,0xa733ec}},{{0x2d,0xa733f0}},{{0x2a,0xa733f4}},{{1,0xa733f8}},{{0x1d,0xa733fc}}}};
 for(auto key:keys){const auto value=memory.word(0xa71e08u+key[0]*4u);const auto address=0xa72208u+memory.word(key[1])*4u;const auto current=memory.word(address);memory.word(address,menu_scene_detail::signed_word(current)>menu_scene_detail::signed_word(value)?memory.word(address):value);}
}
template<class Memory,class Invoke>std::uint32_t read_menu_scene_input(Memory& memory,std::uint32_t binding,bool patched_replay,Invoke&& invoke){
 if(patched_replay&&memory.word(0xb4310c)!=0){const auto bit=binding==11?7u:binding;const auto mask=bit<32?1u<<bit:0u;return (memory.word(0xb43108)&mask)?1u:0u;}
 if(binding<16)return memory.word(0xa72208u+binding*4u);
 if(patched_replay)throw MenuSceneInputTransfer{0x924726,binding,0xb42448};
 invoke(MenuSceneRequest{MenuSceneApi::input_error_log,{}, {menu_scene_detail::text(0x6cf720)}, {}});return 0xffffffffu;
}
}
