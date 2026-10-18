#pragma once
#include "../game/InputDevices.hpp"
namespace th10::browser {
struct InputSnapshot {
    u8 virtual_keys[256],scan_keys[256];
    DirectJoystickState direct[2];LegacyJoystickState legacy[2];
    u32 connected[2];i32 focused;
};
static_assert(sizeof(InputSnapshot)==1172&&offsetof(InputSnapshot,connected)==1160);
struct Input final : InputDeviceEnvironment {
    InputSnapshot snapshot{};InputProfile player_profiles[2]{};
    u32 flags=0x600;std::int16_t thresholds[2]={600,600};
    JoystickRange axis_ranges[2]={{0,65535,0,65535},{0,65535,0,65535}};u8 buttons[128]{};
    Input();
    // Virtual-key indexed state, matching TH08's BrowserRuntime::keyboard_state.
    // The game itself reads scan_keys through read_keyboard(); the thprac UI
    // needs the VK-indexed array for io.KeyMap / ImGuiKey and AddInputCharacter.
    u8* keyboard_state(){return snapshot.virtual_keys;}
    void read_legacy_keyboard(u8* keys) override;
    void write_legacy_keyboard(const u8* keys) override;
    i32 read_direct_keyboard(u8* keys) override;
    void acquire_keyboard() override {}
    u32 read_legacy_joystick(u32 device,LegacyJoystickState& state) override;
    i32 poll_controller(u32 device) override;
    i32 acquire_controller(u32 device) override;
    i32 read_controller(u32 device,DirectJoystickState& state) override;
};
}
