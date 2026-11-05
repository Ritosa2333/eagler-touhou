#include "AudioManager.hpp"
namespace th10 {
void AudioManager::queue_effect(i32 effect,i32 pan,const SoundDefinition* definitions) noexcept {
    const i32 lifetime=definitions[effect].lifetime;
    for(u32 slot=0;slot<12;++slot){
        if(pending_effects[slot]<0){pending_effects[slot]=effect;effect_lifetimes[effect]=lifetime;pan_values[slot][0]=pan;}
        else if(pending_effects[slot]==effect){
            const i32 count=pan_count[slot];if(count>=128)return;
            // A play request following a queued stop uses count -1 in the
            // original. Preserve the write to the preceding word of this object.
            const u32 offset=offsetof(AudioManager,pan_values)+(slot*128+static_cast<u32>(count))*4;
            std::memcpy(reinterpret_cast<u8*>(this)+offset,&pan,4);
        }else continue;
        pan_count[slot]=static_cast<i32>(static_cast<u32>(pan_count[slot])+1);return;
    }
}
void AudioManager::queue_effect_position(i32 effect,float position,const SoundDefinition* definitions) noexcept {queue_effect(effect,(number(position)*number(0x1.4d5556p+2f)).truncate_int(),definitions);}
void AudioManager::stop_effect(i32 effect) noexcept {
    for(u32 slot=0;slot<12;++slot){if(pending_effects[slot]<0)pending_effects[slot]=effect;else if(pending_effects[slot]!=effect)continue;pan_count[slot]=-1;return;}
}
void AudioManager::queue_music(i32 kind,i32 argument,const char* filename){
    for(u32 slot=0;slot<31;++slot)if(!commands[slot].kind){auto& command=commands[slot];command.kind=kind;command.argument=argument;std::strcpy(command.filename,filename);command.step=0;return;}
}
}
