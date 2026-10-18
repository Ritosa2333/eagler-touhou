#pragma once
#include "Types.hpp"
namespace th10 {
struct StartupUtilityEnvironment {
    u32* program_checksum;u32* program_size;void* joystick_capabilities;
    virtual u32 module_filename(char* output,u32 capacity)=0;
    virtual u8* load_program(const char* name,u32* length)=0;
    virtual void free_program(void* bytes)=0;
    virtual u32 joystick_position(u32 device,u32* state)=0;
    virtual u32 joystick_capabilities_for(u32 device,void* output,u32 bytes)=0;
    virtual const char* no_joystick()=0;
    virtual void keyboard_state(u8* output)=0;
    virtual u32 set_keyboard_state(const u8* input)=0;
};
struct StartupUtilities {
    StartupUtilityEnvironment& environment;
    i32 checksum();
    u32 detect_joystick();
    u32 clear_keyboard();
};
}
