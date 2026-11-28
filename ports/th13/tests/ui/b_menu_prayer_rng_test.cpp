#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/ReplayFile.hpp"
#include "game/TitleReplayMenu.hpp"

#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th13;

namespace {
std::vector<u8> asset(const char* name) {
    std::ifstream file(std::string("artifacts/reference/raw/") + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}

struct PrayerHost final : TitleReplayPlatform {
    ReplayFile file;
    i32 started_file = -1, started_stage = -1;
    int sounds = 0;
    PrayerHost() {
        file.top_header.assign(0x74, 0);
        file.stages.resize(1);
        file.stages[0].number = 1;
        file.stages[0].frame_count = 1;
        file.stages[0].header.assign(0xC4, 0);
    }
    i32 replay_count() const override { return 1; }
    const ReplayFile* replay_entry(i32 index) const override { return index == 0 ? &file : nullptr; }
    std::string replay_filename(i32) const override { return "th13_01.rpy"; }
    bool start_replay(i32 file_index, i32 stage_index) override {
        if (started_file >= 0) return false;
        started_file = file_index;
        started_stage = stage_index;
        return true;
    }
};

int prayer_vms(AnmManager& manager) {
    int count = 0;
    for (u32 bucket = 0; bucket < 33; ++bucket)
        for (const auto* vm : manager.registry.layer(bucket))
            if (vm && (vm->source_script == 17 || vm->source_script == 16)) ++count;
    return count;
}
}

TEST(menu_replay_launch_prayer_spawns_eight_times_then_retires) {
    GameRandom random;
    AnmEnvironment environment;
    environment.resolution_scale = 1;
    environment.raster_scale = 1;
    AnmManager manager{random, environment};
    const auto bytes = asset("ascii.anm");
    CHECK(!bytes.empty());
    CHECK(manager.load(2, bytes.data(), u32(bytes.size())));
    CHECK(manager.sprite_fallback(2));

    TitleState state;
    state.screen = TitleScreen::Replay;
    state.substate = 4;
    state.age.set(15);
    PrayerHost host;
    TitleReplayMenu menu{state, manager, host, TitleSceneResources{}};
    menu.play_sound = [&host](i32) {
        ++host.sounds;
        return true;
    };

    const u32 before = random.index();
    CHECK(menu.update(0x01, 0));
    CHECK_EQ(state.substate, 3);
    CHECK_EQ(random.index(), before);

    const float rate = 1.f;
    const auto frame = [&](u32 pressed = 0) {
        CHECK(menu.update(pressed, 0));
        state.age.tick(&rate);
        CHECK(manager.update(true));
        CHECK(manager.update(false));
    };

    frame();
    frame();
    CHECK_EQ(random.index(), before);
    CHECK_EQ(prayer_vms(manager), 0);
    frame();
    CHECK_EQ(random.index() - before, 24u);
    CHECK(prayer_vms(manager) > 0);

    for (int i = 3; i <= 32; ++i)
        frame();
    CHECK_EQ(host.started_file, 0);
    CHECK_EQ(host.started_stage, 0);
    CHECK_EQ(random.index() - before, 192u);
    CHECK_EQ(prayer_vms(manager), 0);
    CHECK(menu.error.empty());
}

int main() { return th13test::run_all("b_menu prayer rng contracts"); }
