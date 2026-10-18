#pragma once
#include "../game/ResourceFiles.hpp"
#include <map>
#include <string>
#include <vector>
namespace th10::browser {
struct FileHost {
    virtual u32 open(const char* name,bool write)=0;
    virtual void close(u32 handle)=0;
    virtual u32 size(u32 handle)=0;
    virtual u32 seek(u32 handle,i32 offset,u32 origin)=0;
    virtual u32 read(u32 handle,u8* bytes,u32 length)=0;
    virtual u32 write(u32 handle,const u8* bytes,u32 length)=0;
    virtual u32 list(const char* directory,const char* pattern,u32 index,char* name,u32 capacity)=0;
};
struct ArchiveEnvironment final : ArchiveLifecycleEnvironment {
    FileHost& host;u8 dictionary[8192]{};
    explicit ArchiveEnvironment(FileHost& host);
    void* allocate_object(u32 bytes) override;
    void free_object(void* object) override;
    u8* allocate_bytes(u32 bytes) override;
    void release_bytes(void* bytes) override;
    void* create_stream(bool memory) override;
    bool open_stream(void* stream,const char* name,const char* mode) override;
    u32 stream_length(void* stream) override;
    void destroy_stream(void* stream) override;
    bool seek(void* stream,u32 offset) override;
    u32 read(void* stream,u8* output,u32 length) override;
    void decrypt(u8* bytes,u32 length,const ArchiveCipher& cipher) override;
    u8* decompress(u8* bytes,u32 length,u8* output,u32 capacity) override;
};
// A single browser worker owns these objects. Imports perform synchronous
// reads from already available browser buffers, so a file operation cannot
// suspend while holding the game resource lock.
struct FileSystem final : ResourceFileEnvironment {
    FileHost& host;ArchiveEnvironment archives;ResourceArchive resources{};
    u8 depth=0;u32 handle=0xffffffff;
    // Immutable archive bytes are cached separately from caller-owned buffers.
    // Scripts still receive their own writable copy, in the original order.
    struct CachedResource {std::vector<u8> bytes;u64 used=0;};
    std::map<std::string,CachedResource> decoded;
    u64 cache_clock=0;u32 cache_bytes=0,cache_hits=0,cache_misses=0;
    static constexpr u32 cache_limit=32*1024*1024;
    bool prewarm(const char* name);
    explicit FileSystem(FileHost& host);
    ~FileSystem();
    bool attach_archive(const char* name);
    u8* allocate_bytes(u32 bytes) override;
    void release_bytes(void* bytes) override;
    void enter() override {}
    void leave() override {}
    u32 open(const char* name,bool write) override;
    u32 length(u32 handle) override;
    void read(u32 handle,u8* output,u32 count,u32* actual) override;
    void write(u32 handle,const u8* input,u32 count,u32* actual) override;
    void close(u32 handle) override;
    void discard_error(const char*) override {}
    void read_archive(const char* name,u8* output) override;
};
}
