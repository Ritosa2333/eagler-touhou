#include "Stage.hpp"
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
#include <type_traits>
namespace th11 {
static_assert(std::is_trivially_copyable<StageState>::value,"StageState requires an explicit value owner");
static_assert(std::is_trivially_copyable<AnmVm>::value,"Stage VM payload must remain trivial");

bool Stage::BeginRollbackFrame(Netplay::RollbackJournal& journal,StageRollbackFrame& frame){
    if(rollback_journal_||deformation_pool.RollbackFrameActive()||!journal.IsFrameOpen()||
       journal.Failed()||frame.history_)return false;
    if(!rollback_identity_)rollback_identity_=std::make_shared<const unsigned char>(0);
    auto history=rollback_history_.lock();
    if(!history){history=std::make_shared<const unsigned char>(0);rollback_history_=history;}
    frame.owner_=this;frame.identity_=rollback_identity_;frame.history_=history;frame.frame_=journal.OpenFrame();
    frame.object_vms_=object_animations.data();frame.object_vm_count_=object_animations.size();
    frame.objects_=resource.objects.data();frame.instances_=resource.instances.data();
    frame.object_flags_.clear();frame.instance_flags_.clear();
    for(const auto& object:resource.objects)frame.object_flags_.push_back(object.flags);
    for(const auto& instance:resource.instances)frame.instance_flags_.push_back(instance.flags);
    frame.error_=error;
    rollback_journal_=&journal;
    bool ok=deformation_pool.BeginRollbackFrame(journal,frame.deformation_);
    const auto touch=[&](auto& value){
        using T=typename std::remove_reference<decltype(value)>::type;
        static_assert(std::is_trivially_copyable<T>::value,"explicit field owner required");
        if(!journal.Touch(&value,sizeof value))ok=false;
    };
    touch(state);touch(mesh);touch(multiplayer_frozen_draw);
    touch(multiplayer_tint_owner);touch(multiplayer_tint_source);
    if(!object_animations.empty()&&!journal.Touch(object_animations.data(),object_animations.size()*sizeof(AnmVm)))ok=false;
    frame.ready_=ok&&!journal.Failed();return frame.ready_;
}
void Stage::EndRollbackFrame()noexcept{
    rollback_journal_=nullptr;deformation_pool.EndRollbackFrame();
}
bool Stage::ValidateRollbackFrame(const StageRollbackFrame& frame,u32 restored_frame)const noexcept{
    if(rollback_journal_||deformation_pool.RollbackFrameActive()||!frame.valid()||
       frame.owner_!=this||frame.identity_.lock()!=rollback_identity_||frame.frame_!=restored_frame||
       frame.object_vms_!=object_animations.data()||frame.object_vm_count_!=object_animations.size()||
       frame.objects_!=resource.objects.data()||frame.object_flags_.size()!=resource.objects.size()||
       frame.instances_!=resource.instances.data()||frame.instance_flags_.size()!=resource.instances.size()||
       !frame.deformation_.valid()||frame.deformation_.frame()!=restored_frame)return false;
    return true;
}
bool Stage::RestoreRollbackFrame(const StageRollbackFrame& frame,u32 restored_frame){
    if(!ValidateRollbackFrame(frame,restored_frame))return false;
    // All common journal bytes (including ANM VMs/geometry and mesh pointer)
    // must already be restored. No callback/Draw runs between typed owners.
    if(!deformation_pool.RestoreRollbackFrame(frame.deformation_,restored_frame))return false;
    for(std::size_t i=0;i<resource.objects.size();++i)resource.objects[i].flags=frame.object_flags_[i];
    for(std::size_t i=0;i<resource.instances.size();++i)resource.instances[i].flags=frame.instance_flags_[i];
    error=frame.error_;return true;
}
}
#endif
