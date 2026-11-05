#include "AudioEvents.hpp"
#include <algorithm>
#include <cmath>

namespace th08::multiplayer {
namespace {
u32 word(u32 hash,u32 value){for(u32 byte=0;byte<4;++byte){hash^=(value>>(byte*8))&255u;hash*=16777619u;}return hash;}
}
void AudioEvents::Reset(u32 first){
    for(auto& frame:frames){frame.events.clear();frame.number=invalid_frame;frame.closed=false;}
    open=invalid_frame;next_append=next_commit=first;committed_events=0;
    committed_digest=2166136261u;failed=first==invalid_frame;draining=false;
}
bool AudioEvents::Valid(const AudioEvent& event){
    if(u32(event.kind)>u32(AudioEventKind::Volumes)||!std::isfinite(event.value))return false;
    return std::memchr(event.text,0,sizeof(event.text))!=nullptr;
}
bool AudioEvents::BeginFrame(u32 frame){
    if(failed||draining||IsOpen()||frame==invalid_frame||frame!=next_append||frame<next_commit)return Fail();
    auto& slot=frames[frame%capacity];if(slot.number!=invalid_frame)return Fail();
    slot.events.clear();slot.closed=false;slot.number=open=frame;return true;
}
bool AudioEvents::Append(const AudioEvent& event){
    if(failed||draining||!IsOpen()||!Valid(event))return Fail();
    auto& events=frames[open%capacity].events;
    if(events.size()>=events_per_frame)return Fail();
    events.push_back(event);return true;
}
bool AudioEvents::EndFrame(){
    if(failed||draining||!IsOpen())return Fail();
    frames[open%capacity].closed=true;next_append=open+1;open=invalid_frame;return true;
}
bool AudioEvents::DiscardFrom(u32 frame){
    if(failed||draining||IsOpen()||frame==invalid_frame||frame<next_commit||frame>next_append)return false;
    for(auto& slot:frames)if(slot.number!=invalid_frame&&slot.number>=frame){
        slot.events.clear();slot.number=invalid_frame;slot.closed=false;
    }
    next_append=frame;return true;
}
u32 AudioEvents::DigestEvent(const AudioEvent& event,u32 hash){
    u32 value;std::memcpy(&value,&event.value,sizeof(value));
    for(const auto part:{u32(event.kind),u32(event.first),u32(event.second),value,u32(event.flag),
        u32(event.settings.wav),u32(event.settings.preload),u32(event.settings.sounds),
        u32(event.settings.sound_volume),u32(event.settings.music_volume)})hash=word(hash,part);
    for(const auto* c=event.text;*c;++c){hash^=u8(*c);hash*=16777619u;}
    return word(hash,0);
}
u32 AudioEvents::FrameDigest(u32 frame)const{
    const auto& slot=frames[frame%capacity];if(slot.number!=frame||!slot.closed)return 0;
    u32 hash=word(word(2166136261u,frame),u32(slot.events.size()));
    for(const auto& event:slot.events)hash=DigestEvent(event,hash);return hash;
}
u32 AudioEvents::FrameEventCount(u32 frame)const{
    const auto& slot=frames[frame%capacity];return slot.number==frame?u32(slot.events.size()):0;
}
bool AudioEvents::CommitThrough(u32 confirmed,u32 simulated,AudioEventOutput& output){
    if(failed||draining||IsOpen())return Fail();
    if(confirmed==invalid_frame||simulated==invalid_frame)return true;
    const auto end=std::min(confirmed,simulated);if(end<next_commit)return true;
    if(end>=next_append)return Fail();
    for(u32 frame=next_commit;frame<=end;++frame){const auto& slot=frames[frame%capacity];
        if(slot.number!=frame||!slot.closed)return Fail();}
    draining=true;
    while(next_commit<=end){
        auto& slot=frames[next_commit%capacity];
        committed_digest=word(word(committed_digest,next_commit),u32(slot.events.size()));
        for(const auto& event:slot.events){
            if(!output.apply_audio_event(event)){draining=false;return Fail();}
            committed_digest=DigestEvent(event,committed_digest);++committed_events;
        }
        slot.events.clear();slot.number=invalid_frame;slot.closed=false;++next_commit;
    }
    draining=false;return true;
}
}
