#include "SoundEffects.hpp"
#include "PortTitle.hpp"
#include "Timer.hpp"
namespace th11 {
// TH12's sample table is NOT TH11's. th12.exe keeps it at .data 0x4aea50 as 49
// consecutive pointers to .rdata "se_*.wav" strings, and that order matches
// th12.dat's archive order exactly. TH11 shipped se_hint00.wav, which TH12 does
// not; TH12 adds se_item01 and ten others. Loading TH11's list therefore failed
// with "Missing sound: se_hint00.wav" before the game could start.
//
// The 56-entry voice table below is still TH11's (see docs/th12/AUDIO.md): its
// `sample` fields are remapped onto TH12's list *by filename*, which is exact for
// every TH11 sample TH12 still ships.
#if PORT_TITLE == 14
#include "Th14AudioTables.inc"
#else
const SoundDefinition sound_definitions[sound_voice_count]={
 {0,-1900,0},{0,-2100,0},{1,-1200,5},{1,-1500,5},{2,-1100,100},{3,-700,100},
 {4,-700,100},{5,-1900,50},{6,-2200,50},{7,-2400,50},{8,-500,100},{9,-400,100},
 {10,-800,10},{11,-1500,10},{12,-1000,100},{5,-1100,50},{13,-1300,50},{14,-1400,50},
 {15,-900,100},{16,-880,0},{17,-1500,0},{5,-300,20},{6,-1800,20},{7,-1800,20},
 {19,-1100,50},{20,-1300,50},{21,-1500,50},{22,-500,100},{23,-1100,20},{24,-800,90},
 {23,-1200,20},{19,-500,50},{25,-800,100},{26,-800,100},{27,-800,100},{28,-500,0},
 {29,-300,100},{30,0,100},{31,0,100},{31,-600,100},{8,-300,100},{32,-300,100},
 {33,-300,100},{0,-300,100},{34,-100,100},{26,0,100},{0,-800,50},{0,-200,100},
 {0,-200,100},{36,-200,100},{0,-1100,0},{37,0,100},{38,0,100},{0,0,100},
 {39,-500,100},{0,-100,100}
};
#endif
// The four `{0,...}` entries above stand in for TH11-only samples TH12 dropped
// (se_hint00, se_water, se_warpl, se_warpr, se_msl, se_cat01, se_alert). Their
// index is a placeholder until TH12's voice table is recovered, not evidence.
// The `.data` pointer array, in .data order, for each title: TH12's at 0x004aea50 (49 entries) and TH13's at
// 0x004bb320 (60). Both were read out of the executable with tools/th-sample-table.py, which finds the run of
// consecutive u32 slots that point at the image's `se_*.wav` strings; for th12 the tool reproduces this file's
// list and th12.dat's archive order entry for entry, which is what makes the same derivation usable for th13.
//
// The two lists are not prefixes of one another: 43 names are shared, th13 adds se_ch02, se_ch03, se_extend2,
// se_pin00, se_pin01, se_lgods1..4, se_lgodsget, se_msl, se_msl2, se_pldead01, se_heal, se_astralup, se_fault
// and se_noise, and drops se_item01, se_option, se_ufo, se_changeitem, se_ufoalert and se_piyo.
namespace {
const char* const th12_sound_samples[49]={
 "se_plst00.wav","se_enep00.wav","se_pldead00.wav","se_power0.wav","se_power1.wav",
 "se_tan00.wav","se_tan01.wav","se_tan02.wav","se_ok00.wav","se_cancel00.wav",
 "se_select00.wav","se_gun00.wav","se_cat00.wav","se_lazer00.wav","se_lazer01.wav",
 "se_enep01.wav","se_damage00.wav","se_item00.wav","se_item01.wav","se_kira00.wav",
 "se_kira01.wav","se_kira02.wav","se_timeout.wav","se_graze.wav","se_powerup.wav",
 "se_pause.wav","se_cardget.wav","se_option.wav","se_damage01.wav","se_timeout2.wav",
 "se_invalid.wav","se_slash.wav","se_ch00.wav","se_ch01.wav","se_extend.wav",
 "se_cardget.wav","se_nep00.wav","se_bonus.wav","se_bonus2.wav","se_enep02.wav",
 "se_ufo.wav","se_lazer02.wav","se_changeitem.wav","se_ufoalert.wav","se_piyo.wav",
 "se_nodamage.wav","se_boon00.wav","se_don00.wav","se_boon01.wav"
};
const char* const th13_sound_samples[60]={
 "se_plst00.wav","se_enep00.wav","se_pldead00.wav","se_power0.wav","se_power1.wav",
 "se_tan00.wav","se_tan01.wav","se_tan02.wav","se_ok00.wav","se_cancel00.wav",
 "se_select00.wav","se_gun00.wav","se_cat00.wav","se_lazer00.wav","se_lazer01.wav",
 "se_enep01.wav","se_damage00.wav","se_item00.wav","se_kira00.wav","se_kira01.wav",
 "se_kira02.wav","se_timeout.wav","se_graze.wav","se_powerup.wav","se_pause.wav",
 "se_cardget.wav","se_damage01.wav","se_timeout2.wav","se_invalid.wav","se_slash.wav",
 "se_ch00.wav","se_ch01.wav","se_extend.wav","se_cardget.wav","se_nep00.wav",
 "se_bonus.wav","se_bonus2.wav","se_enep02.wav","se_lazer02.wav","se_nodamage.wav",
 "se_boon00.wav","se_don00.wav","se_boon01.wav","se_ch02.wav","se_ch03.wav",
 "se_extend2.wav","se_pin00.wav","se_pin01.wav","se_lgods1.wav","se_lgods2.wav",
 "se_lgods3.wav","se_lgods4.wav","se_lgodsget.wav","se_msl.wav","se_msl2.wav",
 "se_pldead01.wav","se_heal.wav","se_astralup.wav","se_fault.wav","se_noise.wav"
};
}
// The three titles with no inline array get the names mined from their own executables
// (tools/th-sample-table.py) and checked against their own archives (tools/th-archive-names.mjs);
// with the TH12 list they stopped at "Missing sound: se_option.wav", a name TH13 and later do
// not ship. Their voice table stays the TH12 one above, because a voice index is not a name.
#if PORT_TITLE == 16
#include "Th16AudioTables.inc"
#elif PORT_TITLE == 17
#include "Th17AudioTables.inc"
#elif PORT_TITLE == 18
#include "Th18AudioTables.inc"
#endif
#if PORT_TITLE == 14
const char* const* sound_samples=th14_sound_samples;
const u32 sound_sample_count=65;
#elif PORT_TITLE == 16
const char* const* sound_samples=th16_sound_samples;
const u32 sound_sample_count=67;
#elif PORT_TITLE == 17
const char* const* sound_samples=th17_sound_samples;
const u32 sound_sample_count=69;
#elif PORT_TITLE == 18
const char* const* sound_samples=th18_sound_samples;
const u32 sound_sample_count=72;
#else
const char* const* sound_samples=kPortTitle==13?th13_sound_samples:th12_sound_samples;
const u32 sound_sample_count=kPortTitle==13?60u:49u;
#endif
void SoundEffects::reset(){state={};for(auto& n:state.metadata)n=-1;for(auto& n:state.indices)n=-1;}
void SoundEffects::enqueue(i32 id,i32 pan){queue(id,kPortTitle==14?0:pan);}
void SoundEffects::queue(i32 id,i32 pan){
    if(id<0||u32(id)>=sound_voice_count)return;
    for(u32 n=0;n<12;++n){
        if(state.indices[n]<0){state.indices[n]=id;
#if PORT_TITLE == 14
            // Loading resolves voice IDs;46e3d0 metadata indexes table order.
            state.metadata[id]=sound_definitions[id].queue_metadata;
#else
            state.metadata[id]=sound_definitions[id].metadata;
#endif
            state.pans[n][0]=pan;
            // Retail46e458/46e502 replace the count when reusing a queue slot.
            // Pause clears only indices[0], so its old count can still be nonzero.
            if constexpr(kPortTitle==14)state.counts[n]=1;
            else state.counts[n]=wrapping_add(state.counts[n],1);
            return;}
        if(state.indices[n]==id){if(state.counts[n]>=0&&state.counts[n]<(kPortTitle==14?60:128))state.pans[n][state.counts[n]++]=pan;return;}
    }
}
void SoundEffects::positioned(i32 id,float x){
    // TH14 46e470 stores both mulss/divss results before integer conversion.
    if constexpr(kPortTitle==14)queue(id,truncate_int(float(float(x*1000.f)/192.f)));
    else queue(id,i32(double(x)*1000./192.));
}
void SoundEffects::pan(i32 id,i32 value){if(initialized&&enabled&&id>=0&&u32(id)<sound_voice_count&&buffers[id])output.sound_pan(buffers[id],value);}
void SoundEffects::stop(i32 id){
    if(id<0||u32(id)>=sound_voice_count)return;
    for(u32 n=0;n<12;++n)if(state.indices[n]<0||state.indices[n]==id){state.indices[n]=id;state.counts[n]=-1;return;}
}
i32 SoundEffects::adjusted_volume(i32 volume,i32 master,bool music){
    if(!master)return -10000;
    if constexpr(kPortTitle==14){
        // Whole46eb30/481200 use SSE, then subtract5000 after cvttss2si.
        const float fraction=float(float(master)/100.f),inverse=float(1.f-fraction);
        const float square=float(inverse*inverse),power=music?square:float(square*inverse),gain=float(1.f-power);
        return wrapping_sub(truncate_int(float(gain*float(wrapping_add(volume,5000)))),5000);
    }
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
        if(u32(id)>=sound_voice_count||count>128)continue;
        const u32 buffer=buffers[id];if(!buffer)continue;
        if(count<0){output.sound_stop(buffer);continue;}
        i32 sum=0;for(i32 j=0;j<count;++j)sum=wrapping_add(sum,state.pans[n][j]);
        output.sound_stop(buffer);output.sound_position(buffer,0);output.sound_pan(buffer,count?sum/count:0);
        output.sound_volume(buffer,adjusted_volume(sound_definitions[id].volume,master_volume));output.sound_play(buffer);
    }
}
}
