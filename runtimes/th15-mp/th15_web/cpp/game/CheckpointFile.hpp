#pragma once
#include "Types.hpp"
#include <array>
#include <string>
#include <vector>
namespace th15 {
// JP 1.00b autosave format: an unchanged 96-byte t15b/v6 header and
// nine chap sections compressed by the game's LZSS codec. Module payloads
// are produced by their typed checkpoint owners, not by this container.
struct CheckpointHeader {
    std::array<u8,0x60> bytes{};
    static CheckpointHeader create(i64 timestamp,i32 character,i32 difficulty,i32 stage,i32 chapter,const std::array<i32,10>& retries,u32 display_flags)noexcept;
    bool open(const u8*,u32)noexcept;
    bool valid()const noexcept;
    i32 character()const noexcept; i32 difficulty()const noexcept;
    i32 stage()const noexcept; i32 chapter()const noexcept;
    bool compatible(i32 character,i32 difficulty,u32 display_flags)const noexcept;
};
class CheckpointFile {
public:
    static constexpr u32 max_payload=100*1024*1024;
    CheckpointHeader header;
    std::array<std::vector<u8>,9> sections;
    std::string error;
    bool open(const u8*,u32);
    bool encode(std::vector<u8>&);
    bool payload(std::vector<u8>&);
    // Native retries rewrite exactly 0x58 bytes; compression lengths and
    // all captured module bytes remain unchanged.
    static bool update_header(std::vector<u8>&,const CheckpointHeader&)noexcept;
};
}
