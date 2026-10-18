#pragma once
#include "Types.hpp"
namespace th10 {
struct MusicFormat {
    char filename[16];u32 file_offset,reserved_014; i32 loop_start,data_bytes;
    u16 encoding,channels;u32 sample_rate,bytes_per_second;u16 alignment,bits_per_sample,extra_bytes,reserved_032;
};
static_assert(sizeof(MusicFormat)==0x34&&offsetof(MusicFormat,bits_per_sample)==0x2e);
struct WaveEnvironment {
    const u32* archive_offset;
    virtual u32 open_file(const char* filename)=0;
    virtual void seek_file(u32 handle,u32 position)=0;
    virtual void read_file(u32 handle,u8* destination,u32 bytes,u32* read)=0;
    virtual void close_file(u32 handle)=0;
};
struct WaveReader {
    u32 reserved_000,reserved_004,remaining_file_bytes;
    u8 reserved_00c[0x20];
    u32 total_bytes;
    u8 reserved_030[0x48];
    i32 open_mode,memory_mode;
    const u8 *memory_begin,*memory_cursor;
    u32 memory_bytes,file_handle;
    const MusicFormat* format;
    void initialize() noexcept;
    i32 open(const char* filename,const MusicFormat* format,i32 mode,WaveEnvironment& environment);
    i32 select(const MusicFormat* format,WaveEnvironment& environment);
    i32 open_memory(const u8* bytes,u32 length,const MusicFormat* format,i32 mode) noexcept;
    i32 reset(bool loop,WaveEnvironment& environment);
    i32 read(u8* destination,u32 requested,u32* actual,WaveEnvironment& environment);
    i32 close(WaveEnvironment& environment);
};
static_assert(sizeof(WaveReader)==0x94&&offsetof(WaveReader,memory_mode)==0x7c&&offsetof(WaveReader,format)==0x90);
}
