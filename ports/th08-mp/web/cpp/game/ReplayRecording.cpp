#include "ReplayRecording.hpp"
namespace th08 {
void ReplayRecording::reset(){metadata_=ReplayMetadata{};for(auto& stage:stages_)stage=ReplayRecordedStage{};current_stage=-1;initialized=false;input=ReplayInputState{};frame_state=ReplayFrameState{};}
bool ReplayRecording::begin(i32 index,const GameplaySession& session,const EclGlobals& game,u8 power_item_counter,ReplayBuildIdentity identity){
    if(u32(index)>=9||u32(game.shot)>=12||game.difficulty>=5||(game.game_flags&8))return false;
    const auto& n=session.numbers;
    if(!initialized){
        metadata_=ReplayMetadata{};metadata_.shot_type=u8(game.shot);metadata_.difficulty=u8(game.difficulty);metadata_.major_version=0x100;metadata_.minor_version=100;
        std::memcpy(metadata_.exe_version,"0100d",6);metadata_.exe_size=identity.size;metadata_.exe_checksum=identity.checksum;metadata_.practice=bool(game.game_flags&1);metadata_.spell_number=game.game_flags&0x4000?game.current_spell:-1;
        auto cleared=[&](i32 stage){for(i32 difficulty=0;difficulty<4;difficulty++)if(session.clears[game.shot].without_retries[difficulty]&(1u<<stage))return true;return false;};
        metadata_.clear_state=cleared(7)||game.shot>=4?2:cleared(6)?1:0;
        std::memcpy(metadata_.player_name,"NO N",4);metadata_.configuration=session.config;initialized=true;
    }else for(i32 previous=index-1;previous>=0;--previous)if(stages_[previous].present){stages_[previous].snapshot.end_score=n.score;break;}
    auto& stage=stages_[index];stage=ReplayRecordedStage{};stage.present=true;auto& out=stage.snapshot;
    out.graze=n.graze;out.bombs=u8(Scalar::truncate(n.bombs));out.lives=u8(Scalar::truncate(n.lives));out.power=u8(Scalar::truncate(n.power));out.rank=u8(session.rank.value);
    out.points=n.points;out.seed=session.replay_seed;out.power_item_counter=power_item_counter;out.captured_spells=u8(n.captured_spells);out.extends=i32(n.point_extends);out.next_extend=n.next_point_extend;out.humanity=n.gauge;out.clock=u8(n.clock_time);out.point_value=n.point_value;
    stage.stream.begin_record(session.random.seed);current_stage=index;input.current=input.previous=0;return true;
}
std::vector<u8> ReplayRecording::assemble(const ReplayMetadata& metadata)const{
    if(!initialized)return {};
    ReplayMetadata out=metadata;std::memset(out.header.stage_offsets,0,sizeof(out.header.stage_offsets));std::memset(out.header.timing_offsets,0,sizeof(out.header.timing_offsets));
    std::vector<u8> bytes(sizeof(out));
    auto append=[&](const void* data,u32 size){const auto* first=static_cast<const u8*>(data);if(size)bytes.insert(bytes.end(),first,first+size);};
    for(u32 i=0;i<9;i++){const auto& s=stages_[i];if(!s.present)continue;if(s.stream.input_end>s.stream.input.size()||s.stream.timing_end>s.stream.timing.size()||!s.stream.timing_end)return {};out.header.stage_offsets[i]=bytes.size();append(&s.snapshot,sizeof(s.snapshot));append(s.stream.input.data(),s.stream.input_end);}
    for(u32 i=0;i<9;i++){const auto& s=stages_[i];if(!s.present)continue;out.header.timing_offsets[i]=bytes.size();append(s.stream.timing.data(),s.stream.timing_end);}
    out.header.payload_size=bytes.size()-sizeof(ReplayHeader);std::memcpy(bytes.data(),&out,sizeof(out));return bytes;
}
}
