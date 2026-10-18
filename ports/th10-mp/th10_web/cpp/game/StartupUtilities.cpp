#include "StartupUtilities.hpp"
namespace th10 {
i32 StartupUtilities::checksum(){
    auto& env=environment;char path[264]{};if(!env.module_filename(path,261))return -1;
    u32 length=0;auto* bytes=env.load_program(path,&length);if(!bytes)return -1;const u32 file_size=length;
    u32 sum=0;const i32 words=static_cast<i32>(length)/4-1;for(i32 i=0;i<words;i++){u32 value;std::memcpy(&value,bytes+static_cast<u32>(i)*4,4);sum+=value;}
    env.free_program(bytes);*env.program_size=file_size;*env.program_checksum=sum;return static_cast<i32>(sum);
}
u32 StartupUtilities::detect_joystick(){
    auto& env=environment;u32 state[13]{};state[0]=52;state[1]=255;
    if(env.joystick_position(0,state)!=0&&env.joystick_position(1,state)!=0)return (static_cast<u32>(reinterpret_cast<uintptr_t>(env.no_joystick()))&0xffff0000)|1;
    return env.joystick_capabilities_for(0,env.joystick_capabilities,404)&0xffff0000;
}
u32 StartupUtilities::clear_keyboard(){auto& env=environment;u8 state[256]{};env.keyboard_state(state);for(auto& key:state)key&=0x7f;return env.set_keyboard_state(state);}
}
