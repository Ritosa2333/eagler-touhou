// EX首道中服务保持原调用顺序；输入绑定数组依次对应A733DC/E0/E4/E8/F4。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/player/PlayerShotFireContext.hpp"
namespace kaikeidou {
inline std::uint32_t ex_first_read_input(std::uint32_t binding,const PlayerShotFireContext& fire,PlayerShotFireInput& input){
 if(fire.replay_variant&&fire.replay_mode){const auto bit=binding==11?7u:binding;return bit<32?((fire.replay_buttons>>bit)&1u):0u;}
 return input.read_counter(binding);
}
struct ExFirstServices:StageServices {
 virtual void kill_enemy(std::uint32_t index)=0;
};
}
