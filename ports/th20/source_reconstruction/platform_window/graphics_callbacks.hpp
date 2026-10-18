#pragma once
#include "platform_window.hpp"
namespace th20::source::platform_window {
int register_graphics_callbacks();                 // 4de1f0
int initialize_graphics_callbacks(void*);           // 4dd600
int update_graphics(void*);                        // 4dc510
int switch_scene(GraphicsStatePrefix&);             // 4da540
void open_game_data();                             // 4d9ea0
#ifndef TH_SDL3
HRESULT set_render_state(GraphicsStatePrefix&,D3DRENDERSTATETYPE,DWORD); // 4d9db0
#else
void set_depth_compare(GraphicsStatePrefix&,touhou::graphics::Compare);  // 4d9db0(ZFUNC)
void set_fog_color(GraphicsStatePrefix&,std::uint32_t);                  // 4d9db0(FOGCOLOR)
void set_fog_range(GraphicsStatePrefix&,float,float);                    // 4d9db0(FOGSTART/FOGEND)
#endif
HRESULT disable_depth_write(GraphicsStatePrefix&);  // 4ddf80
extern void (*surface_callback_first)();            // 5c4d30, initialized by4dd600
extern void (*surface_callback_second)();           // 5c4d34
namespace unrecovered {
int update_sprite_tasks(sprite::Controller&);       // 44dfb0
#ifdef TH_SDL3
// Cooperative pump for the threadless browser build: one round of the
// per-frame background jobs (audio commands, stream refill, ANM postload,
// surface-copy requests) so inline workers can spin on their completion.
void pump_background_jobs(sprite::Controller&);
#endif
void initialize_sprite_assets(sprite::Controller&); // 44d020
sprite::Animation* create_animation_vm();           // 447800
void draw_animation(sprite::Animation&);            // 44c570 ->443880
void draw_animation_layer(sprite::Controller&,int); // 449e40
void select_sprite_layer(sprite::Controller&,int,int); // 44f3d0
// Game-domain operations used by the actual scene transition table.
runtime::CallbackOwner* create_startup_scene();     // 4d85c0
void create_menu();                                // 52cfe0
void destroy_game();                               // 4bd3d0, actual nullable global5ba828
void create_game(int);                             // 4bec70
void create_ending_scene();                        // 4a1010, actual EndingInf
int game_restart_mode();                           // 488830 on current game
void restore_stage_selection();                    // 498f40 ->4dd8d0
bool stage_selection_changed();                    // 498f40 ->474d80/4bd5c0
extern runtime::CallbackOwner* ending_scene;        // 5c49e8, actual EndingInf
extern runtime::CallbackOwner* menu_scene;          // 5c6124
extern std::uint32_t menu_selection;               // 5c6128
}
}
