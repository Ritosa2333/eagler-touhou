#pragma once
#include "GameInput.hpp"
namespace th10 {
struct InputProfile {GameInput input;std::int16_t bindings[9];};
static_assert(sizeof(InputProfile)==0x6a&&offsetof(InputProfile,bindings)==0x58);
struct JoystickRange {u32 x_min,x_max,y_min,y_max;};
struct LegacyJoystickState {u32 size,flags,x,y,z,r,u,v,buttons,button_number,pov,reserved[2];};
struct DirectJoystickState {i32 axes[6],sliders[2];u32 pov[4];u8 buttons[128],remaining[96];};
static_assert(sizeof(LegacyJoystickState)==52&&sizeof(DirectJoystickState)==272&&offsetof(DirectJoystickState,buttons)==48);
struct InputDeviceEnvironment {
    InputProfile* profiles;
    const u32* engine_flags;
    const i32* focused;
    const JoystickRange* ranges[2];
    const std::int16_t *threshold_x,*threshold_y;
    u8* button_output;
    virtual void read_legacy_keyboard(u8* keys)=0;
    virtual void write_legacy_keyboard(const u8* keys)=0;
    virtual i32 read_direct_keyboard(u8* keys)=0;
    virtual void acquire_keyboard()=0;
    virtual u32 read_legacy_joystick(u32 device,LegacyJoystickState& state)=0;
    virtual i32 poll_controller(u32 device)=0;
    virtual i32 acquire_controller(u32 device)=0;
    virtual i32 read_controller(u32 device,DirectJoystickState& state)=0;
};
struct InputDevices {
    InputDeviceEnvironment& environment;
    static u16 map_keyboard(const u8* keys,bool direct) noexcept;
    u16 keyboard();
    u16 mix_controller(u16 buttons,u32 profile,u32 device);
    u8* controller_buttons(u32 device);
    u16 sample();
    void update(u32 profile,bool use_first_bindings=false);
    bool read_keyboard(u8* keys);
    void release_keys();
private:
    void reacquire(u32 device);
};
}
