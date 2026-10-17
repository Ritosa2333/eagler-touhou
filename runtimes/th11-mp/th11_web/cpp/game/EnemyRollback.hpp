#pragma once
#ifdef TH11_MULTIPLAYER
#include "EnemyScript.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
namespace Netplay {class RollbackJournal;}
namespace th11 {
class EnemyManager;

// Physical storage is stable and retained to the manager's high-water mark.
// Logical free lists and allocation cursors are restored through value copies.
struct EclRollbackAllocation {std::shared_ptr<void> memory;u32 bytes=0,index=0;};
struct EclRollbackAllocations {
    std::unordered_map<void*,EclRollbackAllocation> active;
    std::unordered_map<u32,std::vector<u32>> available;
    std::unordered_map<u32,u32> logical_sizes;
};
class EclRollbackAllocator final {
public:
    void* allocate(u32);
    void release(void*);
private:
    friend class EnemyManager;
    friend struct EnemyRollbackTestAccess;
    std::unordered_map<u32,std::vector<std::shared_ptr<void>>> storage_;
    EclRollbackAllocations state_;
    Netplay::RollbackJournal* journal_=nullptr;
    bool BeginFrame(Netplay::RollbackJournal&,EclRollbackAllocations&);
    void EndFrame()noexcept{journal_=nullptr;}
    bool Restore(const EclRollbackAllocations&);
};
struct EnemyRollbackEntry {
    Enemy enemy{};
    EnemyScriptServices script;
    EnemyRollbackEntry(EnemyEnvironment& world,EnemyCommandEnvironment& commands,EclRollbackAllocator& allocator)
        :script(enemy.state,world,commands){script.rollback_allocator_=&allocator;}
};
// Only typed containers / leases here. Never memcpy this snapshot or services.
// Caller keeps the manager and external world/resource owners alive, detaches
// observers, completes the entire journal UndoTo, then restores typed metadata.
class EnemyRollbackFrame final {
public:
    EnemyRollbackFrame()=default;
    EnemyRollbackFrame(const EnemyRollbackFrame&)=delete;
    EnemyRollbackFrame& operator=(const EnemyRollbackFrame&)=delete;
    bool valid()const noexcept{return ready_&&!identity_.expired();}
    u32 frame()const noexcept{return frame_;}
private:
    friend class EnemyManager;
    friend struct EnemyRollbackTestAccess;
    EnemyManager* owner_=nullptr;
    std::weak_ptr<const unsigned char> identity_;
    u32 frame_=0;
    bool ready_=false;
    std::unordered_map<Enemy*,std::shared_ptr<EnemyRollbackEntry>> entries_;
    std::vector<u32> available_;
    EclRollbackAllocations allocations_;
};
}
#endif
