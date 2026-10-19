#pragma once
#include "Lzss.hpp"
#include <string>
namespace th11 {
struct ArchiveEntry {std::string name;u32 offset=0,size=0,reserved=0,compressed=0;};
class Archive {
    const u8* source=nullptr;u32 source_size=0;Lzss codec;
public:
    std::vector<ArchiveEntry> entries;
    bool open(const u8*,u32);
    bool read(u32,std::vector<u8>&);
};
}
