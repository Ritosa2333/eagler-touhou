#pragma once
#ifdef TH11_MULTIPLAYER
#include "Types.hpp"
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace th11 {
class AnmManager;
struct AnmVm;

// Geometry addresses can occur inside VMs and callbacks. A checkpoint owns a
// lease on the original allocation; restoring a pointer must never resurrect
// already-freed storage. These leases are physical lifetime, not game state.
struct AnmGeometryAllocation {
    std::shared_ptr<void> memory;
    u32 bytes=0;
};

// Typed metadata complements the common byte journal. Do not memcpy STL
// containers or this object. The frame must outlive UndoTo for every journal
// record that can refer to its allocations. Resources/scripts and external
// embedded VMs remain the responsibility of their respective title owners.
class AnmRollbackFrame final {
public:
    AnmRollbackFrame()=default;
    AnmRollbackFrame(const AnmRollbackFrame&)=delete;
    AnmRollbackFrame& operator=(const AnmRollbackFrame&)=delete;
    bool valid()const noexcept{return ready_&&!identity_.expired();}
    u32 frame()const noexcept{return frame_;}
private:
    friend class AnmManager;
    friend struct AnmRollbackTestAccess;
    AnmManager* owner_=nullptr;
    std::weak_ptr<const unsigned char> identity_;
    u32 frame_=0;
    bool ready_=false;
    std::vector<AnmVm*> available_;
    std::shared_ptr<const std::unordered_map<u32,AnmVm*>> active_;
    // These keyed values have no simulation-order traversal. Flat snapshots
    // avoid one allocation per map node while preserving every geometry lease
    // and tags on externally owned/embedded VMs. Preserve the original active
    // map's value-copy semantics and iteration order. Unchanged memberships
    // may share the same immutable map; VM payload bytes are still journaled.
    std::vector<std::pair<AnmVm*,AnmGeometryAllocation>> geometry_;
    std::vector<std::pair<const AnmVm*,unsigned>> tags_;
};
}
#endif

