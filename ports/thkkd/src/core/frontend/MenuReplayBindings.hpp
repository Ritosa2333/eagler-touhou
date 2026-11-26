// 在已有Memory中登记同址UI、Title返回字段及平台默认字体，运行期统一广播写入。
#pragma once
#include "core/frontend/MenuReplayInitialData.hpp"
#include "core/frontend/MenuTitleReturnContext.hpp"
#include <vector>
namespace kaikeidou {
struct MenuReplayBinding {std::uint32_t address;void* data;std::size_t bytes;};
inline std::vector<MenuReplayBinding> menu_replay_bindings(MenuReplayStorage& storage,MenuTitleReturnContext& title,std::uint32_t& default_font){
 return {{0xb43000,storage.ui.data.data(),storage.ui.data.size()},{0xb411ce,storage.replay_label.data.data(),storage.replay_label.data.size()},{0x6c7a88,storage.option_text.data.data(),storage.option_text.data.size()},
  {0x8b63f4,&default_font,4},{0xb4310c,&title.replay_mode,4},{0xb43a00,&title.save_disabled,4},{0xb4303e,&title.save_choice,4},{0xb43c00,&title.eof_flags,4},{0xb43c04,&title.accumulator,4},{0xb43056,title.prompt.data.data(),8}};
}
template<class Memory>void bind_menu_replay(Memory& memory,MenuReplayStorage& storage,MenuTitleReturnContext& title,std::uint32_t& default_font){for(const auto& binding:menu_replay_bindings(storage,title,default_font))memory.bind_bytes(binding.address,binding.data,binding.bytes);}
template<class Memory>void initialize_menu_replay_shared_cold(Memory& memory){
 memory.write(0xb43000,menu_replay_initial_ui.data(),menu_replay_initial_ui.size());
 memory.write(0xb411ce,menu_replay_initial_replay_label.data(),menu_replay_initial_replay_label.size());
 memory.write(0x6c7a88,menu_replay_initial_option_text.data(),menu_replay_initial_option_text.size());
}
}
