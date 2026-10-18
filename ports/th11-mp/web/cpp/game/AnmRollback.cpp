#include "AnmManager.hpp"
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
#include <type_traits>

namespace th11 {
static_assert(std::is_trivially_copyable<AnmVm>::value,
              "ANM journal payload must not own dynamic C++ objects");

bool AnmManager::BeginRollbackFrame(Netplay::RollbackJournal& journal,
                                    AnmRollbackFrame& frame){
    if(rollback_journal_||!journal.IsFrameOpen()||journal.Failed()||frame.ready_)
        return false;
    if(!rollback_identity_)rollback_identity_=std::make_shared<const unsigned char>(0);
    frame.owner_=this;frame.identity_=rollback_identity_;frame.frame_=journal.OpenFrame();
    // Snapshot actual containers through their value semantics. Their nodes,
    // capacities and reference-count machinery are never byte-restored.
    frame.available_=available;
    if(!rollback_registry_snapshot_){
        auto snapshot=std::make_shared<std::unordered_map<u32,AnmVm*>>();
        // Same empty-destination copy assignment used by the original frame.
        // Do not rebuild by sorted IDs or change clear()/free-list order.
        *snapshot=active;rollback_registry_snapshot_=std::move(snapshot);
    }
    frame.active_=rollback_registry_snapshot_;
    frame.geometry_.assign(geometry_sizes.begin(),geometry_sizes.end());
    frame.tags_.assign(multiplayer_tags.begin(),multiplayer_tags.end());
    rollback_page_captured_.assign(rollback_pages_.size(),0);
    rollback_journal_=&journal;
    bool ok=true;
    const auto touch=[&](auto& value){
        using T=typename std::remove_reference<decltype(value)>::type;
        static_assert(std::is_trivially_copyable<T>::value,"explicit value owner required");
        if(!journal.Touch(&value,sizeof value))ok=false;
    };
    touch(rate);touch(script_rng);touch(visual_rng);
    touch(reference_positions);touch(camera_delta);touch(default_tangent);
    touch(last_error);touch(next_id);touch(spawn_depth);
    touch(heads);touch(tails);touch(layers);touch(multiplayer_owner);
    touch(logical_storage_size_);
    // Capture only pages containing live VMs. This reduces JS bulk-copy and
    // journal-index crossings without discarding ANY live or inactive bytes
    // in a captured page. Other pages are first-write captured before reuse.
    for(const auto& entry:active){
        const auto slot=rollback_slots_.find(entry.second);
        if(slot==rollback_slots_.end()||!capture_rollback_page(slot->second))ok=false;
    }
    // Some geometry belongs to VMs embedded in Bullet/Stage/etc. Only its
    // allocation is owned here; the embedded VM is captured by that owner.
    for(const auto& entry:geometry_sizes)
        if(!journal.Touch(entry.second.memory.get(),entry.second.bytes))ok=false;
    frame.ready_=ok&&!journal.Failed();
    return frame.ready_;
}

void AnmManager::EndRollbackFrame()noexcept{rollback_journal_=nullptr;}

void AnmManager::preserve_script_write(void* address,u32 bytes){
    // Resource backing storage remains stable until the world history retires.
    // Shared scripts may be touched by several VMs/managers in the same frame;
    // exact duplicate words are first-write-wins in the shared journal.
    if(rollback_journal_)rollback_journal_->Touch(address,bytes);
}

void AnmManager::preserve_rollback_slot(AnmVm& vm){
    if(!rollback_journal_)return;
    const auto slot=rollback_slots_.find(&vm);
    if(slot!=rollback_slots_.end())capture_rollback_page(slot->second);
}

bool AnmManager::capture_rollback_page(std::size_t slot){
    const auto page=slot/rollback_page_slots_;
    if(page>=rollback_pages_.size()||page>=rollback_page_captured_.size())
        return rollback_journal_->Touch(nullptr,1); // fail closed, not a missed write
    if(rollback_page_captured_[page])return true;
    if(!rollback_journal_->Touch(rollback_pages_[page]->data(),sizeof(RollbackVmPage)))return false;
    rollback_page_captured_[page]=1;return true;
}

bool AnmManager::RestoreRollbackFrame(const AnmRollbackFrame& frame,u32 restored_frame){
    if(rollback_journal_||!frame.valid()||frame.owner_!=this||
       frame.identity_.lock()!=rollback_identity_||frame.frame_!=restored_frame)
        return false;
    // The shared journal MUST have completed UndoTo first. Replacing these
    // maps may release post-checkpoint geometry; no outstanding undo copy may
    // still refer to it. The orchestrator retires snapshots only afterwards.
    if(!frame.active_)return false;
    available=frame.available_;active=*frame.active_;
    rollback_registry_snapshot_=frame.active_;
    geometry_sizes.clear();geometry_sizes.reserve(frame.geometry_.size());
    geometry_sizes.insert(frame.geometry_.begin(),frame.geometry_.end());
    multiplayer_tags.clear();multiplayer_tags.reserve(frame.tags_.size());
    multiplayer_tags.insert(frame.tags_.begin(),frame.tags_.end());
    return true;
}
}
#endif
