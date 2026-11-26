// 各既有Runtime共用同一批原址对象；新增资产区域先建立，再统一seal。
#pragma once
#include "core/frontend/MenuSceneBindings.hpp"
#include "core/frontend/MenuBootstrapBindings.hpp"
#include "core/frontend/EndingCreditsRuntime.hpp"
#include "core/frontend/PauseGameoverRuntime.hpp"
namespace kaikeidou {
struct OuterSceneBindingCollector {
 GameStartEntryMemoryExtensions entries;
 void bind_bytes(std::uint32_t address,void* data,std::size_t size){entries.push_back({address,data,size});}
 template<class T>void bind(std::uint32_t address,T& object){bind_bytes(address,&object,sizeof(object));}
};
inline GameStartEntryMemoryExtensions unique_outer_scene_extensions(const GameStartEntryMemoryExtensions& entries){
 GameStartEntryMemoryExtensions out;for(auto x:entries){const auto found=std::find_if(out.begin(),out.end(),[&](const auto& y){return x.address==y.address&&x.data==y.data&&x.size==y.size;});if(found==out.end())out.push_back(x);}return out;
}
inline GameStartEntryMemoryExtensions outer_scene_extensions(GameStartEntryRefs& game,MenuSceneRuntimeContext& menu,
 CollisionContext& collision,EndingCreditsAssets& ending,MenuBootstrapContext& bootstrap,MenuBootstrapGaps& bootstrap_gaps,
 PauseGameoverContext& pause,GameStartEntryMachineFrame& frame,bool replay_variant,GameStartEntryMemoryExtensions extra={}){
 OuterSceneBindingCollector c;bind_menu_scene_state(c,game,menu,replay_variant);bind_menu_bootstrap_collision(c,collision);bind_ending_credits_assets(c,ending);bind_ending_credits_title(c,menu.title);
 c.bind(0xa77558,bootstrap.random_table);c.bind(0xa74e90,bootstrap.scalar_a74e90);
 c.bind(0xa72ff8,game.dialogue_context.player_portraits);c.bind(0xa74ea8,game.dialogue_context.boss_portraits);c.bind(0xa79050,game.dialogue_context.titles);
 c.bind(0xa7586c,game.dialogue_context.panel);c.bind(0xa74bd4,game.dialogue_context.font);c.bind(0xa74f48,game.gameplay.slow_graph);
 c.bind(0xa74bdc,pause.font);c.bind(0xb43c00,game.pause.eof_flags);c.bind_bytes(frame.stack_address,frame.stack.data(),frame.stack.size());
 for(auto& block:bootstrap_gaps.blocks)c.bind_bytes(block.address,block.bytes.data(),block.bytes.size());
 c.entries.insert(c.entries.end(),extra.begin(),extra.end());return unique_outer_scene_extensions(c.entries);
}
}
