// ANM 句柄驱动单测：合成资源的装载/创建/中断/暂停/回收（消费 anm-pipeline
// 工作流的 AnmManager 公共面，不测其内部实现）。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

using namespace th13;
using namespace th13::test;

namespace {
struct AnmHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
};
}  // namespace

TEST(synthetic_resource_parses) {
    AnmResource resource;
    const auto bytes = synthetic_anm(2, trivial_scripts(3));
    CHECK(resource.open(bytes.data(), u32(bytes.size())));
    CHECK_EQ(resource.textures.size(), 1u);
    CHECK_EQ(resource.textures[0].name, std::string("s.png"));
    CHECK_EQ(int(resource.textures[0].kind), int(AnmTexture::Kind::External));
    CHECK_EQ(resource.sprites.size(), 2u);
    CHECK_EQ(resource.scripts.size(), 3u);
    const auto broken = synthetic_anm(1, trivial_scripts(1));
    CHECK(!resource.open(broken.data(), 3));  // 截断拒绝
}

TEST(manager_create_returns_generation_handles) {
    AnmHarness harness;
    auto bank = synthetic_anm(1, trivial_scripts(4));
    CHECK(load_bank(harness.manager, 16, bank));
    const u32 first = harness.manager.create(16, 0);
    const u32 second = harness.manager.create(16, 1);
    CHECK(first != 0);
    CHECK(second != 0);
    CHECK(first != second);
    CHECK(harness.manager.registry.find(first) != nullptr);
    CHECK(harness.manager.create(16, 99) == 0);  // 越界脚本显式失败
    CHECK(!harness.manager.error.empty());
}

TEST(interrupt_jumps_to_label) {
    AnmHarness harness;
    // script0：label 7 → sprite 5 → park；label 9 → sprite 6 → park。
    const auto bank = synthetic_anm(8, {{
        label(7), sprite(5), wait(), label(9), sprite(6), wait(), stop(),
    }});
    CHECK(load_bank(harness.manager, 16, bank));
    const u32 handle = harness.manager.create(16, 0);
    auto* vm = harness.manager.registry.find(handle);
    CHECK(vm != nullptr);
    CHECK(vm->visual.sprite == 5);  // 初值 tick 已执行到第一个 park 前
    CHECK(harness.manager.interrupt(handle, 9));
    CHECK(harness.manager.update(false));
    vm = harness.manager.registry.find(handle);
    CHECK(vm != nullptr);
    CHECK_EQ(vm->visual.sprite, 6);  // 中断跳到 label 9 分支
}

TEST(retire_clears_handle_and_instance) {
    AnmHarness harness;
    const auto bank = synthetic_anm(1, trivial_scripts(2));
    CHECK(load_bank(harness.manager, 16, bank));
    u32 handle = harness.manager.create(16, 0);
    CHECK(handle != 0);
    CHECK(harness.manager.retire(handle));
    CHECK_EQ(handle, 0u);
    CHECK(harness.manager.registry.find(0) == nullptr);
}

TEST(pause_freezes_and_resumes) {
    AnmHarness harness;
    // 计数脚本：每 tick 位置 +1（op400 直写脚本位置，mask 寻址 10013）。
    // 简化：位置插值持续 tick，暂停后插值冻结。
    const auto bank = synthetic_anm(8, {{
        AnmIns{407, 0, {60, 0, 100, 0, 0}, 0},  // 60 帧移到 (100,0)
        wait(), stop(),
    }});
    CHECK(load_bank(harness.manager, 16, bank));
    const u32 handle = harness.manager.create(16, 0);
    auto* vm = harness.manager.registry.find(handle);
    CHECK(vm != nullptr);
    for (int i = 0; i < 10; i++) CHECK(harness.manager.update(false));
    const float moving = harness.manager.registry.find(handle)->variables.position.x;
    CHECK(moving > 0.f);
    CHECK(harness.manager.pause(handle, true));
    for (int i = 0; i < 10; i++) CHECK(harness.manager.update(false));
    const float frozen = harness.manager.registry.find(handle)->variables.position.x;
    CHECK_NEAR(frozen, moving, 1e-6);
    CHECK(harness.manager.pause(handle, false));
    for (int i = 0; i < 10; i++) CHECK(harness.manager.update(false));
    CHECK(harness.manager.registry.find(handle)->variables.position.x > frozen);
}

int main() { return th13test::run_all("ANM 句柄驱动"); }
