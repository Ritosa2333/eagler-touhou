#include "InputDevices.hpp"
#include <cstring>
namespace th10 {
namespace {
struct KeyBinding {u8 key;u16 buttons;};
constexpr KeyBinding virtual_keys[]={{0x5a,1},{0x58,2},{0x10,4},{0x1b,8},{0x26,0x10},{0x68,0x10},{0x28,0x20},{0x62,0x20},{0x25,0x40},{0x64,0x40},{0x27,0x80},{0x66,0x80},{0x11,0x100},{0x51,0x200},{0x53,0x400},{0x50,0x800},{0x24,0x800},{0x0d,0x1000},{0x44,0x2000},{0x52,0x4000},{0x63,0xa0},{0x69,0x90},{0x61,0x60},{0x67,0x50}};
constexpr KeyBinding scan_codes[]={{0x2c,1},{0x2d,2},{0x2a,4},{0x01,8},{0x48,0x10},{0xc8,0x10},{0x50,0x20},{0xd0,0x20},{0x4b,0x40},{0xcb,0x40},{0x4d,0x80},{0xcd,0x80},{0x1d,0x100},{0x10,0x200},{0x1f,0x400},{0x19,0x800},{0xc7,0x800},{0x1c,0x1000},{0x20,0x2000},{0x13,0x4000},{0x49,0x90},{0x4f,0x60},{0x47,0x50},{0x51,0xa0}};
constexpr u32 binding_order[]={0,1,3,2,8};
constexpr u16 binding_masks[]={1,2,8,4,0x100};
}
u16 InputDevices::map_keyboard(const u8* keys,bool direct) noexcept {
    u16 result=0;const auto* table=direct?scan_codes:virtual_keys;
    for(u32 i=0;i<24;++i)if(keys[table[i].key]&0x80)result|=table[i].buttons;
    return result;
}
u16 InputDevices::keyboard(){
    auto& env=environment;if(!*env.focused)return 0;
    u8 keys[256]{};const bool direct=(*env.engine_flags&0x200)!=0;
    if(direct){if(env.read_direct_keyboard(keys)!=0){env.acquire_keyboard();return 0;}}
    else env.read_legacy_keyboard(keys);
    return map_keyboard(keys,direct);
}
void InputDevices::reacquire(u32 device){
    auto& env=environment;u32 result=env.acquire_controller(device);
    for(u32 remaining=400;result==0x8007001eu&&remaining;--remaining)result=env.acquire_controller(device);
}
u16 InputDevices::mix_controller(u16 buttons,u32 profile,u32 device){
    auto& env=environment;const auto& bindings=env.profiles[profile].bindings;
    if(!(*env.engine_flags&0x400)){
        LegacyJoystickState state{};state.size=sizeof(state);state.flags=0xff;
        if(env.read_legacy_joystick(device?1:0,state))return buttons;
        for(u32 i=0;i<5;++i){const auto key=bindings[binding_order[i]];if(key>=0&&(state.buttons&(1u<<(key&31))))buttons|=binding_masks[i];}
        const auto& r=*env.ranges[device];
        const u32 cx=(r.x_max+r.x_min)>>1,cy=(r.y_max+r.y_min)>>1,qx=(r.x_max-r.x_min)>>2,qy=(r.y_max-r.y_min)>>2;
        if(state.x>cx+qx)buttons|=0x80;if(state.x<cx-qx)buttons|=0x40;
        if(state.y<cy-qy)buttons|=0x10;if(state.y>cy+qy)buttons|=0x20;
    }else{
        if(env.poll_controller(device)<0){reacquire(device);return buttons;}
        DirectJoystickState state{};if(env.read_controller(device,state)<0)return buttons;
        // A damaged binding must not read beyond the platform's 128-button state.
        for(u32 i=0;i<5;++i){const auto key=bindings[binding_order[i]];if(key>=0&&key<128&&(state.buttons[key]&0x80))buttons|=binding_masks[i];}
        const i32 tx=*env.threshold_x,ty=*env.threshold_y;
        if(state.axes[0]<-tx)buttons|=0x40;if(state.axes[0]>tx)buttons|=0x80;
        if(state.axes[1]<-ty)buttons|=0x10;if(state.axes[1]>ty)buttons|=0x20;
    }
    return buttons;
}
u8* InputDevices::controller_buttons(u32 device){
    auto& env=environment;auto* output=env.button_output;std::memset(output,0,128);
    if(!(*env.engine_flags&0x400)){
        LegacyJoystickState state{};state.size=sizeof(state);state.flags=0xff;
        // The original button-configuration query always uses legacy device zero.
        if(!env.read_legacy_joystick(0,state))for(u32 i=0;i<32;++i)output[i]=(state.buttons&(1u<<i))?0x80:0;
    }else if(env.poll_controller(device)<0)reacquire(device);
    else{DirectJoystickState state{};env.read_controller(device,state);std::memcpy(output,state.buttons,128);}
    return output;
}
u16 InputDevices::sample(){return mix_controller(keyboard(),0,0);}
void InputDevices::update(u32 profile,bool use_first_bindings){environment.profiles[profile].input.update_raw(mix_controller(keyboard(),use_first_bindings?0:profile,0));}
bool InputDevices::read_keyboard(u8* keys){
    auto& env=environment;std::memset(keys,0,256);if(!*env.focused)return false;
    if(!(*env.engine_flags&0x200)){env.read_legacy_keyboard(keys);return false;}
    if(env.read_direct_keyboard(keys)!=0)env.acquire_keyboard();return true;
}
void InputDevices::release_keys(){u8 keys[256]{};environment.read_legacy_keyboard(keys);for(auto& key:keys)key&=0x7f;environment.write_legacy_keyboard(keys);}
}
