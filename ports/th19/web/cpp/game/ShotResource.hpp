#pragma once
#include "Types.hpp"
#include <vector>
namespace th19 {
// SHT (player shot definition) header: u16 version (=6), then an offset table.
struct ShotFile {
    u16 version=0;u16 header=0;
    std::vector<u8> bytes;
    bool load(const u8* data,u32 size);
};
}
