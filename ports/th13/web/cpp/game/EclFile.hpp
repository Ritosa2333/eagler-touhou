#pragma once

#include "EclThreads.hpp"

#include <vector>

namespace th13 {

class EclFile {
public:
    using Block = std::array<u8, 0x11c4>;
    std::string error;
    bool write(const EclContext&, Block&);
    bool read(const u8* data, u32 size, EclContext&, EclProgram&);
    bool write_threads(const EclThreadsSnapshot&, std::vector<u8>&, u32 owner_offset, u32 child_offset);
    bool read_threads(const u8* data, u32 size, EclThreadsSnapshot&, EclProgram&, u32 owner_offset, u32 child_offset,
                      u32& consumed);
};

}
