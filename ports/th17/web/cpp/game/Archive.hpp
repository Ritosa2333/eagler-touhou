#pragma once
#include "Lzss.hpp"
#include <functional>
#include <string>
namespace th17 {
struct ArchiveEntry {std::string name;u32 offset=0,size=0,reserved=0,compressed=0;};
class Archive {
public:
    using ReadAt=std::function<bool(u32,u8*,u32)>;
private:
    ReadAt source;
    Lzss codec;
public:
    std::vector<ArchiveEntry> entries;
    bool open(const u8*,u32);
    bool open(u32,ReadAt);
    bool read(u32,std::vector<u8>&);
};
}
