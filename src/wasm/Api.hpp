#pragma once
#include <cstdint>

// C linkage is the browser/native boundary. All game objects remain typed C++.
extern "C" {
int th12_load_ecl(const uint8_t*, int);
int th12_load_msg(int, const uint8_t*, int);
int th12_load_std(const uint8_t*, int);
int th12_load_anm(int, const uint8_t*, int);
void th12_unload_anm(int);
int th12_load_sht(int, const uint8_t*, int);
void th12_select_stage(int);
void th12_start(int character, int shot, int difficulty, int seed);
void th12_oracle_fixture(int flags);
void th12_set_initial(int x, int y, int power, int lives, int bombs, int score);
void th12_set_replay_economy(int piv, int lifeFragments, int bombFragments, int red, int blue,
                             int green, int rank);
void th12_tick(int held, int pressed);
const char* th12_state();
int th12_state_size();
const char* th12_message();
int th12_message_size();
const char* th12_spell();
int th12_spell_size();
const char* th12_background();
int th12_background_size();
int th12_draw();
const void* th12_draw_ptr();
int th12_draw_stride();
int th12_draw_version();
int th12_anm_draw();
const void* th12_anm_draw_ptr();
int th12_anm_draw_stride();
int th12_sound_events();
const void* th12_sound_events_ptr();
int th12_sound_events_stride();
int th12_mesh_draw();
const void* th12_mesh_draw_ptr();
int th12_mesh_draw_stride();
const void* th12_mesh_vertices_ptr();
int th12_mesh_vertices_count();
int th12_mesh_vertex_stride();
int th12_score_popups();
const void* th12_score_popups_ptr();
int th12_score_popups_stride();
int th12_laser_draw();
const void* th12_laser_draw_ptr();
int th12_laser_draw_stride();
int th12_background_frame();
int th12_background_visible();
const float* th12_hud();
int th12_hud_size();
const uint32_t* th12_unsupported();
uint32_t th12_rng16(int seed);
uint32_t th12_rng32(int seed);
}
