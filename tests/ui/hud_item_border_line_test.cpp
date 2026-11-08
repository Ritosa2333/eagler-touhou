// HUD Stage 1 "Item Get Border Line!" lifecycle:
// 0x425170:509-512 creates front.anm script67 only when stage==1,
// GameInf+0x74==0 and DAT_004be7c8==0. The ANM tree owns the animation.
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/GameHud.hpp"

#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {
std::vector<u8> read_reference_anm(const char* name) {
    const std::string candidates[] = {
        std::string("../artifacts/reference/raw/") + name,
        std::string("../../../artifacts/reference/raw/") + name,
    };
    for (const auto& path : candidates) {
        std::ifstream file(path, std::ios::binary);
        if (file) return std::vector<u8>(std::istreambuf_iterator<char>(file), {});
    }
    return {};
}

struct HudHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    GameHud hud{manager, progress, player, score, 3, 2};

    bool initialize(i32 stage, i32 practice_mode, i32 continues) {
        progress.stage = stage;
        progress.practice_mode = practice_mode;
        progress.continues = continues;
        auto front = read_reference_anm("front.anm");
        auto ascii = read_reference_anm("ascii.anm");
        if (front.empty() || ascii.empty()) {
            std::printf("# 跳过：真实 front.anm/ascii.anm 不在本机\n");
            return false;
        }
        if (!load_bank(manager, 3, front)) {
            std::printf("# front.anm 装载失败：%s\n", manager.error.c_str());
            return false;
        }
        if (!load_bank(manager, 2, ascii)) {
            std::printf("# ascii.anm 装载失败：%s\n", manager.error.c_str());
            return false;
        }
        environment.fallback_sprite_resource = manager.resource(2);
        if (!hud.initialize(HudEntryContext{})) {
            std::printf("# HUD 初始化失败：%s；动画：%s\n", hud.error.c_str(),
                        manager.error.c_str());
            return false;
        }
        return true;
    }

    AnmVm* item_border() {
        for (u32 i = 0; i < manager.registry.ordered_count(false); ++i) {
            const u32 handle = manager.registry.ordered_handle(false, i);
            AnmVm* vm = manager.registry.find(handle);
            if (vm && vm->source_script == 67 && vm->resource == manager.resource(3)) return vm;
        }
        return nullptr;
    }

    u32 item_border_handle() {
        AnmVm* vm = item_border();
        return vm ? manager.registry.handle(*vm) : 0;
    }
};

void check_gate(i32 stage, i32 practice_mode, i32 continues, bool expected) {
    HudHarness harness;
    if (!harness.initialize(stage, practice_mode, continues)) return;
    CHECK_EQ(harness.item_border() != nullptr, expected);
}
}  // namespace

TEST(item_border_line_uses_original_stage_one_ordinary_gate) {
    check_gate(1, 0, 0, true);
    check_gate(2, 0, 0, false);
    check_gate(1, 1, 0, false);
    check_gate(1, 0, 1, false);
}

TEST(item_border_line_creates_real_front_script_67_tree) {
    HudHarness harness;
    if (!harness.initialize(1, 0, 0)) return;
    AnmVm* root = harness.item_border();
    CHECK(root != nullptr);
    if (!root) return;
    CHECK_EQ(root->visual.sprite, 53);
    CHECK_EQ(root->visual.layer, 19);
    CHECK_EQ(root->variables.position.x, 320.f);
    CHECK_EQ(root->variables.position.y, 144.f);
    const auto children = harness.manager.registry.children(*root);
    CHECK_EQ(children.size(), size_t(2));
    bool left = false, right = false;
    for (AnmVm* child : children) {
        left |= child->source_script == 65;
        right |= child->source_script == 66;
        CHECK_EQ(child->visual.sprite, 55);
        CHECK_EQ(child->visual.layer, 19);
    }
    CHECK(left);
    CHECK(right);
}

TEST(item_border_line_retires_after_original_198_tick_lifetime) {
    HudHarness harness;
    if (!harness.initialize(1, 0, 0)) return;
    const u32 handle = harness.item_border_handle();
    CHECK(handle != 0);
    for (i32 tick = 0; tick < 197; ++tick) {
        CHECK(harness.manager.update());
        CHECK(harness.manager.registry.find(handle) != nullptr);
    }
    CHECK(harness.manager.update());
    CHECK(harness.manager.registry.find(handle) == nullptr);
}

int main() { return th13test::run_all("HUD item border line"); }
