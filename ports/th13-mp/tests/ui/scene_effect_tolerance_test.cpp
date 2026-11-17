// H-SCENE-EFFECT-TOLERANCE: ECL 334 / ANM 508 场景效果的表外 kind 不再
// fail() 卡死对局（2026-11-03 用户 Game Over 满身疮痍界面
// "Scene effect kind 313 not implemented" → 游戏卡死）。
//
// 原版依据（0x415650 capstone 直读 + 表 0x4bba80+kind*0x18 EXE 实读）：
//   - 无 kind 范围检查；行 script 字段（i16@+2）<0 → js 0x4157ba 优雅返回
//     （kind4 即 -1 行）；
//   - 表有效行仅 0..6：0/1/2/3/5 = effect.anm script0 族、6 = script136；
//   - 越界 kind 落 .data 邻区（kind313@0x4bd7d8 全零行）——原版对该输入
//     无「错误」概念。端口降级：表外 = 空操作 + stderr 留证。
#include "../anm/anm_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/AnmManager.hpp"
#include "game/RunScene.hpp"

#include <fstream>

using namespace th13;
using namespace th13test;
namespace {

TEST(scene_effect_action_table_mapping_follows_original_rows) {
    // 表内行（0x4bba80 实读）：0/1/2/3/5→script0 族；6→script136。
    CHECK(RunScene::scene_effect_action(0) == RunScene::SceneEffectAction::Effect0);
    CHECK(RunScene::scene_effect_action(1) == RunScene::SceneEffectAction::Effect0);
    CHECK(RunScene::scene_effect_action(2) == RunScene::SceneEffectAction::Effect0);
    CHECK(RunScene::scene_effect_action(3) == RunScene::SceneEffectAction::Effect0);
    CHECK(RunScene::scene_effect_action(5) == RunScene::SceneEffectAction::Effect0);
    CHECK(RunScene::scene_effect_action(6) == RunScene::SceneEffectAction::Effect136);
    // kind4 = 表行 script=-1 → 原版 0x4156a1 js 空操作。
    CHECK(RunScene::scene_effect_action(4) == RunScene::SceneEffectAction::None);
    // 表外（含用户实测的 313）：原版读 .data 邻区无错误概念 → 端口空操作。
    CHECK(RunScene::scene_effect_action(313) == RunScene::SceneEffectAction::None);
    CHECK(RunScene::scene_effect_action(999) == RunScene::SceneEffectAction::None);
    CHECK(RunScene::scene_effect_action(-1) == RunScene::SceneEffectAction::None);
}

TEST(spawn_effect_out_of_table_script_is_graceful_noop) {
    GameRandom random{0};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    // effect.anm 的 ins_300(-1) 族需要 ascii 负值精灵回落先就位（Application
    // 语义，spell 名测试同构），否则装载本身失败。
    std::ifstream ascii_file("artifacts/reference/raw/ascii.anm", std::ios::binary);
    std::vector<u8> ascii{std::istreambuf_iterator<char>(ascii_file), {}};
    CHECK(!ascii.empty());
    CHECK(manager.load(2, ascii.data(), u32(ascii.size())));
    CHECK(manager.sprite_fallback(2));
    std::ifstream file("artifacts/reference/raw/effect.anm", std::ios::binary);
    std::vector<u8> bytes{std::istreambuf_iterator<char>(file), {}};
    CHECK(!bytes.empty());
    CHECK(manager.load(8, bytes.data(), u32(bytes.size())));
    const u32 handle = manager.create(8, 0, -1, 0);
    auto* vm = manager.registry.find(handle);
    CHECK(vm != nullptr);
    // 表内 sanity：kind2 = 圆环几何。
    CHECK(manager.spawn_effect(*vm, 2));
    CHECK(vm->geometry.circle != nullptr);
    vm->geometry.clear();
    // 表外 script（313/4/999）：不得失败、不得置 error、不得建几何——
    // 旧实现此处 error="Scene effect manager not bound" 并返回 false，
    // 是 Game Over 卡死链的一环。
    CHECK(manager.spawn_effect(*vm, 313));
    CHECK(manager.spawn_effect(*vm, 4));
    CHECK(manager.spawn_effect(*vm, 999));
    CHECK(manager.error.empty());
    CHECK(vm->geometry.circle == nullptr);
    CHECK(vm->geometry.trail == nullptr);
    CHECK(vm->geometry.wind == nullptr);
}

}  // namespace

int main() { return th13test::run_all("scene_effect_tolerance_test"); }
