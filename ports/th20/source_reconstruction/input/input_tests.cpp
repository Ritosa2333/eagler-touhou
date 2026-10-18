#include "test_host.hpp"
#include <iostream>
#include <stdexcept>
namespace input=th20::source::input;
namespace platform=th20::source::platform;
namespace rt=th20::source::runtime;
namespace scheduler=th20::source::scheduler;
void require(bool result,const char* message) { if(!result) throw std::runtime_error(message); }
int main() {
    try {
        for(unsigned i=0;i<4;++i)require(input::button_slot(i)==nullptr,"published button slots start null");
        input::ButtonState state{};
        for(unsigned frame=1;frame<=60;++frame) {
            state.previous=state.current; state.current=1; input::update_buttons(state);
            require(state.pressed==(frame==1?1u:0u),"rising edge");
            require(state.repeat8==(frame>=26 && (frame-26)%8==0?1u:0u),"repeat-8 cadence");
            require(state.repeat12==(frame>=26 && (frame-26)%12==0?1u:0u),"repeat-12 cadence");
            require(state.held_frames[0]==frame,"held duration");
        }
        state.previous=state.current; state.current=0; input::update_buttons(state);
        require(state.released==1 && state.held_frames[0]==0,"release resets duration");
        RecordedHost host;
        platform::Configuration configuration=platform::default_configuration();
        platform::KeyBindings mappings[2]={configuration.bindings[0],configuration.bindings[1]};
        std::int32_t kind=9;
        input::PollContext poll{host,true,200,200,mappings,kind};
        input::Device keyboard{};
        host.keys[0x0d]=0x80;host.keys[0x24]=0x80;host.keys[0x52]=0x80;
        input::poll_device(keyboard,poll);
        require((keyboard.buttons.current&0x2c0000)==0x2c0000 && kind==9,"fixed keyboard controls and retained kind");
        poll.active=false; input::poll_device(keyboard,poll);
        require(keyboard.buttons.current==0 && keyboard.buttons.released!=0,"focus loss releases keys");
        input::Device pad{};pad.kind=1;pad.buttons.current=1;
        auto old=pad.buttons;host.poll_status=DIERR_INPUTLOST;host.inputlost_count=500;
        input::poll_device(pad,poll);
        require(host.acquire_calls==401 && std::memcmp(&pad.buttons,&old,sizeof old)==0,"bounded reacquire preserves history");
        input::LegacyState legacy{};
        input::bind_button_slots(legacy);
        for(unsigned i=0;i<4;++i)require(input::button_slot(i)==&legacy.slots[i],"420990 publishes exact slot storage");
        legacy.slots[0].current=0x40;
        input::sample_startup_input(legacy,host);
        require(legacy.slots[2].current==0x40,"missing legacy device preserves stale state");
        for(unsigned i=0;i<256;++i) host.keys[i]=static_cast<std::uint8_t>(i);
        input::clear_keyboard_high_bits(host);
        for(unsigned i=0;i<256;++i) require(host.last_set[i]==(i&0x7f),"keyboard high bits");
        require(input::matches_xinput_device_id(L"USB\\VID_045E&PID_028E&IG_00",0x028e045e),"WMI XInput filter");
        require(!input::matches_xinput_device_id(L"USB\\VID_045E&PID_028E",0x028e045e),"WMI duplicate filter requires IG_");
        rt::Log log; scheduler::State chains;scheduler::initialize_state(chains);
        auto environment=rt::shared_locks().scheduler_environment();
        std::uint8_t active=1;DIDEVCAPS caps{};
        input::ControllerContext context{nullptr,nullptr,active,configuration,caps,log,legacy,host,chains,environment};
        {
            input::Controller manager(context);
            manager.device_count=2;manager.selected[0]=0;
            input::initialize_keyboard(manager.devices[0],0);
            input::initialize_xinput(manager.devices[1],0,1);
            manager.mappings[0]=configuration.bindings[0];manager.mappings[1]=configuration.bindings[1];
            std::memset(host.keys,0,256);host.keys[0x0d]=0x80;
            host.xbox_status[0]=ERROR_SUCCESS;host.xbox[0].Gamepad.wButtons=XINPUT_GAMEPAD_DPAD_LEFT;
            manager.sample_frame();
            require(manager.frame==1 && legacy.slots[2].current==0x80040 && legacy.slots[0].current==0x80040,"aggregate keyboard and controller frame");
        }
        require(input::controller==nullptr,"controller lifetime clears singleton");
        require(input::button_slot(0)==&legacy.slots[0]&&input::button_slot(2)==&legacy.slots[2],"controller destruction preserves original pointer aliases");
        auto* heap_controller=new input::Controller(context);
        rt::CallbackOwner* base=heap_controller;
        rt::retire_callback_owner(base);
        require(input::controller==nullptr,"virtual base retirement executes derived destructor");
        std::cout<<"Input source tests passed\n";return 0;
    } catch(const std::exception& error) { std::cerr<<error.what()<<'\n';return 1; }
}
