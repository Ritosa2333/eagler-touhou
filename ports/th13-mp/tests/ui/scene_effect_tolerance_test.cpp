#include "../anm/anm_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/AnmManager.hpp"
#include "game/RunScene.hpp"

#include <fstream>

using namespace th13;
using namespace th13test;
namespace {

TEST(scene_effect_action_table_mapping_follows_original_rows) {
    CHECK(RunScene::scene_effect_action(0) == RunScene::SceneEffectAction::Effect0);
    CHECK(RunScene::scene_effect_action(1) == RunScene::SceneEffectAction::Effect0);
    CHECK(RunScene::scene_effect_action(2) == RunScene::SceneEffectAction::Effect0);
    CHECK(RunScene::scene_effect_action(3) == RunScene::SceneEffectAction::Effect0);
    CHECK(RunScene::scene_effect_action(5) == RunScene::SceneEffectAction::Effect0);
    CHECK(RunScene::scene_effect_action(6) == RunScene::SceneEffectAction::Effect136);
    CHECK(RunScene::scene_effect_action(4) == RunScene::SceneEffectAction::None);
    CHECK(RunScene::scene_effect_action(313) == RunScene::SceneEffectAction::None);
    CHECK(RunScene::scene_effect_action(999) == RunScene::SceneEffectAction::None);
    CHECK(RunScene::scene_effect_action(-1) == RunScene::SceneEffectAction::None);
}

TEST(spawn_effect_out_of_table_script_is_graceful_noop) {
    GameRandom random{0};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
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
    CHECK(manager.spawn_effect(*vm, 2));
    CHECK(vm->geometry.circle != nullptr);
    vm->geometry.clear();
    CHECK(manager.spawn_effect(*vm, 313));
    CHECK(manager.spawn_effect(*vm, 4));
    CHECK(manager.spawn_effect(*vm, 999));
    CHECK(manager.error.empty());
    CHECK(vm->geometry.circle == nullptr);
    CHECK(vm->geometry.trail == nullptr);
    CHECK(vm->geometry.wind == nullptr);
}

}

int main() { return th13test::run_all("scene_effect_tolerance_test"); }
