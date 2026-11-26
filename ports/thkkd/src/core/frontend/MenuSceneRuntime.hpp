// 统一菜单场景驱动的公开状态引用与两层调度接口。
#pragma once
#include "core/frontend/MenuScenePlatform.hpp"
#include "core/frontend/MenuSceneBindings.hpp"
#include "core/math/GameMath.hpp"
#include <memory>
namespace kaikeidou {
class MenuSceneRuntime {
 struct Impl;
 std::unique_ptr<Impl> impl_;
public:
 MenuSceneRuntime(GameStartEntryRefs game,GameStartEntryGaps& gaps,GameStartEntryReplayContext& replay,
  GameStartEntryMachineFrame& frame,MenuSceneRuntimeContext& menu,bool replay_variant,unsigned precision,
  GameMath& math,MenuScenePlatform& platform);
 ~MenuSceneRuntime();
 MenuSceneRuntime(const MenuSceneRuntime&)=delete;
 MenuSceneRuntime& operator=(const MenuSceneRuntime&)=delete;
 GameStartEntryMemory& memory();
 // 构造先绑定；宿主在初态就绪后seal，冷启动PE初值只在明确冷启动入口写入。
 void seal();
 void initialize_title();
 void initialize_title_background();
 void return_title_at(std::uint32_t original_entry);
 MenuSceneOutcome dispatch_menu_scene();
 MenuSceneOutcome step_menu_frame();
};
}
#include "core/frontend/MenuSceneRuntimeImpl.hpp"
