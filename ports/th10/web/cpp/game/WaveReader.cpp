#include "WaveReader.hpp"
namespace th10 {
namespace {constexpr i32 uninitialized=static_cast<i32>(0x800401f0u);}
void WaveReader::initialize() noexcept {format=nullptr;reserved_000=0;total_bytes=0;memory_mode=0;}
i32 WaveReader::open(const char* filename,const MusicFormat* info,i32 mode,WaveEnvironment& env){
    open_mode=mode;memory_mode=0;if(mode!=1)return 0;
    if(!filename)return static_cast<i32>(0x80070057u);
    file_handle=env.open_file(filename);if(file_handle==0xffffffffu)return static_cast<i32>(0x80004005u);
    format=info;
    // Platform callbacks can change the reader while the file is opened.
    if(memory_mode){memory_cursor=memory_begin;if(format->data_bytes>0)memory_bytes=format->data_bytes;}
    else if(file_handle){env.seek_file(file_handle,format->file_offset+*env.archive_offset);remaining_file_bytes=format->data_bytes;}
    total_bytes=remaining_file_bytes;return 0;
}
i32 WaveReader::select(const MusicFormat* info,WaveEnvironment& env){
    if(memory_mode||file_handle==0xffffffffu)return static_cast<i32>(0x80004005u);format=info;
    if(file_handle){env.seek_file(file_handle,format->file_offset+*env.archive_offset);remaining_file_bytes=format->data_bytes;}
    total_bytes=remaining_file_bytes;return 0;
}
i32 WaveReader::open_memory(const u8* bytes,u32 length,const MusicFormat* info,i32 mode) noexcept {format=info;memory_bytes=length;memory_begin=memory_cursor=bytes;memory_mode=1;return mode==1?0:static_cast<i32>(0x80004001u);}
i32 WaveReader::reset(bool loop,WaveEnvironment& env){
    if(memory_mode){const auto* begin=memory_begin;const auto* info=format;memory_cursor=begin;if(info->data_bytes>0)memory_bytes=info->data_bytes;if(loop&&info->loop_start>0)memory_cursor=begin+info->loop_start;return 0;}
    if(!file_handle)return uninitialized;
    const auto* info=format;if(loop&&info->loop_start>0){env.seek_file(file_handle,info->file_offset+static_cast<u32>(info->loop_start)+*env.archive_offset);remaining_file_bytes=static_cast<u32>(format->data_bytes)-static_cast<u32>(format->loop_start);}
    else{env.seek_file(file_handle,info->file_offset+*env.archive_offset);remaining_file_bytes=format->data_bytes;}return 0;
}
i32 WaveReader::read(u8* destination,u32 requested,u32* actual,WaveEnvironment& env){
    if(memory_mode){
        if(!memory_cursor)return uninitialized;if(actual)*actual=0;
        const auto begin=static_cast<u32>(reinterpret_cast<uintptr_t>(memory_begin)),cursor=static_cast<u32>(reinterpret_cast<uintptr_t>(memory_cursor));
        if(cursor+requested>begin+memory_bytes)requested=begin-cursor+memory_bytes;
        // REP MOVSD followed by MOVSB has a defined forward order, even when
        // an overlapping destination was supplied to the original function.
        const u8* source=memory_cursor;u32 offset=0;
        for(;offset+4<=requested;offset+=4){u32 word;std::memcpy(&word,source+offset,4);std::memcpy(destination+offset,&word,4);}
        for(;offset<requested;++offset)destination[offset]=source[offset];
        memory_cursor+=requested;if(actual)*actual=requested;return 0;
    }
    if(!file_handle)return uninitialized;if(!destination||!actual)return static_cast<i32>(0x80070057u);
    if(requested>remaining_file_bytes)requested=remaining_file_bytes;remaining_file_bytes-=requested;
    u32 count=0;env.read_file(file_handle,destination,requested,&count);*actual=count;return 0;
}
i32 WaveReader::close(WaveEnvironment& env){if(open_mode==1){env.close_file(file_handle);file_handle=0xffffffffu;}return 0;}
}
