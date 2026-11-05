#include "ReplayPlayback.hpp"
namespace th08 {
void ReplayPlayback::reset(){file=ReplayFile{};metadata_=ReplayMetadata{};stage_mask_=0;loaded=false;stream=ReplayStream{};input=ReplayInputState{};}
u32 ReplayPlayback::block_end(u32 start)const{
    u32 end=sizeof(ReplayHeader)+metadata_.header.payload_size;
    for(const auto* offsets:{metadata_.header.stage_offsets,metadata_.header.timing_offsets})for(u32 i=0;i<9;i++)if(offsets[i]>start&&offsets[i]<end)end=offsets[i];
    return end;
}
bool ReplayPlayback::load(const u8* bytes,u32 size){
    reset();if(!file.decode(bytes,size))return false;std::memcpy(&metadata_,file.decoded().data(),sizeof(metadata_));
    if(metadata_.shot_type>=12||metadata_.difficulty>=5||metadata_.spell_number>=222)return false;
    for(u32 i=0;i<9;i++)if(const u32 start=metadata_.header.stage_offsets[i]){
        const u32 timing=metadata_.header.timing_offsets[i];if(!timing||block_end(start)-start<sizeof(ReplayStage)+2||block_end(timing)-timing<2)return false;
        stage_mask_|=1u<<i;
    }
    loaded=stage_mask_!=0;return loaded;
}
bool ReplayPlayback::snapshot(i32 stage,ReplayStage& out)const{if(!has_stage(stage))return false;std::memcpy(&out,file.decoded().data()+metadata_.header.stage_offsets[stage],sizeof(out));return true;}
bool ReplayPlayback::begin(i32 stage,GameplaySession& s,EclGlobals& game,u8& power_item_counter){
    ReplayStage data;if(!snapshot(stage,data))return false;
    const u32 start=metadata_.header.stage_offsets[stage],time=metadata_.header.timing_offsets[stage];const auto* bytes=file.decoded().data();
    if(!stream.begin_playback(bytes+start+sizeof(ReplayStage),block_end(start)-start-sizeof(ReplayStage),bytes+time,block_end(time)-time,metadata_.header.unknown6!=0))return false;
    game.shot=metadata_.shot_type;game.difficulty=metadata_.difficulty;s.numbers.points=data.points;s.rank.value=data.rank;
    s.values.set_lives(data.lives);s.values.set_bombs(data.bombs);s.values.set_power(data.power);s.numbers.graze=data.graze;
    power_item_counter=data.power_item_counter;s.numbers.point_value=data.point_value;s.config=metadata_.configuration;s.random.seed=data.seed;
    s.numbers.captured_spells=data.captured_spells;s.numbers.point_extends=u32(data.extends);s.numbers.next_point_extend=data.next_extend;s.numbers.gauge=data.humanity;s.numbers.clock_time=i8(data.clock);
    for(i32 previous=stage-1;previous>=0;--previous)if(has_stage(previous)){ReplayStage previous_data;snapshot(previous,previous_data);s.numbers.score=s.numbers.display_score=previous_data.end_score;break;}
    return true;
}
bool ReplayPlayback::update(u32 flags,bool slow_mode){input.flags=flags;input.slow_mode=slow_mode;return stream.play(input);}
JobResult ReplayPlayback::after_update(u32 flags,i32 mode,bool dialogue,bool waiting,bool boss)const{
    if((flags&4)&&!(flags&512)){
        if(dialogue&&waiting&&stream.frame%3!=2)return JobResult::Restart;
        if(mode==2&&!boss&&stream.frame%5!=4)return JobResult::Restart;
    }return JobResult::Continue;
}
}
