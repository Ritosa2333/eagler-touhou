#pragma once
#ifdef TH11_MULTIPLAYER
#include "ScreenDeformation.hpp"
#include <memory>
#include <vector>
namespace Netplay {class RollbackJournal;}
namespace th11 {
class ScreenDeformationPool;

// Value snapshots restore vector contents, never their allocator/object heads.
// Callback VMs contain the ScreenDeformation address, not vector data pointers.
class DeformationRollbackFrame final {
public:
    DeformationRollbackFrame()=default;
    DeformationRollbackFrame(const DeformationRollbackFrame&)=delete;
    DeformationRollbackFrame& operator=(const DeformationRollbackFrame&)=delete;
    bool valid()const noexcept{return ready_&&!identity_.expired();}
    u32 frame()const noexcept{return frame_;}
private:
    friend class ScreenDeformationPool;
    friend struct DeformationRollbackTestAccess;
    struct Payload {
        u32 columns=0,rows=0;
        bool occupied=false;
        std::vector<AnmVertex> vertices;
        std::vector<Vec3> positions;
        std::vector<u32> animations;
    };
    ScreenDeformationPool* owner_=nullptr;
    std::weak_ptr<const unsigned char> identity_;
    u32 frame_=0;
    bool ready_=false;
    std::vector<Payload> slots_;
};

// World-owned stable storage. The owning world and its ANM manager must outlive
// every referring checkpoint. Release preserves callback payload until all
// strips disappear, including partially initialized and logically retired slots.
class ScreenDeformationPool final {
public:
    explicit ScreenDeformationPool(AnmManager& manager):manager_(manager){}
    ~ScreenDeformationPool();
    ScreenDeformationPool(const ScreenDeformationPool&)=delete;
    ScreenDeformationPool& operator=(const ScreenDeformationPool&)=delete;
    ScreenDeformation* create(AnmResource&,u32 width=ScreenDeformation::side,
                              u32 height=ScreenDeformation::side,bool second_target=false);
    bool release(ScreenDeformation*);
    bool BeginRollbackFrame(Netplay::RollbackJournal&,DeformationRollbackFrame&);
    void EndRollbackFrame()noexcept{journal_=nullptr;}
    bool RestoreRollbackFrame(const DeformationRollbackFrame&,u32 restored_frame);
    bool RollbackFrameActive()const noexcept{return journal_!=nullptr;}
private:
    friend struct DeformationRollbackTestAccess;
    struct Slot {std::unique_ptr<ScreenDeformation> object;bool occupied=false;};
    AnmManager& manager_;
    std::vector<Slot> storage_;
    std::size_t logical_size_=0;
    Netplay::RollbackJournal* journal_=nullptr;
    std::shared_ptr<const unsigned char> identity_;
    bool reusable(const Slot&)const;
};
}
#endif
