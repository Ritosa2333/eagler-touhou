#pragma once
#include "Types.hpp"
namespace th08 {
bool compatible_program(const char* version,u32 size,u32 checksum,bool replay=false) noexcept;
}
