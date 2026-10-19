#include "AudioResources.hpp"
namespace th10 {
u32 AudioResources::start_worker(const MusicFormat& format){
    const u32 alignment=format.alignment;
    // Preserve the unsigned product/shift overflow before rounding to a sample.
    const u32 chunk=((format.sample_rate*alignment)<<2)>>4;
    manager.notification_event=environment.create_event();
    manager.music_thread=environment.create_worker(*environment.worker_argument,&manager.music_thread_id);
    if(!alignment)__builtin_trap();return chunk-chunk%alignment;
}
i32 AudioResources::start_file_music(const char* filename){
    auto& m=manager;auto& env=environment;std::strcpy(m.music_filename,filename);
    if(!m.driver||!m.device)return -1;
    env.unload_music(m);const MusicFormat* format=m.formats;const u32 chunk=start_worker(*format);
    return env.create_file_stream(m.driver,&m.music,filename,format,chunk,m.notification_event)<0?-1:0;
}
i32 AudioResources::prepare_track(i32 index,const char* name){
    auto& m=manager;auto& env=environment;
    if(m.cached_data[index]&&!std::strcmp(name,m.music_names[index]))return 0;
    std::strcpy(env.global_music_names[index],name);
    if(!(*env.display_flags&16)||!m.driver)return 0;
    if(m.cached_data[index]){env.free_cache(m.cached_data[index]);m.cached_data[index]=nullptr;}
    const u32 handle=env.files->open_file(m.music_filename);if(handle==0xffffffffu)return -1;
    const i32 record=env.track_index(m,name);env.files->seek_file(handle,m.formats[record].file_offset);
    u8* data=env.allocate_cache(m.formats[record].reserved_014);
    if(!data){env.files->close_file(handle);return -1;}
    u32 actual=0;env.files->read_file(handle,data,m.formats[record].reserved_014,&actual);env.files->close_file(handle);
    m.cached_formats[index]=m.formats+record;m.cached_data[index]=m.cached_cursors[index]=data;m.cached_sizes[index]=m.formats[record].reserved_014;
    return 0;
}
i32 AudioResources::start_track(i32 index){
    auto& m=manager;auto& env=environment;
    if(!m.driver||!*env.music_enabled||!m.device)return -1;
    if(!(*env.display_flags&16))return env.select_music(m,m.music_names[index]);
    if(!m.cached_data[index])return -1;
    const u32 chunk=start_worker(*m.cached_formats[index]);
    if(env.create_memory_stream(m.driver,&m.music,m.cached_cursors[index],m.cached_sizes[index],m.cached_formats[index],chunk,m.notification_event)<0)return -1;
    m.current_cache_index=index;return 0;
}
}
