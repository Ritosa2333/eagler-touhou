#pragma once
#include "ResourceCodec.hpp"
namespace th10 {
struct ArchiveLifecycleEnvironment;
struct ArchiveEntry {char* name;u32 offset,size,reserved;};
struct ArchiveCipher {u8 key,step;u16 reserved;i32 block,limit;};
static_assert(sizeof(ArchiveEntry)==16&&sizeof(ArchiveCipher)==12);
struct ArchiveEnvironment : CodecMemory {
    const ArchiveCipher* ciphers;
    virtual bool seek(void* stream,u32 offset)=0;
    virtual u32 read(void* stream,u8* output,u32 length)=0;
    virtual void decrypt(u8* bytes,u32 length,const ArchiveCipher& cipher)=0;
    virtual u8* decompress(u8* bytes,u32 length,u8* output,u32 capacity)=0;
};
struct ResourceArchive {
    ArchiveEntry* entries;i32 count;char* filename;void* stream;
    static bool name_equal(const char* first,const char* second) noexcept;
    ArchiveEntry* find(const char* name) const noexcept;
    u32 size(const char* name) const noexcept;
    u8* read(const char* name,u8* output,ArchiveEnvironment& environment);
    void initialize() noexcept {entries=nullptr;count=0;filename=nullptr;stream=nullptr;}
    void release(ArchiveLifecycleEnvironment& environment);
    bool open(const char* name,bool memory,ArchiveLifecycleEnvironment& environment);
    bool load_index(const char* name,ArchiveLifecycleEnvironment& environment);
    static char* duplicate_name(const char* name,CodecMemory& memory);
    static ArchiveEntry* parse_index(const u8* bytes,i32 count,u32 table_offset,ArchiveLifecycleEnvironment& environment);
    static void release_entry(ArchiveEntry& entry,CodecMemory& memory);
    static void release_entries(ArchiveEntry* entries,ArchiveLifecycleEnvironment& environment);
};
static_assert(sizeof(ResourceArchive)==16);
struct ArchiveLifecycleEnvironment : ArchiveEnvironment {
    const char* const* open_mode;
    virtual void* allocate_object(u32 bytes)=0;
    virtual void free_object(void* object)=0;
    virtual void* create_stream(bool memory)=0;
    virtual bool open_stream(void* stream,const char* name,const char* mode)=0;
    virtual u32 stream_length(void* stream)=0;
    virtual void destroy_stream(void* stream)=0;
    virtual ArchiveEntry* build_index(const u8* data,i32 count,u32 table_offset){return ResourceArchive::parse_index(data,count,table_offset,*this);}
};
}
