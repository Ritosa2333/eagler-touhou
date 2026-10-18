// Browser frame loop for the TH_SDL3 build. One 60 Hz game tick per display
// deadline (FrameCadence skip-expired-single-tick semantics); the recovered
// frame schedulers (unlimited/timed/present-paced) run unchanged inside each
// tick. Presentation stays locked to the authored fixed tick (no high-refresh
// interpolation — TH20's scene graph is driven by scheduler callbacks, not a
// separable presentation draw; documented).
#include "../../../source_reconstruction/program_entry/program_entry.hpp"
#include "../../../source_reconstruction/program_entry/unrecovered_dependencies.hpp"
#include "../platform/Audio.hpp"
#include "../platform/Time.hpp"
#include "FrameCadence.hpp"

#include <SDL3/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <exception>

EM_JS(int, browser_prepare_frame, (), { return Module['runtimePrepare'] ? Module['runtimePrepare']() : 0; });
EM_JS(void, browser_finish_frame, (int result, double milliseconds), { Module['runtimeFinish'](result, milliseconds); });
EM_JS(void, browser_loop_stopped, (), { if (Module['runtimeStopped']) Module['runtimeStopped'](); });
extern "C" void sdl_native_input();

namespace pe = th20::source::program_entry;
namespace u = th20::source::program_entry::unrecovered;

namespace {
bool running = false, suspended = false;
unsigned loop_epoch = 0;
double elapsed = 0, last = -1, audio_remainder = 0, callback_begin = 0;
touhou::sdl::FrameCadence cadence;

// One poll_message iteration of the recovered main loop. Returns the original
// loop_status: 0 continue, nonzero session finished (restart is unreachable on
// the browser runtime because display modes never change).
int game_tick() try {
    auto& w = pe::window_state;
    auto& g = pe::graphics_state;
    auto& config = g.configuration;
    if (w.quit_requested != 0) return 1;
    if (pe::device(g) == nullptr) return 0;
    // The semantic device cannot be lost and the display mode never changes
    // on the canvas, so the cooperative/reset branch is never taken.
    int status;
    if ((w.flags & 4u) != 0) status = pe::run_unlimited_frame(w);
    else if (g.presentation.PresentationInterval == 1 && config.frame_skip == 0) status = pe::run_present_paced_frame(w);
    else status = pe::run_timed_frame(w);
    if (status != 0) return status;
    pe::graphics_event_flags &= ~8u;
    return 0;
} catch (const std::exception& e) {
    std::fprintf(stderr, "game_tick: exception: %s\n", e.what());
    return -1;
} catch (...) {
    std::fprintf(stderr, "game_tick: unknown exception\n");
    return -1;
}

EM_BOOL frame(double timestamp, void* epoch) try {
    if (!running || uintptr_t(epoch) != loop_epoch) return EM_FALSE;
    const double now = timestamp / 1000., delta = last < 0 ? 0 : std::max(0., now - last);
    last = now;
    callback_begin = emscripten_get_now();
    const int ready = browser_prepare_frame();
    if (!running) return EM_FALSE;
    if (ready <= 0 || suspended) {
        th20::web::audio::pause(true);
        cadence.reset();
        return EM_TRUE;
    }
    th20::web::audio::pause(false);
    elapsed += delta;
    audio_remainder += delta * 1000;
    const auto milliseconds = std::uint32_t(std::floor(audio_remainder));
    audio_remainder -= milliseconds;
    th20::web::audio::advance(milliseconds);
    int result = 0;
    sdl_native_input();
    if (cadence.advance(delta) != 0) result = game_tick();
    th20::web::audio::pump();
    browser_finish_frame(result, emscripten_get_now() - callback_begin);
    return running ? EM_TRUE : EM_FALSE;
} catch (const std::exception& e) {
    static unsigned reported = 0;
    if (reported++ < 5) std::fprintf(stderr, "frame: exception: %s\n", e.what());
    return running ? EM_TRUE : EM_FALSE;
} catch (...) {
    static unsigned reported = 0;
    if (reported++ < 5) std::fprintf(stderr, "frame: unknown exception\n");
    return running ? EM_TRUE : EM_FALSE;
}
}

extern "C" {
__attribute__((export_name("sdl_loop_time"))) double sdl_loop_time() {
    return elapsed + (running && !suspended ? std::max(0., emscripten_get_now() - callback_begin) / 1000. : 0.);
}
// A deterministic, stopped-loop entry point for replay/regression runners.
__attribute__((export_name("sdl_loop_tick"))) int sdl_loop_tick(double seconds, std::uint32_t milliseconds) {
    if (running) return -1;
    elapsed += seconds;
    th20::web::audio::advance(milliseconds);
    sdl_native_input();
    return game_tick();
}
__attribute__((export_name("sdl_loop_pause"))) void sdl_loop_pause(std::uint32_t pause) {
    suspended = pause != 0;
    last = -1;
    cadence.reset();
    th20::web::audio::pause(pause != 0);
}
__attribute__((export_name("sdl_loop_start"))) void sdl_loop_start() {
    elapsed = audio_remainder = 0;
    cadence.reset();
    last = -1;
    callback_begin = emscripten_get_now();
    running = true;
    emscripten_request_animation_frame_loop(frame, reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
__attribute__((export_name("sdl_loop_stop"))) void sdl_loop_stop() {
    if (!running) return;
    running = false;
    ++loop_epoch;
    th20::web::audio::pause(true);
    browser_loop_stopped();
}
}
