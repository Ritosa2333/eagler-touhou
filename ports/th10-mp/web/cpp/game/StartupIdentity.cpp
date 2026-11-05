#include "StartupIdentity.hpp"
namespace th10 {
namespace {
bool link_extension(const char* extension){if(!extension)return false;constexpr char suffix[]=".lnk";for(u32 i=0;i<5;i++){const auto c=static_cast<u8>(extension[i]);const u8 lower=c>='A'&&c<='Z'?c+32:c;if(lower!=suffix[i])return false;}return true;}
}
i32 StartupIdentity::check(){
    auto& env=environment;*env.mutex=env.create_mutex(env.mutex_name);
    if(env.last_error()==183){env.already_running();return -1;}
    StartupInformation info{};info.size=sizeof(info);char module[264]{},launch[264]{};
    env.module_filename(module,261);env.console_title(launch,261);env.startup_information(info);
    if(info.title){
        const char* extension=std::strrchr(info.title,'.');
        if(env.file_exists(info.title)&&extension){
            if(link_extension(extension)){do{env.resolve_shortcut(info.title,launch,260);}while(link_extension(std::strrchr(launch,'.')));}
            else{const auto n=std::strlen(info.title);if(n<sizeof(launch))std::memcpy(launch,info.title,n+1);else launch[0]=0;}
            if(std::strcmp(module,launch)!=0)*env.alternate_launch=1;
        }
        *env.engine_flags&=~0x40u;
    }else *env.engine_flags|=0x40;
    return *env.mutex?0:-1;
}
bool resolve_shell_shortcut(const char* name,char* output,u32 capacity,ShortcutEnvironment& env){
    if(!output)return false;bool success=false;env.initialize();void* link=nullptr;
    if(env.create_link(&link)>=0){void* persist=nullptr;if(env.query_persist(link,&persist)>=0){auto* wide=env.allocate_name(capacity*2);env.convert_name(name,wide,capacity);if(env.load(persist,wide)>=0){u8 file_data[320]{};if(env.get_path(link,output,capacity,file_data)>=0)success=true;}env.delete_name(wide);env.release(persist);}env.release(link);}
    env.uninitialize();return success;
}
u32 display_dialog(u32 window,u32 message,u32 parameter,DisplayDialogEnvironment& env){
    if(message==0x110){env.set_checked(env.get_control(window,202));return 0;}
    if(message==0x111){const auto control=parameter&0xffff;if(control==201||control==203){const auto checked=env.is_checked(window,202);if(checked==1)*env.display_flags|=0x100;else *env.display_flags&=~0x100u;env.finish(window,control==201?6:7);}}
    return 0;
}
}
