#pragma once
#include "../game/Archive.hpp"
#include <map>
#include <memory>
namespace th08 {
// Owns mounted archive I/O, user files and a bounded decoded resource cache.
class ResourceManager {
    std::map<std::string,std::vector<u8>> files;
#ifdef TH_NATIVE_PLATFORM
    struct CachedResource {std::vector<u8> bytes;u64 used=0;};
    std::map<std::string,CachedResource> decoded;u64 cache_clock=0;u32 decoded_bytes=0;
#endif
    std::vector<u8> archive_bytes;std::unique_ptr<ArchiveSource> archive_io;Archive archive;std::vector<u8> file_result;
public:
    static std::string path(const char*);
    bool put(const char*,const u8*,u32);bool put_archive(const u8*,u32);
    bool mount_archive(std::unique_ptr<ArchiveSource> source){if(!source||!archive.open(*source))return false;archive_io=std::move(source);return true;}
    const std::vector<ArchiveEntry>& contents()const{return archive.contents();}
    std::vector<u8> read(const char*);std::vector<u8> read_prefix(const char*,u32);const std::vector<u8>& file(const char*);
    std::vector<std::string> user_replays();
};
}
