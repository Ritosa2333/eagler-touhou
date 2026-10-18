#include "Input.hpp"
namespace th10::browser {
Input::Input(){
    profiles=player_profiles;engine_flags=&flags;focused=&snapshot.focused;snapshot.focused=1;
    ranges[0]=axis_ranges;ranges[1]=axis_ranges+1;threshold_x=thresholds;threshold_y=thresholds+1;button_output=buttons;
    const std::int16_t defaults[]={0,1,2,3,-1,-1,-1,-1,4};for(auto& profile:player_profiles)std::memcpy(profile.bindings,defaults,sizeof(defaults));
}
void Input::read_legacy_keyboard(u8* keys){std::memcpy(keys,snapshot.virtual_keys,256);}
void Input::write_legacy_keyboard(const u8* keys){std::memcpy(snapshot.virtual_keys,keys,256);}
i32 Input::read_direct_keyboard(u8* keys){std::memcpy(keys,snapshot.scan_keys,256);return 0;}
u32 Input::read_legacy_joystick(u32 device,LegacyJoystickState& state){if(device>=2||!snapshot.connected[device])return 167;state=snapshot.legacy[device];return 0;}
i32 Input::poll_controller(u32 device){return device<2&&snapshot.connected[device]?0:static_cast<i32>(0x8007000cu);}
i32 Input::acquire_controller(u32 device){return poll_controller(device);}
i32 Input::read_controller(u32 device,DirectJoystickState& state){const i32 result=poll_controller(device);if(!result)state=snapshot.direct[device];return result;}
}
