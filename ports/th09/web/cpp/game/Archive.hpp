#pragma once
#include "Lzss.hpp"
#include <string>
namespace th09 {
struct ArchiveEntry {std::string name;u32 offset=0,size=0,unknown=0,compressed=0;};
struct ArchiveSource {virtual ~ArchiveSource()=default;virtual u32 size()const=0;virtual bool read(u32 offset,u8* output,u32 count)=0;};
class Archive {
public:
    bool open(const u8* bytes,u32 size);
    bool open(ArchiveSource& input);
    void close() noexcept {source=nullptr;stream=nullptr;source_size=0;entries.clear();}
    const ArchiveEntry* find(const char* name) const noexcept;
    bool packed(const char* name,std::vector<u8>& bytes,u32& unpacked);
    bool read(const char* name,std::vector<u8>& output,bool decrypt=true);
    const std::vector<ArchiveEntry>& contents() const noexcept {return entries;}
private:
    const u8* source=nullptr;u32 source_size=0;
    ArchiveSource* stream=nullptr;
    bool open_index();bool read_source(u32,u8*,u32);
    Lzss codec;
    std::vector<ArchiveEntry> entries;
};
}
