#pragma once
#define NOMINMAX
#include <Windows.h>
#include <d3d9.h>
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>
#include <cstddef>
#include <cstdint>
#include "../core_scheduler/scheduler.hpp"
#include "../platform_services/configuration.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../runtime_core/callback_owner.hpp"
#include "../sprite_renderer/sprite.hpp"
#include "device.hpp"

namespace th20::source::audio { struct SoundInf; }
namespace th20::source::program_entry {
using ConfigPrefix = platform::Configuration;
static_assert(sizeof(void*) == 4, "Recovered field offsets describe the original x86 program");

// Storage sizes are proved by original global initializers 401090/40aa10.
// Some member meanings remain unknown. Fixed original VAs are evidence annotations;
// this module never dereferences those addresses or executes original code.
#pragma pack(push, 1)
struct WindowStatePrefix {                     // original storage 0x005b6758
    HWND window;                               // +0x0000
    HWND previous_window;                      // +0x0004
    std::int32_t quit_requested;                // +0x0008
    HINSTANCE instance;                        // +0x000c
    std::int32_t nominal_width, nominal_height; // +0x10, +0x14
    std::int32_t playfield_width, playfield_height; // +0x18, +0x1c
    std::int32_t offset_x, offset_y;             // +0x20, +0x24
    std::int32_t field_0028[2], field_0030[2];
    std::int32_t field_0038[2], field_0040[2];
    std::int32_t field_0048[2], field_0050[2];
    std::int32_t field_0058, field_005c, field_0060, field_0064;
    std::uint8_t active;                        // +0x68
    std::uint8_t cursor_latch;                  // +0x0069
    std::uint8_t unknown_006a[2];
    std::uint32_t startup_status;               // +0x006c
    std::int8_t draw_counter;                   // +0x0070, MOVSX proves signed
    std::uint8_t unknown_0071[7];
    std::uint64_t performance_frequency;        // +0x0078
    std::uint64_t performance_origin;           // +0x0080
    std::uint8_t unknown_0088;
    char user_data_directory[0x1000];           // +0x0089
    char module_directory[0x1000];              // +0x1089
    std::uint8_t saved_screen_saver;            // +0x2089
    std::uint8_t saved_low_power;               // +0x208a
    std::uint8_t saved_power_off;               // +0x208b
    std::int32_t display_mode;                  // +0x208c
    std::uint32_t flags;                        // +0x2090
    std::uint32_t reset_delay;                  // +0x2094
    std::uint8_t unknown_2098[8];
    std::int32_t scaled_width;                  // +0x20a0
    std::int32_t scaled_height;                 // +0x20a4
    std::int32_t client_width;                  // +0x20a8
    std::int32_t client_height;                 // +0x20ac
    std::int32_t display_width;                 // +0x20b0
    std::int32_t display_height;                // +0x20b4
    std::int32_t viewport_width;                // +0x20b8
    std::int32_t viewport_height;               // +0x20bc
    float scale;                               // +0x20c0
    std::uint8_t unknown_20c4[4];
    double current_time;                       // +0x20c8
    double previous_time;                      // +0x20d0
    double next_update_time;                   // +0x20d8
    double clock_offset;                       // +0x20e0
    double previous_draw_time;                 // +0x20e8
    double current_draw_time;                  // +0x20f0
    std::int32_t field_20f8;
    std::int32_t sleep_budget;                  // +0x20fc
    std::uint32_t input_latch;                  // +0x2100
    struct RepeatCounter { std::int32_t first, second, elapsed; } repeat[4]; // +0x2104
    std::uint8_t trailing_padding[4];            // size0x2138, original CRT zeroing
};
struct ViewportState {                         // 0x16c, constructor0x00471790
    float vectors[7][3];                       // +0x00
    float field_of_view;                       // +0x54
    std::uint32_t field_0058,field_005c;
    D3DMATRIX view,projection;                  // +0x60,+0xa0
    D3DVIEWPORT9 viewport;                      // +0xe0
    std::uint32_t field_00f8;
    std::int32_t offset_x,offset_y;             // +0xfc,+0x100
    D3DVIEWPORT9 adjusted_viewport;             // +0x104
    float bounds[4];                           // +0x11c
    float points[3][2];                         // +0x12c
    float final_vector[3];                     // +0x144
    std::uint32_t final_state[7];               // +0x150
};
struct alignas(8) GraphicsStatePrefix {         // original storage 0x005c4d40
    void* unknown_0000;
    IDirect3D9* direct3d;                       // +0x0004
    Device* device;                             // +0x0008
    RECT window_rectangle;                     // +0x000c
    DIDEVCAPS input_device_caps;                // +0x001c, 0x4219d0 GetCapabilities
    HWND window_handle;                        // +0x0048
    std::uint8_t unknown_004c[0xe4 - 0x4c];
    D3DPRESENT_PARAMETERS presentation;         // +0x00e4
    std::uint8_t unknown_011c[0x18c - 0x11c];
    UINT adapter_width, adapter_height, adapter_refresh_rate; // +0x18c
    D3DFORMAT previous_backbuffer_format;       // +0x0198
    IUnknown* resource_019c;
    IUnknown* resource_01a0;
    IUnknown* resource_01a4;
    std::uint32_t unknown_01a8;
    sprite::Animation* surface_sprites[5];       // +0x1ac..1bc, actual ANM VM owners
    std::uint32_t unknown_01c0,unknown_01c4;
    ConfigPrefix configuration;                // +0x01c8
    ViewportState viewports[6];                 // +0x278..aff
    ViewportState* current_viewport;            // +0xb00
    std::uint32_t field_0b04;
    std::int32_t field_0b08,field_0b0c,field_0b10;
    std::uint32_t field_0b14,field_0b18,field_0b1c,field_0b20,field_0b24,field_0b28;
    std::uint32_t reset_countdown;              // +0xb2c
    std::uint32_t unknown_0b30;
    std::uint32_t disable_vsync;                // +0xb34 = 0x005c5874
    std::uint32_t graphics_ready;               // +0xb38
    std::uint32_t render_counter;               // +0xb3c
    sprite::AnimationFile* surface_animation;    // +0xb40
    std::uint32_t unknown_0b44;
    std::uint32_t event_flags;                  // +0xb48
    std::uint32_t startup_seed,field_0b50;
    D3DCAPS9 device_caps;                       // +0xb54
    std::uint8_t* snapshot_pixels;
    std::int32_t snapshot_pitch;
    char snapshot_path[0x104];
    alignas(4) std::uint8_t worker_storage[2][16]; // +0xd90,+0xda0; actual jthread+atomic
    std::uint32_t field_0db0,field_0db4;
    std::uint32_t render_value;                 // +0xdb8
    std::uint32_t field_0dbc,version_data_size;
    void* dynamic_buffer;                       // +0xdc4 = 0x005c5b04
    runtime::CallbackOwner* startup_scene;
    std::uint32_t field_0dcc,field_0dd0,field_0dd4;
    double update_duration;                     // +0xdd8
    std::uint32_t clear_color,padding_0de4;
};
#pragma pack(pop)
static_assert(offsetof(WindowStatePrefix, draw_counter) == 0x70);
static_assert(offsetof(WindowStatePrefix, performance_frequency) == 0x78);
static_assert(offsetof(WindowStatePrefix, user_data_directory) == 0x89);
static_assert(offsetof(WindowStatePrefix, display_mode) == 0x208c);
static_assert(offsetof(WindowStatePrefix, flags) == 0x2090);
static_assert(offsetof(WindowStatePrefix, current_time) == 0x20c8);
static_assert(offsetof(WindowStatePrefix, active) == 0x68);
static_assert(offsetof(WindowStatePrefix, repeat) == 0x2104);
static_assert(sizeof(WindowStatePrefix) == 0x2138);
static_assert(sizeof(ViewportState)==0x16c);
static_assert(offsetof(ViewportState,adjusted_viewport)==0x104);
static_assert(sizeof(D3DPRESENT_PARAMETERS) == 0x38);
static_assert(offsetof(GraphicsStatePrefix, presentation) == 0xe4);
static_assert(offsetof(GraphicsStatePrefix, configuration) == 0x1c8);
static_assert(offsetof(GraphicsStatePrefix, disable_vsync) == 0xb34);
static_assert(offsetof(GraphicsStatePrefix, device_caps) == 0xb54);
static_assert(offsetof(GraphicsStatePrefix, worker_storage)==0xd90);
static_assert(offsetof(GraphicsStatePrefix, update_duration)==0xdd8);
static_assert(sizeof(GraphicsStatePrefix)==0xde8);
static_assert(offsetof(ConfigPrefix, frame_skip) == 0x7c);

using FunctionController = scheduler::State;   // original FuncCtrlInf, recovered 56-byte layout
using SpriteController=sprite::Controller;      // recovered actual storage layout
using AllocationController = runtime::AllocationController; // recovered 8-byte object
using LogBuffer = runtime::Log;                  // recovered 32-byte pmr string + error flag
using ThreadRegistry = audio::SoundInf;          // actual SoundInf, recovered audio layout
using LockRegistry = runtime::LockRegistry;

// Storage/initializers must be supplied by actual recovered modules. They are
// intentionally not defined as guessed all-zero stand-ins in this library.
extern WindowStatePrefix window_state;          // 0x005b6758
extern GraphicsStatePrefix graphics_state;      // 0x005c4d40
extern FunctionController* function_controller; // 0x005b66d8
extern scheduler::Environment scheduler_environment; // shared lock ownership still requires integration
extern SpriteController* sprite_controller;     // 0x005c0028
extern AllocationController* allocations;       // 0x005b8894
extern LogBuffer log_buffer;                     // 0x005c0678
extern ThreadRegistry thread_registry;           // 0x005ba830
extern LockRegistry& lock_registry;              // 0x005c0240, same object as runtime::shared_locks()
extern std::uint32_t& graphics_event_flags;       // same storage: graphics+0xb48
extern std::uint32_t& device_reset_countdown;     // same storage: graphics+0xb2c
extern std::uint32_t& render_value_005c5af8;       // same storage: graphics+0xdb8
extern double& update_duration;                  // same storage: graphics+0xdd8

// Fully recovered small operations; calling conventions are newly compiled C++
// interfaces, not binary trampolines into the original executable.
std::int32_t display_mode(const WindowStatePrefix&) noexcept;       // 0x0041cc70
bool needs_device_reset(const WindowStatePrefix&) noexcept;          // 0x0041d080
void set_draw_counter(WindowStatePrefix&, std::uint8_t) noexcept;    // 0x0041dcc0
void set_device_reset(WindowStatePrefix&, std::uint32_t) noexcept;   // 0x0041de00
void set_reset_delay(WindowStatePrefix&, std::uint32_t) noexcept;    // 0x0041de30
#ifndef TH_SDL3
IDirect3DDevice9* device(const GraphicsStatePrefix&) noexcept;        // 0x00412730
#else
Device* device(const GraphicsStatePrefix&) noexcept;                  // 0x00412730
#endif
BOOL is_windowed(const GraphicsStatePrefix&) noexcept;               // 0x00415800
void release_device(GraphicsStatePrefix&);                          // 0x0041a240
void release_direct3d(GraphicsStatePrefix&);                        // 0x0041a200
int run_unlimited_frame(WindowStatePrefix&);                        // 0x00419c20
int run_timed_frame(WindowStatePrefix&);                            // 0x00419de0
int run_present_paced_frame(WindowStatePrefix&);                    // 0x0041a030

// The actual four-argument WinMain recovered from 0x0041e7d0. The final return
// zero is proved at 0x0041f18d/0x0041f1a2; it is not a missing-code placeholder.
int WINAPI recovered_win_main(HINSTANCE, HINSTANCE, LPSTR, int);
}
