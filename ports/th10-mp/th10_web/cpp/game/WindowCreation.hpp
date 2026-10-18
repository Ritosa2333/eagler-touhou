#pragma once
#include "Types.hpp"
namespace th10 {
struct WindowClass {u32 style,procedure; i32 class_extra,window_extra;u32 instance,icon,cursor,background;const char* menu;const char* name;};
struct WindowCreationEnvironment {
    u32 *window,*loop_window,*active,*inactive;const u8* windowed;
    u32 procedure;const char* class_name;const char* title;
    virtual u32 stock_object(u32 index)=0;
    virtual u32 load_cursor(u32 instance,u32 resource)=0;
    virtual void register_class(const WindowClass& definition)=0;
    virtual i32 metric(u32 index)=0;
    virtual u32 create_window(const char* name,const char* title,u32 style,i32 x,i32 y,i32 width,i32 height,u32 instance)=0;
    virtual void send_message(u32 window,u32 message,u32 parameter,u32 detail)=0;
    virtual void sleep(u32 milliseconds)=0;
};
struct WindowCreation {WindowCreationEnvironment& environment;i32 create(u32 instance);};
static_assert(sizeof(WindowClass)==40);
}
