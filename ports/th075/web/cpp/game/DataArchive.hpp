#pragma once
#include "Types.hpp"
namespace th075 {
struct ArchiveEntry { std::string name; u32 size=0,offset=0; };
// Original 1.11 data archive (0x41d510). Resource names are kept as CP932
// byte strings: all shipped runtime paths are ASCII and need no conversion.
class DataArchive {
public:
    std::vector<ArchiveEntry> entries;
    bool read_index(const void* index,std::size_t bytes,std::uint64_t archiveBytes);
    const ArchiveEntry* find(const std::string& path)const;
    std::string error;
};
}
