#include "RouteHandoffRules.hpp"

#include <bit>
#include <cstdint>

namespace th128::stage {
namespace {

constexpr std::uint32_t RecordTableBase = 0x4b1a30u;
constexpr std::uint32_t RecordStride = 40u;

std::int32_t incrementProgress(std::int32_t progress) noexcept {
    return std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(progress) + 1u);
}

}

std::optional<RouteHandoffPlan> planRouteHandoff(RouteHandoffInput input) noexcept {
    int nextId = 0;
    std::int32_t nextProgress = input.progress;
    bool writesProgress = false;
    switch (input.currentId) {
    case 1:
    case 6:
    case 11:
        if (input.gate74 == 0) {
            if (input.message78 == 0) {
                nextId = input.currentId + 1;
            } else {
                nextId = input.currentId + 3;
                nextProgress = incrementProgress(input.progress);
                writesProgress = true;
            }
        } else {
            nextId = input.currentId + ((input.progress % 2) == 1 ? 3 : 1);
        }
        break;
    case 2:
    case 4:
    case 7:
    case 9:
    case 12:
    case 14:
        nextId = input.currentId + 1;
        break;
    default:
        return std::nullopt;
    }
    return RouteHandoffPlan{nextId, nextProgress, writesProgress,
        RecordTableBase + static_cast<std::uint32_t>(nextId) * RecordStride};
}

}
