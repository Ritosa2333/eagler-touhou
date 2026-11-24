#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/FrameScheduler.hpp"
#include "game/GameHud.hpp"

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
        CHECK(load_bank(manager, 3, synthetic_anm(8, trivial_scripts(172))));
        CHECK(load_bank(manager, 2, synthetic_anm(302, trivial_scripts(46))));
        std::ifstream file("artifacts/reference/raw/sig.anm", std::ios::binary);
        std::vector<u8> bytes{std::istreambuf_iterator<char>(file), {}};
        CHECK(!bytes.empty());
        CHECK(manager.load(1, bytes.data(), u32(bytes.size())));
        progress.practice_mode = 1;
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
}

TEST(accelerated_entry_keeps_opening_signature_past_battle_frame_three) {
    SignatureHarness harness;
    CHECK(harness.hud.initialize(HudEntryContext{0, true, false}));
    CHECK(harness.hud.opening_signature != 0);
    CHECK(harness.hud.opening_signature_right != 0);
    CHECK_EQ(harness.signature_vms(), 2u);
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
        harness.progress.run_mode_flags |= 0x20u;
        CHECK(harness.hud.initialize(HudEntryContext{}));
        CHECK_EQ(harness.hud.opening_signature, 0u);
        CHECK_EQ(harness.signature_vms(), 0u);
    }
}

int main() { return th13test::run_all("开场签名幕生命周期"); }
