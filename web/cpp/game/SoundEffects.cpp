#include "SoundEffects.hpp"
namespace th15 {
#include "SoundDefinitions.inc"
SoundEffects::SoundEffects(SoundOutput& sink):output(sink){reset();}
void SoundEffects::reset(){state={};voices={};state.indices.fill(-1);error.clear();for(u32 id=0;id<voices.size();id++)for(const auto& row:sound_definitions)if(row.id==i32(id)){voices[id].definition=&row;break;}}
bool SoundEffects::enqueue(i32 id,i32 pan){
 if(id<0||id>=i32(voices.size())){error="Sound request outside original voice range";return false;}
 for(u32 n=0;n<state.indices.size();n++){if(state.indices[n]<0){state.indices[n]=id;state.pans[n][0]=pan;state.counts[n]=1;voices[id].renewal=sound_definitions[id].renew;return true;}if(state.indices[n]==id){auto& count=state.counts[n];if(count>=0&&count<60)state.pans[n][count++]=pan;return true;}}return true;
}
bool SoundEffects::positioned(i32 id,float x){return enqueue(id,truncate_int(float(float(x*1000.f)/192.f)));}
void SoundEffects::stop_voice(u32 id){auto& voice=voices[id];voice.resume=false;if(output.sound_available(id)){voice.resume=output.sound_playing(id);output.sound_stop(id);}}
bool SoundEffects::stop(i32 id){
 if(id<0){for(u32 n=0;n<voices.size();n++)stop_voice(n);return true;}if(id>=i32(voices.size())){error="Sound stop outside original voice range";return false;}
 for(u32 n=0;n<state.indices.size();n++){if(state.indices[n]<0||state.indices[n]==id){state.indices[n]=id;state.counts[n]=-1;break;}}return true;
}
i32 SoundEffects::adjusted_volume(i32 base,i32 master)noexcept{if(!master)return -10000;const float inverse=float(1.f-float(float(master)/100.f)),square=float(inverse*inverse),cube=float(square*inverse),gain=float(1.f-cube);return wrapping_sub(truncate_int(float(gain*float(wrapping_add(base,5000)))),5000);}
void SoundEffects::start_voice(u32 id,i32 pan){if(!output.sound_available(id))return;auto& voice=voices[id];output.sound_stop(id);output.sound_position(id,0);output.sound_pan(id,pan);voice.pan=pan;output.sound_volume(id,adjusted_volume(voice.definition->volume,master_volume));output.sound_play(id,voice.definition->flags);}
void SoundEffects::process(){
 if(!enabled)return;for(u32 n=0;n<state.indices.size();n++){const i32 id=state.indices[n];if(id<0)break;state.indices[n]=-1;const i32 count=state.counts[n];state.counts[n]=0;if(count<0)stop_voice(u32(id));else {i32 sum=0;for(i32 p=0;p<count;p++)sum=wrapping_add(sum,state.pans[n][p]);start_voice(u32(id),count>0?sum/count:0);}}
}
void SoundEffects::suspend(){state.indices[0]=-1;for(u32 id=0;id<voices.size();id++)stop_voice(id);}
void SoundEffects::resume(){for(u32 id=0;id<voices.size();id++)if(output.sound_available(id)&&voices[id].resume)output.sound_play(id,voices[id].definition->flags);}
}
