#include "game/RouteHandoffRules.hpp"

#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

int main() {
    std::ios::sync_with_stdio(false);
    std::string command;
    while (std::cin >> command) {
        if (command != "PLAN") {
            return 2;
        }
        std::int64_t currentId = 0, progress = 0, gate74 = 0, message78 = 0;
        if (!(std::cin >> currentId >> progress >> gate74 >> message78)) {
            return 2;
        }
        const bool inRange = currentId >= std::numeric_limits<int>::min()
            && currentId <= std::numeric_limits<int>::max()
            && progress >= std::numeric_limits<std::int32_t>::min()
            && progress <= std::numeric_limits<std::int32_t>::max()
            && gate74 >= std::numeric_limits<std::int32_t>::min()
            && gate74 <= std::numeric_limits<std::int32_t>::max()
            && message78 >= std::numeric_limits<std::int32_t>::min()
            && message78 <= std::numeric_limits<std::int32_t>::max();
        if (!inRange) {
            return 2;
        }
        const auto plan = th128::stage::planRouteHandoff({static_cast<int>(currentId),
            static_cast<std::int32_t>(progress), static_cast<std::int32_t>(gate74),
            static_cast<std::int32_t>(message78)});
        if (!plan) {
            std::cout << "none\n";
            continue;
        }
        std::cout << "plan " << plan->nextId << ' ' << plan->nextProgress << ' '
                  << (plan->writesProgress ? 1 : 0) << " 0x" << std::hex
                  << plan->originalRecordAddress << std::dec << '\n';
    }
    return 0;
}
