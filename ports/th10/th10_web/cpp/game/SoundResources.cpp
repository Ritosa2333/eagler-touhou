#include "SoundResources.hpp"
namespace th10 {
void SoundResources::initialize(SoundBuffer& value,void* const* source,u32 count,u32 bytes,WaveReader* wave){
    auto& env=environment;value.virtual_table=env.buffer_vtable;value.buffers=static_cast<void**>(env.allocate(count*4));
    for(u32 i=0;i<count;++i)value.buffers[i]=source[i];value.buffer_bytes=bytes;value.buffer_count=count;value.wave=wave;
    env.audio->refill(value,value.buffers[0],false);for(u32 i=0;i<count;++i)env.audio->position(value.buffers[i],0);value.active=0;
}
void SoundResources::initialize_stream(SoundStream& value,void* buffer,u32 bytes,WaveReader* wave,u32 chunk){
    auto& env=environment;value.virtual_table=env.buffer_vtable;value.buffers=static_cast<void**>(env.allocate(4));value.buffers[0]=buffer;
    value.wave=wave;value.buffer_bytes=bytes;value.buffer_count=1;env.audio->refill(value,value.buffers[0],false);env.audio->position(value.buffers[0],0);
    value.active=value.previous_play_cursor=value.played_bytes=value.write_cursor=value.silent=0;value.virtual_table=env.stream_vtable;value.chunk_bytes=chunk;
}
void SoundResources::release_buffers(SoundBuffer& value){
    auto& env=environment;for(u32 i=0;i<value.buffer_count;++i)if(value.buffers[i]){env.release(value.buffers[i]);value.buffers[i]=nullptr;}
    if(value.buffers){env.free(value.buffers);value.buffers=nullptr;}
}
void SoundResources::release(SoundBuffer& value){
    value.virtual_table=environment.buffer_vtable;release_buffers(value);
    if(auto* wave=value.wave){wave->close(*environment.audio->waves);environment.free(wave);value.wave=nullptr;}
}
i32 SoundResources::recreate_notifications(NotifiedSoundStream& value){
    auto& env=environment;value.active=0;release_buffers(value);value.buffers=static_cast<void**>(env.allocate(value.buffer_count*4));void* notification=nullptr;
    constexpr i32 failed=static_cast<i32>(0x80004005u);
    for(u32 i=0;i<value.buffer_count;++i){
        if(env.create_buffer(*value.device,value.description,&value.buffers[i])<0)return failed;
        if(env.query_notifications(value.buffers[i],&notification)<0)return failed;
        auto* positions=static_cast<SoundNotification*>(env.allocate(16*sizeof(SoundNotification)));if(!positions)return static_cast<i32>(0x8007000eu);
        for(u32 j=0;j<16;++j)positions[j]={value.chunk_bytes*(j+1)-1,value.notification_event};
        const bool failed_positions=env.set_notifications(notification,16,positions)<0;
        if(notification){env.release(notification);notification=nullptr;}env.free(positions);if(failed_positions)return failed;
    }
    return 0;
}
i32 SoundResources::create_file_stream(NotifiedSoundStream** output,void** driver,u32 flags,const char* filename,const u8* algorithm,u32 count,u32 chunk,u32 event,const MusicFormat* format){
    auto& env=environment;u8 copied_algorithm[16];std::memcpy(copied_algorithm,algorithm,16);
    if(!*driver)return static_cast<i32>(0x800401f0u);
    auto* wave=static_cast<WaveReader*>(env.allocate(sizeof(WaveReader)));if(!wave)return static_cast<i32>(0x8007000eu);wave->initialize();
    if(wave->open(filename,format,1,*env.audio->waves)!=0){wave->close(*env.audio->waves);env.free(wave);return static_cast<i32>(0x80004005u);}
    return create_stream(output,driver,flags,wave,copied_algorithm,count,chunk,event);
}
i32 SoundResources::create_memory_stream(NotifiedSoundStream** output,void** driver,u32 flags,const u8* data,u32 bytes,const MusicFormat* format,const u8* algorithm,u32 count,u32 chunk,u32 event){
    auto& env=environment;u8 copied_algorithm[16];std::memcpy(copied_algorithm,algorithm,16);
    if(!*driver)return static_cast<i32>(0x800401f0u);
    auto* wave=static_cast<WaveReader*>(env.allocate(sizeof(WaveReader)));if(!wave)return static_cast<i32>(0x8007000eu);wave->initialize();wave->open_memory(data,bytes,format,1);
    return create_stream(output,driver,flags,wave,copied_algorithm,count,chunk,event);
}
i32 SoundResources::create_stream(NotifiedSoundStream** output,void** driver,u32 flags,WaveReader* wave,const u8* algorithm,u32 count,u32 chunk,u32 event){
    auto& env=environment;constexpr i32 failed=static_cast<i32>(0x80004005u);const u32 bytes=count*chunk;
    SoundBufferDescription description{};description.size=sizeof(description);description.flags=flags|0x18188;description.bytes=bytes;
    description.format=reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(wave->format)+0x20);std::memcpy(description.algorithm,algorithm,16);
    void *buffer=nullptr,*notification=nullptr;
    // These partial failure paths deliberately preserve the original ownership.
    if(env.create_buffer(*driver,description,&buffer)<0)return failed;
    if(env.query_notifications(buffer,&notification)<0)return failed;
    auto* positions=static_cast<SoundNotification*>(env.allocate(count*sizeof(SoundNotification)));if(!positions)return static_cast<i32>(0x8007000eu);
    for(u32 i=0;i<count;++i)positions[i]={chunk*(i+1)-1,event};
    const i32 result=env.set_notifications(notification,count,positions);
    if(notification)env.release(notification);env.free(positions);if(result<0)return failed;
    auto* stream=static_cast<NotifiedSoundStream*>(env.allocate(sizeof(NotifiedSoundStream)));if(!stream)return static_cast<i32>(0x8007000eu);
    initialize_stream(*stream,buffer,bytes,wave,chunk);*output=stream;
    stream->description=description;(*output)->device=driver;(*output)->notification_event=event;(*output)->notification_busy=0;
    return 0;
}
}
