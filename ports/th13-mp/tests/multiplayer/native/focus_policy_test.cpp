#include "../../harness/test_framework.hpp"
#include "state_digest.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

using th13::i32;
using th13::u8;
using th13::u16;
using th13::u32;
using App = th13::sdl::ApplicationState;
using th13::GameInput;
using mpdigest::FrameDigest;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;

const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

bool read_repo_file(const std::string& relative, std::vector<u8>& out) {
    std::FILE* file = std::fopen((std::string(repo_root()) + "/" + relative).c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    out.resize(size_t(size < 0 ? 0 : size));
    const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
    std::fclose(file);
    return got == out.size();
}

bool pin_boot(App& app, const char* save_name) {
    ::setenv("TH13_SAVE_PATH", save_name, 1);
    const char* asset_root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    if (!asset_root || !*asset_root) {
        std::printf("  失败 boot: TH13_NATIVE_ASSET_ROOT 未注入\n");
        return false;
    }
    if (!app.audio_device.initialize_for_test(
            asset_root, std::string(repo_root()) + "/games/th13/thbgmogg.dat")) {
        std::printf("  失败 boot: audio initialize_for_test: %s\n", app.audio_device.error.c_str());
        return false;
    }
    if (!app.initialize(false)) {
        std::printf("  失败 boot: initialize: %s\n", app.failure.c_str());
        return false;
    }
    app.visual.reseed_restore(kBootSeed);
    for (int i = 0; i < 256; ++i) (void)app.visual.step16();
    (void)app.visual.next32();
    (void)app.visual.next32();
    app.game_random.reseed(kBootSeed);
    return true;
}

bool boot_replay_battle(App& app, const std::vector<u8>& rpy, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.draw_loading(false)) {
        std::printf("  失败 boot: draw_loading: %s\n", app.failure.c_str());
        return false;
    }
    for (unsigned frame = 0; frame < App::kDebugLoadingFrames; ++frame)
        if (!app.advance_loading_caption()) {
            std::printf("  失败 boot: advance_loading_caption: %s\n", app.failure.c_str());
            return false;
        }
    if (!app.begin_replay_run(rpy.data(), rpy.size(), 0)) {
        std::printf("  失败 boot: begin_replay_run: %s\n", app.failure.c_str());
        return false;
    }
    return app.run != nullptr && app.failure.empty();
}

bool drive_frame(App& app, GameInput& input, u32 held, bool lost_focus) {
    input.update(held);
    return app.update(input, lost_focus) && app.commit(kFps);
}

}

TEST(focus_policy_sp) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    App app;
    CHECK(boot_replay_battle(app, rpy, "focus-sp-save.bin"));
    CHECK(app.run != nullptr);
    CHECK(app.pause != nullptr);

    CHECK(app.focus_pause_enabled == true);

    CHECK(!app.pause->active());
    CHECK(app.pause->state.screen == th13::PauseScreen::Inactive);
    CHECK(app.environment.paused == false);

    GameInput input;
    for (int i = 0; i < 3; ++i) {
        CHECK(drive_frame(app, input, 0, false));
        CHECK(app.failure.empty());
        CHECK(!app.pause->active());
        CHECK(app.environment.paused == false);
    }
    const i32 stage_frame_focused = app.progress.stage_frame;
    const u32 frames_focused = app.frames;
    CHECK(stage_frame_focused > 0);

    CHECK(drive_frame(app, input, 0, true));
    CHECK(app.failure.empty());
    CHECK(app.pause->active());
    CHECK(app.pause->state.screen == th13::PauseScreen::Pause);
    CHECK(app.pause->state.previous == th13::PauseScreen::Inactive);
    CHECK(app.environment.paused == true);
    CHECK((app.progress.scene_flags & 0x10u) != 0u);
    CHECK(app.audio_device.statistics().music_paused == true);
    CHECK(app.progress.stage_frame == stage_frame_focused);
    CHECK(app.frames == frames_focused + 1);

    for (int i = 1; i <= 4; ++i) {
        CHECK(drive_frame(app, input, 0, true));
        CHECK(app.failure.empty());
        CHECK(app.pause->active());
        CHECK(app.pause->state.screen == th13::PauseScreen::Pause);
        CHECK(app.progress.stage_frame == stage_frame_focused);
        CHECK(app.frames == frames_focused + u32(1 + i));
    }

    for (int i = 0; i < 2; ++i) {
        CHECK(drive_frame(app, input, 0, false));
        CHECK(app.failure.empty());
        CHECK(app.pause->active());
        CHECK(app.pause->state.screen == th13::PauseScreen::Pause);
        CHECK(app.environment.paused == true);
        CHECK(app.progress.stage_frame == stage_frame_focused);
    }

    CHECK(app.focus_pause_enabled == true);
}

TEST(focus_policy_mp) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));

    App focused, lost;
    CHECK(boot_replay_battle(focused, rpy, "focus-mp-focus-save.bin"));
    CHECK(boot_replay_battle(lost, rpy, "focus-mp-lost-save.bin"));
    CHECK(focused.run != nullptr);
    CHECK(lost.run != nullptr);
    CHECK(focused.pause != nullptr && lost.pause != nullptr);

    CHECK(focused.focus_pause_enabled == true);
    CHECK(lost.focus_pause_enabled == true);
    focused.focus_pause_enabled = false;
    lost.focus_pause_enabled = false;
    CHECK(focused.focus_pause_enabled == false);
    CHECK(lost.focus_pause_enabled == false);

    const u32 frames_at_start = lost.frames;
    const i32 stage_frame_at_start = lost.progress.stage_frame;

    GameInput in_focus, in_lost;
    u32 prev_frames = 0;
    for (int frame = 0; frame < 120; ++frame) {
        const bool lost_focus = (frame % 7) != 6;
        CHECK(drive_frame(focused, in_focus, 0, false));
        CHECK(drive_frame(lost, in_lost, 0, lost_focus));
        CHECK(focused.failure.empty());
        CHECK(lost.failure.empty());
        CHECK(!focused.pause->active());
        CHECK(!lost.pause->active());
        CHECK(lost.environment.paused == false);
        if (frame == 0) {
            prev_frames = lost.frames;
        } else {
            CHECK(lost.frames == prev_frames + 1);
            prev_frames = lost.frames;
        }
        const FrameDigest df = mpdigest::capture(focused);
        const FrameDigest dl = mpdigest::capture(lost);
        if (df != dl) {
            std::printf("  失败 frame %d: lost_focus=%d digest diff: %s\n",
                        frame, lost_focus ? 1 : 0, df.diff_against(dl).c_str());
            CHECK(false);
        } else {
            CHECK(true);
        }
    }

    CHECK(lost.frames == focused.frames);
    CHECK(lost.frames == frames_at_start + 120u);
    CHECK(lost.progress.stage_frame == focused.progress.stage_frame);
    CHECK(lost.progress.stage_frame > stage_frame_at_start);
    CHECK(lost.environment.paused == false);
    CHECK(focused.environment.paused == false);
    CHECK(lost.focus_pause_enabled == false);
    CHECK(focused.focus_pause_enabled == false);

    CHECK(drive_frame(lost, in_lost, 0x100u, true));
    CHECK(lost.failure.empty());
    CHECK(lost.pause->active());
    CHECK(lost.pause->state.screen == th13::PauseScreen::Pause);
    CHECK(lost.pause->state.previous == th13::PauseScreen::Inactive);
    CHECK(lost.environment.paused == true);

    CHECK(drive_frame(lost, in_lost, 0, true));
    CHECK(lost.failure.empty());
    CHECK(lost.pause->active());
    CHECK(lost.pause->state.screen == th13::PauseScreen::Pause);
    CHECK(lost.environment.paused == true);
}

int main() { return th13test::run_all("m1.unit_focus_policy_sp/mp"); }
