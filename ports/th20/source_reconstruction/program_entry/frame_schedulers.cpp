#include "program_entry.hpp"
#include "unrecovered_dependencies.hpp"
#ifdef TH_SDL3
#include "platform/Time.hpp"
#endif
#include <cstring>
#include <emmintrin.h>

namespace th20::source::program_entry {
namespace u = unrecovered;
namespace {
double add64(double a, double b) noexcept { return _mm_cvtsd_f64(_mm_add_sd(_mm_set_sd(a), _mm_set_sd(b))); }
double sub64(double a, double b) noexcept { return _mm_cvtsd_f64(_mm_sub_sd(_mm_set_sd(a), _mm_set_sd(b))); }
double mul64(double a, double b) noexcept { return _mm_cvtsd_f64(_mm_mul_sd(_mm_set_sd(a), _mm_set_sd(b))); }
double div64(double a, double b) noexcept { return _mm_cvtsd_f64(_mm_div_sd(_mm_set_sd(a), _mm_set_sd(b))); }
void increment_draw_counter(WindowStatePrefix& w) noexcept {
    auto bits = static_cast<std::uint8_t>(w.draw_counter);
    bits = static_cast<std::uint8_t>(bits + 1u);
    std::memcpy(&w.draw_counter, &bits, 1);
}
// Shared statements factored from 0x419c20/0x419de0/0x41a030; all three original
// sequences use the same order. This helper is new source factoring, not a claim
// that the original binary had an extra function at an invented address.
void draw_frame() {
#ifndef TH_SDL3
    device(graphics_state)->BeginScene();                 // original vtable +0xa4
#endif
    u::prepare_sprite_draw(sprite_controller);
    render_value_005c5af8 = 0xff;
    u::fn_004dda60(graphics_state);
    scheduler::dispatch_draw(*function_controller, scheduler_environment); // recovered 0x00412aa0
    u::reset_sprite_queue(sprite_controller);
#ifdef TH_SDL3
    device(graphics_state)->bind_texture(nullptr);        // texture slot detach
#else
    device(graphics_state)->SetTexture(0, nullptr);        // original vtable +0x104
    device(graphics_state)->EndScene();                   // original vtable +0xa8
#endif
}
int update_frame() {
    u::reset_sprite_queue(sprite_controller);
    u::select_viewport(graphics_state, 2);
    const auto result = scheduler::dispatch_update(*function_controller, scheduler_environment); // recovered 0x00412810
    if (result == 0) { u::fn_004d9e30(graphics_state); return 1; }
    if (result == -1) { u::fn_004d9e30(graphics_state); return 2; }
    // 0 represents the observed "continue" branch, not an unimplemented result.
    return 0;
}
void finish_update(WindowStatePrefix& w) {
    update_duration = sub64(u::read_clock(w), w.current_time);
    // Original 0x00412540 saves ECX to a local then executes XOR eax,eax and RET.
    // Its return is unused and it has no externally observable side effect.
}
}

std::int32_t display_mode(const WindowStatePrefix& w) noexcept { return w.display_mode; }
bool needs_device_reset(const WindowStatePrefix& w) noexcept { return ((w.flags >> 1) & 1u) != 0; }
void set_draw_counter(WindowStatePrefix& w, std::uint8_t value) noexcept { std::memcpy(&w.draw_counter, &value, 1); }
void set_device_reset(WindowStatePrefix& w, std::uint32_t value) noexcept { w.flags = (w.flags & ~2u) | ((value & 1u) << 1); }
void set_reset_delay(WindowStatePrefix& w, std::uint32_t value) noexcept { w.reset_delay = value; }
Device* device(const GraphicsStatePrefix& g) noexcept { return g.device; }
BOOL is_windowed(const GraphicsStatePrefix& g) noexcept { return g.presentation.Windowed; }
void release_device(GraphicsStatePrefix& g) {
#ifdef TH_SDL3
    g.device = nullptr; // the wasm host owns the device object
#else
    if (g.device) { g.device->Release(); g.device = nullptr; }
#endif
}
void release_direct3d(GraphicsStatePrefix& g) { if (g.direct3d) { g.direct3d->Release(); g.direct3d = nullptr; } }

// 0x00419c20: flag bit 2 selects the unthrottled branch in WinMain. The deadline
// still advances when this bit is set; there is no Sleep in this scheduler.
int run_unlimited_frame(WindowStatePrefix& w) {
    if ((window_state.flags & 4u) != 0)
        while (w.next_update_time < w.current_time)
            w.next_update_time = add64(div64(1.0, 60.0), w.next_update_time);
    const auto status = update_frame();
    if (status != 0) return status;
    increment_draw_counter(w);
    if (static_cast<int>(graphics_state.configuration.frame_skip) + 1 <= static_cast<int>(w.draw_counter)) {
        draw_frame();
        w.draw_counter = 0;
        u::fn_004193e0(w);
    }
    finish_update(w);
    return 0;
}

// 0x00419de0: software-paced path. Keep strict '<', exact operation order and
// one-millisecond Sleep threshold (1.5 ms), including the no-update branch.
int run_timed_frame(WindowStatePrefix& w) {
    w.current_time = u::read_clock(w);
    if (w.current_time < w.previous_time) w.next_update_time = w.current_time;
    w.previous_time = w.current_time;
#ifdef TH_SDL3
    if (1.5 <= mul64(sub64(w.next_update_time, w.current_time), 1000.0)) web::time::sleep(1);
#else
    if (1.5 <= mul64(sub64(w.next_update_time, w.current_time), 1000.0)) Sleep(1);
#endif
    if (w.next_update_time < w.current_time) {
        while (w.next_update_time < w.current_time)
            w.next_update_time = add64(div64(1.0, 60.0), w.next_update_time);
        const auto status = update_frame();
        if (status != 0) return status;
        increment_draw_counter(w);
        if (static_cast<int>(graphics_state.configuration.frame_skip) + 1 <= static_cast<int>(w.draw_counter)) {
            draw_frame();
            w.draw_counter = 0;
            u::fn_004199a0(w);
        }
        finish_update(w);
    }
    return 0;
}

// 0x0041a030: PresentationInterval==1 and frame_skip==0 selection in WinMain.
int run_present_paced_frame(WindowStatePrefix& w) {
    const auto status = update_frame();
    if (status != 0) return status;
    increment_draw_counter(w);
    if (static_cast<int>(graphics_state.configuration.frame_skip) + 1 <= static_cast<int>(w.draw_counter)) {
        draw_frame();
        w.draw_counter = 0;
        u::fn_00419a50(w);
    }
    finish_update(w);
    return 0;
}
}
