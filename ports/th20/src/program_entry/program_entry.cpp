#include "program_entry.hpp"
#include "unrecovered_dependencies.hpp"
#include "text_constants.hpp"
#include <mmsystem.h>
#include <cstring>

// Present in the original USER32 import table; some modern SDK configurations
// omit the obsolete public declaration. This remains a normal OS import.
extern "C" BOOL WINAPI WINNLSEnableIME(HWND, BOOL);

namespace th20::source::program_entry {
namespace u = unrecovered;
#ifndef TH_SDL3
namespace {
void hide_cursor() {
    while (ShowCursor(FALSE) >= 0) {}
    SetCursor(nullptr);
}
void show_cursor() { while (ShowCursor(TRUE) < 0) {} }
void restore_system_settings(WindowStatePrefix& w) { // complete 0x0041b490 body
    SystemParametersInfoW(0x11, w.saved_screen_saver, nullptr, 2);
    SystemParametersInfoW(0x55, w.saved_low_power, nullptr, 2);
    SystemParametersInfoW(0x56, w.saved_power_off, nullptr, 2);
    WINNLSEnableIME(nullptr, TRUE);
}
void release_resource(IUnknown*& resource) {
    if (resource) { resource->Release(); resource = nullptr; }
}
}

// Recovered control flow of 0x0041e7d0..0x0041f1b8. Original arguments 2..4
// are passed by CRT but unused. The labels preserve the original restart and
// cleanup edges; unavailable engine operations remain unresolved declarations.
int WINAPI recovered_win_main(HINSTANCE instance, HINSTANCE, LPSTR, int) {
    auto& w = window_state;
    auto& g = graphics_state;
    auto& config = g.configuration;
    int loop_status = 0;
    MSG message{};
    u::RuntimeListenerStorage listener;
    char log_path[0x1000];
    void* allocation = nullptr;
    HRESULT cooperative_status = S_OK;

    // 0x0041e7e8..0x0041e89b, process-local initialization.
    w.instance = instance;
    w.startup_status = 0;
    timeBeginPeriod(1);
    u::set_rounding_mode(0);
    u::lock_registry_enable(lock_registry);
    allocation = u::allocate(8);
    allocations = allocation ? u::construct_allocations(allocation) : nullptr;
    u::construct_listener(listener);
    u::install_listener(&listener);
    u::log_append(log_buffer, text_constants::start);
    if (u::fn_0041c020(instance) == -1) goto release_runtime;
    u::fn_004117a0(g, instance);
    if (u::fn_0041b4f0(w) != 0) {
        u::log_error(log_buffer, text_constants::cannot_save);
        goto release_runtime;
    }
    if (u::load_configuration(g, "th20.cfg") != 0) goto release_runtime;
    u::fn_00420f80();
    u::fn_00421040();
    u::fn_0041ae70(instance);
    if (((w.flags >> 3) & 3u) != 0) goto release_runtime;
    w.display_mode = config.saved_display_mode;
    // The original call 0x004111e0 returns constant -1; its result is unused.
    u::fn_00416d20();

restart_runtime: // 0x0041e93d
    if (u::fn_0041c320() != 0) goto release_runtime;
    if (u::create_window(w, instance) != 0) goto release_runtime;
    if (u::fn_0041c3e0(w) != 0) goto release_runtime;
    function_controller = u::make_function_controller(allocations, text_constants::function_allocation_site);
    sprite_controller = u::make_sprite_controller(allocations, text_constants::sprite_allocation_site);
    if (!is_windowed(g) || u::fn_0041d0f0(g) != 0) {
        WINNLSEnableIME(nullptr, FALSE);
        hide_cursor();
    }
    w.clock_offset = 0.0;
    w.next_update_time = u::read_clock(w);
    w.current_time = w.next_update_time;
    w.previous_time = w.current_time;
    w.current_draw_time = u::read_clock(w);
    w.previous_draw_time = w.current_draw_time;
    SetForegroundWindow(w.window); // complete original 0x0041b480 wrapper
    loop_status = u::fn_004de1f0();
    if (loop_status == -1) goto finish_session;
    if (loop_status != 0) { loop_status = 2; goto finish_session; }
    loop_status = 0;
    set_draw_counter(w, 0xfc); // assembly PUSH -4; setter stores low byte
    w.flags |= 1u;

poll_message: // 0x0041ea9c
    if (w.quit_requested != 0) goto finish_session;
    if (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
        goto poll_message;
    }
    // The original calls the getter twice, so preserve the two reads.
    if (device(g) == nullptr) goto poll_message;
#ifdef TH_SDL3
    cooperative_status = S_OK; // the semantic device cannot be lost
#else
    cooperative_status = device(g)->TestCooperativeLevel(); // vtable +0x0c
#endif
    if (cooperative_status == S_OK) {
        if (!needs_device_reset(w)) {
            if ((w.flags & 4u) != 0) {
                loop_status = run_unlimited_frame(w);
            } else if (g.presentation.PresentationInterval == 1 && config.frame_skip == 0) {
                loop_status = run_present_paced_frame(w);
            } else {
                loop_status = run_timed_frame(w);
            }
            if (loop_status != 0) goto finish_session;
            graphics_event_flags &= ~8u;
            goto poll_message;
        }
    } else if (cooperative_status != D3DERR_DEVICENOTRESET) {
        goto poll_message;
    }

    // 0x0041ebde..0x0041eeef, device recovery and display-mode transition.
    set_reset_delay(w, 10);
    if (needs_device_reset(w)) {
        if (display_mode(w) < 3) {
            GetWindowRect(w.window, &g.window_rectangle);
            g.presentation.Windowed = FALSE;
            g.presentation.BackBufferFormat = config.alternate_pixel_format ? D3DFMT_R5G6B5 : D3DFMT_A8R8G8B8;
        } else {
            g.presentation.BackBufferFormat = g.previous_backbuffer_format;
            g.presentation.FullScreen_RefreshRateInHz = 0;
            g.presentation.Windowed = TRUE;
        }
        u::fn_0041e050(w, 0);
    }
    u::fn_004dd840(g);
    u::fn_0041dbe0(sprite_controller);
    if (u::fn_0041c730(1) != 0) goto finish_session;
    u::fn_0041a2c0();
    u::fn_0041d9f0(sprite_controller);
    device_reset_countdown = 3;
    graphics_event_flags |= 8u;
    if (needs_device_reset(w)) {
        u::fn_004dbce0(g);
        if (display_mode(w) == 8 || display_mode(w) == 9) {
            SetWindowLongW(w.window, GWL_STYLE, 0);
            ShowWindow(w.window, SW_SHOW);
            const auto width = w.client_width;
            const auto height = w.client_height;
            // 0x0041ed4d calls 0x0040c6b0 with printf-like arguments, but
            // 0x0040c6b0 is an actual five-byte no-op. It does not print.
            SetWindowPos(w.window, nullptr, 0, 0, width, height, 0x40);
            WINNLSEnableIME(nullptr, FALSE);
            hide_cursor();
            w.cursor_latch = 0;
        } else if (display_mode(w) == 3) {
            SetWindowLongW(w.window, GWL_STYLE, 0x10cb0000);
            const auto width = w.client_width + 2 * GetSystemMetrics(7);
            auto height = w.client_height + 2 * GetSystemMetrics(8);
            height += GetSystemMetrics(4);
            SetWindowPos(w.window, nullptr, g.window_rectangle.left, g.window_rectangle.top, width, height, 0x60);
            ShowWindow(w.window, SW_SHOWNORMAL);
            WINNLSEnableIME(nullptr, TRUE);
            show_cursor();
        } else {
            const auto width = w.scaled_width;
            const auto height = w.scaled_height;
            SetWindowLongW(w.window, GWL_STYLE, static_cast<LONG>(0x90000000u));
            SetWindowPos(w.window, nullptr, 0, 0, width, height, 0x20);
            WINNLSEnableIME(nullptr, FALSE);
            hide_cursor();
            w.cursor_latch = 0;
        }
    }
    GetWindowRect(w.window, &g.window_rectangle);
    u::fn_004dbd70(g);
    set_device_reset(w, 0);
    goto poll_message;

finish_session: // 0x0041eef5
    config.saved_display_mode = display_mode(w);
    if (config.saved_display_mode >= 3) {
        GetWindowRect(w.window, &g.window_rectangle);
        config.saved_window_x = g.window_rectangle.left;
        config.saved_window_y = g.window_rectangle.top;
    }
    u::fn_004dd490(g);
    u::free_function_controller(allocations, function_controller);
    function_controller = nullptr;

release_runtime: // 0x0041ef5e, also reached by initial configuration failures
    u::fn_00426170(thread_registry);
    u::free_sprite_controller(allocations, sprite_controller);
    sprite_controller = nullptr;
    release_resource(g.resource_019c);
    release_resource(g.resource_01a0);
    release_resource(g.resource_01a4);
    release_device(g);
    release_direct3d(g);
    if (w.window) {
        ShowWindow(w.window, SW_HIDE);
        DestroyWindow(w.window);
        w.window = nullptr;
    }
    show_cursor();
    if (loop_status == 2) {
        u::log_restart(log_buffer);
        u::log_append(log_buffer, text_constants::restart);
        if (!is_windowed(g)) WINNLSEnableIME(nullptr, TRUE);
        for (int i = 0; i < 60; ++i) {
            if (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
        graphics_event_flags &= 0xffffff9fu;
        goto restart_runtime;
    }
    u::save_configuration(config);
    timeEndPeriod(1);
    // The original calls at 0x00548610/0x005486a0 are strcpy_s/strcat_s.
    // The assembled path is intentionally not passed to the following log
    // method: that is what the original 0x0041f106..0x0041f13d does.
    strcpy_s(log_path, sizeof(log_path), w.user_data_directory);
    strcat_s(log_path, sizeof(log_path), "log.txt");
    u::log_flush(log_buffer);
    restore_system_settings(w);
    if (allocations) u::destroy_allocations(allocations, 1);
    u::lock_registry_disable(lock_registry);
    u::destroy_listener(listener);
    // Proved MOV [ebp-0x1064],0 at 0x0041f18d, loaded into EAX at 0x0041f1a2.
    return 0;
}
#endif
}
