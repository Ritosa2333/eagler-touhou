#include "ScreenDeformationPool.hpp"
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
namespace th11 {
ScreenDeformationPool::~ScreenDeformationPool(){
    EndRollbackFrame();
    // Future physical slots left after a rewind are not current owners.
    // Their stale IDs must not retire animations in the restored timeline.
    for(std::size_t i=0;i<logical_size_;++i)
        if(storage_[i].occupied)storage_[i].object->erase_animations();
}
bool ScreenDeformationPool::reusable(const Slot& slot)const{
    if(slot.occupied)return false;
    for(auto id:slot.object->animations)if(id&&manager_.find(id))return false;
    return true;
}
ScreenDeformation* ScreenDeformationPool::create(AnmResource& resource,u32 width,
                                               u32 height,bool second_target){
    std::size_t index=0;
    for(;index<logical_size_;++index)if(reusable(storage_[index]))break;
    if(index==logical_size_){
        if(index==storage_.size()){
            Slot slot;slot.object=std::make_unique<ScreenDeformation>(manager_);
            slot.object->erase_on_destruction_=false;
            storage_.push_back(std::move(slot));
        }
        ++logical_size_;
    }
    auto& slot=storage_[index];auto& value=*slot.object;
    // No live callback may refer to a reused payload. Future slots are safe
    // only after the orchestrator has restored the ANM owner as well.
    value.columns=value.rows=ScreenDeformation::side;
    value.vertices.clear();value.positions.clear();value.animations.clear();
    slot.occupied=true;
    if(!value.initialize(resource,width,height,second_target)){
        // Initialization can already have made VMs. Keep the callback object
        // and its partial vectors alive until their semantic erase completes.
        value.erase_animations();slot.occupied=false;return nullptr;
    }
    return &value;
}
bool ScreenDeformationPool::release(ScreenDeformation* value){
    if(!value)return true;
    for(std::size_t i=0;i<logical_size_;++i){auto& slot=storage_[i];
        if(slot.object.get()!=value)continue;
        if(!slot.occupied)return false;
        value->erase_animations();slot.occupied=false;return true;
    }
    return false;
}
bool ScreenDeformationPool::BeginRollbackFrame(Netplay::RollbackJournal& journal,
                                               DeformationRollbackFrame& frame){
    if(journal_||!journal.IsFrameOpen()||journal.Failed()||frame.ready_)return false;
    if(!identity_)identity_=std::make_shared<const unsigned char>(0);
    frame.owner_=this;frame.identity_=identity_;frame.frame_=journal.OpenFrame();
    frame.slots_.resize(logical_size_);
    for(std::size_t i=0;i<logical_size_;++i){
        const auto& slot=storage_[i];const auto& value=*slot.object;auto& saved=frame.slots_[i];
        saved.columns=value.columns;saved.rows=value.rows;saved.occupied=slot.occupied;
        saved.vertices=value.vertices;saved.positions=value.positions;saved.animations=value.animations;
    }
    journal_=&journal;frame.ready_=true;return true;
}
bool ScreenDeformationPool::RestoreRollbackFrame(const DeformationRollbackFrame& frame,
                                                 u32 restored_frame){
    if(journal_||!frame.valid()||frame.owner_!=this||frame.identity_.lock()!=identity_||
       frame.frame_!=restored_frame||frame.slots_.size()>storage_.size())return false;
    // Complete the shared journal UndoTo first, restore ALL typed owners, then
    // retire future checkpoints. No callback runs between those operations.
    logical_size_=frame.slots_.size();
    for(std::size_t i=0;i<logical_size_;++i){
        auto& slot=storage_[i];auto& value=*slot.object;const auto& saved=frame.slots_[i];
        value.columns=saved.columns;value.rows=saved.rows;slot.occupied=saved.occupied;
        value.vertices=saved.vertices;value.positions=saved.positions;value.animations=saved.animations;
    }
    return true;
}
}
#endif
