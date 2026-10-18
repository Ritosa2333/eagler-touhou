#pragma once
#include "Replay.hpp"
#include "PracticeConfig.hpp"
namespace th15 {
struct ReplayExportDetails {
 i32 score=0;double elapsed=0,total=1;
 i32 year=2000,month=1,day=1,hour=0,minute=0;
};
struct RecordedReplayStage {
 std::array<u8,0x238> snapshot{};
 std::vector<std::array<u16,3>> inputs;std::vector<u8> frame_rates;
 std::vector<ReplayTouch> touches;
 bool present=false;
};
// File snapshots are encoded bytes, not executable guest memory. The recording
// keeps each original input edge and the separate 30-tick FPS stream.
class ReplayRecording {
 std::array<u8,0xa4> metadata{};std::array<RecordedReplayStage,8> stages{};
 i32 current=0,clock=-1;bool sealed=false;std::vector<u8> file;std::string failure;
 std::vector<u8> practice_block;
 bool fail(const char*);bool serialize(const char*,const ReplayExportDetails&);
public:
 bool set_practice(const PracticeConfig& p){if(!p.valid())return false;const auto block=practice_replay_block(p);if(sealed)return block==practice_block&&!block.empty();practice_block=block;return !practice_block.empty();}
 std::array<u8,0xa4>& description(){return metadata;}
 const RecordedReplayStage* stage(u32 n)const{return n<8&&stages[n].present?&stages[n]:nullptr;}
 const std::vector<u8>& output()const{return file;}
 const std::string& error()const{return failure;}
 bool begin(const u8* snapshot,u32 size);
 bool spell_timing(u32 stage,u32 sequence,i32 value);
 void activate(){clock=0;}
 bool tick(ReplayInput,float fps,bool suspended=false,PlayerTouch touch={});
 bool uses_touch()const noexcept{for(const auto& stage:stages)if(!stage.touches.empty())return true;return false;}
 void finish(i64 timestamp,i32 stage,bool cleared);
 bool write(const char* name,const ReplayExportDetails&,bool terminate);
 i32 frame_clock()const{return clock;}
};
}
