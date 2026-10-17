#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define DIRECTINPUT_VERSION 0x0800
#include <Windows.h>
#include <dinput.h>
#include <Xinput.h>
#include <mmsystem.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include "../runtime_core/runtime_core.hpp"
#include "../runtime_core/callback_owner.hpp"
#include "../platform_services/configuration.hpp"

namespace th20::source::input {
// 0x41f9b0 initializes every byte. The arrays not touched by 0x4228b0
// remain present because other input operations retain and copy them.
struct ButtonState {
    std::uint32_t current, previous, repeat8, repeat12, pressed, released;
    std::uint32_t repeat8_count[32];       // +0x018
    std::uint32_t repeat12_count[32];      // +0x098
    std::uint32_t retained_118[32];        // +0x118
    std::uint32_t held_frames[32];         // +0x198
    std::uint32_t retained_218[32];        // +0x218
    std::uint32_t retained_298[6];         // +0x298
    std::uint32_t held8;                  // +0x2b0
    std::uint32_t retained_2b4;
    std::uint32_t last_input_kind;        // +0x2b8, written for aggregate slot 3
    std::uint32_t suppress_previous;      // +0x2bc, read by 0x41fe80
};
static_assert(sizeof(ButtonState) == 0x2c0);
static_assert(offsetof(ButtonState, held8) == 0x2b0);
struct Device {
    std::int32_t kind, logical_index;      // 0 keyboard, 1 DirectInput, 2 XInput
    IDirectInputDevice8W* direct_input;
    std::int32_t xinput_index;
    ButtonState buttons;                  // +0x10
    std::uint8_t raw[256];                 // +0x2d0
    std::uint32_t retained_3d0;
};
static_assert(sizeof(Device) == 0x3d4 && offsetof(Device, raw) == 0x2d0);
ButtonState& initialize(ButtonState&) noexcept;          // 0x41f9b0
Device& initialize(Device&) noexcept;                    // 0x41fcb0
void reset_device_header(Device&) noexcept;              // 0x421720, preserves history
void initialize_keyboard(Device&, std::int32_t) noexcept;// 0x421ab0
void initialize_xinput(Device&, std::int32_t, std::int32_t) noexcept; // 0x421ad0
void update_buttons(ButtonState&) noexcept;              // 0x4228b0
std::uint32_t map_bit_button(std::uint32_t&, std::int16_t, std::uint32_t, std::uint32_t) noexcept; // 0x4204a0
std::uint32_t map_byte_button(std::uint32_t&, std::int16_t, std::uint32_t, const std::uint8_t*); // 0x420510
// Original table at 0x5ae220. Valid configurable XInput indices are 0..11.
inline constexpr std::uint32_t xinput_button_masks[12] = {
    0x1000,0x2000,0x4000,0x8000,0x100,0x200,0x10000,0x20000,0x40,0x80,0x10,0x20
};

// Real OS operations by default. A test host supplies isolated recorded input;
// this interface is not a fallback that returns fake success in production.
class Host {
public:
    virtual ~Host() = default;
    virtual BOOL keyboard(std::uint8_t*);
    virtual BOOL set_keyboard(std::uint8_t*);
    virtual DWORD xinput(DWORD, XINPUT_STATE*);
    virtual MMRESULT joystick(UINT, JOYINFOEX*);
    virtual MMRESULT joystick_caps(UINT, JOYCAPSW*);
    virtual HRESULT poll(IDirectInputDevice8W*);
    virtual HRESULT acquire(IDirectInputDevice8W*);
    virtual HRESULT device_state(IDirectInputDevice8W*, DIJOYSTATE2*);
};
Host& win32_host();
#ifdef TH_SDL3
int read_scan_keyboard(std::uint8_t* output);
// Browser playback keeps live device input out of gameplay/replay state while
// still allowing the pause action. Native/oracle builds do not use this hook.
bool sdl_replay_input_locked();
#endif
struct LegacyState {
    ButtonState slots[4]{};                 // original 0x5b88b0, stride 0x2c0
    ButtonState previous_slots[4]{};        // original 0x5b93b0
    JOYCAPSW capabilities[2]{};             // original 0x5b9ef0, stride 0x2d8
};
LegacyState& shared_state();
ButtonState* button_slot(unsigned);                         //non-owning global5b889c[index], initially null
ButtonState*& button_slot_storage(unsigned);                //same pointer storage, including5b88a4 atindex2
void bind_button_slots(LegacyState&) noexcept;               //420990 final four pointer assignments
int probe_legacy_devices(LegacyState&, Host&, std::uint32_t& graphics_flags); // 0x420f80
void clear_keyboard_high_bits(Host&);       // 0x421040
void poll_legacy_device(LegacyState&, Host&, unsigned); // 0x420760
void sample_startup_input(LegacyState&, Host&);         // 0x420580

struct PollContext {
    Host& host;
    bool active;
    std::int16_t deadzone_x, deadzone_y;
    const platform::KeyBindings* mappings; // at least 2 entries
    std::int32_t& last_input_kind;
};
void poll_device(Device&, PollContext&);    // 0x421b00

struct ControllerContext {
    HINSTANCE instance;
    HWND window;
    const std::uint8_t& active;
    platform::Configuration& configuration;
    DIDEVCAPS& capabilities;
    runtime::Log& log;
    LegacyState& states;
    Host& host;
    scheduler::State& scheduler_state;
    scheduler::Environment& scheduler_environment;
};
// The original object is 0x2ef8 bytes, including a three-slot vtable. A source
// context pointer follows those recovered members, replacing hardcoded globals.
class Controller : public runtime::CallbackOwner {
public:
    explicit Controller(ControllerContext&);             // 0x41f8a0 / 0x41fd20
    ~Controller() override;                              // 0x41fd70
    std::uint32_t frame;                                 // +10
    std::int32_t device_count;                           // +14
    std::int32_t last_input_kind;                        // +18
    IDirectInput8W* direct_input;                        // +1c
    Device devices[12];                                 // +20
    IDirectInputDevice8W* keyboard;                      // +2e10
    IDirectInputDevice8W* gamepads[4];                    // +2e14
    std::int32_t selected[2];                            // +2e24
    std::int32_t frame_input_kind;                       // +2e2c
    std::uint32_t retained_2e30, retained_2e34;
    platform::KeyBindings mappings[4];                   // +2e38
    ControllerContext* context;                         // source dependency injection

    int initialize();                                   // 0x420990
    void rebuild_devices();                             // 0x420d80
    void shutdown_devices();                            // 0x4202c0
    int initialize_direct_input();                      // 0x420aa0
    void enumerate_xinput();                            // 0x4210d0
    void initialize_direct_input_device(unsigned, int);  // 0x4219d0
    void sample_frame();                                // 0x41fe80
    std::int32_t selected_device(unsigned index) const noexcept { return index==1?-1:selected[index]; }// 0x421990, player 1 is disabled
    ButtonState& device_buttons(unsigned) noexcept;      // 0x421950/0x421970
    void release_direct_input();                        // 0x4216e0
};
static_assert(offsetof(Controller, devices) == 0x20);
static_assert(offsetof(Controller, mappings) == 0x2e38);
static_assert(offsetof(Controller, context) == 0x2ef8);
bool is_xinput_product(const GUID&);                      // 0x421180, WMI PNP IDs
bool matches_xinput_device_id(const wchar_t*, std::uint32_t); // pure predicate within 0x421180
Controller* create_controller(ControllerContext&);        // 0x422b00/0x41f830
void destroy_controller(Controller*);                     // 0x41f7c0
extern Controller* controller;                           // original 0x5b8898
// These game-global adapters are in th20_input_entry_adapter. They require the
// real window, graphics, scheduler and runtime globals supplied by their modules.
Controller* create_game_controller();                    // 0x422b00 with original globals
void sample_game_frame();                                // 0x41fe80 with original singleton
void destroy_game_controller();                          // 0x4217c0 with original singleton
}
