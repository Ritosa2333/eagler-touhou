// th16 浏览器宿主：Emscripten 导出集中一处。
//
// 启动器协议 eagler-touhou/1（web/sdl-runtime/shell.mjs 调用，与 ports/th20 同一组原生入口）：
//   sdl_game_open(chinese, seed) → 句柄  打开 /game/th16.dat，存档根 /savesth16/<jp|chs>，乐曲 /bgm-ogg
//   sdl_loop_start / sdl_loop_stop / sdl_loop_pause   rAF 主循环；每帧先问 Module.runtimePrepare，末尾回 Module.runtimeFinish
//   sdl_key(code, down) / sdl_keys_clear              启动器托管键盘（DOM KeyboardEvent.code）
//   sdl_touch_controls / sdl_touch* / sdl_touch_options 触屏按钮与虚拟摇杆（canvas 上的手势只接收不解释）
//   application_save / application_error / sdl_game_status / sdl_audio_stats / sdl_music_enabled
//   sdl_canvas / sdl_stats（[5] = Present 次数）由 portable/sdl/Renderer.cpp 导出；graphics_allocate / graphics_free 供壳传 C 字符串
//
// 开发页 th16-dev.html 仍用 th16_* 一族：th16_initialize 打开 /assets/th16.dat（存档根 /save），th16_loop_start 起 rAF，
// th16_frame_tick 手动泵一帧，其余为观测面（场景 / 标题光标 / 全局难度等）。两套入口共用同一个 Application。
#include "Application.hpp"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <string>

namespace {
std::unique_ptr<th16::sdl::Application> app;
std::string last_error;
bool running = false;
bool suspended = false;      // sdl_loop_pause：不推进逻辑、音频流挂起
uintptr_t loop_epoch = 0;
double callback_begin = 0;   // 本帧回调起点（毫秒）；runtimeFinish 的 duration 由它算

constexpr unsigned kHandle = 1;   // 单实例：句柄恒为 1

// 壳的帧回调：runtimePrepare 返回 0 表示页面不可见（本帧不推进）；runtimeFinish(result, 毫秒) 汇报一帧
EM_JS(int, browser_prepare_frame, (), { return Module['runtimePrepare'] ? Module['runtimePrepare']() : 1; });
EM_JS(void, browser_finish_frame, (int result, double milliseconds), { if (Module['runtimeFinish']) Module['runtimeFinish'](result, milliseconds); });
EM_JS(void, browser_loop_stopped, (), { if (Module['runtimeStopped']) Module['runtimeStopped'](); });
EM_JS(void, browser_file_changed, (), { if (Module['runtimeFileChanged']) Module['runtimeFileChanged'](); else if (Module['th16_persist']) Module['th16_persist'](); });

bool create_app(const char* archive, const std::string& save_root, const std::string& music_directory) {
    app = std::make_unique<th16::sdl::Application>();
    app->session.save_directory = save_root;
    app->music_directory = music_directory;
    // scoreth16.dat / th16.cfg / replay/th16_XX.rpy 写盘后由壳回写 IndexedDB
    app->session.on_saved = [] { browser_file_changed(); };
    if (!app->initialize(archive)) {
        last_error = app->failure;
        app.reset();
        return false;
    }
    return true;
}

void stop_loop() {
    running = false;
    ++loop_epoch;
    if (app) app->audio.suspend(true);
}

// 一帧：挂起时只让 FrameClock 失去节拍（恢复后不追帧）；result 1 = 原版进程退出（场景 2/3），-1 = 致命错误
int host_frame(double time_ms) {
    if (!app || !app->initialized) return -1;
    if (suspended || browser_prepare_frame() <= 0) {
        app->audio.suspend(true);
        app->clock.started = false;
        return 0;
    }
    app->audio.suspend(false);
    if (!app->step_at(time_ms / 1000.0)) {
        last_error = app->failure;
        return -1;
    }
    return app->session.quit_requested ? 1 : 0;
}
}  // namespace

extern "C" {

// ---------- 启动器协议 eagler-touhou/1 ----------

EMSCRIPTEN_KEEPALIVE unsigned sdl_game_open(unsigned chinese, unsigned /*seed*/) {
    // 原版随机种子来自 timeGetTime（0x45a8a0 入口）；GameSession 自己取墙钟，这里不消费 seed
    if (app && app->initialized) return kHandle;
    const std::string root = std::string("/savesth16/") + (chinese ? "chs" : "jp");
    return create_app("/game/th16.dat", root, "/bgm-ogg") ? kHandle : 0;
}

EMSCRIPTEN_KEEPALIVE void sdl_game_close() {
    stop_loop();
    app.reset();
}

EMSCRIPTEN_KEEPALIVE void* graphics_allocate(unsigned bytes) { return std::malloc(bytes ? bytes : 1); }
EMSCRIPTEN_KEEPALIVE void graphics_free(void* p) { std::free(p); }

EMSCRIPTEN_KEEPALIVE void sdl_loop_start(unsigned /*handle*/) {
    if (running || !app || !app->initialized) return;
    running = true;
    suspended = false;
    app->clock.started = false;
    emscripten_request_animation_frame_loop(
        [](double time_ms, void* epoch) -> EM_BOOL {
            if (!running || uintptr_t(epoch) != loop_epoch) return EM_FALSE;
            callback_begin = emscripten_get_now();
            const int result = host_frame(time_ms);
            browser_finish_frame(result, emscripten_get_now() - callback_begin);
            return running ? EM_TRUE : EM_FALSE;
        },
        reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}

EMSCRIPTEN_KEEPALIVE void sdl_loop_stop() {
    if (!running) return;
    stop_loop();
    browser_loop_stopped();
}

EMSCRIPTEN_KEEPALIVE void sdl_loop_pause(unsigned pause) {
    suspended = pause != 0;
    if (app) {
        app->audio.suspend(suspended);
        app->clock.started = false;
        if (suspended) app->clear_hosted_keys();
    }
}

// 落盘：scoreth16.dat（0x449a00）与 th16.cfg（0x459830 退出时写）——页面隐藏 / 退出前由壳调用
EMSCRIPTEN_KEEPALIVE void application_save(unsigned /*handle*/) {
    if (!app || !app->initialized) return;
    app->session.save_scores();
    app->session.save_config();
}

// 0 = 无致命错误；1 = 初始化 / 帧推进失败（文本见 th16_error）
EMSCRIPTEN_KEEPALIVE unsigned application_error(unsigned /*handle*/) {
    if (!last_error.empty()) return 1;
    return app && !app->failure.empty() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE void sdl_key(const char* code, unsigned down) {
    if (app) app->set_hosted_key(code, down != 0);
}

EMSCRIPTEN_KEEPALIVE void sdl_keys_clear() {
    if (app) app->clear_hosted_keys();
}

// 画面上的触点手势：th16 的移动是按位输入（0x4018e0），不接受位置注入；手势只被接收，不解释
EMSCRIPTEN_KEEPALIVE void sdl_touch(unsigned /*type*/, int /*id*/, float /*x*/, float /*y*/) {}
EMSCRIPTEN_KEEPALIVE void sdl_touch_cancel() {
    if (app) app->touch_controls = {};
}
EMSCRIPTEN_KEEPALIVE void sdl_touch_options(unsigned /*enabled*/, unsigned /*unlimited*/, float /*sensitivity*/) {}
EMSCRIPTEN_KEEPALIVE void sdl_touch_gestures(unsigned /*two_finger*/, unsigned /*double_tap*/) {}
EMSCRIPTEN_KEEPALIVE void sdl_touch_mode(unsigned /*mode*/) {}
// 启动器触屏按钮快照：射击 / 低速为电平，炸弹 / Esc 为序号（变化即一次按下），摇杆 ±32767
EMSCRIPTEN_KEEPALIVE void sdl_touch_controls(unsigned fire, unsigned focus, unsigned bomb_serial, unsigned escape_serial, float x, float y) {
    if (!app) return;
    app->touch_controls.update(fire != 0, focus != 0, bomb_serial, escape_serial,
                               int(std::clamp(x, -32767.0f, 32767.0f)), int(std::clamp(y, -32767.0f, 32767.0f)));
}

// [0] 场景（DAT_004c17c4）[1] 关卡 [2] 错误 [3] 标题画面 [4] 标题子状态 [5] 难度 [6] 机体 [7] 标题光标 [8] 季节 [9] 当前 held 位
EMSCRIPTEN_KEEPALIVE const int* sdl_game_status() {
    static int out[10];
    std::fill(std::begin(out), std::end(out), -1);
    if (!app || !app->initialized) return out;
    const auto& g = app->session.globals;
    out[0] = int(app->supervisor().current);
    out[1] = g.stage;
    out[2] = int(application_error(kHandle));
    out[3] = app->title().screen;
    out[4] = app->title().sub;
    out[5] = g.difficulty;
    out[6] = g.shot;
    out[7] = app->title().cursor.index;
    out[8] = g.season;
    out[9] = int(app->session.host_input | app->session.injected_input);
    return out;
}

// [5] = SDL 音频流当前排队帧数，[7] = 自上次读取以来的最低排队帧数（44.1 kHz）
EMSCRIPTEN_KEEPALIVE const unsigned* sdl_audio_stats() {
    static unsigned out[12];
    std::fill(std::begin(out), std::end(out), 0u);
    if (!app) return out;
    out[0] = app->audio.ready() ? 1 : 0;
    out[5] = app->audio.queued_frames();
    out[7] = app->audio.take_min_queued_frames();
    return out;
}

EMSCRIPTEN_KEEPALIVE void sdl_music_enabled(unsigned enabled) {
    if (app) app->audio.set_music_enabled(enabled != 0);
}
// 乐曲 OGG 在 configure 阶段全部进 MEMFS 后才 launch；串流器按曲目惰性打开，之后到达的文件在下一曲自然生效
EMSCRIPTEN_KEEPALIVE void sdl_music_resource_changed() {}
// 全量解码 / 串流解码对 miniaudio 解码器无区别（都按帧拉取）
EMSCRIPTEN_KEEPALIVE void sdl_ogg_decode_mode(unsigned /*full*/) {}

// ---------- 开发页 th16-dev.html ----------

EMSCRIPTEN_KEEPALIVE int th16_initialize() {
    if (app && app->initialized) return 1;
    return create_app("/assets/th16.dat", "/save", "") ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE const char* th16_error() { return last_error.c_str(); }
// 音频观测（AudioDevice::status 文本）
EMSCRIPTEN_KEEPALIVE const char* th16_audio_status() {
    static std::string text;
    text = app ? app->audio.status() : "no app";
    return text.c_str();
}
EMSCRIPTEN_KEEPALIVE unsigned th16_frame() { return app ? app->frames : 0; }
// 0 = 未初始化，1 = 启动幕，2 = 循环运行中
EMSCRIPTEN_KEEPALIVE unsigned th16_phase() { return !app || !app->initialized ? 0 : running ? 2 : 1; }
// DAT_004c17c4 口径：1 = Boot，4 = Title；未初始化返回 0
EMSCRIPTEN_KEEPALIVE int th16_scene() {
    if (!app || !app->initialized) return 0;
    return int(app->supervisor().current);
}
EMSCRIPTEN_KEEPALIVE unsigned th16_boot_frames() {
    return app ? app->supervisor().boot_frames : 0;
}
EMSCRIPTEN_KEEPALIVE int th16_title_live() {
    return app && app->supervisor().title_menu_live ? 1 : 0;
}
// 标题观测：画面（+0x18）/ 子状态（+0x20）/ 光标（+0x24）/ 全局难度·机体·季节·关卡 / 退出请求
EMSCRIPTEN_KEEPALIVE int th16_title_screen() { return app ? app->title().screen : -1; }
EMSCRIPTEN_KEEPALIVE int th16_title_sub() { return app ? app->title().sub : -1; }
EMSCRIPTEN_KEEPALIVE int th16_title_cursor() { return app ? app->title().cursor.index : -1; }
EMSCRIPTEN_KEEPALIVE int th16_difficulty() { return app ? app->session.globals.difficulty : -1; }
EMSCRIPTEN_KEEPALIVE int th16_shot() { return app ? app->session.globals.shot : -1; }
EMSCRIPTEN_KEEPALIVE int th16_season() { return app ? app->session.globals.season : -1; }
EMSCRIPTEN_KEEPALIVE int th16_stage() { return app ? app->session.globals.stage : -1; }
EMSCRIPTEN_KEEPALIVE int th16_quit_requested() { return app && app->session.quit_requested ? 1 : 0; }
// 测试注入：与键盘采样 OR 的 held 位（InputState 位布局：bit0 Z, bit1 X, bit4 ↑, bit5 ↓, bit8 Esc, bit19 Enter）
EMSCRIPTEN_KEEPALIVE void th16_inject_input(unsigned bits) { if (app) app->session.injected_input = bits; }
// 最近逻辑步数（FrameClock 追帧观测）
EMSCRIPTEN_KEEPALIVE unsigned th16_last_subframes() { return app ? app->clock.subframes : 0; }

EMSCRIPTEN_KEEPALIVE int th16_frame_tick() {
    if (!app || !app->initialized) return 0;
    if (!app->step()) { last_error = app->failure; return 0; }
    return 1;
}

EMSCRIPTEN_KEEPALIVE void th16_loop_stop() { stop_loop(); }

EMSCRIPTEN_KEEPALIVE void th16_loop_start() {
    if (running || !app || !app->initialized) return;
    running = true;
    suspended = false;
    emscripten_request_animation_frame_loop(
        [](double time_ms, void* epoch) -> EM_BOOL {
            if (!running || uintptr_t(epoch) != loop_epoch) return EM_FALSE;
            if (!app->step_at(time_ms / 1000.0)) { last_error = app->failure; running = false; return EM_FALSE; }
            return EM_TRUE;
        },
        reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}

}  // extern "C"
