#pragma once
#include "Types.hpp"
namespace th10 {
struct StartupInformation {u32 size;char *reserved,*desktop,*title;u32 other[13];};
static_assert(sizeof(StartupInformation)==68);
struct StartupIdentityEnvironment {
    u32* mutex;u32* engine_flags;u8* alternate_launch;const char* mutex_name;
    virtual u32 create_mutex(const char* name)=0;
    virtual u32 last_error()=0;
    virtual void already_running()=0;
    virtual void module_filename(char* output,u32 capacity)=0;
    virtual void console_title(char* output,u32 capacity)=0;
    virtual void startup_information(StartupInformation& information)=0;
    virtual bool file_exists(const char* name)=0;
    virtual bool resolve_shortcut(const char* name,char* output,u32 capacity)=0;
};
struct StartupIdentity {StartupIdentityEnvironment& environment;i32 check();};
struct ShortcutEnvironment {
    virtual void initialize()=0;
    virtual void uninitialize()=0;
    virtual i32 create_link(void** output)=0;
    virtual i32 query_persist(void* link,void** output)=0;
    virtual u16* allocate_name(u32 bytes)=0;
    virtual void delete_name(void* memory)=0;
    virtual void convert_name(const char* name,u16* output,u32 capacity)=0;
    virtual i32 load(void* persist,const u16* name)=0;
    virtual i32 get_path(void* link,char* output,u32 capacity,void* file_data)=0;
    virtual void release(void* object)=0;
};
bool resolve_shell_shortcut(const char* name,char* output,u32 capacity,ShortcutEnvironment& environment);
struct DisplayDialogEnvironment {
    u32* display_flags;
    virtual u32 get_control(u32 window,u32 control)=0;
    virtual void set_checked(u32 control)=0;
    virtual u32 is_checked(u32 window,u32 control)=0;
    virtual void finish(u32 window,u32 result)=0;
};
u32 display_dialog(u32 window,u32 message,u32 parameter,DisplayDialogEnvironment& environment);
}
