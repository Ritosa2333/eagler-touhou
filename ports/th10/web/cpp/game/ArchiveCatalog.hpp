#pragma once
#include "ResourceArchive.hpp"
#include "ArchiveStreams.hpp"
namespace th10 {
struct ArchiveCatalogEnvironment {
    const char* const* file_mode;ArchiveStreamEnvironment* streams;
    virtual bool open_archive(ResourceArchive& archive,const char* name)=0;
    virtual u8* read_archive(ResourceArchive& archive,const char* name,u8* output)=0;
    virtual u32 archive_size(ResourceArchive& archive,const char* name)=0;
};
struct ArchiveCatalog {
    ResourceArchive* archives;i32& count;ArchiveCatalogEnvironment& environment;
    static const char* basename(const char* name) noexcept;
    ResourceArchive* find(const char* name) const noexcept;
    bool add(const char* name);
    u8* read(const char* name,u8* output);
    u32 size(const char* name);
    bool open_memory(ArchiveMemoryStream& stream,const char* name);
};
}
