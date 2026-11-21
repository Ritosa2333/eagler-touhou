#pragma once

#include <cstdint>
#include <optional>

namespace th128::stage {

struct RouteHandoffInput {
    int currentId = 0;
    std::int32_t progress = 0;
    std::int32_t gate74 = 0;
    std::int32_t message78 = 0;
};

struct RouteHandoffPlan {
    int nextId = 0;
    std::int32_t nextProgress = 0;
    bool writesProgress = false;
    std::uint32_t originalRecordAddress = 0;
};

std::optional<RouteHandoffPlan> planRouteHandoff(RouteHandoffInput input) noexcept;

}
