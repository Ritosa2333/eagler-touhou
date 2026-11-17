#include "../harness/test_framework.hpp"
#include "game/AnmRegistry.hpp"
#include <vector>
using namespace th13;

TEST(reused_anm_slot_does_not_keep_alternate_chain) {
    AnmRegistry registry;
    auto* chain = registry.allocate();
    CHECK(chain != nullptr);
    CHECK(registry.submit(*chain, 4) != 0);
    CHECK_EQ(registry.ordered_count(true), 1u);
    CHECK(registry.discard(*chain));
    CHECK_EQ(registry.ordered_count(true), 0u);
    auto* reused = registry.allocate();
    CHECK(reused != nullptr);
    reused->source_script = 0x13572468;
    AnmRegistry::SnapshotData snap;
    std::vector<const AnmVm*> embedded;
    CHECK(registry.capture(snap, embedded));
    const AnmRegistry::SnapshotData::Entry* fresh = nullptr;
    for (const auto& entry : snap.entries)
        if (entry.vm.source_script == 0x13572468) fresh = &entry;
    CHECK(fresh != nullptr);
    CHECK(fresh->active);
    CHECK(!fresh->alternate);
    CHECK(registry.discard(*reused));
}
int main() { return th13test::run_all("anm slot reuse"); }
