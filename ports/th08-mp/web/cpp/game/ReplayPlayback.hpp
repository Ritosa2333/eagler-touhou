#pragma once
#include "ReplayFile.hpp"
#include "ReplayStream.hpp"
#include "GameplaySession.hpp"
#include "EclVm.hpp"
#include "Chain.hpp"
namespace th08 {
// Owns decoded replay bytes, per-stage bounds and the original playback snapshot.
// Game input and timing are consumed by ReplayStream at calculation priority 6.
class ReplayPlayback {
    ReplayFile file;ReplayMetadata metadata_;u32 stage_mask_=0;bool loaded=false;
    u32 block_end(u32 start)const;
public:
    ReplayStream stream;ReplayInputState input;
    bool load(const u8* bytes,u32 size);
    void reset();
    bool begin(i32 stage,GameplaySession&,EclGlobals&,u8& power_item_counter);
    bool has_stage(i32 stage)const{return loaded&&u32(stage)<9&&(stage_mask_&(1u<<stage));}
    u32 stage_mask()const{return stage_mask_;}
    const ReplayMetadata& metadata()const{return metadata_;}
    bool snapshot(i32 stage,ReplayStage& out)const;
    bool update(u32 flags,bool slow_mode);
    JobResult after_update(u32 flags,i32 mode,bool dialogue,bool dialogue_waiting,bool boss_present)const;
};
}
