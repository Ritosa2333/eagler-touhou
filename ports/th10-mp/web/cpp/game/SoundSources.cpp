#include "SoundSources.hpp"
namespace th10 {
namespace {
u32 word(const u8* p){u32 value;std::memcpy(&value,p,4);return value;}
void copy_forward(u8* destination,const u8* source,u32 bytes){
    u32 offset=0;for(;bytes-offset>=4;offset+=4){const u32 value=word(source+offset);std::memcpy(destination+offset,&value,4);}for(;offset<bytes;++offset)destination[offset]=source[offset];
}
}
const u8* SoundSources::find_chunk(const u8* begin,u32 bytes,const char* name,u32& length){
    while(bytes){length=word(begin+4);if(!std::strncmp(reinterpret_cast<const char*>(begin),name,4))return begin+8;bytes-=length+8;begin+=length+8;}return nullptr;
}
i32 SoundSources::load(i32 index,const char* filename){
    SoundSourceLoad task;task.index=index;task.filename=filename;while(!task.advance(manager,environment))environment.sleep(10);return task.result;
}
bool SoundSources::begin(i32 index){if(!manager.driver)return false;void** output=manager.source_buffers+index;if(*output){environment.resources->release(*output);*output=nullptr;}return true;}
bool SoundSourceLoad::advance(AudioManager& manager,SoundSourceEnvironment& env){
    if(done)return true;SoundSources sources{manager,env};
    if(!started){started=true;if(!sources.begin(index)){done=true;return true;}}
    else if(waiting&&*env.load_stop==2){done=true;return true;}
    if(!manager.pending_waves[index]){waiting=true;return false;}
    result=sources.complete(index,filename);done=true;return true;
}
i32 SoundSources::complete(i32 index,const char* filename){
    auto& m=manager;auto& env=environment;auto& resources=*env.resources;void** output=m.source_buffers+index;
    u8* file=m.pending_waves[index];m.pending_waves[index]=nullptr;if(!file)return -1;
    if(std::strncmp(reinterpret_cast<const char*>(file),"RIFF",4)){env.invalid_source(true,filename);env.free_source(file);return -1;}
    const u32 chunk_bytes=word(file+4)-12;
    if(std::strncmp(reinterpret_cast<const char*>(file+8),"WAVE",4)){env.invalid_source(false,filename);env.free_source(file);return -1;}
    u32 length=0;const u8* header=find_chunk(file+12,chunk_bytes,"fmt ",length);
    if(!header){env.invalid_source(false,filename);env.free_source(file);return -1;}
    u8 format[18];std::memcpy(format,header,sizeof(format));
    const u8* source=find_chunk(file+12,chunk_bytes,"data",length);
    if(!source){env.invalid_source(false,filename);env.free_source(file);return -1;}
    SoundBufferDescription description{};description.size=sizeof(description);description.flags=0x80c8;description.bytes=length;description.format=format;
    if(resources.create_buffer(m.device,description,output)<0){env.free_source(file);return -1;}
    SoundLock lock{};if(resources.audio->lock(*output,0,length,lock,true)<0){env.free_source(file);return -1;}
    copy_forward(lock.first,source,lock.first_bytes);if(lock.second_bytes)copy_forward(lock.second,source+lock.first_bytes,lock.second_bytes);
    env.unlock_source(*output,lock);env.free_source(file);return 0;
}
}
