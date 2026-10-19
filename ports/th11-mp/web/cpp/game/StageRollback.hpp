#pragma once
#ifdef TH11_MULTIPLAYER
#include "ScreenDeformationPool.hpp"
#include <string>
namespace th11 {
class Stage;
struct StageObject;
struct StageInstance;

// Holds typed flags and deformation values; byte storage remains at the live
// Stage's fixed addresses. A retained frame prohibits structural reallocation.
class StageRollbackFrame final {
public:
    StageRollbackFrame()=default;
    StageRollbackFrame(const StageRollbackFrame&)=delete;
    StageRollbackFrame& operator=(const StageRollbackFrame&)=delete;
    bool valid()const noexcept{return ready_&&!identity_.expired();}
    u32 frame()const noexcept{return frame_;}
private:
    friend class Stage;
    Stage* owner_=nullptr;
    std::weak_ptr<const unsigned char> identity_;
    std::shared_ptr<const unsigned char> history_;
    u32 frame_=0;
    bool ready_=false;
    const AnmVm* object_vms_=nullptr;
    std::size_t object_vm_count_=0;
    const StageObject* objects_=nullptr;
    const StageInstance* instances_=nullptr;
    std::vector<u8> object_flags_;
    std::vector<i16> instance_flags_;
    std::string error_;
    DeformationRollbackFrame deformation_;
};
}
#endif
