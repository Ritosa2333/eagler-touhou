#include "ResourceFiles.hpp"
#ifdef TH_ENABLE_THCRAP
#include "RuntimeOverride.hpp"
#endif
namespace th10 {
void ResourceFiles::lock(){environment.enter();++*environment.lock_depth;}
void ResourceFiles::unlock(){environment.leave();--*environment.lock_depth;}
u8* ResourceFiles::load(const char* name,u32* size,bool external){
    auto& env=environment;lock();u8* bytes=nullptr;
#ifdef TH_ENABLE_THCRAP
    // thcrap-style offline pack: /thcrap/th10/<entry> wins over the original
    // archive entry (dialogue .msg, endings, .anm and PNG overrides). External
    // save files stay untouched.
    if(!external){
        const char* last_backslash=std::strrchr(name,'\\');const char* slash=std::strrchr(last_backslash?last_backslash+1:name,'/');if(slash)name=slash+1;
        std::vector<u8> override_bytes;
        if(RuntimeOverride::Read(name,override_bytes)){
            const u32 length=u32(override_bytes.size());if(size)*size=length;
            if(length){bytes=env.allocate_bytes(length);if(bytes)std::memcpy(bytes,override_bytes.data(),length);}
            unlock();return bytes;
        }
    }
#endif
    if(!external){
        // Backslashes restrict the subsequent slash search, but without a
        // matching slash the original keeps the entire input path.
        const char* last_backslash=std::strrchr(name,'\\');const char* slash=std::strrchr(last_backslash?last_backslash+1:name,'/');if(slash)name=slash+1;
        const u32 length=env.archive->size(name);if(size)*size=length;
        if(length){bytes=env.allocate_bytes(length);if(bytes)env.read_archive(name,bytes);}
    }else{
        const u32 handle=env.open(name,false);if(handle!=0xffffffffu){
            u32 count=env.length(handle);bytes=env.allocate_bytes(count);
            if(bytes){env.read(handle,bytes,count,&count);if(size)*size=count;}env.close(handle);
        }
    }
    unlock();return bytes;
}
bool ResourceFiles::exists(const char* name){lock();const u32 handle=environment.open(name,false);const bool found=handle!=0xffffffffu;if(found)environment.close(handle);unlock();return found;}
i32 ResourceFiles::save(const char* name,const u8* bytes,u32 length,u32 actual){
    auto& env=environment;lock();const u32 handle=env.open(name,true);
    if(handle==0xffffffffu){env.discard_error(name);unlock();return -1;}
    env.write(handle,bytes,length,&actual);const bool complete=actual==length;env.close(handle);unlock();return complete?0:-2;
}
i32 ResourceFiles::open(const char* name,bool write){
    auto& env=environment;lock();*env.current_handle=env.open(name,write);
    if(*env.current_handle!=0xffffffffu)return 0;env.discard_error(name);unlock();return -1;
}
i32 ResourceFiles::write(const u8* bytes,u32 length,u32 actual){
    auto& env=environment;if(*env.current_handle==0xffffffffu)return -1;
    env.write(*env.current_handle,bytes,length,&actual);if(actual==length)return 0;env.close(*env.current_handle);unlock();return -2;
}
u8* ResourceFiles::read(u32 length){
    auto& env=environment;if(*env.current_handle==0xffffffffu)return nullptr;u8* bytes=env.allocate_bytes(length);
    if(!bytes){env.close(*env.current_handle);return nullptr;}u32 actual=0;env.read(*env.current_handle,bytes,length,&actual);return bytes;
}
void ResourceFiles::close(){auto& env=environment;if(*env.current_handle!=0xffffffffu){env.close(*env.current_handle);unlock();}}
}
