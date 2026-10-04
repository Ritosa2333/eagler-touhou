#pragma once
#include "PracticeConfig.hpp"
#include <string>
#include <vector>
namespace th11 {
// Owned decompressed buffers, in original ECL loaded-file ordinal order.
// The transaction runs before game pointer tables / ANM and STD decoding.
struct PracticeBuffers {
    std::vector<std::vector<u8>> ecl;
    std::vector<u8> scene,background;
    i32 stage_section=0;
};
bool patch_practice_buffers(PracticeBuffers&,const PracticeConfig&,std::string& error);
}
