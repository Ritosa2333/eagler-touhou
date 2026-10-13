#pragma once

#include "thprac/portable/ecl_patch.hpp"

namespace thprac::portable::th07 {

// Original thprac: TH07_ST1_MID2, ECLSetTime(ecl, 0x2580, 0x3c, 0).
const PatchSet& Stage1Mid2();
const PatchSet* Stage1Section(std::int32_t section);

} // namespace thprac::portable::th07
