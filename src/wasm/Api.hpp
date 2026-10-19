#pragma once
#include <cstdint>

// C linkage is the browser/native boundary. All game objects remain typed C++.
extern "C" {
void th12_practice_enabled(int);
void th12_practice_reset(int);
int th12_practice_configure(const int32_t*,int);
int th12_practice_start(int,int,int,int);
int th12_practice_prepare_replay(int);
void th12_practice_replay(int);
void th12_practice_tools(int);
void th12_practice_speed(int,int,int,int);
double th12_practice_interval(int,int,int,int);
const char* th12_practice_state();
int th12_practice_state_size();
int th12_practice_boss_music();
int th12_practice_everlasting_bgm();
int th12_practice_damage_begin(int,int,int,int);
void th12_practice_damage_reset();
int th12_practice_damage_transition(int,int);
int th12_practice_damage_load(const int32_t*,int);
const int32_t* th12_practice_damage_stage(int);
int th12_practice_damage_stage_words(int);
const char* th12_practice_damage_state();
int th12_practice_damage_state_size();
const char* th12_presentation_motion();
int th12_presentation_motion_size();
void th12_motion(int mode,int x,int y);
const int32_t* th12_motion_result();
int th12_player_state();
int th12_player_instance();
float th12_player_speed(int focused);
int th12_load_ecl(const uint8_t*, int);
int th12_load_msg(int, const uint8_t*, int);
int th12_load_std(const uint8_t*, int);
int th12_load_anm(int, const uint8_t*, int);
void th12_unload_anm(int);
int th12_load_sht(int, const uint8_t*, int);
void th12_select_stage(int);
void th12_start(int character, int shot, int difficulty, int seed);
int th12_start_full_game(int character, int shot, int difficulty, int seed, int practice);
void th12_set_full_game_mode(int practice);
int th12_start_next_stage();
int th12_continue_game();
const char* th12_stage_flow();
int th12_stage_flow_size();
int th12_load_spell_history(const int32_t* words, int count);
const char* th12_spell_history();
int th12_spell_history_size();
void th12_set_wall_time(double milliseconds);
int th12_start_replay(int character, int shot, int difficulty, int seed, const int32_t* initial,
                      int count);
#if defined(TH12_DIAGNOSTIC) && TH12_DIAGNOSTIC
int th12_diagnostic_build();
const char* th12_diagnostic_state();
int th12_diagnostic_state_size();
void th12_oracle_fixture(int flags);
void th12_set_initial(int x, int y, int power, int lives, int bombs, int score);
void th12_set_replay_economy(int piv, int lifeFragments, int bombFragments, int red, int blue,
                             int green, int rank);
#endif
void th12_tick(int held, int pressed);
const char* th12_state();
int th12_state_size();
const char* th12_message();
int th12_message_size();
const char* th12_spell();
int th12_spell_size();
const char* th12_ufo();
int th12_ufo_size();
const char* th12_boss_hud();
int th12_boss_hud_size();
int th12_music_track();
uint32_t th12_music_serial();
const char* th12_background();
int th12_background_size();
int th12_draw();
const void* th12_draw_ptr();
int th12_draw_stride();
int th12_draw_version();
int th12_anm_draw();
const void* th12_anm_draw_ptr();
int th12_anm_draw_stride();
int th12_anm_schedule_count();
const void* th12_anm_schedule_ptr();
int th12_anm_schedule_stride();
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
int32_t th12_score_stored();
#if defined(TH12_DIAGNOSTIC) && TH12_DIAGNOSTIC
const uint32_t* th12_unsupported();
uint32_t th12_rng16(int seed);
uint32_t th12_rng32(int seed);
#endif
}
