#pragma once
#ifdef TH11_MULTIPLAYER
#include "../game/TextureImage.hpp"
#include <memory>
#include <vector>
namespace Netplay {class RollbackJournal;}
namespace th11::sdl {
class TextureRollback;
class TextureRollbackFrame final {
public:
    TextureRollbackFrame()=default;
    TextureRollbackFrame(const TextureRollbackFrame&)=delete;
    TextureRollbackFrame& operator=(const TextureRollbackFrame&)=delete;
    bool valid()const noexcept{return ready_&&!identity_.expired();}
    u32 frame()const noexcept{return frame_;}
    std::size_t texture_count()const noexcept{return writes_.size();}
private:
    friend class TextureRollback;
    struct Write {
        u32 handle=0;
        TextureImage* image=nullptr;
        u8* pixels=nullptr;
        std::size_t size=0;
        u32 width=0,height=0,pitch=0;
        u16 packed_format=0;
        touhou::graphics::PixelFormat format=touhou::graphics::PixelFormat::Bgra8;
    };
    TextureRollback* owner_=nullptr;
    std::weak_ptr<const unsigned char> identity_;
    std::shared_ptr<const unsigned char> history_;
    u32 frame_=0;bool ready_=false;
    std::vector<Write> writes_;
};

// CPU text pixels only. Render targets, renderer state and ANM VM flags have
// different owners. Resource load/unload must wait for all frame leases.
// No SDL/GL dependency: the host supplies live identity lookup and monotonic
// dirty notification. Neither vector headers nor GPU revisions are restored.
class TextureRollback final {
public:
    using Resolve=TextureImage*(*)(void*,u32);
    using Dirty=void(*)(void*,u32);
    TextureRollback(void* owner,Resolve resolve,Dirty dirty)
        :owner_(owner),resolve_(resolve),dirty_(dirty),identity_(std::make_shared<const unsigned char>(0)){}
    ~TextureRollback(){EndRollbackFrame();}
    TextureRollback(const TextureRollback&)=delete;
    TextureRollback& operator=(const TextureRollback&)=delete;
    bool BeginRollbackFrame(Netplay::RollbackJournal&,TextureRollbackFrame&);
    bool EndRollbackFrame()noexcept;
    bool BeforeWrite(u32 handle,TextureImage&);
    bool RollbackFrameActive()const noexcept{return active_!=nullptr;}
    bool RestorePending()const noexcept{return restore_pending_;}
    bool ResourceChangeAllowed()const noexcept{return !active_&&!restore_pending_&&history_.expired();}
    const char* error()const noexcept{return error_;}
    // frames MUST contain every checkpoint crossed by the common UndoTo,
    // including newer frames whose textures do not occur in the target frame.
    bool ValidateRollbackFrames(const TextureRollbackFrame* const* frames,std::size_t count,u32 restored_frame)const noexcept;
    bool PrepareRollbackRestore(const TextureRollbackFrame* const* frames,std::size_t count,u32 restored_frame);
    // Host flushes pending rendering BEFORE common UndoTo. Only after UndoTo
    // and all world typed restores does Finish mark the entire union dirty.
    bool FinishRollbackRestore();
    void CancelRollbackRestore()noexcept{prepared_.clear();restore_pending_=false;}
private:
    using Write=TextureRollbackFrame::Write;
    void* owner_;Resolve resolve_;Dirty dirty_;
    std::shared_ptr<const unsigned char> identity_;
    std::weak_ptr<const unsigned char> history_;
    Netplay::RollbackJournal* journal_=nullptr;
    TextureRollbackFrame* active_=nullptr;
    std::vector<Write> prepared_;
    bool restore_pending_=false;
    const char* error_="";
    bool matches(const Write&)const noexcept;
    bool fail(const char* message){error_=message;if(active_)active_->ready_=false;return false;}
};
}
#endif
