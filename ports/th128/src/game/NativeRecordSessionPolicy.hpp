#pragma once
#include <bit>
#include <cstdint>

namespace th128::record {

class NativeRecordSessionPolicy {
public:
    static bool stageClearHeld(std::uint32_t managerFlags, std::uint32_t timerBits) noexcept {
        return (managerFlags & 0x100u) && std::bit_cast<std::int32_t>(timerBits) < 120;
    }
};

}
