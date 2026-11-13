// Allocation/copy diagnostic for the production checkpoint path. No game
// pumping, resource replacement, drawing or state digest field changes.
#include "game/AnmRegistry.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <vector>
using namespace th13;
int main() {
    AnmRegistry registry;
    std::vector<AnmVm*> vms;
    for (unsigned i = 0; i < 1024; ++i) {
        auto* vm = registry.allocate();
        if (!vm || !registry.submit(*vm)) return 1;
        vm->visual.translation = {float(i), float(i * 3), 0};
        if ((i % 4) == 0) {
            vm->geometry.screen_vertices.resize(24);
            vm->geometry.screen_vertices.back().position.x = float(i + 1);
        }
        if ((i % 3) == 1 && !registry.attach(*vm, *vms.back())) return 2;
        vms.push_back(vm);
    }
    AnmRegistry::SnapshotData snapshot;
    if (!registry.capture(snapshot) || snapshot.entries.size() != vms.size()) return 3;
    std::array<double, 7> elapsed;
    volatile unsigned checksum = 0;
    for (auto& time : elapsed) {
        const auto start = std::chrono::steady_clock::now();
        for (unsigned frame = 0; frame < 128; ++frame) {
            if (!registry.capture(snapshot)) return 4;
            checksum += unsigned(snapshot.entries[frame].vm.visual.translation.x);
        }
        time = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
    }
    std::sort(elapsed.begin(), elapsed.end());
    if (checksum != 7 * 127 * 128 / 2) return 5;
    std::printf("PASS 1024-node checkpoint reuse; median_ms=%.3f min_ms=%.3f max_ms=%.3f checksum=%u\n",
                elapsed[3], elapsed.front(), elapsed.back(), checksum);
}
