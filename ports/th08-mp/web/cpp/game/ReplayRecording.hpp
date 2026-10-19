#pragma once
#include "GameplaySession.hpp"
#include "ReplayFile.hpp"
#include "ReplayStream.hpp"
#include "EclVm.hpp"
namespace th08 {
// The original priority-7 callback samples the seed, resets only the generation
// count and consumes the viewport-change marker. Normal playback omits it.
struct ReplayFrameState {
    u16 seed=0,flags=0;
    void sample(Rng& random,i32& viewport_changed){seed=random.seed;flags=viewport_changed?0x100:0;random.calls=0;viewport_changed=0;}
};
struct ReplayRecordedStage {ReplayStage snapshot;ReplayStream stream;bool present=false;};
struct ReplayBuildIdentity {i32 size=840704,checksum=i32(2724749753u);};
// Owns all stage buffers across scene changes. No original pointers or executable
// memory are retained; offsets are created when assembling the replay file.
class ReplayRecording {
    ReplayMetadata metadata_{};ReplayRecordedStage stages_[9];i32 current_stage=-1;bool initialized=false;
public:
    ReplayInputState input;ReplayFrameState frame_state;
    void reset();
    bool begin(i32 stage,const GameplaySession&,const EclGlobals&,u8 power_item_counter,ReplayBuildIdentity identity={});
    void update(){if(current_stage>=0)stages_[current_stage].stream.record(input);}
    void stop(){if(current_stage>=0)stages_[current_stage].stream.stop_recording();}
    bool ready()const{return initialized;}
    i32 current()const{return current_stage;}
    const ReplayMetadata& metadata()const{return metadata_;}
    ReplayRecordedStage* stage(i32 index){return u32(index)<9?&stages_[index]:nullptr;}
    const ReplayRecordedStage* stage(i32 index)const{return u32(index)<9?&stages_[index]:nullptr;}
    void finish_score(i32 stage,u32 score){if(auto* s=this->stage(stage);s&&s->present)s->snapshot.end_score=score;}
    std::vector<u8> assemble(const ReplayMetadata& metadata)const;
};
}
