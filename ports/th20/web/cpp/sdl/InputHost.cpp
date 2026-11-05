#include "browser-gamepad-input.hpp"
// SDL3 input host for the TH_SDL3 build, replacing DirectInput8/XInput/WinMM
// and the WMI device query. Sources: launcher keyboard bridge (sdl_key), SDL
// keyboard state, SDL gamepads (as DirectInput-kind devices) and the shared
// gesture controller (touch). Everything reported to the game is real input
// state; APIs that have no browser meaning report honest absence (XInput and
// WinMM joysticks stay unconnected).
#include "../../../src/input/input.hpp"
#include "../../../src/program_entry/program_entry.hpp"
#include "../../../src/game_session/session.hpp"
#include "../../../src/player_entity/owner.hpp"
#include "../../../src/hud_system/hud.hpp"
#include "../../../src/pause_system/pause.hpp"
#include "../../../src/platform_services/services.hpp"
#include "../../../src/replay_system/replay.hpp"
#include "../../../src/runtime_state/state.hpp"
#include "../../../src/gameplay/loading_dependencies.hpp"
namespace th20::source::gameplay { class GameController; extern GameController* controller; }
#include "../platform/Time.hpp"
#include "../../../portable/input/TouchController.hpp"
#include "../../../portable/input/MotionTrack.hpp"
#include "TouchMotion.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <algorithm>
#include <cmath>
#include <cstring>

EM_JS(int, th20_browser_keyboard, (), { return typeof Module['resetBrowserKeyboard'] === 'function'; });
EM_JS(void, th20_reset_browser_keyboard, (), { Module['resetBrowserKeyboard']?.(); });

namespace th20::source::input {
namespace {
struct Key {const char* code;const char* sdl;std::uint32_t scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"

touhou::input::TouchController gestures;
std::uint8_t synthetic_keys[256]{};
bool touch_keys[256]{};
// Continuous direct-touch direction for the current logical frame (movement
// units), plus the pulses the shared controller produced this frame so the
// browser regression check can observe the menu path.
float analog_x=0.f,analog_y=0.f;
bool analog_active=false,analog_unlimited=false,confirm_pulse=false,escape_pulse=false;
// Last motion code the shared controller produced this frame (1 = drag towards
// the finger, 2 = unlimited drag) and sticky one-shot pulse counters. The
// counters exist because a pulse lasts only a few logical frames, so a browser
// check that samples the live flags can miss it; reading the probe drains them.
int last_motion=0;
unsigned confirm_pulses=0,escape_pulses=0;
// Monotonic count of logical input samples. A browser check needs it to tell a
// gesture that was ignored apart from a frame loop that is not ticking at all.
unsigned sample_ticks=0;
bool replay_probe_unlocked=false;
// The Launcher's mobile Esc button is an out-of-band control rather than a
// gameplay touch gesture. Replay playback suppresses every touch action that
// could alter the recorded stream, but must still allow this one pause action
// exactly like a physical Esc key.
std::uint32_t launcher_escape_serial=0;
int playback_escape_ticks=0;

SDL_Gamepad* pads[4]{};
constexpr SDL_GamepadButton pad_slots[]={SDL_GAMEPAD_BUTTON_SOUTH,SDL_GAMEPAD_BUTTON_EAST,SDL_GAMEPAD_BUTTON_WEST,
    SDL_GAMEPAD_BUTTON_NORTH,SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
    SDL_GAMEPAD_BUTTON_BACK,SDL_GAMEPAD_BUTTON_START,SDL_GAMEPAD_BUTTON_LEFT_STICK,SDL_GAMEPAD_BUTTON_RIGHT_STICK,
    SDL_GAMEPAD_BUTTON_GUIDE};
constexpr SDL_GamepadAxis pad_axes[]={SDL_GAMEPAD_AXIS_LEFTX,SDL_GAMEPAD_AXIS_LEFTY,SDL_GAMEPAD_AXIS_RIGHTX,
    SDL_GAMEPAD_AXIS_RIGHTY,SDL_GAMEPAD_AXIS_LEFT_TRIGGER,SDL_GAMEPAD_AXIS_RIGHT_TRIGGER};

int pad_index(IDirectInputDevice8W* device) {
    return static_cast<int>(reinterpret_cast<std::uintptr_t>(device)) - 1;
}
void add_pad(SDL_JoystickID id) {
    for (const auto* p : pads) if (p && SDL_GetGamepadID(const_cast<SDL_Gamepad*>(p)) == id) return;
    for (auto*& p : pads) if (!p) { p = SDL_OpenGamepad(id); return; }
}
void remove_pad(SDL_JoystickID id) {
    for (auto*& p : pads) if (p && SDL_GetGamepadID(p) == id) { SDL_CloseGamepad(p); p = nullptr; }
}
void press(std::uint8_t* out, std::uint32_t vk) { out[vk] |= 0x80; }

// Scene adapter for gesture context: gameplay when a GameController exists.
touhou::input::TouchState touch_state() {
    touhou::input::TouchState s;
    const bool in_game = gameplay::controller != nullptr;
    const bool dialogue = in_game && hud::controller && hud::controller->collecting;
    // The pause menu keeps its owner for the whole stage, so its state (0 means
    // "not showing") decides whether a gesture belongs to gameplay or to the
    // menu. Menu context is what lets a tap confirm and a two-finger tap cancel
    // instead of driving the player and firing.
    const bool menu = in_game && pause::controller() && pause::controller()->state != 0;
    // A pause opened during dialogue owns the gesture until it closes. During
    // playback, touch must never alter the recorded input stream.
    const bool playback = replay::controller() && replay::controller()->mode == 1;
    s.context = menu ? 0 : playback ? 3 : dialogue ? 2 : in_game ? 1 : 0;
    if (s.context == 1) {
        auto* player = static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);
        if (player && player->state == 1) {
            s.ready = true;
            s.instance = static_cast<int>(reinterpret_cast<std::uintptr_t>(player));
            s.x = player->position_614.x;
            s.y = player->position_614.y;
            s.fast = float(player->speeds_20b4[0]) / 128.f;
            s.slow = float(player->speeds_20b4[1]) / 128.f;
        }
    }
    s.min_x = -184; s.max_x = 184; s.min_y = 32; s.max_y = 432;
    return s;
}

// The SDL implementation is the Host definition on this platform (no base
// win32 implementation exists here).
int launcher_gamepad=-1;
struct SdlHost : Host {
    BOOL keyboard(std::uint8_t* output) override {
        std::memcpy(output, synthetic_keys, 256);
        const bool* physical = th20_browser_keyboard() ? nullptr : SDL_GetKeyboardState(nullptr);
        for (const auto& k : keyboard_map)
            if ((physical && k.native != SDL_SCANCODE_UNKNOWN && physical[k.native]) || k.hosted) {
                press(output, k.vk);
                if (k.vk >= 160 && k.vk <= 165) press(output, 16 + (k.vk - 160) / 2);
            }
        for (int vk = 0; vk < 256; ++vk) if (touch_keys[vk]) press(output, std::uint32_t(vk));
        // Resolve actions through the current native keyboard bindings too.
        // Otherwise a customized Z/X binding could change the action's meaning.
        if(launcher_gamepad>=0&&controller){
            for(unsigned n=0;n<8;++n)if(launcher_gamepad&(1<<n)){
                const auto vk=controller->mappings[0].keyboard[n];
                if(vk&&vk<256)press(output,vk);
            }
            eagler::input::ApplyGamepadVirtualKeys(output,launcher_gamepad&~255);
        }else eagler::input::ApplyGamepadVirtualKeys(output,launcher_gamepad);
        return TRUE;
    }
    BOOL set_keyboard(std::uint8_t* input) override {
        // Our keyboard state is synthetic, so the original clear-high-bits
        // write genuinely applies to it.
        std::memcpy(synthetic_keys, input, 256);
        return TRUE;
    }
    DWORD xinput(DWORD, XINPUT_STATE*) override {
        // No XInput on the web; SDL gamepads enumerate as DirectInput-kind
        // devices instead. Honest "not connected" for all four slots.
        return ERROR_DEVICE_NOT_CONNECTED;
    }
    MMRESULT joystick(UINT, JOYINFOEX*) override { return JOYERR_UNPLUGGED; }
    MMRESULT joystick_caps(UINT, JOYCAPSW*) override { return JOYERR_UNPLUGGED; }
    HRESULT poll(IDirectInputDevice8W*) override { return S_OK; }
    HRESULT acquire(IDirectInputDevice8W*) override { return S_OK; }
    HRESULT device_state(IDirectInputDevice8W* device, DIJOYSTATE2* output) override {
        if(launcher_gamepad>=0)return DIERR_INPUTLOST;
        const int index = pad_index(device);
        if (index < 0 || index >= 4 || !pads[index] || !SDL_GamepadConnected(pads[index])) return DIERR_INPUTLOST;
        auto* pad = pads[index];
        std::memset(output, 0, sizeof *output);
        LONG* axes = &output->lX;
        for (int n = 0; n < 6; ++n) {
            const int value = SDL_GetGamepadAxis(pad, pad_axes[n]);
            const double unit = value < 0 ? value / 32768. : value / 32767.;
            axes[n] = LONG(std::floor((unit + 1) * 32767.5 + .5));
        }
        for (std::uint32_t n = 0; n < sizeof(pad_slots) / sizeof(*pad_slots); ++n)
            if (SDL_GetGamepadButton(pad, pad_slots[n])) output->rgbButtons[n] = 0x80;
        const int x = int(SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) - int(SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_LEFT));
        const int y = int(SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_DOWN)) - int(SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_UP));
        if (x) { output->lX = x > 0 ? 65535 : 0; }
        if (y) { output->lY = y > 0 ? 65535 : 0; }
        output->rgdwPOV[0] = (x || y)
            ? std::uint32_t((int(std::round(std::atan2(double(x), double(-y)) * 180 / 3.141592653589793)) + 360) % 360) * 100
            : ~0u;
        for (int n = 1; n < 4; ++n) output->rgdwPOV[n] = ~0u;
        return S_OK;
    }
};
SdlHost sdl_host;

void pump_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_GAMEPAD_ADDED) add_pad(event.gdevice.which);
        else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) remove_pad(event.gdevice.which);
        else if (event.type == SDL_EVENT_FINGER_CANCELED) { gestures.cancel_transient(); }
        else if (event.type == SDL_EVENT_FINGER_DOWN || event.type == SDL_EVENT_FINGER_MOTION || event.type == SDL_EVENT_FINGER_UP) {
            const auto type = event.type == SDL_EVENT_FINGER_DOWN ? 0 : event.type == SDL_EVENT_FINGER_MOTION ? 1 : 2;
            const auto state = touch_state();
            if (state.context != 3)
                gestures.pointer(type, int(event.tfinger.fingerID), event.tfinger.x, event.tfinger.y, SDL_GetTicks(),
                                 state, (synthetic_keys[16] & 0x80) != 0);
        }
    }
}
}

BOOL Host::keyboard(std::uint8_t* output) { return sdl_host.SdlHost::keyboard(output); }
BOOL Host::set_keyboard(std::uint8_t* input) { return sdl_host.SdlHost::set_keyboard(input); }
DWORD Host::xinput(DWORD index, XINPUT_STATE* output) { return sdl_host.SdlHost::xinput(index, output); }
MMRESULT Host::joystick(UINT index, JOYINFOEX* output) { return sdl_host.SdlHost::joystick(index, output); }
MMRESULT Host::joystick_caps(UINT index, JOYCAPSW* output) { return sdl_host.SdlHost::joystick_caps(index, output); }
HRESULT Host::poll(IDirectInputDevice8W* device) { return sdl_host.SdlHost::poll(device); }
HRESULT Host::acquire(IDirectInputDevice8W* device) { return sdl_host.SdlHost::acquire(device); }
HRESULT Host::device_state(IDirectInputDevice8W* device, DIJOYSTATE2* output) { return sdl_host.SdlHost::device_state(device, output); }

Host& sdl_input_host() { return sdl_host; }

bool sdl_replay_input_locked() {
    const auto* recorder = replay::controller();
    const auto* paused = pause::controller();
    return recorder && recorder->mode == 1 && !(paused && paused->state != 0) && !replay_probe_unlocked;
}

int read_scan_keyboard(std::uint8_t* output) {
    std::memset(output, 0, 256);
    const bool* physical = th20_browser_keyboard() ? nullptr : SDL_GetKeyboardState(nullptr);
    for (const auto& key : keyboard_map)
        if (key.scan < 256 && ((physical && key.native != SDL_SCANCODE_UNKNOWN && physical[key.native]) || key.hosted))
            output[key.scan] = 0x80;
    eagler::input::ApplyGamepadScanKeys(output,launcher_gamepad);
    return 1;
}

// Called once per game tick by the frame loop (the sdl_native_input role).
void sample_native_input() {
    pump_events();
    launcher_gamepad=eagler::input::BrowserGamepadState();
    ++sample_ticks;
    const auto state = touch_state();
    if (state.context == 3) {
        gestures.cancel_transient();
        // Keep the gesture controller's context in sync for the first menu
        // input after playback ends, without forwarding a replay input.
        (void)gestures.sample(state, SDL_GetTicks(), false, false);
        std::memset(touch_keys, 0, sizeof touch_keys);
        analog_active = analog_unlimited = confirm_pulse = false;
        escape_pulse = playback_escape_ticks > 0;
        if (playback_escape_ticks > 0) {
            // Feed only the configured pause/menu binding. Do not forward the
            // generic touch Escape/Bomb aliases used by menus, because those
            // would become gameplay input during replay playback.
            if (controller) {
                const auto vk = controller->mappings[0].keyboard[3];
                if (vk && vk < 256) touch_keys[vk] = true;
            }
            --playback_escape_ticks;
            ++escape_pulses;
        }
        analog_x = analog_y = 0.f;
        last_motion = 0;
        return;
    }
    playback_escape_ticks = 0;
    const auto sample = gestures.sample(state, SDL_GetTicks(), (synthetic_keys[16] & 0x80) != 0,
        (synthetic_keys[37] | synthetic_keys[38] | synthetic_keys[39] | synthetic_keys[40]) & 0x80);
    std::memcpy(touch_keys, sample.keys, sizeof(touch_keys));
    confirm_pulse = sample.keys[90];
    escape_pulse = sample.keys[27];
    last_motion = sample.motion;
    if (confirm_pulse) ++confirm_pulses;
    if (escape_pulse) ++escape_pulses;
    analog_active = false; analog_unlimited = false; analog_x = analog_y = 0.f;
    if (sample.motion && state.ready) {
        // Free-direction movement: the shared controller owns the reachable
        // target (already clamped to the playfield), so this frame's vector is
        // simply the reach towards the finger. The ordinary drag is rate-limited
        // to the player's own speed; the unlimited drag (motion 2) is not, which
        // is what the Launcher's "touch-unlimited" option selects.
        float dx = (sample.x - state.x) * 128.f, dy = (sample.y - state.y) * 128.f;
        analog_unlimited = sample.motion == 2;
        if (!analog_unlimited) {
            const float speed = std::max(1.f, sample.keys[16] ? state.slow : state.fast) * 128.f;
            touhou::input::limit_vector(dx, dy, speed);
        }
        analog_x = dx; analog_y = dy; analog_active = (dx != 0.f || dy != 0.f);
    }
    // A new run's replay recording (mode 0, before its first frame, new-game
    // state) starts a clean run-level unlimited-drag marker. Stage transitions
    // reuse the same recording, so the marker survives them.
    if (const auto* recorder = replay::controller();
        recorder && recorder->mode == 0 && recorder->frame == -1 &&
        program_entry::graphics_state.field_0b18)
        platform::set_unlimited_touch_used(false);
    if (!state.ready) {
        // Menu/dialogue/dialogue-skip contexts: the shared controller emits Z
        // for a tap and Escape for a two-finger tap. TH20's menus read the
        // configured shoot/bomb/menu actions (default Z / X / Escape), so mirror
        // the pulse onto those bindings as well; Enter is a fixed confirm VK.
        const auto* bindings = controller ? &controller->mappings[0] : nullptr;
        const auto press_vk = [](std::uint32_t vk) { if (vk && vk < 256) touch_keys[vk] = true; };
        if (confirm_pulse) {
            touch_keys[0x0d] = true;
            if (bindings) press_vk(bindings->keyboard[0]);
        }
        if (escape_pulse && bindings) {
            press_vk(bindings->keyboard[3]);
            press_vk(bindings->keyboard[1]);
        }
    }
}

// Port-owned direct-touch source for the recovered movement (TouchMotion.hpp).
// Reaching here means gameplay is consuming this frame's movement, so a
// non-zero unlimited movement is what marks the run for the Launcher protocol.
bool analog_motion(float& x, float& y) {
    if (!analog_active) return false;
    x = analog_x; y = analog_y;
    if (analog_unlimited) platform::set_unlimited_touch_used(true);
    return true;
}

void initialize_input_host() {
    for (auto& k : keyboard_map) k.native = SDL_GetScancodeFromName(k.sdl);
    SDL_InitSubSystem(SDL_INIT_GAMEPAD);
    int count = 0;
    auto* ids = SDL_GetGamepads(&count);
    for (int i = 0; i < count; ++i) add_pad(ids[i]);
    SDL_free(ids);
    gestures.begin_session();
}
void shutdown_input_host() {
    for (auto*& p : pads) if (p) { SDL_CloseGamepad(p); p = nullptr; }
    gestures.cancel_transient();
}

// Gamepad slots for Controller::initialize_direct_input: ids as pointers.
IDirectInputDevice8W* host_pad(unsigned index) {
    return index < 4 && pads[index] ? reinterpret_cast<IDirectInputDevice8W*>(std::uintptr_t(index + 1)) : nullptr;
}
}

extern "C" {
__attribute__((export_name("sdl_native_input"))) void sdl_native_input() { th20::source::input::sample_native_input(); }
__attribute__((export_name("sdl_replay_probe_unlock"))) void sdl_replay_probe_unlock(std::uint32_t value) {
    th20::source::input::replay_probe_unlocked = value != 0;
}
__attribute__((export_name("sdl_key"))) void sdl_key(const char* code, std::uint32_t down) {
    for (auto& k : th20::source::input::keyboard_map)
        if (!std::strcmp(code, k.code)) { k.hosted = down != 0; return; }
}
__attribute__((export_name("sdl_keys_clear"))) void sdl_keys_clear() {
    th20_reset_browser_keyboard();
    SDL_ResetKeyboard();
    for (auto& k : th20::source::input::keyboard_map) k.hosted = false;
    std::memset(th20::source::input::synthetic_keys, 0, sizeof(th20::source::input::synthetic_keys));
    th20::source::input::gestures.reset();
}
__attribute__((export_name("sdl_touch"))) void sdl_touch(std::uint32_t type, std::int32_t id, float x, float y) {
    const auto state = th20::source::input::touch_state();
    if (state.context != 3)
        th20::source::input::gestures.pointer(int(type), id, x, y, SDL_GetTicks(), state, false);
}
__attribute__((export_name("sdl_touch_cancel"))) void sdl_touch_cancel() { th20::source::input::gestures.cancel_transient(); }
// Read-only coordinates for browser regression checks. Values are game units,
// independent of the canvas size and of the GPU render-target pixel density.
__attribute__((export_name("sdl_player_state"))) const float* sdl_player_state() {
    static float out[4]{};
    using namespace th20::source;
    out[0]=0;
    if(gameplay::controller){
        auto* p=static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);
        if(p){out[0]=1;out[1]=float(p->state);out[2]=p->position_614.x;out[3]=p->position_614.y;}
    }
    return out;
}
__attribute__((export_name("sdl_touch_options"))) void sdl_touch_options(std::uint32_t on, std::uint32_t free_mode, float speed) {
    auto& g = th20::source::input::gestures;
    g.enabled = on; g.unlimited = free_mode; g.sensitivity = std::clamp(speed, 1.f, 3.f);
    if (!on) g.cancel_transient();
}
__attribute__((export_name("sdl_touch_gestures"))) void sdl_touch_gestures(std::uint32_t two, std::uint32_t taps) {
    th20::source::input::gestures.two_finger = two; th20::source::input::gestures.double_tap = taps;
}
__attribute__((export_name("sdl_touch_mode"))) void sdl_touch_mode(std::uint32_t mode) {
    th20::source::input::gestures.set_mode(int(mode));
}
__attribute__((export_name("sdl_touch_controls"))) void sdl_touch_controls(std::uint32_t shoot, std::uint32_t slow,
    std::uint32_t bomb, std::uint32_t escape, float x, float y) {
    // Track the Launcher Esc serial independently of TouchController. The
    // controller's transient pulses are intentionally cleared during replay,
    // while the mobile pause button must remain available.
    if (escape != th20::source::input::launcher_escape_serial) {
        th20::source::input::launcher_escape_serial = escape;
        if (th20::source::input::touch_state().context == 3)
            th20::source::input::playback_escape_ticks = 1;
    }
    th20::source::input::gestures.controls(shoot, slow, bomb, escape, x, y);
}
// Read-only touch diagnostics for the browser regression check. Reading drains
// the one-shot counters, so fields 14/15 count pulses that appeared since the
// previous read:
// [context, dragging, analog_active, analog_x, analog_y, confirm_pulse, escape_pulse, pause_state,
//  enabled, ready, motion, buttons_current, buttons_pressed, window_active,
//  confirm_pulses, escape_pulses, sample_ticks, unlimited_used, movement_mode, unlimited_mode,
//  pause_substate, pause_cursor, replay_mode, replay_frame, replay_stage, replay_cursor,
//  dialogue, replay_selection, replay_applied_current, replay_applied_pressed,
//  replay_applied_released, replay_expected_current, replay_expected_pressed,
//  replay_expected_released, player_x, player_y, player_state, rng0_last].
__attribute__((export_name("sdl_touch_probe"))) const float* sdl_touch_probe() {
    static float out[38]{};
    using namespace th20::source;
    const auto& g = input::gestures;
    out[0] = float(g.current_context());
    out[1] = g.active() ? 1.f : 0.f;
    out[2] = input::analog_active ? 1.f : 0.f;
    out[3] = input::analog_x;
    out[4] = input::analog_y;
    out[5] = input::confirm_pulse ? 1.f : 0.f;
    out[6] = input::escape_pulse ? 1.f : 0.f;
    out[7] = pause::controller() ? float(pause::controller()->state) : -1.f;
    out[8] = g.enabled ? 1.f : 0.f;
    out[9] = input::touch_state().ready ? 1.f : 0.f;
    out[10] = float(input::last_motion);
    const auto* buttons = input::button_slot(0);
    out[11] = buttons ? float(buttons->current) : -1.f;
    out[12] = buttons ? float(buttons->pressed) : -1.f;
    out[13] = float(program_entry::window_state.active);
    out[14] = float(input::confirm_pulses);
    out[15] = float(input::escape_pulses);
    out[16] = float(input::sample_ticks);
    out[17] = platform::unlimited_touch_used() ? 1.f : 0.f;
    out[18] = float(g.mode);
    out[19] = g.unlimited ? 1.f : 0.f;
    const auto* paused = pause::controller();
    const auto* recorder = replay::controller();
    out[20] = paused ? float(paused->substate) : -1.f;
    out[21] = paused ? float(paused->cursor.current) : -1.f;
    out[22] = recorder ? float(recorder->mode) : -1.f;
    out[23] = recorder ? float(recorder->frame) : -1.f;
    out[24] = recorder ? float(recorder->active_stage) : -1.f;
    out[25] = recorder && recorder->active_stage >= 0 && recorder->active_stage < 8
        ? float(recorder->playback[recorder->active_stage].frame) : -1.f;
    out[26] = hud::controller && hud::controller->collecting ? 1.f : 0.f;
    out[27] = float(gameplay::replay_selection);
    const auto& shared = input::shared_state().slots[0];
    out[28] = float(shared.retained_298[1]);
    out[29] = float(shared.retained_298[4]);
    out[30] = float(shared.retained_298[5]);
    out[31] = out[32] = out[33] = -1.f;
    if (recorder && recorder->mode == 1 && recorder->active_stage >= 0 && recorder->active_stage < 8) {
        const auto& cursor = recorder->playback[recorder->active_stage];
        if (cursor.input_cursor && cursor.inputs && cursor.input_cursor > cursor.inputs) {
            const auto& expected = cursor.input_cursor[-1];
            out[31] = float(expected.current);
            out[32] = float(expected.pressed);
            out[33] = float(expected.released);
        }
    }
    out[34] = out[35] = out[36] = -1.f;
    if (gameplay::controller) {
        if (auto* p = static_cast<player_entity::Player*>(game_session::context(0).objects_04[0])) {
            out[34] = p->position_614.x;
            out[35] = p->position_614.y;
            out[36] = float(p->state);
        }
    }
    out[37] = float(state::random_streams[0].last);
    input::confirm_pulses = input::escape_pulses = 0;
    return out;
}
}
