#include "AudioEvents.hpp"
#include <algorithm>
#include <cstring>

namespace th10::multiplayer {
void AudioEvents::Reset(){
    for(auto& frame:frames_){frame.events.clear();frame.number=Netplay::INVALID_FRAME;frame.closed=false;}
    open_=Netplay::INVALID_FRAME;next_commit_=effects_=stops_=music_=0;
    digest_=2166136261u;failed_=false;
}
bool AudioEvents::BeginFrame(u32 frame){
    if(failed_||IsOpen()||frame==Netplay::INVALID_FRAME||frame<next_commit_)return false;
    auto& slot=frames_[frame%capacity];
    if(slot.number!=Netplay::INVALID_FRAME)return false;
    slot.number=open_=frame;slot.closed=false;slot.events.clear();return true;
}
bool AudioEvents::EndFrame(){
    if(!IsOpen())return false;
    frames_[open_%capacity].closed=true;open_=Netplay::INVALID_FRAME;return !failed_;
}
bool AudioEvents::DiscardFrom(u32 frame){
    if(failed_||IsOpen()||frame==Netplay::INVALID_FRAME||frame<next_commit_)return false;
    for(auto& slot:frames_)if(slot.number!=Netplay::INVALID_FRAME&&slot.number>=frame){
        slot.events.clear();slot.number=Netplay::INVALID_FRAME;slot.closed=false;
    }
    return true;
}
void AudioEvents::append(const Event& event){
    if(failed_)return;
    if(!IsOpen()){failed_=true;return;}
    auto& events=frames_[open_%capacity].events;
    if(events.size()>=events_per_frame){failed_=true;return;}
    events.push_back(event);
}
void AudioEvents::capture_effect(i32 effect,i32 pan,const SoundDefinition* definitions) noexcept {
    if(effect<0||effect>=128||!definitions){failed_=true;return;}
    Event event{};event.kind=Kind::Effect;event.first=effect;event.second=pan;event.definitions=definitions;append(event);
}
void AudioEvents::capture_stop(i32 effect) noexcept {
    if(effect<0||effect>=128){failed_=true;return;}
    Event event{};event.kind=Kind::Stop;event.first=effect;append(event);
}
void AudioEvents::capture_music(i32 kind,i32 argument,const char* filename){
    if(!filename){failed_=true;return;}
    std::size_t length=0;while(length<256&&filename[length])++length;
    if(length==256){failed_=true;return;}
    Event event{};event.kind=Kind::Music;event.first=kind;event.second=argument;
    std::memcpy(event.filename,filename,length+1);append(event);
}
void AudioEvents::word(u32 value){for(u32 i=0;i<4;++i){digest_^=(value>>(i*8))&255u;digest_*=16777619u;}}
u32 AudioEvents::PendingFrames()const{
    u32 count=0;for(const auto& frame:frames_)count+=frame.number!=Netplay::INVALID_FRAME;return count;
}
bool AudioEvents::CommitThrough(u32 confirmed,u32 last,AudioManager& output,
                                bool (*finish_frame)(void*),void* context){
    if(failed_||IsOpen()||output.command_sink==this)return false;
    if(confirmed==Netplay::INVALID_FRAME||last==Netplay::INVALID_FRAME)return true;
    const auto end=std::min(confirmed,last);
    while(next_commit_<=end){
        auto& slot=frames_[next_commit_%capacity];
        if(slot.number!=next_commit_||!slot.closed){failed_=true;return false;}
        word(next_commit_);word(u32(slot.events.size()));
        for(const auto& event:slot.events){
            word(u32(event.kind));word(u32(event.first));word(u32(event.second));
            switch(event.kind){
            case Kind::Effect:output.queue_effect(event.first,event.second,event.definitions);++effects_;break;
            case Kind::Stop:output.stop_effect(event.first);++stops_;break;
            case Kind::Music:
                for(const char* c=event.filename;*c;++c){digest_^=u8(*c);digest_*=16777619u;}
                output.queue_music(event.first,event.second,event.filename);++music_;break;
            }
        }
        if(finish_frame&&!finish_frame(context)){failed_=true;return false;}
        slot.events.clear();slot.number=Netplay::INVALID_FRAME;slot.closed=false;++next_commit_;
    }
    return true;
}
}
