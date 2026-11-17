// th13-mp 生产 Runtime C 导出（P2 帧泵 + P5 正式页闭环）。
// 实现体在 BrowserRuntime.cpp（app/session/时间同步状态是该 TU 的私有面）；
// 本文件只提供稳定的 C 名称与声明，页面/桥按约定名调用。
#include "BrowserRuntime.hpp"

#include <emscripten.h>

extern "C" {

// 桥的约定名（web/sdl-runtime/multiplayer.mjs）：configure/connect/
// spectator_connect/start/pump/status/rollback_status/error/close/
// replay_save/replay_load/wake。start 返回 0 = READY 门未闭合（桥轮询）。
// M9/P6 观战面：spectator_state/spectator_control/spectator_result/
// spectator_save_replay（launcher spectator-playback 桥消费）。
int th13_mp_configure(const unsigned* words, unsigned count);
int th13_mp_connect(const char* relay_url);
int th13_mp_spectator_connect(const char* relay_url, const char* id);
int th13_mp_start();
int th13_mp_pump(double now_ms);
const unsigned* th13_mp_status();
const unsigned* th13_mp_rollback_status();
const char* th13_mp_error();
void th13_mp_close();
int th13_mp_replay_save();
int th13_mp_replay_load(const unsigned char* bytes, unsigned size,
                        const unsigned* build);
void th13_mp_wake();
const char* th13_mp_spectator_state();
int th13_mp_spectator_control(const char* run, const char* action,
                              unsigned frame);
int th13_mp_spectator_result(const unsigned char* bytes, unsigned size);
int th13_mp_spectator_save_replay();
// M9/P6 多人 Replay 回放观测/控制面（launcher ReplayWorkspace 消费）。
const char* eagler_replay_inspect(const unsigned char* bytes, unsigned size);
const unsigned* eagler_replay_state();
int th13_mp_replay_pause(int paused);
int th13_mp_replay_rate(unsigned frames_per_tick);
int th13_mp_replay_seek_stage(unsigned stage);

EMSCRIPTEN_KEEPALIVE int th13_mp_run_frame(double delta_ms) {
    return th13::sdl::mp_run_frame(delta_ms);
}

}  // extern "C"
