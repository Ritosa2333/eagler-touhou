// 浏览器宿主（th15 BrowserRuntime 裁剪：EM 导出 + RAF 循环 + FrameCadence；
// 触控/thprac/practice lab 全部裁掉，docs/plans/ui-base.md §三）。
// 输入：SDL3 emscripten 端口把键盘事件汇入 SDL 键位态，再经
// GameKeyboard::keyboard_keys 生成 32 位游戏掩码（KeyboardMap.inc 同代布局）。
#include "ApplicationState.hpp"
#include "../game/GameInput.hpp"
#include "../game/GameKeyboard.hpp"
#include "../game/Probe.hpp"
#include "../game/ReplayEntityState.hpp"
#include "../../../portable/sdl/FrameCadence.hpp"
#include "../../../portable/input/TouchController.hpp"

#include <SDL3/SDL.h>
#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>

namespace th13 {
using AnmRandomObserver = void (*)(const AnmVm&, i32, i32, i32, u32, u32, u32, u32, bool);
extern AnmRandomObserver anm_random_observer;
// 定义在 AnmInterpreter.cpp：当前正在 tick 的 VM（抽取站点探针上下文）。
extern const AnmVm* anm_tick_current_vm;
extern void (*anm_tick_observer)(const AnmVm&, float);
}

EM_JS(void, th13_browser_frame, (int ok, double milliseconds, unsigned ticks), {
    Module["onGameFrame"]?.(ok, milliseconds, ticks);
});

// [R3-M] 标题 Replay 菜单：文件选择触发钩子（页面壳 th13.html 注册
// onReplayImportRequest → 隐藏 <input type=file> / 拖放承接）。
EM_JS(void, th13_browser_replay_request, (), {
    Module["onReplayImportRequest"]?.();
});

namespace th13::sdl {
namespace {
std::unique_ptr<ApplicationState> app;
std::string last_error;
std::string audio_observations;
unsigned audio_sequence = 0;
std::string anm_random_observations;
int anm_random_first = 0, anm_random_last = -1;
void observe_anm_random(const AnmVm& vm, i32 opcode, i32 variable, i32 argument,
                        u32 before, u32 seed_before, u32 after, u32 seed_after, bool game_stream) {
    if (!app || !app->run) return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < anm_random_first || frame > anm_random_last) return;
    anm_random_observations += "{\"frame\":" + std::to_string(frame) +
        ",\"bank\":" + std::to_string(app->animations.resource_id(vm.resource)) +
        ",\"script\":" + std::to_string(vm.source_script) +
        ",\"handle\":" + std::to_string(app->animations.registry.handle(vm)) +
        ",\"pc\":" + std::to_string(vm.instruction_offset) +
        ",\"opcode\":" + std::to_string(opcode) +
        ",\"variable\":" + std::to_string(variable) +
        ",\"argument\":" + std::to_string(argument) +
        ",\"consumed\":" + std::to_string(app->replay_consumed) +
        ",\"stage_frame\":" + std::to_string(app->progress.stage_frame) +
        ",\"sim_frame\":" + std::to_string(app->run ? app->run->battle.sim.frame : -1) +
        ",\"vm_age\":" + std::to_string(vm.age.current) +
        ",\"vm_timer\":" + std::to_string(vm.timer.current) +
        ",\"vm\":" + std::to_string(reinterpret_cast<uintptr_t>(&vm)) +
        ",\"stream\":\"" + (game_stream ? "game" : "visual") + "\"" +
        ",\"before\":" + std::to_string(before) +
        ",\"seed_before\":" + std::to_string(seed_before) +
        ",\"after\":" + std::to_string(after) +
        ",\"seed_after\":" + std::to_string(seed_after) + "}\n";
}
struct Key {
    const char* code;
    const char* sdl;
    u32 scan, vk;
    bool hosted = false;
    SDL_Scancode native = SDL_SCANCODE_UNKNOWN;
};
#include "../../../portable/input/KeyboardMap.inc"

GameInput controls;
touhou::input::TouchController gestures;

touhou::input::TouchState touch_state() {
    touhou::input::TouchState state;
    if (!app || !app->run || (app->pause && app->pause->active())) return state;
    if (app->replay_active) { state.context = 3; return state; }
    const auto& battle = app->run->battle;
    state.context = app->run->dialog_active() ? 2 : 1;
    state.instance = app->progress.stage;
    state.ready = battle.player_phase == StageBattle::PlayerPhase::Active && !battle.player_dead;
    state.retain_motion = battle.player_phase == StageBattle::PlayerPhase::Deathbomb && !battle.player_dead;
    state.x = battle.player_x; state.y = battle.player_y;
    state.fast = float(battle.speed_orth) / 128;
    state.slow = float(battle.speed_orth_focus) / 128;
    state.min_x = -184; state.max_x = 184; state.min_y = 32; state.max_y = 432;
    return state;
}
SDL_Joystick* controller = nullptr;
touhou::sdl::FrameCadence cadence;
bool running = false, suspended = false, lost_focus = false;
u32 render_scale = 1, loop_epoch = 0;
double previous_frame = -1;
double presentation_elapsed = 0;
u32 presentation_count = 0;
float presentation_fps = 60;

void add_controller(SDL_JoystickID id) {
    if (controller) return;
    controller = SDL_OpenJoystick(id);
}

void initialize_controller() {
    if (controller) {
        SDL_CloseJoystick(controller);
        controller = nullptr;
    }
    int count = 0;
    auto* ids = SDL_GetJoysticks(&count);
    if (ids) {
        for (int i = 0; i < count; ++i) add_controller(ids[i]);
        SDL_free(ids);
    }
}

u32 sample_controller(u32& raw_buttons) {
    raw_buttons = 0;
    if (!controller || !app) return 0;
    const int num_buttons = std::min(32, SDL_GetNumJoystickButtons(controller));
    for (int i = 0; i < num_buttons; ++i) {
        if (SDL_GetJoystickButton(controller, i)) raw_buttons |= 1u << i;
    }
    u32 keys = 0;
    // 0044fcc0 行 0..4 动作对应位：0=Shot(0x1), 1=Special(0x2),
    // 2=Trance(0x800), 3=Slow(0x8), 4=Pause(0x100)。
    constexpr u32 actions[5] = {0x1, 0x2, 0x800, 0x8, 0x100};
    const auto& bindings = app->pad_bindings();
    for (size_t i = 0; i < 5; ++i) {
        const i16 btn = bindings[i];
        if (btn >= 0 && btn < 32 && (raw_buttons & (1u << btn))) keys |= actions[i];
    }
    // 方向轴/Hat采样
    if (SDL_GetNumJoystickAxes(controller) > 0) {
        const float x = float(SDL_GetJoystickAxis(controller, 0)) / 32768.f;
        if (x < -0.5f) keys |= 0x40;       // ←
        else if (x > 0.5f) keys |= 0x80;  // →
    }
    if (SDL_GetNumJoystickAxes(controller) > 1) {
        const float y = float(SDL_GetJoystickAxis(controller, 1)) / 32768.f;
        if (y < -0.5f) keys |= 0x10;       // ↑
        else if (y > 0.5f) keys |= 0x20;  // ↓
    }
    if (SDL_GetNumJoystickHats(controller) > 0) {
        const auto hat = SDL_GetJoystickHat(controller, 0);
        if (hat & SDL_HAT_UP) keys |= 0x10;
        if (hat & SDL_HAT_DOWN) keys |= 0x20;
        if (hat & SDL_HAT_LEFT) keys |= 0x40;
        if (hat & SDL_HAT_RIGHT) keys |= 0x80;
    }
    return keys;
}

void clear_inputs() {
    gestures.cancel();
    if (app && app->run) app->run->battle.touch = {};
    for (auto& key : keyboard_map) key.hosted = false;
    SDL_ResetKeyboard();
    controls = {};
}

bool sample_and_tick(float fps = 60.f) {
    if (!app) return false;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
            clear_inputs();
            lost_focus = true;
        } else if (event.type == SDL_EVENT_QUIT) {
            app->quit();
        } else if (event.type == SDL_EVENT_JOYSTICK_ADDED) {
            add_controller(event.jdevice.which);
        } else if (event.type == SDL_EVENT_JOYSTICK_REMOVED && controller &&
                   SDL_GetJoystickID(controller) == event.jdevice.which) {
            SDL_CloseJoystick(controller);
            controller = nullptr;
            initialize_controller();
        }
    }
    // A successful SDL quit has already saved and ended the application.
    // step() returns false once exiting, which is not a runtime failure.
    if (app->exiting) return true;
    bool keys[256]{};
    const auto* physical = SDL_GetKeyboardState(nullptr);
    for (const auto& key : keyboard_map) {
        if (key.hosted || (!key.hosted && key.native != SDL_SCANCODE_UNKNOWN && physical[key.native])) {
            if (key.vk < 256) keys[key.vk] = true;
            if (key.vk >= 160 && key.vk <= 165) keys[16 + (key.vk - 160) / 2] = true;  // 双 Shift 归一
            if (key.scan == 28 || key.scan == 156) keys[13] = true;                    // Enter
        }
    }
    u32 raw_buttons = 0;
    const u32 pad_keys = sample_controller(raw_buttons);
    app->raw_pad_buttons = raw_buttons;
    const auto touch = gestures.sample(touch_state(), SDL_GetTicks(), keys[16] || (pad_keys & 8),
        keys[37] || keys[38] || keys[39] || keys[40] || (pad_keys & 0xf0));
    for (u32 i = 0; i < 256; ++i) keys[i] = keys[i] || touch.keys[i];
    if (app->run) app->run->battle.touch = {touch.motion, touch.x, touch.y};
    u32 held = keyboard_keys(keys) | pad_keys;
    // Physical input always belongs to menus, including Replay pause/end.
    // RunScene consumes recorded gameplay input only after its freeze gate;
    // a priority-11 pause callback can resume before the priority-20 tick.
    controls.update(held);
    // 回放不受失焦暂停（无头驱动的窗口焦点不参与回放时间线）。
    const bool focus = app->replay_active ? false : lost_focus;
    lost_focus = false;
    // Physical held/edge/repeat state drives the application and its menus.
    // For replay gameplay RunScene selects the frozen recorded history.
    return app->step(controls, fps, focus);
}

// 单帧驱动（rAF 回调与 th13_frame_tick 手动泵共用）：装载挡板 → 帧钟 → 提交。
bool drive_one_frame(double delta) {
    const double begin = emscripten_get_now();
    if (suspended) {
        cadence.reset();
        return true;
    }
    if (app->loading_waiting()) {
        cadence.reset();
        if (!(app->loading_startup ? app->advance_loading(delta) : app->complete_loading())) {
            running = false;
            th13_browser_frame(0, emscripten_get_now() - begin, 0);
            return false;
        }
        app->audio_device.pump();
        return true;
    }
    const auto ticks = cadence.advance(delta);
    presentation_elapsed += std::max(0., delta);
    if (presentation_elapsed >= 1.) {
        presentation_fps = float(presentation_count / presentation_elapsed);
        presentation_elapsed = 0;
        presentation_count = 0;
    }
    bool ok = true;
    app->graphics.backend.defer = true;
    for (unsigned i = 0; i < ticks && ok; i++) {
        ok = sample_and_tick(presentation_fps);
        if (app->exiting || app->pending_load) break;
    }
    if (app->graphics.backend.commit()) ++presentation_count;
    app->graphics.backend.defer = false;
    app->audio_device.pump();
    th13_browser_frame(ok ? 1 : 0, emscripten_get_now() - begin, ticks);
    if (!ok || app->exiting) running = false;
    return running;
}
}  // namespace
}  // namespace th13::sdl

extern "C" {

EMSCRIPTEN_KEEPALIVE int th13_prepare_loading() {
    using namespace th13::sdl;
    if (running) return 0;
    app = std::make_unique<ApplicationState>();
    app->graphics.render_scale = render_scale;
    return app->prepare_loading();
}

EMSCRIPTEN_KEEPALIVE int th13_draw_startup(unsigned frames) {
    using namespace th13::sdl;
    // 共享外壳的启动品牌动画期间推进装载 ANM（独立时钟），最多 120 帧。
    if (!app || running) return 0;
    for (unsigned frame = 0; frame < std::min(frames, 120u); ++frame) {
        if (!app->loading_animations.update(false) || !app->loading_animations.update(true))
            return 0;
        ++app->loading_frames;
    }
    return app->render_loading();
}

EMSCRIPTEN_KEEPALIVE int th13_initialize() {
    using namespace th13::sdl;
    if (running) return 1;
    clear_inputs();
    if (!SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
        last_error = SDL_GetError();
        return 0;
    }
    initialize_controller();
    if (!app || !app->platform_prepared || app->initialized) {
        app = std::make_unique<ApplicationState>();
        app->graphics.render_scale = render_scale;
    }
    if (!app->initialize(true)) {
        last_error = app->failure;
        app.reset();
        return 0;
    }
    app->graphics.release_startup_branding();
    for (auto& key : keyboard_map) key.native = SDL_GetScancodeFromName(key.sdl);
    previous_frame = -1;
    cadence.reset();
    return 1;
}

EMSCRIPTEN_KEEPALIVE void th13_render_scale(unsigned scale) {
    if (!th13::sdl::running) th13::sdl::render_scale = scale >= 2 ? 2 : 1;
}

EMSCRIPTEN_KEEPALIVE const char* th13_error() {
    using namespace th13::sdl;
    if (app && !app->graphics.submission_trace.good()) return app->graphics.submission_trace.error().c_str();
    if (app && !app->graphics.error.empty()) return app->graphics.error.c_str();
    return app ? app->failure.c_str() : last_error.c_str();
}

EMSCRIPTEN_KEEPALIVE unsigned th13_phase() {
    using namespace th13::sdl;
    return !app ? 4 : app->exiting ? 4 : app->title ? 0 : app->pause && app->pause->active() ? 2 : 1;
}

EMSCRIPTEN_KEEPALIVE unsigned th13_frame() { return th13::sdl::app ? th13::sdl::app->frames : 0; }
EMSCRIPTEN_KEEPALIVE unsigned th13_boot_seed() {
    return th13::sdl::app ? th13::sdl::app->boot_seed : 0;
}

// 只读会话模式探针（Practice 网页验证用；不写状态/不泵帧）。无 app 按 0：
// run_mode_flags 位面 = SessionState 原样；practice_mode 非零 = Practice 入口
// 已排队。
EMSCRIPTEN_KEEPALIVE unsigned th13_run_mode_flags() {
    return th13::sdl::app ? unsigned(th13::sdl::app->progress.run_mode_flags) : 0u;
}

EMSCRIPTEN_KEEPALIVE int th13_practice_mode() {
    return th13::sdl::app ? th13::sdl::app->progress.practice_mode : 0;
}

EMSCRIPTEN_KEEPALIVE const int* th13_probe_state() {
    using namespace th13::sdl;
    return app ? app->projection() : nullptr;
}

EMSCRIPTEN_KEEPALIVE void th13_key(unsigned scan, unsigned down) {
    for (auto& key : th13::sdl::keyboard_map)
        if (key.scan == scan) key.hosted = down != 0;
}

EMSCRIPTEN_KEEPALIVE void th13_touch_options(unsigned enabled, unsigned mode, float sensitivity,
                                           unsigned two_finger, unsigned double_tap) {
    auto& touch = th13::sdl::gestures;
    if (touch.enabled != (enabled != 0) || touch.mode != int(mode)) touch.cancel();
    touch.enabled = enabled != 0; touch.mode = mode <= 3 ? int(mode) : 0;
    touch.unlimited = touch.mode == 1;
    touch.sensitivity = std::isfinite(sensitivity) ? std::clamp(sensitivity, 1.f, 3.f) : 1.f;
    touch.two_finger = two_finger != 0; touch.double_tap = double_tap != 0;
}

EMSCRIPTEN_KEEPALIVE void th13_touch_controls(unsigned enabled, unsigned fire, unsigned focus,
                                            unsigned bomb, unsigned escape, float x, float y) {
    auto& touch = th13::sdl::gestures;
    if (touch.enabled != (enabled != 0)) touch.cancel();
    touch.enabled = enabled != 0;
    touch.controls(fire != 0, focus != 0, bomb, escape,
        std::isfinite(x) ? x : 0, std::isfinite(y) ? y : 0);
}

EMSCRIPTEN_KEEPALIVE void th13_touch(unsigned type, int id, float x, float y) {
    using namespace th13::sdl;
    if (type > 2 || !std::isfinite(x) || !std::isfinite(y)) return;
    gestures.pointer(int(type), id, x, y, SDL_GetTicks(), touch_state(), (controls.held & 8) != 0);
}

EMSCRIPTEN_KEEPALIVE void th13_touch_cancel() {
    using namespace th13::sdl;
    gestures.cancel();
    if (app && app->run) app->run->battle.touch = {};
}

EMSCRIPTEN_KEEPALIVE void th13_keys_clear() { th13::sdl::clear_inputs(); }

EMSCRIPTEN_KEEPALIVE void th13_loop_stop() {
    using namespace th13::sdl;
    running = false;
    ++loop_epoch;
    previous_frame = -1;
    cadence.reset();
    clear_inputs();
    if (controller) {
        SDL_CloseJoystick(controller);
        controller = nullptr;
    }
    if (app) app->audio_device.suspend(true);
}

EMSCRIPTEN_KEEPALIVE void th13_loop_pause(unsigned enabled) {
    using namespace th13::sdl;
    suspended = enabled != 0;
    previous_frame = -1;
    cadence.reset();
    if (app) app->audio_device.suspend(suspended);
}

EMSCRIPTEN_KEEPALIVE void th13_loop_start() {
    using namespace th13::sdl;
    if (running || !app || !app->initialized) return;
    // 启动保留原生 180 帧装载动画（含外壳品牌期已推进部分）；场景切换只等资源。
    if (app->loading_frames >= 120) {
        app->loading_startup = false;
        app->loading_debt = 0;
    }
    running = true;
    suspended = false;
    previous_frame = -1;
    cadence.reset();
    clear_inputs();
    initialize_controller();
    app->audio_device.suspend(false);
    emscripten_request_animation_frame_loop(
        [](double time, void* epoch) -> EM_BOOL {
            if (!running || uintptr_t(epoch) != loop_epoch) return EM_FALSE;
            const double delta = previous_frame < 0 ? 0 : (time - previous_frame) / 1000.;
            previous_frame = time;
            return drive_one_frame(delta) ? EM_TRUE : EM_FALSE;
        },
        reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}

// 手动帧泵：宿主页面在 requestAnimationFrame 停摆的环境（如嵌入式 webview 的
// 后台页）经 th13_frame_tick 以 setTimeout 节奏驱动同一帧体（html 内看门狗）。
EMSCRIPTEN_KEEPALIVE int th13_frame_tick(double delta_milliseconds) {
    using namespace th13::sdl;
    if (!running) return -1;  // 循环未开/已停：不泵
    return drive_one_frame(delta_milliseconds < 0 ? 0 : delta_milliseconds / 1000.) ? 1 : 0;
}

// ---- [R3-M] 标题 Replay 菜单：本地 .rpy 导入 ----
// 页面壳读取用户选择/拖放的文件后经此注入：bytes 原样交给
// ApplicationState::import_replay_bytes（解码失败置 replay_import_error，
// 成功落 catalog 槽并提示菜单选中）。0 = 失败（th13_error 读原因）。
EMSCRIPTEN_KEEPALIVE int th13_replay_import(const unsigned char* data, unsigned size,
                                            const char* name) {
    using namespace th13::sdl;
    if (!app || !data || size == 0 || !name) return 0;
    return app->import_replay_bytes(data, size_t(size), std::string(name)) ? 1 : 0;
}

// 最近导入成功槽位提示（只读；-1 = 无待消费提示。菜单选中后清零）。
EMSCRIPTEN_KEEPALIVE int th13_replay_import_hint() {
    using namespace th13::sdl;
    return app && app->title ? app->title->state.replay_import_hint : -1;
}

// 最近导入失败文案（= replay_import_error()；空串 = 无失败）。页面壳导入
// 失败 note 优先读此值而非应用级 th13_error()：解码失败只落菜单 Import 行
// 语义，不走 fail()——坏 .rpy 不中断会话（原版导入失败仅行内错误）。
// ApplicationState::replay_import_error() 按值返回 std::string，指针若指向
// 临时量会在 UTF8ToString 读取前悬垂——落函数级 static 快照（本文件既有
// 持久存续范式，如 th13_debug_layer_report 的 report），导入成功时应用侧
// clear 后此快照同样回到空串。
EMSCRIPTEN_KEEPALIVE const char* th13_replay_import_error() {
    using namespace th13::sdl;
    static std::string error;
    error = app ? app->replay_import_error() : std::string{};
    return error.c_str();
}

// ---- 回放调试会话（th13-debug.html 无头取帧专用）----
// th13_debug_replay：<name> 须已备入 /assets（rpy 本体 + 由录像头推导的
// plXXa/b.sht）；解码后直入对局，返回 stage 总帧数，0 = 失败。
EMSCRIPTEN_KEEPALIVE unsigned th13_debug_replay(const char* name, unsigned stage_index) {
    using namespace th13::sdl;
    if (running || !app || !app->initialized || !name) return 0;
    std::vector<th13::u8> bytes;
    if (!app->read(name, bytes)) {
        last_error = app->failure;
        return 0;
    }
    // Re-establish the native direct-boot visual-RNG lineage before the loader
    // frames. The capture page always runs th13.html's 90-frame signature gate
    // (mod._th13_draw_startup(90)) before this entry; that gate is the native
    // boot signature scene and, since the caption tree now lives through it
    // (draw_loading(true) creates ascii script17), its 23 prayer spawns consume
    // 552 visual index units (agent-g segment attribution: native signature
    // gate frames 1..89 = 552 units). The original debug/boot reference does
    // not pass through that gate: oracle boot goes device-init -> font noise
    // 256x step16 (0x45ac90) -> 2 next32 (boot f0 draws, index 260 / seed
    // 30195, artifacts/round-s12/A5/native-trace-boot) -> loader frame
    // (24 units) -> battle frame 0 (20 units) = index 304 / seed 33690. Replay
    // that fixed prefix from the recorded boot seed so the debug bloodline is
    // independent of how many title-gate ticks the page happened to run.
    app->visual.reseed_restore(app->boot_seed);
    for (int step = 0; step < 256; ++step) (void)app->visual.step16();
    (void)app->visual.next32();
    (void)app->visual.next32();
    // Use the same loading caption path as a menu launch: 0x439ce0's task
    // (create call site 0x439ded) creates ASCII script17.
    if (!app->draw_loading(false)) { last_error = app->failure; return 0; }
    // [A1/A5] Original loader evidence (Unicorn boot probe, retail v1.00c):
    // the prayer tree (script17 -> children 16/15/14) is advanced by the
    // loader's own supervisor frames: create at f0, first ordinary
    // main+secondary update at f1 (script16's 12 signed/unit variable reads =
    // 24 visual index points, trace
    // artifacts/round-s12/A5/native-trace-boot/battle-visual-rng-trace.jsonl
    // caller 0x461f3c/0x461f5f/0x461f82 -> index 260..284, seed 40152), f2/f3
    // add no draws, battle frame 0 at f4. Run the same three loader frames
    // through ApplicationState::advance_loading_caption() instead of the old
    // one-off 24-unit compensation: the caption's VM timers therefore end at
    // f3 exactly like the original, and boot lineage (256 font noise + 2 sig
    // + 24 prayer + 20 battle frame 0 = index 304, seed 33690) stays
    // bit-identical.
    for (unsigned frame = 0; frame < th13::sdl::ApplicationState::kDebugLoadingFrames; ++frame)
        if (!app->advance_loading_caption()) { last_error = app->failure; return 0; }
    if (!app->begin_replay_run(bytes.data(), bytes.size(), stage_index)) {
        last_error = app->failure;
        return 0;
    }
    clear_inputs();
    cadence.reset();
    return app->replay_cursor ? app->replay_cursor->stage()
                                  ? app->replay_cursor->stage()->frame_count
                                  : 0
                              : 0;
}

// 确定性单帧泵：固定 1/60 步长、恰一逻辑帧 + 绘制提交，不经过 rAF 帧钟与
// FrameCadence。装载挡板（正常启动序）按同一步长推进。1 = ok，0 = 失败。
EMSCRIPTEN_KEEPALIVE int th13_debug_frame() {
    using namespace th13::sdl;
    if (!app) return 0;
    if (!app->failure.empty()) return 0;
    if (app->loading_waiting()) {
        if (!(app->loading_startup ? app->advance_loading(1. / 60.) : app->complete_loading()))
            return 0;
        return 1;
    }
    const double begin = emscripten_get_now();
    app->graphics.backend.defer = true;
    const bool ok = sample_and_tick();
    app->graphics.backend.commit();
    app->graphics.backend.defer = false;
    app->audio_device.pump();
    th13_browser_frame(ok ? 1 : 0, emscripten_get_now() - begin, 1);
    return ok ? 1 : 0;
}

// 回放会话帧号（已消费输入帧数；golden 游戏帧 F ↔ 消费 F+1 后的画面）。
EMSCRIPTEN_KEEPALIVE unsigned th13_debug_replay_frame() {
    return th13::sdl::app ? th13::sdl::app->replay_consumed : 0;
}

// Actual cursor identity is independent of the prepared next-stage index.
// A continuous observer can distinguish loading/activation from an input
// consume without replacing the local frame number or driving the cursor.
EMSCRIPTEN_KEEPALIVE const char* th13_debug_replay_progress() {
    using namespace th13::sdl;
    static std::string report;
    if (!app) return "";
    const auto* cursor = app->replay_cursor.get();
    const auto* stage = cursor ? cursor->stage() : nullptr;
    int slot = -1;
    th13::u64 position = 0;
    for (size_t i = 0; i < app->replay_file.stages.size(); ++i) {
        if (&app->replay_file.stages[i] == stage) { slot = int(i); break; }
        position += app->replay_file.stages[i].frame_count;
    }
    if (slot >= 0) position += cursor->frame_index();
    report = "{\"active\":" + std::string(app->replay_active ? "true" : "false") +
        ",\"finished\":" + (app->replay_finished ? "true" : "false") +
        ",\"cursor_valid\":" + (cursor && cursor->valid() ? "true" : "false") +
        ",\"slot_index\":" + std::to_string(slot) +
        ",\"slot_number\":" + std::to_string(stage ? stage->number : -1) +
        ",\"slot_frame\":" + std::to_string(cursor ? cursor->frame_index() : 0) +
        ",\"slot_frames\":" + std::to_string(stage ? stage->frame_count : 0) +
        ",\"recorded_position\":" + std::to_string(slot >= 0 ? position : 0) +
        ",\"consumed\":" + std::to_string(app->replay_consumed) +
        ",\"current_stage\":" + std::to_string(app->progress.stage) +
        ",\"host_frame\":" + std::to_string(app->frames) +
        ",\"loading\":" + (app->loading_waiting() ? "true" : "false") +
        ",\"transition\":" + (app->stage_transition_active ? "true" : "false") +
        ",\"startup_age\":" + std::to_string(app->stage_startup_age) +
        // 结束后覆盖层：原版关终 0x42cd40 → 0x43e5a0 之后输入消费停止，
        // 屏幕由 RunPause 覆盖层接管；这三个字段把该事实暴露给持续采样器，
        // 不参与任何输入/帧推进决策。
        ",\"overlay\":" + std::to_string(app->pause ? th13::i32(app->pause->state.screen) : 0) +
        ",\"phase\":" + std::to_string(th13_phase()) +
        ",\"scene_flags\":" + std::to_string(app->progress.scene_flags) +
        ",\"environment_paused\":" + (app->environment.paused ? "true" : "false") +
        ",\"stage_frame\":" + std::to_string(app->progress.stage_frame) + "}";
    return report.c_str();
}

// 诊断开关：仅打开 0x446870/0x419a90 的伤害贡献打印，不改变任何数值。
EMSCRIPTEN_KEEPALIVE void th13_debug_damage_probe(unsigned enabled) {
    th13::damage_probe_enabled = enabled != 0;
}

// 对局场景帧（RunScene::update 计数）：与 replay_frame 同步为帧泵对齐自检。
EMSCRIPTEN_KEEPALIVE unsigned th13_debug_stage_frame() {
    return th13::sdl::app && th13::sdl::app->run ? unsigned(th13::sdl::app->progress.stage_frame) : 0;
}

// 画面诊断：33 层逐 VM 清单（脚本/精灵/位置/纹理源），帧定位问题定位用。
EMSCRIPTEN_KEEPALIVE const char* th13_debug_layer_report() {
    using namespace th13::sdl;
    static std::string report;
    if (!app) return "";
    report.clear();
    char line[256];
    std::snprintf(line, sizeof line, "registry vms=%u\n", app->animations.registry.count());
    report += line;
    for (th13::u32 layer = 0; layer < 33; layer++) {
        const auto& vms = app->animations.registry.layer(layer);
        if (vms.empty()) continue;
        std::snprintf(line, sizeof line, "layer %u: %zu vm(s)\n", layer, vms.size());
        report += line;
        for (const auto* vm : vms) {
            if (!vm) continue;
            const auto* source = vm->sprite_resource ? vm->sprite_resource : vm->resource;
            const char* texture =
                source && !source->textures.empty() ? source->textures[0].name.c_str() : "?";
            const th13::i32 sprite = vm->visual.sprite;
            const char* sprite_texture =
                source && vm->texture_sprite()
                    ? source->textures[vm->texture_sprite()->texture].name.c_str()
                    : "-";
            std::snprintf(line, sizeof line,
                          "  script=%d sprite=%d pos=(%.0f,%.0f) flags=0x%x tex=%s/%s\n",
                          vm->source_script, sprite, vm->visual.translation.x,
                          vm->visual.translation.y, vm->visual.render_flags,
                          texture, sprite_texture);
            report += line;
            std::snprintf(line, sizeof line,
                          "    visual=%08x color=%08x mode=%u scale=(%.5g,%.5g) size=(%.5g,%.5g) origin=(%.5g,%.5g,%.5g) pc=%d\n",
                          vm->visual.flags, vm->visual.color, vm->visual.draw_mode(),
                          vm->visual.scale.x, vm->visual.scale.y, vm->visual.sprite_size.x,
                          vm->visual.sprite_size.y, vm->variables.position.x, vm->variables.position.y,
                          vm->variables.position.z, vm->instruction_offset);
            report += line;
        }
    }
    return report.c_str();
}

// Read-only snapshot of the actual registered ANM VMs, for bit-exact numeric
// comparison. The layer buckets are the same source consumed by the renderer;
// no extra ticking or alternate geometry is introduced by this export.
EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_state() {
    using namespace th13::sdl;
    static std::string result;
    result = "{\"schema\":\"th13-anm-state-v1\",\"vms\":[";
    if (!app) { result += "]}"; return result.c_str(); }
    const auto bits = [](float f) { th13::u32 value; std::memcpy(&value, &f, sizeof value); return value; };
    const auto vector3 = [&](const th13::Vec3& v) {
        return "[" + std::to_string(bits(v.x)) + "," + std::to_string(bits(v.y)) + "," +
               std::to_string(bits(v.z)) + "]";
    };

    // JSON string escaping for texture identity (read-only diagnostic path).
    const auto json_string = [](const std::string& text) {
        std::string escaped = "\"";
        for (const unsigned char value : text) {
            switch (value) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (value < 0x20) {
                    char code[7];
                    std::snprintf(code, sizeof code, "\\u%04x", unsigned(value));
                    escaped += code;
                } else {
                    escaped += char(value);
                }
            }
        }
        escaped += '"';
        return escaped;
    };
    bool first = true;
    auto& registry = app->animations.registry;
    for (th13::u32 bucket = 0; bucket < 33; ++bucket) {
        for (const auto* vm : registry.layer(bucket)) {
            if (!vm) continue;
            if (!first) result += ',';
            first = false;
            const auto* parent = vm->creation_parent;
            const auto* texture_sprite = vm->texture_sprite();
            const auto* texture_source = vm->sprite_resource ? vm->sprite_resource : vm->resource;
            const th13::i32 texture_index = texture_sprite ? th13::i32(texture_sprite->texture) : -1;
            const std::string texture_name =
                texture_source && texture_index >= 0 &&
                th13::u32(texture_index) < texture_source->textures.size()
                    ? texture_source->textures[th13::u32(texture_index)].name
                    : std::string();
            result += "{\"bank\":" + std::to_string(app->animations.resource_id(vm->resource)) +
                ",\"script\":" + std::to_string(vm->source_script) +
                ",\"handle\":" + std::to_string(registry.handle(*vm)) +
                ",\"parent\":" + std::to_string(parent ? registry.handle(*parent) : 0) +
                ",\"bucket\":" + std::to_string(bucket) +
                ",\"layer\":" + std::to_string(vm->visual.layer) +
                ",\"sprite\":" + std::to_string(vm->visual.sprite) +
                ",\"color\":" + std::to_string(vm->visual.color) +
                ",\"flags\":" + std::to_string(vm->visual.flags) +
                ",\"render_flags\":" + std::to_string(vm->visual.render_flags) +
                ",\"pending\":" + std::to_string(vm->pending_interrupt) +
                ",\"pc\":" + std::to_string(vm->instruction_offset) +
                ",\"timer_previous\":" + std::to_string(vm->timer.previous) +
                ",\"timer_current\":" + std::to_string(vm->timer.current) +
                ",\"timer_fraction_bits\":" + std::to_string(bits(vm->timer.fractional)) +
                ",\"position_bits\":" + vector3(vm->variables.position) +
                ",\"translation_bits\":" + vector3(vm->visual.translation) +
                ",\"scale_bits\":[" + std::to_string(bits(vm->visual.scale.x)) + "," +
                    std::to_string(bits(vm->visual.scale.y)) + "]" +
                ",\"rotation_bits\":" + vector3(vm->variables.rotation) +
                ",\"texture_index\":" + std::to_string(texture_index) +
                ",\"texture_name\":" + json_string(texture_name) + "}";
        }
    }
    result += "]";
    result += ",\"dialogue\":{\"entry\":" +
              std::to_string(app->run ? app->run->dialog_entry() : -1) +
              ",\"active\":" + (app->run && app->run->dialog_active() ? "true" : "false") +
              ",\"ecl_released\":" + (app->run && app->run->dialog_wait_released() ? "true" : "false") +
              "}}";
    return result.c_str();
}

EMSCRIPTEN_KEEPALIVE void th13_debug_anm_random_observe(int first, int last) {
    using namespace th13::sdl;
    anm_random_observations.clear();
    anm_random_first = first;
    anm_random_last = last;
    th13::anm_random_observer = first <= last ? observe_anm_random : nullptr;
}

// 抽取站点探针：每次 GameRandom::next/step16 记一行（帧、before/after、
// 发生时的 VM 上下文或 null）。与 anm_random_observer 互补：能看见解释器
// 之外的消费者（Trail/特效几何/表现层直读）。
namespace {
std::string rng_site_rows;
int rng_site_first = 0, rng_site_last = -1;
void observe_rng_site(unsigned before, unsigned after) {
    using namespace th13::sdl;
    if (!app) return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < rng_site_first || frame > rng_site_last) return;
    const th13::AnmVm* vm = th13::anm_tick_current_vm;
    rng_site_rows += "{\"frame\":" + std::to_string(frame) +
        ",\"before\":" + std::to_string(before) +
        ",\"after\":" + std::to_string(after) +
        ",\"vm\":" + (vm && vm->resource
            ? "{\"bank\":" + std::to_string(app->animations.resource_id(vm->resource)) +
              ",\"script\":" + std::to_string(vm->source_script) +
              ",\"handle\":" + std::to_string(app->animations.registry.handle(*vm)) +
              ",\"pc\":" + std::to_string(vm->instruction_offset) +
              // ptr/t：逐弹对拍身份（弹内嵌 VM 不入 registry，handle 恒 0）；
              // t=timer.current 区分重绑首 tick（t0 重放 body）与常规 tick。
              ",\"ptr\":" + std::to_string(reinterpret_cast<uintptr_t>(vm)) +
              ",\"t\":" + std::to_string(vm->timer.current) + "}" : "null") + "}\n";
}
}

EMSCRIPTEN_KEEPALIVE void th13_debug_rng_site_observe(int first, int last) {
    rng_site_rows.clear();
    rng_site_first = first;
    rng_site_last = last;
    th13::GameRandom::draw_observer = first <= last ? observe_rng_site : nullptr;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_rng_site_take() {
    static std::string result;
    result.swap(rng_site_rows);
    rng_site_rows.clear();
    return result.c_str();
}
// option 根 spawn 观察器（只读）：spawn_child 成功挂树时记一行
// (frame, 父 bank/script/handle, 子 script/handle)。窗口 = stage_frame-1。
namespace {
std::string opt_spawn_rows;
int opt_spawn_first = 0, opt_spawn_last = -1;
void observe_opt_spawn(const th13::AnmVm& parent, th13::i32 child_script,
                       th13::u32 child_handle) {
    using namespace th13::sdl;
    if (!app) return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < opt_spawn_first || frame > opt_spawn_last) return;
    const th13::i32 bank = app->animations.resource_id(parent.resource);
    opt_spawn_rows += "{\"frame\":" + std::to_string(frame) +
        ",\"bank\":" + std::to_string(bank) +
        ",\"parent_script\":" + std::to_string(parent.source_script) +
        ",\"parent_handle\":" + std::to_string(app->animations.registry.handle(parent)) +
        ",\"child_script\":" + std::to_string(child_script) +
        ",\"child_handle\":" + std::to_string(child_handle) + "}\n";
}
}
EMSCRIPTEN_KEEPALIVE void th13_debug_opt_spawn_observe(int first, int last) {
    opt_spawn_rows.clear();
    opt_spawn_first = first;
    opt_spawn_last = last;
    th13::anm_spawn_observer = first <= last ? observe_opt_spawn : nullptr;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_opt_spawn_take() {
    static std::string result;
    result.swap(opt_spawn_rows);
    opt_spawn_rows.clear();
    return result.c_str();
}
// 对话条目观察器（只读）：RunScene::open_dialog(entry) 每次登记 (frame, entry)。
namespace {
std::string dialog_rows;
int dialog_first = 0, dialog_last = -1;
void observe_dialog(th13::i32 entry) {
    using namespace th13::sdl;
    if (!app) return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < dialog_first || frame > dialog_last) return;
    dialog_rows += "{\"frame\":" + std::to_string(frame) +
        ",\"entry\":" + std::to_string(entry) + "}\n";
}
}
EMSCRIPTEN_KEEPALIVE void th13_debug_dialog_observe(int first, int last) {
    dialog_rows.clear();
    dialog_first = first;
    dialog_last = last;
    th13::run_dialog_observer = first <= last ? observe_dialog : nullptr;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_dialog_take() {
    static std::string result;
    result.swap(dialog_rows);
    dialog_rows.clear();
    return result.c_str();
}

// 逐 tick 观察器（[anm-tick] 行）：窗口内每次 AnmVm::tick 记 VM 身份与
// timer/pc。默认只记弹族脚本（166/167/40），与原版 vm-watch 探针对拍。
namespace {
std::string anm_tick_rows;
int anm_tick_first = 0, anm_tick_last = -1;
void observe_anm_tick(const th13::AnmVm& vm, float rate) {
    using namespace th13::sdl;
    if (!app) return;
    // 取消特效族（170/171/200/201）与灵梦 S1 粒子族（166/167/40）一并观察：
    // 这些 VM 的 op120 随机 + op6 等待决定子体出生帧，是 round-16 残余
    // 相位差（妖梦 10222 / 早苗 11277 / 魔理沙 5545）的直接判据。
    const th13::i32 script = vm.source_script;
    if (script != 166 && script != 167 && script != 40 &&
        script != 170 && script != 171 && script != 200 && script != 201)
        return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < anm_tick_first || frame > anm_tick_last) return;
    anm_tick_rows += "{\"frame\":" + std::to_string(frame) +
        ",\"script\":" + std::to_string(vm.source_script) +
        ",\"ptr\":" + std::to_string(reinterpret_cast<uintptr_t>(&vm)) +
        ",\"timer\":" + std::to_string(vm.timer.current) +
        ",\"pc\":" + std::to_string(vm.instruction_offset) +
        ",\"rate\":" + std::to_string(rate) + "}\n";
}
}

EMSCRIPTEN_KEEPALIVE void th13_debug_anm_tick_observe(int first, int last) {
    anm_tick_rows.clear();
    anm_tick_first = first;
    anm_tick_last = last;
    th13::anm_tick_observer = first <= last ? observe_anm_tick : nullptr;
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_tick_take() {
    static std::string result;
    result.swap(anm_tick_rows);
    anm_tick_rows.clear();
    return result.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_random_take() {
    using namespace th13::sdl;
    static std::string result;
    result.swap(anm_random_observations);
    anm_random_observations.clear();
    return result.c_str();
}

// 主链成员序只读诊断（默认关闭，仅浏览器探针臂显式开窗）：按
// AnmRegistry groups[0]（= 原版 0x46f170 主链同构物）的遍历次序逐节点导出
// 稳定身份字段，供与原生 0x46f170 链掘（节点序 + bank/script/timer）逐帧
// 对拍。不改变链表、不 tick、不写任何状态；外部（外置 VM）不在链内，
// 与原版一致（原版外部内嵌 VM 同样不入 0x46f170 链）。
namespace {
std::string anm_chain_row;
int anm_chain_first = 0, anm_chain_last = -1;
bool anm_chain_enabled = false;
}
EMSCRIPTEN_KEEPALIVE void th13_debug_anm_chain_observe(int first, int last) {
    anm_chain_row.clear();
    anm_chain_first = first;
    anm_chain_last = last;
    anm_chain_enabled = first <= last;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_chain_take() {
    static std::string result;
    result.swap(anm_chain_row);
    anm_chain_row.clear();
    return result.c_str();
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_chain() {
    using namespace th13::sdl;
    static std::string result;
    result.clear();
    if (!app || !anm_chain_enabled) return result.c_str();
    const int frame = app->progress.stage_frame - 1;
    if (frame < anm_chain_first || frame > anm_chain_last) return result.c_str();
    auto& registry = app->animations.registry;
    const th13::u32 count = registry.ordered_count(false);
    result = "{\"frame\":" + std::to_string(frame) +
        ",\"count\":" + std::to_string(count) + ",\"nodes\":[";
    bool first = true;
    for (th13::u32 index = 0; index < count; ++index) {
        const th13::u32 handle = registry.ordered_handle(false, index);
        const th13::AnmVm* vm = registry.find(handle);
        if (!vm) continue;
        if (!first) result += ',';
        first = false;
        result += "{\"index\":" + std::to_string(index) +
            ",\"handle\":" + std::to_string(handle) +
            ",\"bank\":" + std::to_string(app->animations.resource_id(vm->resource)) +
            ",\"script\":" + std::to_string(vm->source_script) +
            ",\"layer\":" + std::to_string(vm->visual.layer) +
            ",\"pc\":" + std::to_string(vm->instruction_offset) +
            ",\"timer\":" + std::to_string(vm->timer.current) +
            ",\"visible\":" + (vm->visible ? "true" : "false") +
            ",\"flags\":" + std::to_string(vm->visual.flags) +
            ",\"render_flags\":" + std::to_string(vm->visual.render_flags) + "}";
    }
    result += "]}\n";
    anm_chain_row += result;
    return result.c_str();
}

// 主链 walk 入口只读 dump（alternate=false 的 AnmRegistry::tick_chain 入口，
// 即本帧子体追加之前）：与 th13_debug_anm_chain 同字段，附 entry:true。
namespace {
std::string chain_walk_rows;
int chain_walk_first = 0, chain_walk_last = -1;
std::string build_chain_walk_json() {
    using namespace th13::sdl;
    if (!app) return std::string();
    const int frame = app->progress.stage_frame - 1;
    auto& registry = app->animations.registry;
    const th13::u32 count = registry.ordered_count(false);
    std::string result = "{\"frame\":" + std::to_string(frame) +
        ",\"entry\":true,\"count\":" + std::to_string(count) + ",\"nodes\":[";
    bool first = true;
    for (th13::u32 index = 0; index < count; ++index) {
        const th13::u32 handle = registry.ordered_handle(false, index);
        const th13::AnmVm* vm = registry.find(handle);
        if (!vm) continue;
        if (!first) result += ',';
        first = false;
        result += "{\"index\":" + std::to_string(index) +
            ",\"handle\":" + std::to_string(handle) +
            ",\"bank\":" + std::to_string(app->animations.resource_id(vm->resource)) +
            ",\"script\":" + std::to_string(vm->source_script) +
            ",\"layer\":" + std::to_string(vm->visual.layer) +
            ",\"pc\":" + std::to_string(vm->instruction_offset) +
            ",\"timer\":" + std::to_string(vm->timer.current) +
            ",\"visible\":" + (vm->visible ? "true" : "false") +
            ",\"flags\":" + std::to_string(vm->visual.flags) +
            ",\"render_flags\":" + std::to_string(vm->visual.render_flags) + "}";
    }
    result += "]}\n";
    return result;
}
void observe_chain_walk(const th13::AnmRegistry&, bool alternate) {
    using namespace th13::sdl;
    if (alternate || !app) return;
    const int frame = app->progress.stage_frame - 1;
    if (frame < chain_walk_first || frame > chain_walk_last) return;
    chain_walk_rows += build_chain_walk_json();
}
}
EMSCRIPTEN_KEEPALIVE void th13_debug_anm_chain_walk_observe(int first, int last) {
    chain_walk_rows.clear();
    chain_walk_first = first;
    chain_walk_last = last;
    th13::chain_entry_observer = first <= last ? observe_chain_walk : nullptr;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_anm_chain_walk_take() {
    static std::string result;
    result.swap(chain_walk_rows);
    chain_walk_rows.clear();
    return result.c_str();
}

// 弹 etEx 队列只读诊断（默认关闭，仅探针臂开窗）：与原生 0x40f030 的
// +0xbd0 游标及队列项逐字段对齐，用于定位 port 队列游标分歧。不改变
// 任何运行状态；输出由 StageSim::debug_bullet_queue() 提供。
namespace {
int bullet_queue_first = 0, bullet_queue_last = -1;
bool bullet_queue_enabled = false;
}
EMSCRIPTEN_KEEPALIVE void th13_debug_bullet_queue_observe(int first, int last) {
    bullet_queue_first = first;
    bullet_queue_last = last;
    bullet_queue_enabled = first <= last;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_bullet_queue() {
    using namespace th13::sdl;
    static std::string result;
    result.clear();
    if (!app || !app->run || !bullet_queue_enabled) return result.c_str();
    const int frame = app->progress.stage_frame - 1;
    if (frame < bullet_queue_first || frame > bullet_queue_last) return result.c_str();
    result = app->run->battle.sim.debug_bullet_queue();
    return result.c_str();
}

// ECL etSprite(602) 实参只读观测（默认关闭）：记录 (frame, enemy serial,
// slot, sprite, subtype)，与原生 0x41d7ad handler / 0x40ecf3 出生点对齐。
namespace {
std::string ecl_sprite_rows;
int ecl_sprite_first = 0, ecl_sprite_last = -1;
void observe_ecl_sprite(th13::i32 frame, th13::u32 serial, th13::i32 slot,
                        th13::i32 sprite, th13::i32 subtype) {
    if (frame < ecl_sprite_first || frame > ecl_sprite_last) return;
    ecl_sprite_rows += "{\"frame\":" + std::to_string(frame) +
        ",\"serial\":" + std::to_string(serial) +
        ",\"slot\":" + std::to_string(slot) +
        ",\"sprite\":" + std::to_string(sprite) +
        ",\"subtype\":" + std::to_string(subtype) + "}\n";
}
}
EMSCRIPTEN_KEEPALIVE void th13_debug_ecl_sprite_observe(int first, int last) {
    ecl_sprite_rows.clear();
    ecl_sprite_first = first;
    ecl_sprite_last = last;
    th13::ecl_sprite_observer = first <= last ? observe_ecl_sprite : nullptr;
}
EMSCRIPTEN_KEEPALIVE const char* th13_debug_ecl_sprite_take() {
    static std::string result;
    result.swap(ecl_sprite_rows);
    ecl_sprite_rows.clear();
    return result.c_str();
}

// 对局表现层诊断：敌机/敌弹/自机弹/自机直绘 VM 清单（弹型定位用）。
EMSCRIPTEN_KEEPALIVE const char* th13_debug_item_report() {
    using namespace th13::sdl;
    static std::string report;
    report.clear();
    if (!app || !app->run) return "";
    auto& battle=app->run->battle;
    report="rate="+std::to_string(battle.sim.time_rate())+"\n";
    const auto& items=battle.item_manager.items();
    for(size_t slot=0;slot<items.size();++slot){
        const auto& item=items[slot];if(!item.alive)continue;
        char row[256];
        std::snprintf(row,sizeof row,
            "item slot=%zu state=%d type=%d pos=(%.9g,%.9g,%.9g) velocity=(%.9g,%.9g,%.9g) homing=%.9g time=%.9g\n",
            slot,item.state,item.type,item.x,item.y,item.z,item.vx,item.vy,item.vz,item.homing_speed,item.anm_time);
        report+=row;
    }
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_stage_report() {
    using namespace th13::sdl;
    static std::string report;
    if (!app || !app->run || !app->run->presentation) return "";
    report = app->run->presentation->debug_report();
    report += "dialogue active=" + std::to_string(app->run->dialog_active()) +
              " ecl_released=" + std::to_string(app->run->dialog_wait_released()) + "\n";
    const auto& sim = app->run->battle.sim;
    {
        const auto& spell = sim.spell.state();
        report += "spell id=" + std::to_string(spell.id) + " name=" +
                  th13::cp932_to_utf8(reinterpret_cast<const th13::u8*>(spell.name.data()),
                                      th13::u32(spell.name.size())) + " bytes=";
        for (const unsigned char value : spell.name) {
            char hex[3]; std::snprintf(hex, sizeof hex, "%02x", value); report += hex;
        }
        report += "\n";
        if (auto* text = app->animations.resource(0)) {
            const auto handle = app->graphics.texture(*text, 0);
            const auto* image = app->graphics.pixels(handle);
            if (image) for (th13::u32 index : {0u, 1u, 9u}) {
                if (index >= text->sprites.size()) continue;
                const auto& sprite = text->sprites[index];
                th13::u32 count = 0, left = image->width, right = 0;
                for (th13::u32 y = th13::u32(sprite.y); y < sprite.y + sprite.height; ++y)
                    for (th13::u32 x = th13::u32(sprite.x); x < sprite.x + sprite.width; ++x) {
                        th13::u16 value = 0;
                        std::memcpy(&value, image->pixels.data() + size_t(y) * image->pitch + x * 2, 2);
                        if (!(value >> 12)) continue;
                        ++count; left = std::min(left, x); right = std::max(right, x);
                    }
                report += "text sprite=" + std::to_string(index) + " ink=" + std::to_string(count) +
                          " bounds=" + std::to_string(left) + "," + std::to_string(right) + "\n";
            }
        }
    }
    {
        const auto& animation_random = app->animations.random_source();
        const auto& distortion_random = app->environment.game_rng;
        char line[192];
        std::snprintf(line, sizeof line,
            "rng game=(%u,%u) animation=(%u,%u) distortion=(%u,%u)\n",
            sim.random.seed(), sim.random.index(), animation_random.seed(),
            animation_random.index(), distortion_random.seed(), distortion_random.index());
        report += line;
    }
    for (const auto& enemy : sim.enemies) {
        if (enemy->dead) continue;
        char line[320];
        std::snprintf(line, sizeof line,
            "  enemy id=%d serial=%u pos=(%.9g,%.9g) life=%.9g age=%.9g flags=%u invulnerable=%d spell_life=%d phase_life=%d\n",
            enemy->script_id, enemy->serial, enemy->position[0], enemy->position[1],
            double(enemy->life), double(enemy->age), enemy->flags, enemy->invulnerable,
            enemy->spell_life, enemy->phase_life_remaining);
        report += line;
    }
    for (const auto& bullet : sim.bullets) {
        if (!bullet.alive) continue;
        char line[320];
        std::snprintf(line, sizeof line,
            "  projectile generation=%u type=%u state=%d pos=(%.9g,%.9g) hit=(%.9g,%.9g) scale=%.9g flags=%u grazed=%d ready=%d\n",
            bullet.generation, bullet.sprite, bullet.state, bullet.x, bullet.y,
            bullet.hit_size[0], bullet.hit_size[1], bullet.hit_scale,
            bullet.flags, bullet.grazed, bullet.collision_ready);
        report += line;
    }
    {
        const auto& battle = app->run->battle;
        char line[160];
        std::snprintf(line, sizeof line, "combat player=(%.9g,%.9g) phase=%d invulnerable=%d\n",
                      battle.player_x, battle.player_y, int(battle.player_phase), battle.invulnerable);
        report += line;
        if(battle.bomb_age>=0) {
            report+="bomb age="+std::to_string(battle.bomb_age)+"\n";
            for(size_t i=0;i<battle.bomb_orbs.size();++i) {
                const auto& orb=battle.bomb_orbs[i];char row[256];
                std::snprintf(row,sizeof row,"  orb i=%zu active=%d pos=(%.9g,%.9g) angle=%.9g speed=%.9g radius=%.9g velocity=(%.9g,%.9g) damage=%.9g\n",
                    i,orb.active,orb.x,orb.y,orb.angle,orb.speed,orb.radius,orb.velocity_x,orb.velocity_y,orb.accumulated_damage);
                report+=row;
            }
        }
        for(const auto& shot:battle.shots) {
            if(!shot.alive)continue;
            char details[256];
            std::snprintf(details,sizeof details,
                "  player_shot gen=%u age=%d pos=(%.9g,%.9g) angle=%.9g speed=%.9g target=%u script=%d\n",
                shot.generation,shot.age,shot.x,shot.y,shot.angle,shot.speed,shot.target_serial,shot.script);
            report+=details;
        }
        for (const auto& bullet : sim.bullets) {
            if (!bullet.alive) continue;
            std::snprintf(line, sizeof line,
                          "  hazard pos=(%.9g,%.9g) velocity=(%.9g,%.9g) size=(%.9g,%.9g) flags=%u\n",
                          bullet.x, bullet.y, bullet.vel[0], bullet.vel[1],
                          bullet.hit_size[0], bullet.hit_size[1], bullet.flags);
            report += line;
            std::snprintf(line, sizeof line,
                "    bullet gen=%u type=%u color=%u state=%d age=%d ex=%d behavior=%u bounce=%d/%d mode=%u\n",
                bullet.generation, bullet.sprite, bullet.subtype, bullet.state,
                bullet.state_age, bullet.ex_cursor, bullet.behavior_flags,
                bullet.ex40_bounces, bullet.ex40_limit, bullet.ex40_mode);
            report += line;
        }
    }
    report += "lasers=" + std::to_string(sim.lasers.size()) +
              " births=" + std::to_string(sim.laser_serial - 0x10000u) + "\n";
    for (const auto& laser : sim.lasers) {
        char line[240];
        std::snprintf(line, sizeof line,
            "  laser gen=%u id=%d phase=%d time=%d pos=(%.9g,%.9g) angle=%.9g length=%.9g width=%.9g\n",
            laser.generation, laser.id, laser.phase, laser.time,
            laser.origin[0], laser.origin[1], laser.angle, laser.length, laser.width);
        report += line;
    }
    return report.c_str();
}

// Read-only production state, same raw-field schema as the native oracle.
// Frame is the last consumed replay input; -1 is the pre-input state.
EMSCRIPTEN_KEEPALIVE const char* th13_debug_state_report() {
    using namespace th13::sdl;
    static char report[768];
    if (!app || !app->run) return "";
    const auto& battle = app->run->battle;
    const auto& progress = app->progress;
    const auto& player = app->selection_player;
    std::string keys;
    constexpr std::pair<unsigned, unsigned> mapping[] = {
        {8,16},{64,37},{16,38},{128,39},{32,40},{2,88},{1,90}};
    const unsigned held = app->replay_active ? app->replay_input.held : controls.held;
    for (const auto& pair : mapping) if (held & pair.first) {
        if (!keys.empty()) keys += ',';
        keys += std::to_string(pair.second);
    }
    std::snprintf(report, sizeof report,
        "{\"schema\":\"th13-state-raw-v1\",\"frame\":%d,"
        "\"player_position\":[%.9g,%.9g],\"power\":%d,\"power_step\":%d,"
        "\"spell_id\":null,\"keys\":[%s],"
        "\"raw_globals\":{\"004be7c0\":%d,\"004be7c8\":%d,\"004be7d0\":%d,\"004be7dc\":%d,"
        "\"004be810\":%d,\"004be814\":%d},\"raw_enemy_manager\":null}",
        int(app->replay_active ? app->replay_consumed : progress.stage_frame) - 1,
        double(battle.player_x), double(battle.player_y), player.power, player.power_step,
        keys.c_str(), app->selection_score.score, progress.continues, progress.graze, progress.max_point,
        progress.high_score, progress.high_score_extra);
    return report;
}

// Read-only projection of the production replay state. Address names follow
// 00449070's restored block; float coordinates retain their binary32 bits.
// Entity/ANM coverage is reported separately and is not implied by this core.
EMSCRIPTEN_KEEPALIVE const char* th13_debug_replay_core() {
    using namespace th13::sdl;
    using th13::u32;
    static std::string report;
    if (!app || !app->run || !app->replay_active) return "";
    const auto& p = app->progress;
    const auto& h = app->selection_player;
    const auto& battle = app->run->battle;
    const auto bits = [](float value) {
        unsigned raw;
        std::memcpy(&raw, &value, sizeof raw);
        return raw;
    };
    report = "{\"schema\":\"th13-replay-core-v2\",\"frame\":" +
        std::to_string(int(app->replay_consumed) - 1) + ",\"stage\":" +
        std::to_string(p.stage) + ",\"input\":{\"held\":" +
        std::to_string(app->replay_input.held) + ",\"pressed\":" +
        std::to_string(app->replay_input.pressed) + ",\"released\":" +
        std::to_string(app->replay_input.released) + "},\"globals\":{";
    const std::pair<const char*, u32> values[] = {
        {"004be7b8", u32(p.character)}, {"004be7c0", u32(app->selection_score.score)},
        {"004be7c4", u32(p.difficulty)}, {"004be7c8", u32(p.continues)},
        {"004be7d0", u32(p.graze)}, {"004be7d4", u32(p.selected_spell_id)},
        {"004be7dc", u32(p.max_point)}, {"004be7e4", u32(p.max_point_cap)},
        {"004be7e8", u32(h.power)}, {"004be7ec", u32(h.max_power)},
        {"004be7f0", u32(h.power_step)}, {"004be7f4", u32(h.extra_lives)},
        {"004be7f8", u32(h.life_pieces)}, {"004be7fc", u32(p.life_piece_threshold_index)},
        {"004be800", u32(h.bombs)}, {"004be804", u32(h.bomb_pieces)},
        {"004be808", u32(p.ending_points)}, {"004be810", u32(p.high_score)},
        {"004be814", u32(p.high_score_extra)}
    };
    bool first = true;
    for (const auto& value : values) {
        if (!first) report += ',';
        first = false;
        report += '"'; report += value.first;
        report += "\":" + std::to_string(value.second);
    }
    const auto& game = app->game_random;
    const auto& visual = app->animations.random_source();
    report += "},\"player\":{\"x_bits\":" + std::to_string(bits(battle.player_x)) +
        ",\"y_bits\":" + std::to_string(bits(battle.player_y)) + "},\"rng\":{\"game_seed\":" +
        std::to_string(game.seed()) + ",\"game_index\":" + std::to_string(game.index()) +
        ",\"visual_seed\":" + std::to_string(visual.seed()) +
        ",\"visual_index\":" + std::to_string(visual.index()) + "}}";
    return report.c_str();
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_replay_entities() {
    static std::string report;
    const auto& app = th13::sdl::app;
    if (!app || !app->run) return "";
    auto state = th13::sample_replay_entities(app->run->battle);
    state.frame = app->replay_active ? app->replay_consumed - 1 : app->progress.stage_frame - 1;
    report = state.json();
    return report.c_str();
}

// Observes production requests before queue coalescing; never drives audio.
EMSCRIPTEN_KEEPALIVE void th13_debug_audio_observe(unsigned enable) {
    using namespace th13::sdl;
    if (!app) return;
    audio_observations.clear(); audio_sequence = 0;
    app->audio_device.request_observer = {};
    if (!enable) return;
    app->audio_device.request_observer = [](const AudioDevice::RequestObservation& event) {
        const char* kind = event.kind == AudioDevice::RequestObservation::Kind::Voice ? "voice"
                         : event.kind == AudioDevice::RequestObservation::Kind::Stop ? "stop"
                         : event.kind == AudioDevice::RequestObservation::Kind::Pan ? "pan" : "music";
        unsigned x_bits;
        std::memcpy(&x_bits, &event.x, sizeof x_bits);
        const int frame = app->replay_active ? int(app->replay_consumed) - 1
                                             : app->progress.stage_frame - 1;
        audio_observations += "{\"frame\":" + std::to_string(frame) +
            ",\"sequence\":" + std::to_string(audio_sequence++) + ",\"kind\":\"" + kind +
            "\",\"id\":" + std::to_string(event.id) + ",\"value\":" +
            std::to_string(event.value) + ",\"positional\":" + (event.positional ? "true" : "false") +
            ",\"x_bits\":" + std::to_string(x_bits) + "}\n";
    };
}

EMSCRIPTEN_KEEPALIVE const char* th13_debug_audio_take() {
    using namespace th13::sdl;
    static std::string result;
    result.swap(audio_observations);
    audio_observations.clear();
    return result.c_str();
}

// 调试直绘过滤：mask bit0..3 = 自机弹/自机/敌机/敌弹（0 = 全关，仅查看）。
EMSCRIPTEN_KEEPALIVE void th13_debug_stage_pass(unsigned mask) {
    th13::StagePresentation::debug_pass_mask = mask;
}

// 调试探针：renderer.draw 计量开关（1 开 0 关；开则从零累计）。
EMSCRIPTEN_KEEPALIVE void th13_debug_draw_probe(int enable) {
    th13::anm_draw_probe.reset();
    th13::anm_draw_probe.enabled = enable != 0;
}

// 渲染统计契约（th15 graphics_stats 同形态；Renderer::Statistics 18 项
// u32 聚合，布局 = 成员声明序）。浏览器实测断言「每批 glBufferData 整体
// 替换、无 glBufferSubData 分段流式、直通顶点无重复拷贝」——Mali/Adreno
// 真机卡顿对策的固化门禁（AGENTS.md 性能检查单）。
EMSCRIPTEN_KEEPALIVE const unsigned* th13_debug_stats() {
    return th13::sdl::app
               ? reinterpret_cast<const unsigned*>(&th13::sdl::app->graphics.backend.stats)
               : nullptr;
}

// 探针读数：calls/quads/shapes/strips/glyphs + 按精灵聚合表。
EMSCRIPTEN_KEEPALIVE const char* th13_debug_draw_report() {
    using namespace th13::sdl;
    static std::string report;
    const auto& probe = th13::anm_draw_probe;
    report.clear();
    char line[128];
    std::snprintf(line, sizeof line,
                  "calls=%u quads=%u shapes=%u strips=%u glyphs=%u other=%u\n", probe.calls,
                  probe.quads, probe.shapes_drawn, probe.strips, probe.glyphs, probe.other);
    report += line;
    for (th13::u32 i = 0; i < th13::AnmDrawProbe::kSlots; i++) {
        if (!probe.sprite_counts[i]) break;
        std::snprintf(line, sizeof line, "  sprite=%d count=%u size=%ux%u\n",
                      probe.sprite_ids[i], probe.sprite_counts[i], probe.sprite_sizes[i][0],
                      probe.sprite_sizes[i][1]);
        report += line;
    }
    return report.c_str();
}
// 临时探针（-96px 排查；用后即删）：置位相机/顶点预算打印 + 环境快照。
EMSCRIPTEN_KEEPALIVE const char* th13_probe_camera(int enable) {
    using namespace th13::sdl;
    th13::probe::enabled = enable != 0;
    th13::probe::budget = enable != 0 ? 1u : 0u;
    th13::probe::probe_cam_arm(enable != 0 ? 300u : 0u);
    static std::string report;
    report.clear();
    if (app) {
        const auto& e = app->environment;
        char line[160];
        std::snprintf(line, sizeof line,
                      "env offsets={%d,%d,%d,%d} screen=%ux%u raster=%.3f res=%.3f\n",
                      e.screen_offsets[0], e.screen_offsets[1], e.screen_offsets[2],
                      e.screen_offsets[3], e.screen_width, e.screen_height,
                      double(e.raster_scale), double(e.resolution_scale));
        report += line;
        for (th13::u32 i = 0; i < 4; i++) {
            const auto& v = app->views.view(th13::DrawCamera(i));
            std::snprintf(line, sizeof line,
                          "view[%u] vp={%u,%u,%u,%u} view12=%.4f view13=%.4f proj0=%.9g\n", i,
                          v.viewport.x, v.viewport.y, v.viewport.width, v.viewport.height,
                          double(v.camera.view.m[12]), double(v.camera.view.m[13]),
                          double(v.camera.projection.m[0]));
            report += line;
        }
    }
    return report.c_str();
}

// Production submission trace v2, NOT legacy draw-v1. Each present completes
// one frame. Consumers must validate schema and the successful end frame count.
EMSCRIPTEN_KEEPALIVE int th13_debug_capture_begin(const char* path) {
    using namespace th13::sdl;
    if (!app || !path) return 0;
    if (!app->graphics.capture_begin(path)) {
        last_error = app->graphics.error.empty() ? "draws capture open failed"
                                                 : app->graphics.error;
        return 0;
    }
    return 1;
}

// 结束捕获：关闭输出，返回已落帧数。
EMSCRIPTEN_KEEPALIVE unsigned th13_debug_capture_end() {
    using namespace th13::sdl;
    if (!app) { last_error = "capture end without application"; return ~0u; }
    if (!app->graphics.capture_finish()) {
        last_error = app->graphics.error;
        return ~0u;
    }
    return unsigned(app->graphics.submission_trace.frames_out());
}

// 已写输出字节偏移（宿主按偏移分块拉取 MEMFS 文件，长窗不驻留整文件）。
EMSCRIPTEN_KEEPALIVE double th13_debug_capture_tell() {
    using namespace th13::sdl;
    return app ? double(app->graphics.submission_trace.output_position()) : -1;
}

// 像素采样（GL 层确定性证据）：从当前 WebGL 帧缓冲读 RGBA。坐标系 = GL
// （原点左下、y 向上）——锚点推导时按 canvas 高 480 换算 y_gl = 479 - y。
// 须与帧泵同一 JS 任务内调用（后备缓冲合成前内容完整）。
EMSCRIPTEN_KEEPALIVE unsigned char* th13_debug_read_pixels(int x, int y, int w, int h) {
    static std::vector<unsigned char> buffer;
    if (w <= 0 || h <= 0 || w > 4096 || h > 4096) return nullptr;
    buffer.resize(size_t(w) * size_t(h) * 4);
    // External reads must not desynchronize Renderer's private binding cache.
    // WebGL2 pack offsets/PBO are also part of the effective destination.
    GLint read = 0, draw = 0, pack = 0, row = 0, rows = 0, pixels = 0, pbo = 0;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw);
    glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
    glGetIntegerv(GL_PACK_ROW_LENGTH, &row);
    glGetIntegerv(GL_PACK_SKIP_ROWS, &rows);
    glGetIntegerv(GL_PACK_SKIP_PIXELS, &pixels);
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &pbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
    glBindFramebuffer(GL_READ_FRAMEBUFFER, GLuint(read));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GLuint(draw));
    glBindBuffer(GL_PIXEL_PACK_BUFFER, GLuint(pbo));
    glPixelStorei(GL_PACK_ALIGNMENT, pack);
    glPixelStorei(GL_PACK_ROW_LENGTH, row);
    glPixelStorei(GL_PACK_SKIP_ROWS, rows);
    glPixelStorei(GL_PACK_SKIP_PIXELS, pixels);
    return buffer.data();
}
}  // extern "C"
