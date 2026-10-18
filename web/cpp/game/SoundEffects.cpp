#include "SoundEffects.hpp"
namespace th08 {
const SoundDefinition sound_definitions[46]={
 {0,-1900,0},{0,-2100,0},{1,-1200,5},{1,-1500,5},{2,-1100,100},{3,-700,100},
 {4,-700,100},{5,-1900,50},{6,-2200,50},{7,-2400,50},{8,-1100,100},{9,-1100,100},
 {10,-1500,10},{11,-1500,10},{12,-1000,100},{5,-1100,50},{13,-1300,50},{14,-1400,50},
 {15,-900,100},{16,-400,100},{17,-880,0},{18,-1500,0},{5,-300,20},{6,-1800,20},
 {7,-1800,20},{19,-1100,50},{20,-1300,50},{21,-1500,50},{22,-500,140},{23,-500,100},
 {24,-1100,20},{25,-800,90},{24,-1200,20},{19,-500,50},{26,-800,100},{27,-800,100},
 {28,-800,100},{29,-700,0},{30,-300,100},{31,-800,100},{32,-800,100},{33,-200,100},
 {34,0,100},{34,-600,100},{35,-800,0},{8,-100,100}
};
const char* const sound_samples[36]={
 "se_plst00.wav","se_enep00.wav","se_pldead00.wav","se_power0.wav","se_power1.wav","se_tan00.wav",
 "se_tan01.wav","se_tan02.wav","se_ok00.wav","se_cancel00.wav","se_select00.wav","se_gun00.wav",
 "se_cat00.wav","se_lazer00.wav","se_lazer01.wav","se_enep01.wav","se_nep00.wav","se_damage00.wav",
 "se_item00.wav","se_kira00.wav","se_kira01.wav","se_kira02.wav","se_extend.wav","se_timeout.wav",
 "se_graze.wav","se_powerup.wav","se_pause.wav","se_cardget.wav","se_option.wav","se_damage01.wav",
 "se_timeout2.wav","se_opshow.wav","se_ophide.wav","se_invalid.wav","se_slash.wav","se_item01.wav"
};
void SoundEffects::reset(){state={};for(auto& n:state.metadata)n=-1;for(auto& n:state.indices)n=-1;}
void SoundEffects::enqueue(i32 index,i32 pan){
    if(index<0||index>=46)return;
    for(u32 i=0;i<12;++i){
        if(state.indices[i]<0){state.indices[i]=index;state.metadata[index]=sound_definitions[index].metadata;state.pans[i][0]=pan;state.counts[i]=wrapping_add(state.counts[i],1);return;}
        if(state.indices[i]==index){if(state.counts[i]>=0&&state.counts[i]<128)state.pans[i][state.counts[i]++]=pan;return;}
    }
}
void SoundEffects::positioned(i32 index,float x){enqueue(index,((number(x)-number(192))*number(1000)/number(192)).truncate_int());}
i32 SoundEffects::adjusted_volume(i32 volume,i32 master,bool music){
    const float initial=(Extended::from_int(master)/number(100)).to_float();
    if(!master)return -10000;
    const auto inverse=number(1)-number(initial);const float stored=inverse.to_float();
    auto power=inverse*number(stored);if(!music)power=power*number(stored);
    const float scaled=(number(1)-number(power.to_float())).to_float();
    return wrapping_sub((Extended::from_int(wrapping_add(volume,5000))*number(scaled)).truncate_int(),5000);
}
void SoundEffects::process(){
    if(!initialized||!enabled)return;
    for(u32 i=0;i<12;++i){const i32 index=state.indices[i];if(index<0)break;state.indices[i]=-1;
        i32 sum=0;const i32 count=state.counts[i];if(count<=0||count>128){state.counts[i]=0;continue;}
        for(i32 j=0;j<count;++j)sum=wrapping_add(sum,state.pans[i][j]);sum/=count;state.counts[i]=0;
        if(index>=46||!buffers[index])continue;const auto buffer=buffers[index];
        output.stop(buffer);output.position(buffer,0);output.pan(buffer,sum);
        output.volume(buffer,adjusted_volume(sound_definitions[index].volume,master_volume,false));output.play(buffer,0,0);
    }
}
i32 AudioFades::volume(i32 level){return buffer?output.volume(buffer,SoundEffects::adjusted_volume(level,master_volume,true)):i32(0x800401f0u);}
void AudioFades::fade(i32 type,float seconds){state.type=type;state.total=state.progress=(number(seconds)*number(60)).truncate_int();if(type==2)volume(-10000);else if(type==3)volume(-1000);}
i32 AudioFades::update(i32 type){
    if(state.type!=type)return 0;state.progress=wrapping_sub(state.progress,1);
    if(state.progress<=0){state.type=0;if(type==1&&buffer)output.stop(buffer);return 1;}
    if(!state.total)return 0;
    const i32 range=type<=2?5000:1000,product=signed_bits(u32(state.progress)*u32(range));
    const i32 level=product/state.total;volume(type==1||type==4?wrapping_sub(level,range):wrapping_sub(0,level));return 0;
}
void AudioFades::update_all(){update(1);update(2);update(4);update(3);}
i32 AudioFades::stop(){if(!buffer)return i32(0x800401f0u);state.playing=false;i32 result=output.stop(buffer);result|=output.position(buffer,0);state.type=0;return result;}
i32 AudioFades::pause(){if(!buffer)return i32(0x800401f0u);state.playing=false;return output.stop(buffer);}
i32 AudioFades::unpause(){if(!buffer)return i32(0x800401f0u);state.playing=true;return output.play(buffer,state.priority,state.flags);}
i32 AudioFades::play(u32 priority,u32 flags){if(!buffer)return i32(0x800401f0u);state.type=state.progress=state.total=0;volume(0);state.playing=true;state.priority=priority;state.flags=flags;return output.play(buffer,priority,flags);}
}
