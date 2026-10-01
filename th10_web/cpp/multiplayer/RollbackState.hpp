#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error RollbackState is multiplayer-only
#endif

#include <eagler/netplay/RollbackJournal.hpp>
#include <eagler/netplay/SparsePoolCapture.hpp>
#include <cstddef>
#include <cstdint>

namespace th10::browser { struct World; }
namespace th10 { struct AnmVm; struct EnemyBullet; struct Item; }

namespace th10::multiplayer {

class RollbackState {
public:
    bool Reset();
    void Clear();
    bool BeginFrame(browser::World& world,std::uint32_t frame,bool capture=true);
    bool Touch(void* address,std::size_t bytes);
    bool EndFrame();
    bool RestoreTo(std::uint32_t frame,std::uint32_t* replayFrom=nullptr);
    void DiscardBefore(std::uint32_t frame){journal_.DiscardBefore(frame);}
    bool Failed()const{return journal_.Failed();}
    bool IsCapturing()const{return configured_&&journal_.IsFrameOpen();}
    // Audio and resource retirement still own an exact logical frame when its
    // authoritative prefix no longer needs an undo checkpoint.
    bool IsFrameOpen()const{return frame_open_;}
    std::uint32_t OpenFrame()const{return open_frame_;}
    std::size_t CapturedBytes(std::uint32_t frame)const{return journal_.BytesForFrame(frame);}
    std::size_t CapturedBlocks(std::uint32_t frame)const{return journal_.BlocksForFrame(frame);}
    std::size_t LastBytes()const{return last_bytes_;}
    std::size_t PeakBytes()const{return peak_bytes_;}
    std::size_t LiveFrames()const{return journal_.FrameCount();}
    std::uint64_t SnapshotBytes()const{return total_bytes_;}
    std::uint32_t SnapshotFrames()const{return snapshots_;}
    std::uint32_t ElidedFrames()const{return elided_frames_;}
    std::size_t LastBlocks()const{return last_blocks_;}
    std::uint64_t ArenaGrowths()const{return journal_.ArenaGrowths();}
    std::uint64_t RestoreCopiedBytes()const{return journal_.RestoreCopiedBytes();}
    std::uint64_t RestoreSkippedBytes()const{return journal_.RestoreSkippedBytes();}

private:
    Netplay::RollbackJournal journal_{};
    bool configured_=false;
    bool frame_open_=false;
    std::uint32_t open_frame_=0,elided_frames_=0;
    // One-checkpoint coverage metadata, never rewindable gameplay. Spawn,
    // reuse and whole-pool clear hooks must not overlap an already saved run.
    Netplay::SparsePoolCapture<4096> animation_capture_{};
    Netplay::SparsePoolCapture<2000> bullet_capture_{};
    Netplay::SparsePoolCapture<150> regular_capture_{};
    Netplay::SparsePoolCapture<2048> faith_capture_{};
    AnmVm* animation_pool_=nullptr;
    EnemyBullet* bullet_pool_=nullptr;
    Item* regular_pool_=nullptr;
    Item* faith_pool_=nullptr;
    // Diagnostics are outside the deterministic rewindable world.
    std::size_t last_bytes_=0,peak_bytes_=0,last_blocks_=0;
    std::uint64_t total_bytes_=0;
    std::uint32_t snapshots_=0;
};

} // namespace th10::multiplayer
