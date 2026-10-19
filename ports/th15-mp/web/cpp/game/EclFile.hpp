#pragma once
#include "EclThreads.hpp"
namespace th15 {
// Saved contexts contain routine indices and byte offsets, including the
// eight interpolation destinations. Process addresses are never restored.
class EclFile {
public:
 using Block=std::array<u8,0x11e8>;
 std::string error;
 bool write(const EclContext&,Block&);
 bool read(const u8*,u32,EclContext&,EclProgram&);
 bool write_threads(const EclThreadsSnapshot&,std::vector<u8>&,u32 owner_offset,u32 child_offset);
 bool read_threads(const u8*,u32,EclThreadsSnapshot&,EclProgram&,u32 child_offset,u32& consumed);
};
}
