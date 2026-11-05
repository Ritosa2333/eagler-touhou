#include "SessionReplay.hpp"
#include "SpellPresentationClock.hpp"
#include "PracticeConfig.hpp"
namespace th15 {
namespace {template<class T>void write(u8* to,u32 at,T value){std::memcpy(to+at,&value,sizeof value);}}
bool SessionReplay::fail(const std::string& reason){if(error.empty())error=reason.empty()?"Session replay operation failed":reason;return false;}
bool SessionReplay::initialize_live(const ReplayRunState& state){
 if(initialized)return fail("Session replay already initialized");if(!error.empty())return false;const i32 stage=state.progress.stage;
 if(stage<1||stage>7)return fail("Recording scene outside stage range");auto& meta=recording.description();std::copy(settings.bytes.begin(),settings.bytes.end(),meta.begin()+0x18);
 write(meta.data(),0x8c,state.progress.character);write(meta.data(),0x90,state.progress.subcharacter);write(meta.data(),0x94,state.progress.difficulty);write(meta.data(),0x9c,state.progress.continues);write(meta.data(),0xa0,state.progress.spell_id);
 meta[10]=u8(((state.player.mode_flags>>4)&1)|((state.player.mode_flags&0x30)==0x20?2:0));
 if(!snapshots[stage].create(state,game)||!recording.begin(snapshots[stage].bytes().data(),0x238))return fail(snapshots[stage].error.empty()?recording.error():snapshots[stage].error);
 state.player.replay_state=0;game.calls=0;selected=stage;initialized=true;return true;
}
bool SessionReplay::open(const u8* bytes,u32 size,i32 stage,ReplayRunState& state){
 if(initialized)return fail("Session replay already initialized");if(!error.empty())return false;if(!file.open(bytes,size))return fail(file.error());if(stage<1||stage>7||!file.stage(stage))return fail("Selected replay stage unavailable");
 for(u32 n=1;n<8;n++)if(file.stage(n)&&!snapshots[n].read(file.header(n),0x238))return fail(snapshots[n].error);
 const auto& metadata=file.decoded();std::copy(metadata.begin()+0x18,metadata.begin()+0x84,settings.bytes.begin());
 if(!snapshots[stage].restore_progress(state)||!snapshots[stage].restore_seed(game))return fail(snapshots[stage].error);
 PracticeConfig practice;const bool practice_replay=practice_replay_read(bytes,size,practice)&&practice.mode==1;
 state.player.mode_flags=(state.player.mode_flags&~0x30u)|(state.progress.spell_id>=0?0x20u:practice_replay?0x10u:0);if(!(state.player.mode_flags&0x20))state.progress.spell_id=-1;
 playback=std::make_unique<ReplayPlayback>(file);if(!playback->select(stage))return fail(playback->error());state.player.replay_state=1;selected=stage;initialized=true;return true;
}
bool SessionReplay::prepare_stage(ReplayRunState& state){
 if(!initialized||!error.empty())return fail("Session replay unavailable during scene preparation");const i32 stage=state.progress.stage;if(stage<1||stage>7)return fail("Replay scene outside stage range");
 if(playback){if(!file.stage(stage))return fail("Replay next stage unavailable");if(!snapshots[stage].restore_progress(state)||!snapshots[stage].restore_seed(game,&visual)||!playback->select(stage))return fail(snapshots[stage].error.empty()?playback->error():snapshots[stage].error);}
 else {if(!snapshots[stage].create(state,game,false))return fail(snapshots[stage].error);if(!snapshots[stage].restore_seed(game,&visual))return fail("Recording seed unavailable");}
 selected=stage;return true;
}
bool SessionReplay::activate_stage(ReplayRunState& state){
 if(!initialized||!error.empty())return fail("Session replay unavailable during scene activation");if(state.progress.stage!=selected)return fail("Replay activated a different scene");
 if(playback){if(!snapshots[selected].restore_player(state.motion))return fail("Replay player snapshot unavailable");playback->activate();return true;}
 if(!snapshots[selected].refresh(state)||!recording.begin(snapshots[selected].bytes().data(),0x238))return fail(snapshots[selected].error.empty()?recording.error():snapshots[selected].error);recording.activate();return true;
}
bool SessionReplay::spell_timing(i32 sequence,i32& encoded){
 if(!initialized||!error.empty()||selected<1||selected>7)return fail("Replay unavailable for spell timing");
 if(!snapshots[selected].spell_timing(u32(sequence),encoded,!playback))return fail(snapshots[selected].error);
 if(playback)encoded=SpellPresentationClock::replay_value(encoded);
 else if(!recording.spell_timing(selected,u32(sequence),encoded))return fail(recording.error());
 return true;
}

}
