#pragma once
#include "Lzss.hpp"
#include "ResourceCrypt.hpp"
#include <string>
namespace th11 {
struct ArchiveEntry {std::string name;u32 offset=0,size=0,reserved=0,compressed=0;};
class Archive {
    const u8* source=nullptr;u32 source_size=0;Lzss codec;
    const CryptParams* ciphers=archive_crypt_th12;u32 version=12;
public:
    std::vector<ArchiveEntry> entries;
    // ZUN THA1 generations and Tasofro DAT generations 75, 105, 123.
    bool open(const u8*,u32,u32 version=12);
    // Tasofro archives can exceed 1 GB. Parse just the directory, then feed one
    // encrypted entry at a time using its offset, without retaining the archive.
    bool open_directory(const u8*,u32 prefix_size,u32 total_size,u32 version);
    bool decode_entry(u32,const u8*,u32,std::vector<u8>&) const;
    bool read(u32,std::vector<u8>&);
    u32 archive_version() const noexcept {return version;}
    const CryptParams* archive_ciphers() const noexcept {return ciphers;}
};
}
