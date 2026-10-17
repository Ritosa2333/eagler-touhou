#pragma once
#include "Types.hpp"
namespace th10 {
struct WindowMessageEnvironment {
    u32* engine_flags;u32* active;u32* inactive;const u8* windowed;void** midi;
    virtual u32 default_message(u32 window,u32 message,u32 parameter,u32 detail)=0;
    virtual void show_cursor(bool visible)=0;
    virtual u32 load_cursor(u32 instance,u32 resource)=0;
    virtual void set_cursor(u32 cursor)=0;
    virtual void foreground(u32 window)=0;
    virtual void midi_completed(void* midi,u32 header)=0;
};
struct WindowMessages {
    WindowMessageEnvironment& environment;
    u32 dispatch(u32 window,u32 message,u32 parameter,u32 detail);
};
}
