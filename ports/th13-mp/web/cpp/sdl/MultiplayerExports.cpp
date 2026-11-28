#include "BrowserRuntime.hpp"

#include <emscripten.h>

extern "C" {

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
const char* eagler_replay_inspect(const unsigned char* bytes, unsigned size);
const unsigned* eagler_replay_state();
int th13_mp_replay_pause(int paused);
int th13_mp_replay_rate(unsigned frames_per_tick);
int th13_mp_replay_seek_stage(unsigned stage);

EMSCRIPTEN_KEEPALIVE int th13_mp_run_frame(double delta_ms) {
    return th13::sdl::mp_run_frame(delta_ms);
}

}
