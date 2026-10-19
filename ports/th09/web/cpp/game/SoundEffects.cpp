#include "SoundEffects.hpp"
namespace th09 {
// TH09 1.50a's 54 logical effects share 39 original PCM samples.
const SoundDefinition sound_definitions[54]={
 {0,-2100,0},{0,-2400,0},{1,-1600,5},{1,-1900,5},{2,-1500,100},{3,-1100,100},
 {4,-1100,100},{5,-1900,50},{6,-2200,50},{7,-2400,50},{8,-1500,100},{9,-1500,100},
 {10,-1800,10},{11,-1800,10},{12,-1100,100},{5,-1100,50},{13,-1300,50},{14,-1400,50},
 {15,-1300,100},{0,-1900,0},{16,-1080,0},{0,-1900,0},{5,-900,20},{6,-1800,20},
 {7,-1800,20},{17,-1100,50},{18,-1300,50},{19,-1500,50},{20,-500,140},{0,-1900,0},
 {21,-1100,20},{38,-300,100},{21,-1200,20},{17,-500,50},{22,-800,100},{23,-800,100},
 {0,-1900,0},{24,-1100,0},{25,-300,100},{26,-200,100},{27,0,100},{27,-600,100},
 {8,-400,100},{28,-1000,100},{29,-1000,100},{30,-400,100},{31,-700,0},{32,-700,0},
 {33,-350,100},{34,-500,100},{35,-900,100},{36,-900,100},{37,-900,100},{12,-500,100}
};
const char* const sound_samples[39]={
 "se_plst00.wav","se_enep00.wav","se_pldead00.wav","se_power0.wav","se_power1.wav","se_tan00.wav",
 "se_tan01.wav","se_tan02.wav","se_ok00.wav","se_cancel00.wav","se_select00.wav","se_gun00.wav",
 "se_cat00.wav","se_lazer00.wav","se_lazer01.wav","se_enep01.wav","se_damage00.wav","se_kira00.wav",
 "se_kira01.wav","se_kira02.wav","se_extend.wav","se_graze.wav","se_pause.wav","se_cardget.wav",
 "se_damage01.wav","se_timeout2.wav","se_invalid.wav","se_slash.wav","se_charge00.wav","se_charge01b.wav",
 "se_exattack.wav","se_eterase.wav","se_gosp.wav","se_life1.wav","se_playerdead.wav","se_chargeup.wav",
 "se_warning.wav","se_timestop0.wav","se_powerup.wav"
};
void SoundEffects::reset(){state={};for(auto& n:state.metadata)n=-1;for(auto& n:state.indices)n=-1;}
void SoundEffects::enqueue(i32 id,i32 pan){
    if(id<0||id>=54)return;for(u32 n=0;n<12;++n){
        if(state.indices[n]<0){state.indices[n]=id;state.metadata[id]=sound_definitions[id].metadata;state.pans[n][0]=pan;state.counts[n]=wrapping_add(state.counts[n],1);return;}
        if(state.indices[n]==id){if(state.counts[n]>=0&&state.counts[n]<128)state.pans[n][state.counts[n]++]=pan;return;}
    }
}
void SoundEffects::positioned(i32 id,float x){enqueue(id,i32(x*6.9444446563720703125f));}
i32 SoundEffects::adjusted_volume(i32 volume,i32 master,bool music){
    if(!master)return -10000;const float inverse=1.f-float(master)*.009999999776482582f;
    float power=inverse*inverse;if(!music)power*=inverse;return i32((1.f-power)*float(volume+5000))-5000;
}
void SoundEffects::process(){
    if(!initialized||!enabled)return;for(u32 n=0;n<12;++n){const i32 id=state.indices[n];if(id<0)break;state.indices[n]=-1;
        const i32 count=state.counts[n];state.counts[n]=0;if(count<=0||count>128||id>=54)continue;i32 sum=0;for(i32 j=0;j<count;++j)sum=wrapping_add(sum,state.pans[n][j]);
        const auto buffer=buffers[id];if(!buffer)continue;output.sound_stop(buffer);output.sound_position(buffer,0);output.sound_pan(buffer,sum/count);
        output.sound_volume(buffer,adjusted_volume(sound_definitions[id].volume,master_volume));output.sound_play(buffer);
    }
}
}
