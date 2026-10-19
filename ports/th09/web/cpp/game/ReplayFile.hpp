#pragma once
#include "Lzss.hpp"
#include <array>
namespace th09 {
// File offsets are retained as offsets, not rebased into guest addresses.
// Four groups of ten rounds: P1, P2, shared/menu input, and frame-rate samples.
class ReplayFile {
public:
    static constexpr u32 header_size=0xc0,metadata_end=0x1ec,max_size=32*1024*1024;
    bool decode(const u8* input,u32 size);
    bool assign(const u8* decoded,u32 size);
    std::vector<u8> encode();
    const std::vector<u8>& data() const noexcept {return bytes;}
    u32 stream_offset(u32 group,u32 round) const noexcept;
private:
    bool valid() const;
    std::vector<u8> bytes;
};
}
