#include "EnemyManager.hpp"
#include "RollbackHooks.hpp"
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
#include <cstdlib>
#include <cstring>
#include <type_traits>
namespace th11 {
static_assert(std::is_trivially_copyable<Enemy>::value,"Enemy payload must not own containers");
static_assert(std::is_trivially_copyable<EclContext>::value,"ECL context must own inline stack bytes");
static_assert(std::is_trivially_copyable<EclThread>::value,"ECL thread links must be plain state");

void* EclRollbackAllocator::allocate(u32 bytes){
    if(!bytes)return nullptr;
    auto& free=state_.available[bytes];u32 index;
    if(!free.empty()){index=free.back();free.pop_back();}
    else index=state_.logical_sizes[bytes]++;
    auto& storage=storage_[bytes];
    if(index==storage.size()){
        void* raw=TH11_GAME_CALLOC(1,bytes);
        if(!raw){--state_.logical_sizes[bytes];return nullptr;}
        storage.emplace_back(raw,[](void* memory){TH11_GAME_FREE(memory);});
    }
    auto memory=storage[index];
    // Capture before calloc-equivalent zeroing. Journal failure deliberately
    // does not masquerade as a physical allocator failure / ECL trap.
    if(journal_)journal_->Touch(memory.get(),bytes);
    std::memset(memory.get(),0,bytes);
    state_.active.emplace(memory.get(),EclRollbackAllocation{memory,bytes,index});
    return memory.get();
}
void EclRollbackAllocator::release(void* pointer){
    if(!pointer)return;
    const auto found=state_.active.find(pointer);
    // This allocator only receives allocations made by its bound service.
    // A foreign pointer is an ownership error, not a license to free it.
    if(found==state_.active.end())__builtin_trap();
    const auto bytes=found->second.bytes,index=found->second.index;
    state_.active.erase(found);state_.available[bytes].push_back(index);
}
bool EclRollbackAllocator::BeginFrame(Netplay::RollbackJournal& journal,EclRollbackAllocations& snapshot){
    if(journal_||!journal.IsFrameOpen()||journal.Failed())return false;
    snapshot=state_;journal_=&journal;bool ok=true;
    for(const auto& allocation:state_.active)
        if(!journal.Touch(allocation.first,allocation.second.bytes))ok=false;
    return ok;
}
bool EclRollbackAllocator::Restore(const EclRollbackAllocations& snapshot){
    if(journal_)return false;state_=snapshot;return true;
}
void EnemyManager::preserve_rollback_entry(Entry& entry){
    if(!rollback_journal_)return;
    rollback_journal_->Touch(&entry.enemy,sizeof entry.enemy);
    rollback_journal_->Touch(&entry.script.error,sizeof entry.script.error);
}
std::shared_ptr<EnemyManager::Entry> EnemyManager::acquire_rollback_entry(){
    u32 index;
    if(!rollback_available_.empty()){index=rollback_available_.back();rollback_available_.pop_back();}
    else index=rollback_logical_size_++;
    if(index==rollback_storage_.size()){
        auto entry=std::make_shared<Entry>(environment,commands,rollback_allocator_);
        rollback_slots_.emplace(&entry->enemy,index);rollback_storage_.push_back(std::move(entry));
    }
    auto entry=rollback_storage_[index];
    preserve_rollback_entry(*entry);
    entry->enemy={};entry->script.error={};
    return entry;
}
void EnemyManager::release_rollback_entry(const std::shared_ptr<Entry>& entry){
    const auto found=rollback_slots_.find(&entry->enemy);
    if(found==rollback_slots_.end())__builtin_trap();
    rollback_available_.push_back(found->second);
}
bool EnemyManager::BeginRollbackFrame(Netplay::RollbackJournal& journal,EnemyRollbackFrame& frame){
    if(rollback_journal_||!journal.IsFrameOpen()||journal.Failed()||frame.ready_)return false;
    if(!rollback_identity_)rollback_identity_=std::make_shared<const unsigned char>(0);
    frame.owner_=this;frame.identity_=rollback_identity_;frame.frame_=journal.OpenFrame();
    frame.entries_=entries;frame.available_=rollback_available_;
    rollback_journal_=&journal;bool ok=true;
    const auto capture=[&](auto& value){
        using T=typename std::remove_reference<decltype(value)>::type;
        static_assert(std::is_trivially_copyable<T>::value,"explicit plain value owner required");
        if(!journal.Touch(&value,sizeof value))ok=false;
    };
    capture(first);capture(last);capture(count);capture(total_created);capture(timer);
    capture(last_error);capture(script_error);capture(program);capture(spawn_depth);capture(rollback_logical_size_);
    for(const auto& entry:entries)preserve_rollback_entry(*entry.second);
    if(!rollback_allocator_.BeginFrame(journal,frame.allocations_))ok=false;
    frame.ready_=ok&&!journal.Failed();return frame.ready_;
}
void EnemyManager::EndRollbackFrame()noexcept{rollback_journal_=nullptr;rollback_allocator_.EndFrame();}
bool EnemyManager::RestoreRollbackFrame(const EnemyRollbackFrame& frame,u32 restored_frame){
    if(rollback_journal_||!frame.valid()||frame.owner_!=this||frame.identity_.lock()!=rollback_identity_||frame.frame_!=restored_frame)return false;
    // UndoTo has finished before any typed metadata replacement. Keep this
    // frame and every newer lease until all world owners have been restored.
    entries=frame.entries_;rollback_available_=frame.available_;
    return rollback_allocator_.Restore(frame.allocations_);
}
}
#endif
