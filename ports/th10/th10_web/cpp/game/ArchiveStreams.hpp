#pragma once
#include "ResourceCodec.hpp"
namespace th10 {
struct ArchiveStreamEnvironment : CodecMemory {
    virtual u32 open_file(const char* name,u32 access,u32 creation)=0;
    virtual void close_file(u32 handle)=0;
    virtual void delete_file(const char* name)=0;
    virtual void module_filename(char* output,u32 capacity)=0;
    virtual u32 file_size(u32 handle)=0;
    virtual u32 seek_file(u32 handle,i32 offset,u32 origin)=0;
    virtual void read_file(u32 handle,u8* output,u32 length,u32* actual)=0;
    virtual void write_file(u32 handle,const u8* input,u32 length,u32* actual)=0;
    virtual u32 find_resource(const char* name)=0;
    virtual u32 load_resource(u32 resource)=0;
    virtual const u8* lock_resource(u32 resource)=0;
    virtual u32 resource_size(u32 resource)=0;
    virtual void free_resource(u32 resource)=0;
};
struct ArchiveFileStream {
    u32 original_virtual_table,handle,access;
    void initialize() noexcept {original_virtual_table=0x46f230;handle=0xffffffff;access=0;}
    void close(ArchiveStreamEnvironment& env);
    void destroy(ArchiveStreamEnvironment& env){original_virtual_table=0x46f230;close(env);original_virtual_table=0x46f210;}
    static void full_path(const char* input,char* output,ArchiveStreamEnvironment& env);
    bool open(const char* name,const char* mode,ArchiveStreamEnvironment& env);
    u32 read(u8* output,u32 length,ArchiveStreamEnvironment& env);
    bool write(const u8* input,u32 length,ArchiveStreamEnvironment& env);
    u32 position(ArchiveStreamEnvironment& env);
    u32 size(ArchiveStreamEnvironment& env);
    bool seek(i32 offset,u32 origin,ArchiveStreamEnvironment& env);
    u8* read_all(u32 limit,ArchiveStreamEnvironment& env);
};
struct ArchiveMemoryStream {
    u32 original_virtual_table,length;u8* cursor;u8* bytes;
    void initialize(bool resource) noexcept {original_virtual_table=resource?0x46f328:0x46f308;length=0;cursor=bytes=nullptr;}
    void close(CodecMemory& env);
    void destroy(CodecMemory& env){original_virtual_table=0x46f308;close(env);original_virtual_table=0x46f210;}
    u32 position() const noexcept {return static_cast<u32>(reinterpret_cast<uintptr_t>(cursor)-reinterpret_cast<uintptr_t>(bytes));}
    bool seek(i32 offset,u32 origin) noexcept;
    u32 read(u8* output,u32 count) noexcept;
    bool open_resource(const char* name,ArchiveStreamEnvironment& env);
};
static_assert(sizeof(ArchiveFileStream)==12&&sizeof(ArchiveMemoryStream)==16);
}
