#pragma once
#include "../program_entry/program_entry.hpp"
#include "../input/input.hpp"

namespace th20::source::platform_window {
using program_entry::WindowStatePrefix;
using program_entry::GraphicsStatePrefix;
// Only fields read here are named. The input sampler and its construction are
// separate unrecovered dependencies; this prefix does not invent input data.
using InputPrefix=th20::source::input::ButtonState;
extern InputPrefix*& input;              // aliases input's unique original0x005b88a4 pointer slot
extern HANDLE single_instance_mutex;     // original 0x005b8890
namespace unrecovered {
void sample_input();                     // original 0x00420580
void bind_animation_script(sprite::AnimationFile&,void* vm,int script,int); // 0x004382b0
int poll_background_jobs(program_entry::ThreadRegistry&); // 0x004277f0
void shutdown_scene_objects();            // 0x004dd730, dependent scene managers
void retire_scheduler_object(runtime::CallbackOwner*); // 0x004217c0 -> 0x0041f7c0
extern runtime::CallbackOwner* scheduler_object_005c4a00; // actual derived owner not yet recovered
extern th20::source::input::Controller*& scheduler_object_005b8898; // aliases input::controller
void stop_audio(program_entry::ThreadRegistry&,int operation,int value,const char*); // 0x00428c90
void close_archive_manager();             // 0x0053a170 bound to global0x005b66c8
void destroy_animation_vm(void*);          // 0x00447410
void update_frame_statistics(void*);       // 0x004abd50, FPS/scoring state
void update_post_frame_game_state();       // 0x00487560, current game object
}

bool is_japanese_user_locale();          // 0x0041d0c0
void calculate_layout(WindowStatePrefix&, int choose_scale); // 0x0041e050
int create_game_window(WindowStatePrefix&, HINSTANCE);       // 0x0041ccf0
LRESULT CALLBACK window_proc(HWND, UINT, WPARAM, LPARAM);    // 0x0041d350
INT_PTR CALLBACK settings_dialog_proc(HWND, UINT, WPARAM, LPARAM); // 0x0041abe0
void apply_settings_dialog();            // 0x0041ab30
void show_startup_settings();            // 0x0041ae70
void register_dialog_raw_input(HWND);    // 0x0041a1b0
std::uint32_t pressed(const InputPrefix&, std::uint32_t) noexcept; // 0x00419c00
int repeated_or_pressed(const InputPrefix&, std::uint32_t) noexcept; // 0x0041a280
int acquire_single_instance();           // 0x0041c020
int create_direct3d();                   // 0x0041c320
int prepare_presentation();              // 0x0041c3e0
bool try_create_device(D3DPRESENT_PARAMETERS&); // 0x0041c1a0, true=failure
int create_or_reset_backbuffer(int reset);     // 0x0041c730
void initialize_render_state();                // 0x0041a2c0
void initialize_window_global(WindowStatePrefix&); // 401090+418ac0 composition
void initialize_graphics_global(GraphicsStatePrefix&); // 40aa10+4d8990 composition
void destroy_graphics_global(GraphicsStatePrefix&); // 56b3f0+4d8da0
void sync_close_graphics_worker(GraphicsStatePrefix&); // 4d9e30
void join_graphics_worker(GraphicsStatePrefix&,unsigned index); // 40bcf0 on original embedded worker
void release_render_surfaces();                // 4dd840
void acquire_render_surfaces(GraphicsStatePrefix&); // 4dbd70
void center_render_viewports();                // 4dbce0
void initialize_render_viewports(GraphicsStatePrefix&); // 4daba0
void set_render_offsets(GraphicsStatePrefix&,int x,int y); // 4ddb20
void update_camera(program_entry::ViewportState&,const D3DVIEWPORT9&); // 4da1f0
int shutdown_graphics();                   // 4dd490, explicit unrecovered domains above
void apply_camera(program_entry::ViewportState&); // 4da120
void select_viewport(GraphicsStatePrefix&,int); // 41dce0
HRESULT disable_fog(GraphicsStatePrefix&);  // 4dda60
void finish_unlimited_frame(WindowStatePrefix&); // 4193e0
void finish_timed_frame(WindowStatePrefix&); // 4199a0
void finish_present_paced_frame(WindowStatePrefix&); // 419a50
void before_present();                     // 419760
void after_present();                      // 4193c0
void copy_render_surface(sprite::Controller&,const std::uint32_t (&request)[10]); // 44bb10
void process_surface_copies(sprite::Controller&); // 41bfc0
void capture_snapshot(GraphicsStatePrefix&,const char* cp932_path); // 4de040
void save_snapshot_worker();                 // 4d9210
bool find_image_encoder(const wchar_t* mime_type,CLSID&); // 4d97f0
void save_bitmap_png(HBITMAP,const char* cp932_path); // 4d9c40
}
