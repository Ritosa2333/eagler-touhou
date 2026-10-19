#include "audio.hpp"
#include <cstring>
#include <stdexcept>
#include <xmmintrin.h>

namespace th20::source::audio {
void initialize(EffectRequest& value) noexcept {std::memset(&value,0,sizeof value);}
void initialize(Command& value) noexcept {std::memset(&value,0,sizeof value);}
void initialize(EffectChannel& value) noexcept {std::memset(&value,0,sizeof value);value.cooldown=-1;}
void bind_effect(EffectChannel& channel,std::int32_t id) {
    channel.cooldown=-1;
    for(const auto& definition:effect_definitions) if(definition.id==id) {
        channel.definition=&definition;channel.id=id;return;
    }
    throw std::out_of_range("Unknown sound effect id");
}
SoundInf::SoundInf() : direct_sound(nullptr),silent_buffer(nullptr),window(nullptr),device_owner(nullptr),
    notify_thread_id(0),notify_thread(nullptr),retained_18(0),requests{},preloaded(),preloaded_index(0),
    track_formats(nullptr),current_track{},effects{},source_buffers{},duplicate_counts{},queued_track{},commands{},
    track_names{},music_file{},stream(nullptr),retained_57c8(0),notification(nullptr),retained_57d0(0),
    retained_57d4(0),retained_57d8(0),music_level(0),effect_level(0),retained_57e4(0),context(nullptr) {
    for(auto& request:requests) audio::initialize(request);
    for(auto& command:commands) audio::initialize(command);
    for(unsigned i=0;i<90;++i) {audio::initialize(effects[i]);bind_effect(effects[i],i);}
}
void SoundInf::enqueue(std::int32_t type,std::int32_t argument,const char* name) {
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(11));
    for(unsigned i=0;i<31;++i) if(commands[i].type==0) {
        commands[i].type=type;commands[i].argument=argument;
        if(strcpy_s(commands[i].name,sizeof commands[i].name,name)!=0)
            throw std::length_error("Audio command name exceeds 255 bytes");
        commands[i].stage=0;return;
    }
    // The original deliberately drops a command when all 31 usable slots fill.
}
void SoundInf::request_effect(std::int32_t id,std::int32_t pan) {
    if(id<0 || id>=90) throw std::out_of_range("Sound effect id");
    unsigned i=0;
    while(i<12 && requests[i].id>=0) {
        if(requests[i].id==id) {
            auto& request=requests[i];
            if(request.count<0 || request.count>59) return;
            request.pans[request.count]=pan;++request.count;return;
        }
        ++i;
    }
    if(i<12) {
        requests[i].id=id;requests[i].pans[0]=pan;requests[i].count=1;
        // Original indexes the table by id here, despite bind_effect searching
        // its non-sorted id field. Preserve this observed asymmetry.
        effects[id].cooldown=effect_definitions[id].cooldown;
    }
}
std::int32_t pan_from_position(float position) noexcept {
    // 0x426eb7/0x426ecc: scalar float MUL, DIV, CVTTSS2SI; including x86's
    // INT_MIN result for NaN/overflow. Constants are checked by the CPU oracle.
    const float product=position*1000.0f;
    const float scaled=product/192.0f;
    return _mm_cvtt_ss2si(_mm_set_ss(scaled));
}
void SoundInf::request_effect_at(std::int32_t id,float position) {request_effect(id,pan_from_position(position));}
void SoundInf::set_effect_pan(std::int32_t id,float position) {
    if(id<0 || id>=90) throw std::out_of_range("Sound effect id");
    effects[id].buffer->SetPan(pan_from_position(position));
}
void SoundInf::stop_effects(std::int32_t id) {
    if(id<0) {for(auto& effect:effects) stop_effect(effect);return;}
    unsigned index=0;
    for(;index<12 && requests[index].id>=0;++index) if(requests[index].id==id) {
        requests[index].count=-1;return;
    }
    if(index<12) {requests[index].id=id;requests[index].count=-1;}
}
LONG effect_volume(std::int16_t attenuation,std::int32_t volume) noexcept {
    if(volume==0) return -10000;
    const float fraction=static_cast<float>(volume)/100.0f;
    const float inverse=1.0f-fraction;
    const float squared=inverse*inverse;
    const float cubed=squared*inverse;
    const float gain=1.0f-cubed;
    const float result=static_cast<float>(static_cast<int>(attenuation)+5000)*gain;
    const auto converted=_mm_cvtt_ss2si(_mm_set_ss(result));
    return static_cast<LONG>(static_cast<std::uint32_t>(converted)-5000u);
}
LONG music_volume(std::int32_t attenuation,std::int32_t volume) noexcept {
    if(volume==0) return -10000;
    const float fraction=static_cast<float>(volume)/100.0f;
    const float inverse=1.0f-fraction;
    const float gain=1.0f-inverse*inverse;
    const auto summed=static_cast<std::int32_t>(static_cast<std::uint32_t>(attenuation)+5000u);
    const float result=static_cast<float>(summed)*gain;
    return static_cast<LONG>(static_cast<std::uint32_t>(_mm_cvtt_ss2si(_mm_set_ss(result)))-5000u);
}
void release_effect(EffectChannel& channel) {
    if(channel.buffer) {channel.buffer->Release();channel.buffer=nullptr;}
}
void stop_effect(EffectChannel& channel) {
    channel.was_playing=0;
    if(channel.buffer) {
        DWORD status=0;
        channel.was_playing=channel.buffer->GetStatus(&status);
        channel.was_playing=status&DSBSTATUS_PLAYING;
        channel.buffer->Stop();
    }
}
void play_effect(EffectChannel& channel,std::int32_t pan,std::int32_t volume) {
    if(!channel.buffer) return;
    channel.buffer->Stop();channel.buffer->SetCurrentPosition(0);channel.buffer->SetPan(pan);channel.pan=pan;
    channel.buffer->SetVolume(effect_volume(channel.definition->volume,volume));
    channel.buffer->Play(0,0,channel.definition->play_flags);
}
}
