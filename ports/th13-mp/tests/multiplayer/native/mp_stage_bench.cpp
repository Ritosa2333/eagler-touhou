#include "../../harness/test_framework.hpp"
#include "sdl/ApplicationState.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/WorldState.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace th13;
using Clock = std::chrono::steady_clock;
using App = sdl::ApplicationState;

namespace {
double ms_since(Clock::time_point t) {
    return std::chrono::duration<double, std::milli>(Clock::now() - t).count();
}
long env_int(const char* name, long fallback) {
    const char* v = std::getenv(name);
    return v && *v ? std::atol(v) : fallback;
}
struct Series {
    std::vector<double> v;
    void add(double x) { v.push_back(x); }
    double pct(double p) const {
        if (v.empty()) return 0;
        auto copy = v;
        std::sort(copy.begin(), copy.end());
        return copy[std::min(copy.size() - 1, size_t(copy.size() * p))];
    }
    double mean() const {
        double s = 0;
        for (double x : v) s += x;
        return v.empty() ? 0 : s / double(v.size());
    }
};
u32 script_input(i32 seat, long frame) {
    u32 held = 1;
    const long phase = (frame / 45 + seat * 7) % 8;
    switch (phase) {
        case 0: held |= 0x40; break;
        case 1: held |= 0x40 | 0x10; break;
        case 2: held |= 0x10; break;
        case 3: held |= 0x80 | 0x10; break;
        case 4: held |= 0x80; break;
        case 5: held |= 0x80 | 0x20; break;
        case 6: held |= 0x20; break;
        default: held |= 0x40 | 0x20; break;
    }
    if ((frame / 200 + seat) % 3 == 0) held |= 8;
    return held;
}
}

int main() {
    const long stage = env_int("TH13_BENCH_STAGE", 3);
    const long difficulty = env_int("TH13_BENCH_DIFF", 3);
    const long seats = env_int("TH13_BENCH_SEATS", 2);
    const long frames = env_int("TH13_BENCH_FRAMES", 30000);
    const long capture_every = env_int("TH13_BENCH_CAPTURE", 1);
    const char* assets = std::getenv("TH13_NATIVE_ASSET_ROOT");
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    if (!assets || !root) { std::puts("need TH13_NATIVE_ASSET_ROOT/REPO_ROOT"); return 2; }
    multiplayer::session.Stop();
    ::setenv("TH13_SAVE_PATH", "bench.bin", 1);
    App app;
    if (!app.audio_device.initialize_for_test(assets, std::string(root) + "/games/th13/thbgmogg.dat") ||
        !app.initialize(false)) { std::puts("init failed"); return 3; }
    app.progress.difficulty = i32(difficulty);
    app.progress.configured_lives = 9;
    const long spell_slot = env_int("TH13_BENCH_SPELL_SLOT", -1);
    if (spell_slot >= 0) {
        const auto* entry = app.spell_catalog.variant(i32(stage), i32(spell_slot), i32(difficulty));
        if (!entry) { std::puts("no such spell variant"); return 9; }
        std::printf("spell practice stage %ld slot %ld id %d rank %d count %d\n", stage, spell_slot,
                    entry->id, entry->rank, app.spell_catalog.spell_count(i32(stage)));
        app.progress.selected_spell_id = entry->id;
        app.progress.run_mode_flags = (app.progress.run_mode_flags & ~0x70u) | 0x20u;
        app.progress.practice_mode = 0;
    }
    app.progress.stage = app.progress.starting_stage = i32(stage);
    if (!app.configure_multiplayer(i32(seats), {0, 1, 3}, 0) || !app.start_game(i32(stage))) return 4;
    app.pending_destination = -1;
    app.frames = 0;
    app.game_random.reseed_restore(1234);
    app.visual.reseed_restore(1234);
    if (!app.begin_run()) { std::puts("begin_run failed"); return 5; }

    GameInput menu_input;
    std::vector<GameInput> seat_inputs(static_cast<size_t>(seats), GameInput{});
    constexpr size_t kRing = 12;
    std::vector<multiplayer::WorldState> ring(kRing);
    multiplayer::WorldState restore_probe;
    Series sim_all, present_all, capture_all, restore_all, total_all;
    Series sim_bucket, present_bucket, capture_bucket;
    std::uint64_t trace = 1469598103934665603ull;
    long bucket_start = 0;
    auto report = [&](long frame, bool final) {
        std::printf("frame %6ld stage_frame %6d  sim mean %.3f p99 %.3f | present mean %.3f p99 %.3f | capture mean %.3f p99 %.3f\n",
                    frame, app.progress.stage_frame,
                    sim_bucket.mean(), sim_bucket.pct(.99), present_bucket.mean(), present_bucket.pct(.99),
                    capture_bucket.mean(), capture_bucket.pct(.99));
        sim_bucket = present_bucket = capture_bucket = Series{};
        bucket_start = frame;
        (void)final;
    };
    for (long frame = 0; frame < frames; ++frame) {
        if (!app.run) { std::printf("run ended at frame %ld\n", frame); break; }
        auto& battle = app.run->battle;
        if (app.pause && app.pause->active()) { std::printf("paused at frame %ld\n", frame); break; }
        for (i32 s = 0; s < i32(seats); ++s) {
            seat_inputs[size_t(s)].update(script_input(s, frame));
            battle.seat_inputs[size_t(s)] = seat_inputs[size_t(s)];
            if (battle.seats[size_t(s)].invulnerable < 5) battle.seats[size_t(s)].invulnerable = 5;
        }
        menu_input.update(0);
        if (capture_every > 0 && frame % capture_every == 0) {
            auto& slot = ring[size_t(frame) % kRing];
            const auto t = Clock::now();
            const bool ok = slot.capture(app.run->battle, app.visual, nullptr,
                app.run->presentation.get(), app.run.get(), app.run_transition.handle());
            const double d = ms_since(t);
            if (!ok) { std::puts("capture failed"); return 6; }
            capture_all.add(d); capture_bucket.add(d);
            trace = (trace ^ slot.digest_core()) * 1099511628211ull;
        }
        const auto t0 = Clock::now();
        if (!(app.update(menu_input, false) && app.commit(60.f))) {
            std::printf("update failed at frame %ld: %s\n", frame, app.failure.c_str());
            return 7;
        }
        const double sim = ms_since(t0);
        const auto t1 = Clock::now();
        const bool present_ok = app.present();
        const double pres = ms_since(t1);
        if (!present_ok) { std::puts("present failed"); return 8; }
        sim_all.add(sim); sim_bucket.add(sim);
        present_all.add(pres); present_bucket.add(pres);
        total_all.add(sim + pres);
        if (frame - bucket_start + 1 == 1000) report(frame + 1, false);
        if (app.pending_destination >= 0) {
            std::printf("destination %d at frame %ld\n", app.pending_destination, frame);
            break;
        }
    }
    std::printf("TRACE %016llx\n", static_cast<unsigned long long>(trace));
    std::printf("TOTAL sim mean %.3f p95 %.3f p99 %.3f max %.3f | present mean %.3f p95 %.3f p99 %.3f max %.3f | capture mean %.3f p95 %.3f p99 %.3f max %.3f\n",
                sim_all.mean(), sim_all.pct(.95), sim_all.pct(.99), sim_all.pct(1.),
                present_all.mean(), present_all.pct(.95), present_all.pct(.99), present_all.pct(1.),
                capture_all.mean(), capture_all.pct(.95), capture_all.pct(.99), capture_all.pct(1.));
    return 0;
}
