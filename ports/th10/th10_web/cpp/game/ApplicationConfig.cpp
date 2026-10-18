#include "ApplicationConfig.hpp"
namespace th10 {
void ApplicationConfig::initialize(const u16* current_keys) noexcept {
    std::memset(this,0,sizeof(*this));version=0x100003;axis_x=axis_y=600;options[1]=options[2]=1;display_flags|=0x100;
    // Match the original four DWORD loads followed by one WORD load, including
    // overlap with the live input mapping.
    for(u32 i=0;i<4;i++){u32 value;std::memcpy(&value,current_keys+i*2,4);std::memcpy(keys+i*2,&value,4);}keys[8]=current_keys[8];
    options[5]=2;music_volume=100;effects_volume=80;
}
bool ApplicationConfig::valid(u32 length) const noexcept {return options[0]<2&&options[1]<3&&options[2]<2&&options[3]<2&&options[4]<3&&options[5]<3&&version==0x100003&&length==sizeof(*this);}
i32 ApplicationConfiguration::load(const char* name,const ApplicationConfig& settings,i32& timing_option){
    auto& env=environment;env.global->initialize(env.keys);u32 length=0;auto* bytes=env.load(name,&length);bool valid=false;
    if(bytes){for(u32 i=0;i<13;i++){u32 value;std::memcpy(&value,bytes+i*4,4);std::memcpy(reinterpret_cast<u8*>(env.global)+i*4,&value,4);}env.release(bytes);valid=env.global->valid(length);}
    if(valid){for(u32 i=0;i<4;i++){u32 value;std::memcpy(&value,env.global->keys+i*2,4);std::memcpy(env.keys+i*2,&value,4);}env.keys[8]=env.global->keys[8];}
    else{env.notice(bytes?ConfigurationNotice::Invalid:ConfigurationNotice::Missing,name);env.global->initialize(env.keys);}
    timing_option=0;
    if(settings.display_flags&4)env.notice(ConfigurationNotice::SkipTextures,name);
    if(settings.display_flags&1)env.notice(ConfigurationNotice::SmallTextures,name);
    if(settings.options[3])env.notice(ConfigurationNotice::Windowed,name);
    if(settings.display_flags&2)env.notice(ConfigurationNotice::Software,name);
    if(settings.display_flags&8)env.notice(ConfigurationNotice::ForceHardware,name);
    if(settings.display_flags&0x10)env.notice(ConfigurationNotice::IgnoreTextureCaps,name);
    if(settings.display_flags&0x20){env.notice(ConfigurationNotice::DisableVsync,name);*env.disable_vsync=1;}
    if(settings.display_flags&0x40)env.notice(ConfigurationNotice::TextureFilter,name);
    if(env.save(name,*env.global)!=0){env.notice(ConfigurationNotice::SaveFailed,name);env.notice(ConfigurationNotice::SaveHelp,name);return -1;}return 0;
}
}
