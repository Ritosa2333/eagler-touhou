#pragma once
#include "Types.hpp"
namespace th10 {
struct ApplicationConfig {
    u32 version;u16 keys[9];u16 axis_x,axis_y;
    u8 options[6];std::int8_t music_volume,effects_volume;u8 music_mode;u8 reserved[13];u32 display_flags;
    void initialize(const u16* current_keys) noexcept;
    bool valid(u32 length) const noexcept;
};
enum class ConfigurationNotice {Missing,Invalid,SkipTextures,SmallTextures,Windowed,Software,ForceHardware,IgnoreTextureCaps,DisableVsync,TextureFilter,SaveFailed,SaveHelp};
struct ApplicationConfigEnvironment {
    ApplicationConfig* global;u16* keys;i32* disable_vsync;
    virtual u8* load(const char* name,u32* length)=0;
    virtual void release(void* memory)=0;
    virtual i32 save(const char* name,const ApplicationConfig& settings)=0;
    virtual void notice(ConfigurationNotice notice,const char* name)=0;
};
struct ApplicationConfiguration {
    ApplicationConfigEnvironment& environment;
    i32 load(const char* name,const ApplicationConfig& settings,i32& timing_option);
};
static_assert(sizeof(ApplicationConfig)==0x34&&offsetof(ApplicationConfig,display_flags)==0x30);
}
