#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "game/SpellState.hpp"
#include "game/TitleOptionsMenu.hpp"
#include "sdl/ApplicationState.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::u32;
using th13::i32;
using App = th13::sdl::ApplicationState;
using th13::sdl::Destination;
using th13::sdl::PendingFileEvent;
using EventKind = PendingFileEvent::Kind;
using th13::GameInput;
using th13::SpellEvent;
using th13::SpellEventKind;
using th13::kSpellCapture;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;
constexpr u32 kShift = 0x08;

const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

std::string run_output_path(const char* name) {
    const char* dir = std::getenv("TH13_NATIVE_RUN_DIR");
    if (!dir || !*dir) return name;
    return std::string(dir) + "/" + name;
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
    ::setenv("TH13_SAVE_PATH", run_output_path(save_name).c_str(), 1);
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

bool boot_live_run(App& app, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == i32(Destination::Run) && app.failure.empty();
}

bool drive(App& app, GameInput& input, u32 held) {
    input.update(held);
    if (!app.update(input, false)) return false;
    if (!app.commit(kFps)) return false;
    return app.present();
}

bool encode_pinned(const App& app, std::vector<u8>& out) {
    th13::ReplaySaveMeta meta;
    meta.date = 1;
    meta.slow_numerator = 0.0;
    meta.slow_denominator = 1099511627776.0;
    return app.recorder.encode(out, "t7se", 0, meta);
}

bool bytes_equal(const std::vector<u8>& a, const std::vector<u8>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

SpellEvent begin_event(i32 slot, const char* name) {
    SpellEvent event;
    event.kind = SpellEventKind::Begin;
    event.state.id = slot;
    event.state.name = name;
    return event;
}

i32 total_frames(const App& app) {
    return app.files.ordinary_records[size_t(app.progress.character)].total_frames;
}

struct DrawProbe {
    enum class Mode { RunBaffle, BaffleLast, NextStage, AfterEnding, Title, Ending };
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;
    Mode mode = Mode::RunBaffle;
    int fires = 0;
    bool observed_ok = false;
    static i32 run(void* p) {
        auto& probe = *static_cast<DrawProbe*>(p);
        App& app = *probe.app;
        ++probe.fires;
        switch (probe.mode) {
        case Mode::RunBaffle:
            probe.observed_ok = app.pending_load == 0 && app.title != nullptr && app.run == nullptr;
            break;
        case Mode::BaffleLast:
            probe.observed_ok = app.run != nullptr && app.pending_load == 0 &&
                                app.loading_prayer != 0 && app.progress.stage_frame == 1;
            break;
        case Mode::NextStage:
            probe.observed_ok = app.run != nullptr && !app.stage_transition_loading;
            break;
        case Mode::AfterEnding:
            probe.observed_ok = app.run != nullptr &&
                                app.pending_destination == i32(Destination::AfterEnding);
            break;
        case Mode::Title:
            probe.observed_ok = app.run != nullptr && app.pause != nullptr && app.pause->active();
            break;
        case Mode::Ending:
            probe.observed_ok = app.run != nullptr &&
                                app.pending_destination == i32(Destination::Ending);
            break;
        }
        probe.callback.enabled = false;
        return i32(th13::FrameAction::Continue);
    }
    bool attach(App& target, Mode m) {
        app = &target;
        mode = m;
        callback.owner = this;
        callback.enabled = true;
        callback.run = &DrawProbe::run;
        if (target.scheduler.add(callback, th13::FramePass::Draw, 73) < 0) return false;
        attached_scheduler = &target.scheduler;
        return true;
    }
    void detach() {
        if (attached_scheduler) attached_scheduler->remove(callback);
        attached_scheduler = nullptr;
        app = nullptr;
    }
    ~DrawProbe() { detach(); }
};

struct KindSpan {
    int clear = 0, draw = 0;
    int first_clear = -1, first_draw = -1;
};

KindSpan scan_span(const std::string& text, size_t begin, size_t end) {
    KindSpan span;
    size_t pos = begin;
    int index = 0;
    while (pos < end) {
        const size_t line_end = text.find('\n', pos);
        const size_t stop = line_end == std::string::npos || line_end > end ? end : line_end;
        const std::string line = text.substr(pos, stop - pos);
        const bool is_clear = line.find("\"kind\":\"clear\"") != std::string::npos;
        const bool is_draw = line.find("\"kind\":\"draw\"") != std::string::npos;
        if (is_clear) {
            if (span.first_clear < 0) span.first_clear = index;
            ++span.clear;
        }
        if (is_draw) {
            if (span.first_draw < 0) span.first_draw = index;
            ++span.draw;
        }
        ++index;
        if (line_end == std::string::npos || line_end >= end) break;
        pos = line_end + 1;
    }
    return span;
}

}

TEST(side_effect_transition_probes_five_paths) {
    App app;
    CHECK(boot_live_run(app, "t7se-paths-save.bin"));
    if (app.failure.empty()) {
        GameInput input;

        CHECK_EQ(app.pending_destination, i32(Destination::Run));
        {
            DrawProbe probe;
            CHECK(probe.attach(app, DrawProbe::Mode::RunBaffle));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);
            CHECK_EQ(app.pending_load, 1u);
            CHECK_EQ(app.loading_frames, 0u);
            CHECK(app.loading_prayer != 0);
            CHECK_EQ(app.pending_destination, i32(Destination::Run));
            probe.detach();
        }
        {
            DrawProbe probe;
            CHECK(probe.attach(app, DrawProbe::Mode::BaffleLast));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);
            CHECK_EQ(app.pending_load, 0u);
            CHECK(app.run != nullptr);
            CHECK_EQ(app.progress.stage_frame, 1);
            probe.detach();
        }
        for (int F = 1; F <= 2; ++F) {
            CHECK(drive(app, input, kZ));
            CHECK_EQ(app.progress.stage_frame, F + 1);
        }
        {
            DrawProbe probe;
            app.pending_destination = i32(Destination::NextStage);
            CHECK(probe.attach(app, DrawProbe::Mode::NextStage));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);
            CHECK(app.stage_transition_loading);
            CHECK(app.run != nullptr);
            CHECK_EQ(app.pending_destination, -1);
            CHECK_EQ(app.current_destination, i32(Destination::Run));
            probe.detach();
            app.stage_transition_loading = false;
        }
        CHECK(drive(app, input, 0));
        CHECK(!app.stage_transition_loading);

        {
            CHECK(app.ending == nullptr);
            if (app.ending != nullptr) return;
            DrawProbe probe;
            app.pending_destination = i32(Destination::AfterEnding);
            CHECK(probe.attach(app, DrawProbe::Mode::AfterEnding));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);
            CHECK(app.run == nullptr);
            CHECK(app.ending == nullptr);
            CHECK(app.title != nullptr);
            CHECK_EQ(app.title->state.return_reason, 3);
            CHECK_EQ(app.current_destination, i32(Destination::Title));
            CHECK_EQ(app.pending_destination, -1);
            CHECK_EQ(app.pending_load, 0u);
            probe.detach();
        }
        CHECK(app.start_game(1));
        CHECK(drive(app, input, 0));
        CHECK(drive(app, input, 0));
        CHECK(app.run != nullptr);
        for (int F = 0; F < 2; ++F) CHECK(drive(app, input, kZ));

        {
            CHECK(app.pause != nullptr);
            CHECK(app.pause->open(th13::PauseEntrance::Pause));
            CHECK(app.pause->active());
            CHECK(app.return_to_title());
            CHECK_EQ(app.pending_destination, i32(Destination::Title));
            DrawProbe probe;
            CHECK(probe.attach(app, DrawProbe::Mode::Title));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);
            CHECK(app.run == nullptr);
            CHECK(app.pause == nullptr);
            CHECK(app.title != nullptr);
            CHECK_EQ(app.pending_destination, -1);
            CHECK_EQ(app.current_destination, i32(Destination::Title));
            probe.detach();
        }
        {
            CHECK(app.start_game(1));
            CHECK(drive(app, input, 0));
            CHECK(drive(app, input, 0));
            CHECK(app.run != nullptr);
            CHECK(drive(app, input, kZ));
            app.progress.character = 0;
            app.progress.difficulty = 1;
            app.progress.continues = 0;
            app.progress.practice_mode = 0;
            app.progress.stage = 6;
            DrawProbe probe;
            app.pending_destination = i32(Destination::Ending);
            CHECK(probe.attach(app, DrawProbe::Mode::Ending));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);
            CHECK(app.run != nullptr);
            CHECK_EQ(app.pending_load, 1u);
            CHECK_EQ(app.pending_destination, i32(Destination::Ending));
            probe.detach();
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK(app.run == nullptr);
            CHECK(app.ending != nullptr);
            CHECK_EQ(app.current_destination, i32(Destination::Ending));
            CHECK_EQ(app.pending_destination, -1);
            CHECK_EQ(app.pending_load, 0u);
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
        }
    }
}

TEST(side_effect_ending_clear_migration_trace) {
    App app;
    CHECK(boot_live_run(app, "t7se-trace-save.bin"));
    if (app.failure.empty()) {
        GameInput input;
        CHECK(drive(app, input, 0));
        CHECK(drive(app, input, kZ));
        app.progress.character = 0;
        app.progress.difficulty = 1;
        app.progress.continues = 0;
        app.progress.practice_mode = 0;
        app.progress.stage = 6;
        app.title.reset();
        app.pending_destination = i32(Destination::Ending);
        CHECK(app.apply_destination());
        CHECK(app.failure.empty());
        CHECK(app.ending != nullptr);
        CHECK(app.run == nullptr);
        CHECK_EQ(app.pending_load, 0u);

        const std::string trace_path = run_output_path("t7se-ending-trace.jsonl");
        CHECK(app.graphics.submission_trace.open(trace_path.c_str()));
        if (!app.graphics.submission_trace.active()) return;
        constexpr int kTracedFrames = 60;
        std::vector<long> update_ends;
        bool drive_failed = false;
        for (int frame = 0; frame < kTracedFrames && !drive_failed; ++frame) {
            input.update(0);
            if (!app.update(input, false) || !app.failure.empty()) {
                std::printf("  失败 ending update f%d: %s\n", frame, app.failure.c_str());
                drive_failed = true;
                break;
            }
            const long mark = app.graphics.submission_trace.output_position();
            CHECK(mark >= 0);
            if (mark < 0) { drive_failed = true; break; }
            update_ends.push_back(mark);
            if (!app.commit(kFps) || !app.present() || !app.failure.empty()) {
                std::printf("  失败 ending present f%d: %s\n", frame, app.failure.c_str());
                drive_failed = true;
            }
        }
        CHECK(app.graphics.submission_trace.close());
        CHECK(app.graphics.submission_trace.good());
        if (drive_failed || update_ends.size() != size_t(kTracedFrames)) return;

        std::FILE* trace = std::fopen(trace_path.c_str(), "rb");
        CHECK(trace != nullptr);
        if (!trace) return;
        std::string text;
        char buffer[4096];
        size_t got = 0;
        while ((got = std::fread(buffer, 1, sizeof buffer, trace)) > 0)
            text.append(buffer, got);
        std::fclose(trace);

        size_t boundary = 0;
        int total_present_draws = 0;
        int unordered_present_spans = 0;
        int update_clear_total = 0, present_clear_total = 0, draw_frames = 0;
        for (int frame = 0; frame < kTracedFrames; ++frame) {
            const size_t update_end = size_t(update_ends[size_t(frame)]);
            CHECK(update_end <= text.size());
            const KindSpan update_span = scan_span(text, boundary, update_end);
            const size_t next_boundary =
                frame + 1 < kTracedFrames ? size_t(update_ends[size_t(frame) + 1]) : text.size();
            const KindSpan present_span = scan_span(text, update_end, next_boundary);
            boundary = next_boundary;
            update_clear_total += update_span.clear;
            present_clear_total += present_span.clear;
            total_present_draws += present_span.draw;
            if (present_span.draw > 0) ++draw_frames;
            if (update_span.clear != 0)
                std::printf("  失败 trace f%d: update 段含 %d 个 Clear 项（clear 未迁出模拟段）\n",
                            frame, update_span.clear);
            CHECK_EQ(update_span.clear, 0);
            CHECK(present_span.clear >= 1);
            if (present_span.draw > 0) {
                CHECK(present_span.first_clear >= 0);
                CHECK(present_span.first_draw >= 0);
                if (present_span.first_clear >= present_span.first_draw) {
                    ++unordered_present_spans;
                    std::printf("  失败 trace f%d: present 段首 Clear(%d) 不先于首 Draw(%d)\n",
                                frame, present_span.first_clear, present_span.first_draw);
                }
            }
        }
        std::printf("# trace: %d 帧 update 段 Clear 合计=%d，present 段 Clear 合计=%d、"
                    "Draw 合计=%d（含图元帧=%d，乱序帧=%d）\n",
                    kTracedFrames, update_clear_total, present_clear_total,
                    total_present_draws, draw_frames, unordered_present_spans);
        CHECK_EQ(update_clear_total, 0);
        CHECK(present_clear_total >= kTracedFrames);
        CHECK(total_present_draws >= 1);
        CHECK_EQ(unordered_present_spans, 0);
    }
}

TEST(side_effect_reconcile_record_once) {
    constexpr int kFrames = 60;
    App app;
    CHECK(boot_live_run(app, "t7se-rec-save.bin"));
    if (app.failure.empty()) {
        GameInput input;
        input.update(kShift);
        CHECK(drive(app, input, 0));
        CHECK_EQ(app.pending_load, 1u);
        CHECK_EQ(total_frames(app), 0);
        CHECK_EQ(app.ordinary_records_dirty, false);
        CHECK_EQ(app.pending_play_frames_, 0u);

        auto pattern = [](u32 f) {
            u32 held = 0;
            if (f % 12 < 7) held |= kZ;
            if (f % 19 >= 11 && f % 19 < 15) held |= 0x80u;
            return held;
        };
        int advanced_total = 0;
        for (int f = 0; f < kFrames; ++f) {
            input.update(f == 0 ? (kShift | kZ) : pattern(u32(f)));
            const i32 stage_pre = app.progress.stage_frame;
            const u32 frames_pre = app.frames;
            const i32 mirror_pre = total_frames(app);
            const bool dirty_pre = app.ordinary_records_dirty;
            const u32 pending_pre = app.pending_play_frames_;
            CHECK(app.update(input, false));
            CHECK(app.failure.empty());
            const bool advanced = app.progress.stage_frame != stage_pre;
            CHECK_EQ(total_frames(app), mirror_pre);
            CHECK_EQ(app.ordinary_records_dirty, dirty_pre);
            CHECK_EQ(app.frames, frames_pre);
            CHECK_EQ(app.pending_play_frames_, pending_pre + (advanced ? 1u : 0u));
            if (advanced) {
                ++advanced_total;
                CHECK(!app.recorder.pending_inputs.empty());
                if (app.recorder.pending_inputs.empty()) return;
                const auto& slot = app.recorder.pending_inputs.back();
                CHECK_EQ(slot.frame, frames_pre);
                CHECK_EQ(slot.slow, u8(60));
                CHECK_EQ(slot.edge_reset, f == 0);
            }
            int commits = 1;
            if (f >= 20 && f <= 22) commits = 0;
            if (f == 40) commits = 2;
            for (int c = 0; c < commits; ++c) {
                CHECK(app.commit(kFps));
                CHECK(app.failure.empty());
                if (c == 0) {
                    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));
                    CHECK_EQ(app.pending_play_frames_, 0u);
                    CHECK_EQ(total_frames(app), advanced_total);
                    CHECK_EQ(app.ordinary_records_dirty, true);
                    CHECK_EQ(app.frames, frames_pre + 1u);
                } else {
                    CHECK_EQ(total_frames(app), advanced_total);
                    CHECK_EQ(app.pending_play_frames_, 0u);
                    CHECK_EQ(app.frames, frames_pre + 2u);
                }
            }
            if (commits == 0) CHECK_EQ(app.pending_play_frames_, u32(f - 19));
            CHECK(app.present());
            CHECK(app.failure.empty());
        }
        CHECK_EQ(advanced_total, kFrames);
        CHECK_EQ(total_frames(app), kFrames);
        std::vector<u8> enc;
        CHECK(encode_pinned(app, enc));
        th13::ReplayFile decoded;
        CHECK(decoded.decode(enc.data(), enc.size()));
        CHECK_EQ(decoded.stages.size(), size_t(1));
        if (decoded.stages.empty()) return;
        const th13::ReplayStage& stage = decoded.stages.front();
        CHECK_EQ(stage.frame_count, u32(kFrames));
        for (u32 i = 0; i < stage.frame_count; ++i) {
            const th13::ReplayFrame frame = stage.frame(i);
            const u32 f = i == 0 ? (kShift | kZ) : pattern(i);
            CHECK_EQ(u32(frame.state), f);
            if (u32(frame.state) != f) break;
        }
        CHECK(app.recorder.ready());
        CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
        app.update_spell_time(true);
        app.update_spell_time(false);
        CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(1));
        if (app.recorder.pending_spell_times.size() == 1) {
            const auto& entry = app.recorder.pending_spell_times.front();
            CHECK_EQ(entry.index, 0u);
            CHECK_EQ(entry.packed, app.spell_time_packed);
            std::vector<u8> before;
            CHECK(encode_pinned(app, before));
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            CHECK(app.commit(kFps));
            CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
            CHECK_EQ(app.spell_time_packed, entry.packed);
            std::vector<u8> after;
            CHECK(encode_pinned(app, after));
            CHECK(!bytes_equal(after, before));
            th13::ReplayFile decoded2;
            CHECK(decoded2.decode(after.data(), after.size()));
            CHECK_EQ(decoded2.stages.size(), size_t(1));
            if (!decoded2.stages.empty()) {
                const th13::ReplayStage& st = decoded2.stages.front();
                CHECK(st.header.size() >= size_t(0xC4));
                const u32 stored = u32(st.header[0x70]) | u32(st.header[0x71]) << 8 |
                                   u32(st.header[0x72]) << 16 | u32(st.header[0x73]) << 24;
                CHECK_EQ(i32(stored), entry.packed);
            }
        }
    }
}

TEST(side_effect_file_event_queue_full) {
    App app;
    CHECK(pin_boot(app, "t7se-queue-save.bin"));
    if (app.failure.empty()) {
        GameInput input;
        app.progress.character = 0;
        app.progress.selected_spell_id = 0;
        CHECK(app.record_spell_event(begin_event(2, "T7queue")));
        CHECK(app.record_spell_practice_score(12345));
        CHECK(app.change_pause_option(0, +10));
        CHECK(app.change_pause_option(0, -30));
        CHECK(app.record_music_unlocked(3));
        app.mark_heard(5);
        CHECK(app.persist_options(th13::TitleOptionVolumes{40, 60}));
        CHECK(app.record_result_entry("T7QUEUE", 8));

        const u32 capture_frame = app.recorder.capture_frame;
        CHECK_EQ(app.pending_file_events_.size(), size_t(8));
        {
            const EventKind expected[] = {
                EventKind::SpellEvent, EventKind::SpellPracticeScore,
                EventKind::PauseOption, EventKind::PauseOption,
                EventKind::MusicUnlocked, EventKind::MarkHeard,
                EventKind::PersistOptions, EventKind::ResultEntry};
            for (size_t i = 0; i < 8 && i < app.pending_file_events_.size(); ++i) {
                CHECK(app.pending_file_events_[i].kind == expected[i]);
                CHECK_EQ(app.pending_file_events_[i].capture_frame, capture_frame);
            }
        }
        const i32 attempts_before =
            app.files.spell_counters(0, 2) ? app.files.spell_counters(0, 2)->attempts[0] : -1;
        const i32 best_before =
            app.files.spell_counters(0, 0) ? app.files.spell_counters(0, 0)->practice_best_score : -1;
        const i32 bgm_before = app.files.bgm_volume;
        const i32 eff_before = app.files.effect_volume;
        const i32 dev_bgm_before = app.audio_device.music_volume;
        const i32 dev_eff_before = app.audio_device.effect_volume;
        const bool heard3_before = app.files.music_heard(3);
        const bool heard5_before = app.files.music_heard(5);
        CHECK(attempts_before >= 0 && best_before >= 0);
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_before);
        CHECK_EQ(app.files.spell_counters(0, 0)->practice_best_score, best_before);
        CHECK_EQ(app.files.bgm_volume, bgm_before);
        CHECK_EQ(app.files.effect_volume, eff_before);
        CHECK_EQ(app.audio_device.music_volume, dev_bgm_before);
        CHECK_EQ(app.audio_device.effect_volume, dev_eff_before);
        CHECK_EQ(app.files.music_heard(3), heard3_before);
        CHECK_EQ(app.files.music_heard(5), heard5_before);
        CHECK(app.title->option_menu().error.empty());

        std::vector<th13::sdl::AudioDevice::RequestObservation> pan_log;
        app.audio_device.request_observer =
            [&pan_log](const th13::sdl::AudioDevice::RequestObservation& obs) {
                pan_log.push_back(obs);
            };
        CHECK(app.set_voice_pan(55, 100.0f));
        CHECK_EQ(pan_log.size(), size_t(1));
        CHECK_EQ(pan_log.front().kind, th13::sdl::AudioDevice::RequestObservation::Kind::Pan);
        CHECK_EQ(app.pending_voice_pans_.size(), size_t(1));
        CHECK_EQ(app.pending_voice_pans_.front().id, 55);

        CHECK(app.commit(kFps));
        CHECK(app.failure.empty());
        CHECK_EQ(app.pending_file_events_.size(), size_t(0));
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_before + 1);
        CHECK(app.files.spell_counters(0, 0)->practice_best_score >= 12340);
        CHECK_EQ(app.files.bgm_volume, 40);
        CHECK_EQ(app.audio_device.music_volume, 70);
        CHECK_EQ(app.files.effect_volume, 60);
        CHECK_EQ(app.audio_device.effect_volume, eff_before);
        CHECK_EQ(app.files.music_heard(3), true);
        CHECK_EQ(app.files.music_heard(5), true);
        CHECK(app.title->option_menu().error.empty());
        CHECK_EQ(pan_log.size(), size_t(1));
        CHECK_EQ(app.pending_voice_pans_.size(), size_t(0));

        const i32 attempts_mid = app.files.spell_counters(0, 2)->attempts[0];
        const i32 bgm_mid = app.files.bgm_volume;
        CHECK(app.commit(kFps));
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_mid);
        CHECK_EQ(app.files.bgm_volume, bgm_mid);
        CHECK_EQ(app.files.music_heard(3), true);
        CHECK_EQ(pan_log.size(), size_t(1));
        app.audio_device.request_observer = nullptr;

        CHECK(app.start_game(1));
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
        CHECK(app.recorder.ready());
        CHECK_EQ(app.pending_file_events_.size(), size_t(0));
        CHECK(app.save_replay_slot(1, "T7SE"));
        CHECK_EQ(app.pending_file_events_.size(), size_t(0));
        CHECK(app.replay_slots[0].decoded != nullptr);
        CHECK_EQ(app.replay_slots[0].name, std::string("th13_01.rpy"));
    }
}

int main() { return th13test::run_all("m1.side_effect_boundary"); }
