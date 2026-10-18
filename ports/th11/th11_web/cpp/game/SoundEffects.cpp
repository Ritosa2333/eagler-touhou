#include "SoundEffects.hpp"
namespace th11 {
// TH11 Japanese 1.00a: original tables 4a34f0 and 4a36b0.
const SoundDefinition sound_definitions[56]={
 {0,-1900,0},{0,-2100,0},{1,-1200,5},{1,-1500,5},{2,-1100,100},{3,-700,100},
 {4,-700,100},{5,-1900,50},{6,-2200,50},{7,-2400,50},{8,-500,100},{9,-400,100},
 {10,-800,10},{11,-1500,10},{12,-1000,100},{5,-1100,50},{13,-1300,50},{14,-1400,50},
 {15,-900,100},{16,-880,0},{17,-1500,0},{5,-300,20},{6,-1800,20},{7,-1800,20},
 {18,-1100,50},{19,-1300,50},{20,-1500,50},{21,-500,100},{22,-1100,20},{23,-800,90},
 {22,-1200,20},{18,-500,50},{24,-800,100},{25,-800,100},{26,-800,100},{27,-500,0},
 {28,-300,100},{29,0,100},{30,0,100},{30,-600,100},{8,-300,100},{31,-300,100},
 {32,-300,100},{33,-300,100},{34,-100,100},{35,0,100},{36,-800,50},{37,-200,100},
 {38,-200,100},{39,-200,100},{40,-1100,0},{41,0,100},{42,0,100},{43,0,100},
 {44,-500,100},{45,-100,100}
};
const char* const sound_samples[46]={
 "se_plst00.wav","se_enep00.wav","se_pldead00.wav","se_power0.wav","se_power1.wav",
 "se_tan00.wav","se_tan01.wav","se_tan02.wav","se_ok00.wav","se_cancel00.wav",
 "se_select00.wav","se_gun00.wav","se_cat00.wav","se_lazer00.wav","se_lazer01.wav",
 "se_enep01.wav","se_damage00.wav","se_item00.wav","se_kira00.wav","se_kira01.wav",
 "se_kira02.wav","se_timeout.wav","se_graze.wav","se_powerup.wav","se_pause.wav",
 "se_cardget.wav","se_option.wav","se_damage01.wav","se_timeout2.wav","se_invalid.wav",
 "se_slash.wav","se_ch00.wav","se_ch01.wav","se_hint00.wav","se_extend.wav",
 "se_cardget.wav","se_water.wav","se_warpl.wav","se_warpr.wav","se_nep00.wav",
 "se_msl.wav","se_bonus.wav","se_bonus2.wav","se_cat01.wav","se_enep02.wav","se_alert.wav"
};
void SoundEffects::reset(){state={};for(auto& n:state.metadata)n=-1;for(auto& n:state.indices)n=-1;}
void SoundEffects::enqueue(i32 id,i32 pan){
    if(id<0||id>=56)return;
    for(u32 n=0;n<12;++n){
        if(state.indices[n]<0){state.indices[n]=id;state.metadata[id]=sound_definitions[id].metadata;state.pans[n][0]=pan;state.counts[n]=wrapping_add(state.counts[n],1);return;}
        if(state.indices[n]==id){if(state.counts[n]>=0&&state.counts[n]<128)state.pans[n][state.counts[n]++]=pan;return;}
    }
}
void SoundEffects::positioned(i32 id,float x){enqueue(id,i32(double(x)*1000./192.));}
void SoundEffects::stop(i32 id){
    if(id<0||id>=56)return;
    for(u32 n=0;n<12;++n)if(state.indices[n]<0||state.indices[n]==id){state.indices[n]=id;state.counts[n]=-1;return;}
}
i32 SoundEffects::adjusted_volume(i32 volume,i32 master,bool music){
    if(!master)return -10000;
    // Preserve the original single-precision stores around its x87 expression.
    const float fraction=float(double(master)/100.);
    const float inverse=float(1.-double(fraction));
    const float power=float(music?double(inverse)*inverse:double(inverse)*inverse*inverse);
    const float gain=float(1.-double(power));
    return i32(double(gain)*double(volume+5000))-5000;
}
void SoundEffects::process(){
    if(!initialized||!enabled)return;
    for(u32 n=0;n<12;++n){
        const i32 id=state.indices[n];if(id<0)break;state.indices[n]=-1;
        const i32 count=state.counts[n];state.counts[n]=0;
        if(id>=56||count>128)continue;
        const u32 buffer=buffers[id];if(!buffer)continue;
        if(count<0){output.sound_stop(buffer);continue;}
        i32 sum=0;for(i32 j=0;j<count;++j)sum=wrapping_add(sum,state.pans[n][j]);
        output.sound_stop(buffer);output.sound_position(buffer,0);output.sound_pan(buffer,count?sum/count:0);
        output.sound_volume(buffer,adjusted_volume(sound_definitions[id].volume,master_volume));output.sound_play(buffer);
    }
}
}
