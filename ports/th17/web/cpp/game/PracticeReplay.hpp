#pragma once
#include "Practice.hpp"

namespace th17 {
// Native thprac writes a zero-padded USER/PRAC JSON object at the replay tail.
bool read_native_practice(const u8*,u32 size,i32 stage,i32 rank,i32 spell,PracticeSettings&);
}
