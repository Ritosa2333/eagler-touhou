#pragma once
#include "Rng.hpp"
#include <array>
#include <vector>
#include <string>
namespace th17 {
// Original saved-file blocks, not executable object memory. Unknown/reserved
// file bytes survive imports; gameplay accesses named records separately.
class ScoreFile {
 std::array<u8,0x18> header{};
 static u32 checksum(const u8*,u32)noexcept;bool fail(const char*);
public:
 static constexpr u32 character_size=0x4820,settings_size=0x4b0;
 std::array<std::array<u8,character_size>,10> characters{};
 std::array<u8,settings_size> settings{};std::string error;
 void reset(Rng&);
 bool open(const u8*,u32);bool save(std::vector<u8>&);
 const std::array<u8,0x18>& file_header()const noexcept{return header;}
};
}
