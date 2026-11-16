// 开场签名幕（sig.anm 槽 1）生命周期单测：0x44c0d0 标题线程尾段的卸载语义。
// 反汇编证据（0044c0fc-0044c152）：mov edi,0xb4; cmp [TitleInf+0x5e4],edi;
// test [0x4dcc5c],0x180; 任一成立即 call 0x46e620 卸载 [ANM mgr+0xb77b38]
// （槽 1 = sig.anm）并 free。0x439ce0 在 boot 期建 VM 并每趟 ++[TitleInf+0x5e4]。
// 原生 gate-01 battle-draws 全窗 3330 帧每帧 2 个 sig op（6660 条，含
// frame 3..3329）——加速直入不跑标题线程，VM 存续整场；旧实现按 terra-udsr
// 5 帧短窗误读成「只画到 battle frame 3」并在 frame 4 退休，是 boot-draw
// 窗口首差的根因。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/GameHud.hpp"
#include "game/FrameScheduler.hpp"

#include <fstream>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {
struct SignatureHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    FrameScheduler scheduler;
    GameHud hud{manager, progress, player, score, 3, 2};
    struct SoundStub : HudFrameServices {
        bool hud_sound(i32) override { return true; }
    } sound;
    SignatureHarness() {
        // front/ascii 合成 bank 与 hud_test 同构（残机 30-37、灵击 38-45、
        // 难度 79/85、灵界计 171；ascii 302 sprites 供 boss 计时池）。
        CHECK(load_bank(manager, 3, synthetic_anm(8, trivial_scripts(172))));
        CHECK(load_bank(manager, 2, synthetic_anm(302, trivial_scripts(46))));
        std::ifstream file("artifacts/reference/raw/sig.anm", std::ios::binary);
        std::vector<u8> bytes{std::istreambuf_iterator<char>(file), {}};
        CHECK(!bytes.empty());
        CHECK(manager.load(1, bytes.data(), u32(bytes.size())));
        progress.practice_mode = 1;  // 回放/调试直入（begin_replay_stage）
    }
    u32 signature_vms() const {
        u32 found = 0;
        auto& registry = const_cast<AnmManager&>(manager).registry;
        for (u32 i = 0; i < registry.ordered_count(false); ++i) {
            const auto* vm = registry.find(registry.ordered_handle(false, i));
            if (vm && vm->resource == manager.resource(1)) ++found;
        }
        return found;
    }
};
}  // namespace

TEST(accelerated_entry_keeps_opening_signature_past_battle_frame_three) {
    SignatureHarness harness;
    CHECK(harness.hud.initialize(HudEntryContext{0, true, false}));
    CHECK(harness.hud.opening_signature != 0);
    CHECK(harness.hud.opening_signature_right != 0);
    CHECK_EQ(harness.signature_vms(), 2u);
    // 原生全窗每帧 2 个 sig op：超过旧退休点（battle frame 3）后 VM 仍须在。
    for (i32 frame = 0; frame < 20; ++frame)
        CHECK(harness.hud.update(HudFrameContext{}, harness.sound));
    CHECK(harness.hud.opening_signature != 0);
    CHECK(harness.manager.registry.find(harness.hud.opening_signature) != nullptr);
    CHECK(harness.manager.registry.find(harness.hud.opening_signature_right) != nullptr);
    CHECK_EQ(harness.signature_vms(), 2u);
}

TEST(title_flow_retires_the_boot_signature_before_the_run) {
    SignatureHarness harness;
    CHECK(harness.hud.initialize(HudEntryContext{0, true, true}));
    CHECK_EQ(harness.hud.opening_signature, 0u);
    CHECK_EQ(harness.hud.opening_signature_right, 0u);
    CHECK_EQ(harness.signature_vms(), 0u);
}

TEST(normal_and_spell_practice_entries_do_not_rebuild_the_signature) {
    {
        SignatureHarness harness;
        harness.progress.practice_mode = 0;
        CHECK(harness.hud.initialize(HudEntryContext{}));
        CHECK_EQ(harness.hud.opening_signature, 0u);
        CHECK_EQ(harness.signature_vms(), 0u);
    }
    {
        SignatureHarness harness;
        harness.progress.run_mode_flags |= 0x20u;  // 符卡练习
        CHECK(harness.hud.initialize(HudEntryContext{}));
        CHECK_EQ(harness.hud.opening_signature, 0u);
        CHECK_EQ(harness.signature_vms(), 0u);
    }
}

int main() { return th13test::run_all("开场签名幕生命周期"); }
