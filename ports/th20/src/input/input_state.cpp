#include "input.hpp"
#include <cstring>
#include <stdexcept>

namespace th20::source::input {
namespace {std::array<ButtonState*,4> published_button_slots{};}
ButtonState* button_slot(unsigned index){return published_button_slots.at(index);}
ButtonState*& button_slot_storage(unsigned index){return published_button_slots.at(index);}
void bind_button_slots(LegacyState& state) noexcept{for(unsigned i=0;i<4;++i)published_button_slots[i]=&state.slots[i];}
ButtonState& initialize(ButtonState& state) noexcept { std::memset(&state,0,sizeof state); return state; }
Device& initialize(Device& device) noexcept { std::memset(&device,0,sizeof device); return device; }
void reset_device_header(Device& device) noexcept {
    device.kind=0; device.logical_index=0; device.direct_input=nullptr; device.xinput_index=0;
}
void initialize_keyboard(Device& device, std::int32_t index) noexcept { device.kind=0; device.logical_index=index; }
void initialize_xinput(Device& device, std::int32_t physical, std::int32_t logical) noexcept {
    device.kind=2; device.xinput_index=physical; device.logical_index=logical;
}
void update_buttons(ButtonState& state) noexcept {
    state.repeat8=state.repeat12=state.held8=0;
    for (unsigned i=0;i<32;++i) {
        const std::uint32_t bit=std::uint32_t{1}<<i;
        if (!(state.current&bit)) {
            state.repeat8_count[i]=state.repeat12_count[i]=state.held_frames[i]=0;
        } else {
            ++state.repeat8_count[i]; ++state.repeat12_count[i]; ++state.held_frames[i];
            if (state.repeat8_count[i]>7) state.held8|=bit;
            if (state.repeat8_count[i]>25) { state.repeat8|=bit; state.repeat8_count[i]-=8; }
            if (state.repeat12_count[i]>25) { state.repeat12|=bit; state.repeat12_count[i]-=12; }
        }
    }
    state.pressed=(state.current^state.previous)&state.current;
    state.released=(state.current^state.previous)&~state.current;
}
std::uint32_t map_bit_button(std::uint32_t& output, std::int16_t index, std::uint32_t bit, std::uint32_t raw) noexcept {
    if (index<0 || !(raw&(std::uint32_t{1}<<(static_cast<std::uint16_t>(index)&31)))) return 0;
    output|=bit; return bit;
}
std::uint32_t map_byte_button(std::uint32_t& output, std::int16_t index, std::uint32_t bit, const std::uint8_t* raw) {
    if (index<0 || !(raw[index]&0x80)) return 0;
    output|=bit; return bit;
}
namespace {
std::uint32_t directions(const XINPUT_GAMEPAD& pad) noexcept {
    std::uint32_t bits=pad.wButtons;
    if (pad.sThumbLY>7848) bits|=1;
    if (pad.sThumbLY<-7848) bits|=2;
    if (pad.sThumbLX>7848) bits|=8;
    if (pad.sThumbLX<-7848) bits|=4;
    return bits;
}
std::uint32_t mapped_directions(std::uint32_t bits) noexcept {
    return ((bits&1)?0x10u:0u)|((bits&2)?0x20u:0u)|((bits&4)?0x40u:0u)|((bits&8)?0x80u:0u);
}
std::int16_t signed_mapping(std::uint16_t value) noexcept { return static_cast<std::int16_t>(value); }
}
void poll_device(Device& device, PollContext& context) {
    std::memset(device.raw,0,sizeof device.raw);
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(15));
    // The original selected_device(1) always returns -1, even when the stored
    // second selection differs. Preserve its comparison for unusual indices.
    const auto& mapping=context.mappings[device.logical_index==-1?1:0];
    std::uint32_t bits=0;
    if (device.kind==0) {
        if (context.active) {
            context.host.keyboard(device.raw);
            const auto key=[&](unsigned index, std::uint32_t mask) {
                if (index>=256) throw std::out_of_range("Original keyboard mapping is outside the 256-byte state");
                if (device.raw[index]&0x80) bits|=mask;
            };
            constexpr std::uint32_t configured[8]={1,4,8,0x100,0x10,0x20,0x40,0x80};
            for(unsigned i=0;i<8;++i) key(mapping.keyboard[i],configured[i]);
            // Fixed VKs are recovered from byte offsets, not guessed controls:
            // +2dd Enter, +2f4 Home, +320 P, +2f3 End, +322 R.
            key(0x0d,0x80000); key(0x24,0x40000); key(0x50,0x40000);
            key(0x23,0x200000); key(0x52,0x200000);
            key(0x68,0x10); key(0x62,0x20); key(0x64,0x40); key(0x66,0x80);
            key(0x67,0x50); key(0x69,0x90); key(0x61,0x60); key(0x63,0xa0);
            // The original tests the still-zero accumulator BEFORE mapping
            // keyboard keys, so keyboard input does not set last_input_kind.
        }
    } else if (device.kind==1) {
        if (FAILED(context.host.poll(device.direct_input))) {
            HRESULT result=context.host.acquire(device.direct_input);
            for(unsigned retry=0;retry<400 && result==DIERR_INPUTLOST;++retry)
                result=context.host.acquire(device.direct_input);
            return; // preserve all button history even after successful reacquire
        }
        DIJOYSTATE2 raw{};
        if (FAILED(context.host.device_state(device.direct_input,&raw))) return;
        for(unsigned index : {0u,1u,3u,2u}) {
            const auto button=signed_mapping(mapping.pad[index]);
            if (button>=128) throw std::out_of_range("DirectInput binding exceeds rgbButtons");
            constexpr std::uint32_t actions[4]={1,4,8,0x100};
            map_byte_button(bits,button,actions[index],raw.rgbButtons);
        }
        if(raw.lX>context.deadzone_x) bits|=0x80;
        if(raw.lX<-static_cast<std::int32_t>(context.deadzone_x)) bits|=0x40;
        if(raw.lY>context.deadzone_y) bits|=0x20;
        if(raw.lY<-static_cast<std::int32_t>(context.deadzone_y)) bits|=0x10;
        for(unsigned i=0;i<32;++i) if(raw.rgbButtons[i]) device.raw[i]=0x80;
        if(bits) context.last_input_kind=device.kind;
    } else if (device.kind==2) {
        XINPUT_STATE raw{};
        if(context.host.xinput(static_cast<DWORD>(device.xinput_index),&raw)==ERROR_SUCCESS) {
            auto raw_bits=directions(raw.Gamepad);
            if(raw.Gamepad.bLeftTrigger>29) raw_bits|=0x10000;
            if(raw.Gamepad.bRightTrigger>29) raw_bits|=0x20000;
            bits|=mapped_directions(raw_bits);
            constexpr std::uint32_t actions[4]={1,4,8,0x100};
            for(unsigned i=0;i<4;++i) {
                const auto index=mapping.alternate_pad[i];
                if(index>=12) throw std::out_of_range("XInput binding exceeds recovered 12-entry table");
                if(raw_bits&xinput_button_masks[index]) bits|=actions[i];
            }
            for(unsigned i=0;i<12;++i) if(raw_bits&xinput_button_masks[i]) device.raw[i]=0x80;
            if(bits) context.last_input_kind=device.kind;
        }
    }
    device.buttons.previous=device.buttons.current;
    device.buttons.current=bits;
    update_buttons(device.buttons);
}
int probe_legacy_devices(LegacyState& states, Host& host, std::uint32_t& flags) {
    JOYINFOEX info{}; info.dwSize=sizeof info; info.dwFlags=0xff;
    if(host.joystick(0,&info)!=JOYERR_NOERROR && host.joystick(1,&info)!=JOYERR_NOERROR) {
        flags&=~0x1000u; return 1;
    }
    host.joystick_caps(0,&states.capabilities[0]); host.joystick_caps(1,&states.capabilities[1]);
    flags|=0x1000; return 0;
}
void clear_keyboard_high_bits(Host& host) {
    std::uint8_t state[256]{};
    if(!host.keyboard(state)) throw std::runtime_error("GetKeyboardState failed while clearing input");
    for(auto& byte:state) byte&=0x7f;
    host.set_keyboard(state);
}
void poll_legacy_device(LegacyState& states, Host& host, unsigned index) {
    if(index>=2) throw std::out_of_range("Legacy joystick index");
    JOYINFOEX info{}; info.dwSize=sizeof info; info.dwFlags=0xff;
    if(host.joystick(index!=0,&info)!=JOYERR_NOERROR) return;
    std::uint32_t bits=0;
    for(std::int16_t i=0;i<4;++i) map_bit_button(bits,i,1,info.dwButtons);
    const auto& caps=states.capabilities[index];
    const auto xquarter=(caps.wXmax-caps.wXmin)>>2;
    const auto xmiddle=(caps.wXmin+caps.wXmax)>>1;
    const auto yquarter=(caps.wYmax-caps.wYmin)>>2;
    const auto ymiddle=(caps.wYmin+caps.wYmax)>>1;
    if(info.dwXpos>xmiddle+xquarter) bits|=0x80;
    if(info.dwXpos<xmiddle-xquarter) bits|=0x40;
    if(info.dwYpos>ymiddle+yquarter) bits|=0x20;
    if(info.dwYpos<ymiddle-yquarter) bits|=0x10;
    auto& output=states.slots[index]; output.previous=output.current; output.current=bits; update_buttons(output);
}
void sample_startup_input(LegacyState& states, Host& host) {
    auto& output=states.slots[2]; output.previous=output.current;
    poll_legacy_device(states,host,0); poll_legacy_device(states,host,1);
    std::uint32_t bits=0;
    for(DWORD index=0;index<4;++index) {
        XINPUT_STATE raw{};
        if(host.xinput(index,&raw)!=ERROR_SUCCESS) continue;
        const auto raw_bits=directions(raw.Gamepad);
        bits|=mapped_directions(raw_bits);
        if(raw_bits&0xf000) bits|=1;
    }
    output.current=states.slots[0].current|states.slots[1].current|bits;
    update_buttons(output);
}
}
