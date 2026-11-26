// 优先绑定既有状态；只有尚未由调用方提供的资产字节进入Gaps。
#pragma once
#include "core/frontend/MenuBootstrapContext.hpp"
#include "core/frontend/MenuBootstrapConstants.hpp"
#include "core/runtime/GameStartEntryBindings.hpp"
#include "core/frontend/MenuSceneBindings.hpp"
#include "core/frontend/EndingCreditsAssets.hpp"
#include "core/player/PlayerCollision.hpp"
namespace kaikeidou {
template<class Memory>void bind_menu_bootstrap_collision(Memory& m,CollisionContext& c){
 m.bind(0xa7ebf8,c.option_a7ebf8);m.bind(0xa7e618,c.mode_a7e618);m.bind(0xb3e2ec,c.stage_b3e2ec);
 m.bind(0xa77390,c.part_a77390);m.bind(0xa733c0,c.phase_a733c0);m.bind(0xa74c0c,c.character_a74c0c);
 m.bind(0xa7ea98,c.mode_a7ea98);m.bind(0xa78e2c,c.scene_a78e2c);m.bind(0xa87f48,c.scene_step_a87f48);m.bind(0xa87f50,c.scene_choice_a87f50);
 m.bind(0xa74c10,c.projectile_radii);
}
template<class Memory>void bind_menu_bootstrap_gaps(Memory& memory,MenuBootstrapGaps& gaps){
 for(auto& b:gaps.blocks)memory.bind_bytes(b.address,b.bytes.data(),b.bytes.size());
 for(auto range:menu_bootstrap_detail::asset_ranges){std::uint64_t at=range[0],end=at+range[1];
  while(at<end){const auto bound=memory.regions();std::uint64_t covered=at,next=end;
   for(auto r:bound){const auto lo=std::uint64_t(r[0]),hi=lo+r[1];if(lo<=at&&at<hi)covered=std::max(covered,hi);else if(at<lo)next=std::min(next,lo);}
   if(covered!=at){at=std::min(end,covered);continue;}
   MenuBootstrapGaps::Block block;block.address=static_cast<std::uint32_t>(at);block.bytes.resize(static_cast<std::size_t>(next-at));gaps.blocks.push_back(std::move(block));auto& saved=gaps.blocks.back();memory.bind_bytes(saved.address,saved.bytes.data(),saved.bytes.size());at=next;
  }
 }
}
inline void bind_menu_bootstrap(GameStartEntryMemory& memory,GameStartEntryRefs& game,GameStartEntryGaps& game_gaps,
 GameStartEntryReplayContext& replay,MenuSceneRuntimeContext& menu,CollisionContext& collision,
 EndingCreditsAssets& ending,MenuBootstrapContext& context,MenuBootstrapGaps& gaps,bool replay_variant,
 const GameStartEntryMemoryExtensions& extra={}){
 bind_game_start_entry(memory,game,game_gaps,replay,replay_variant);bind_menu_scene_state(memory,game,menu,replay_variant);
 bind_menu_bootstrap_collision(memory,collision);bind_ending_credits_assets(memory,ending);
 memory.bind(0xa77558,context.random_table);memory.bind(0xa74e90,context.scalar_a74e90);
 memory.bind(0xa72ff8,game.dialogue_context.player_portraits);memory.bind(0xa74ea8,game.dialogue_context.boss_portraits);
 memory.bind(0xa79050,game.dialogue_context.titles);memory.bind(0xa7586c,game.dialogue_context.panel);memory.bind(0xa74bd4,game.dialogue_context.font);
 memory.bind(0xa74f48,game.gameplay.slow_graph);memory.bind(0xa87f40,game.menu.option_selected);
 for(const auto& b:extra)memory.bind_bytes(b.address,b.data,b.size);bind_menu_bootstrap_gaps(memory,gaps);
}
}
