#include "AudioEvents.hpp"
#include <algorithm>
#include <cstring>

namespace th10::multiplayer {
void AudioEvents::Reset(){
    for(auto& frame:frames_){frame.events.clear();frame.number=Netplay::INVALID_FRAME;frame.closed=false;}
    open_=Netplay::INVALID_FRAME;next_commit_=effects_=stops_=music_=0;
    digest_=2166136261u;failure_=Failure::None;
}
bool AudioEvents::BeginFrame(u32 frame){
    if(Failed()||IsOpen()||frame==Netplay::INVALID_FRAME||frame<next_commit_)return false;
    auto& slot=frames_[frame%capacity];
    if(slot.number!=Netplay::INVALID_FRAME)return false;
    slot.number=open_=frame;slot.closed=false;slot.events.clear();return true;
}
bool AudioEvents::EndFrame(){
    if(!IsOpen())return false;
    frames_[open_%capacity].closed=true;open_=Netplay::INVALID_FRAME;return !Failed();
}
bool AudioEvents::DiscardFrom(u32 frame){
    if(Failed()||IsOpen()||frame==Netplay::INVALID_FRAME||frame<next_commit_)return false;
    for(auto& slot:frames_)if(slot.number!=Netplay::INVALID_FRAME&&slot.number>=frame){
        slot.events.clear();slot.number=Netplay::INVALID_FRAME;slot.closed=false;
    }
    return true;
}
void AudioEvents::append(const Event& event,const char* filename){
    if(Failed())return;
    if(!IsOpen()){fail(Failure::CaptureClosed);return;}
    auto& frame=frames_[open_%capacity];auto& events=frame.events;
    const auto name_bytes=event.kind==Kind::Music?filename_bytes(event):0;
    const auto record_bytes=sizeof(Event)+name_bytes,offset=events.size();
    if(offset>bytes_per_frame||record_bytes>bytes_per_frame-offset){fail(Failure::Capacity);return;}
    const auto size=offset+record_bytes;
    // Do not let vector's geometric resize growth exceed the payload budget.
    if(size>events.capacity())events.reserve(std::min(bytes_per_frame,
        std::max(size,events.capacity()*2)));
    events.resize(size);std::memcpy(events.data()+offset,&event,sizeof(Event));
    if(name_bytes)std::memcpy(events.data()+offset+sizeof(Event),filename,name_bytes);
}
void AudioEvents::capture_effect(i32 effect,i32 pan,const SoundDefinition* definitions) noexcept {
    if(effect<0||effect>=128){fail(Failure::EffectId);return;}
    if(!definitions){fail(Failure::Definitions);return;}
    Event event{};event.kind=Kind::Effect;event.first=effect;event.second=pan;std::memcpy(event.payload.data(),&definitions,sizeof(definitions));append(event);
}
void AudioEvents::capture_stop(i32 effect) noexcept {
    if(effect<0||effect>=128){fail(Failure::StopId);return;}
    Event event{};event.kind=Kind::Stop;event.first=effect;append(event);
}
void AudioEvents::capture_music(i32 kind,i32 argument,const char* filename){
    if(!filename){fail(Failure::Filename);return;}
    std::size_t length=0;while(length<256&&filename[length])++length;
    if(length==256){fail(Failure::Filename);return;}
    Event event{};event.kind=Kind::Music;event.first=kind;event.second=argument;
    const auto bytes=u32(length+1);std::memcpy(event.payload.data(),&bytes,sizeof(bytes));append(event,filename);
}
u32 AudioEvents::filename_bytes(const Event& event){
    u32 bytes=0;std::memcpy(&bytes,event.payload.data(),sizeof(bytes));return bytes;
}
bool AudioEvents::scan(const Frame& frame,u32& count){
    count=0;std::size_t offset=0;
    while(offset<frame.events.size()){
        if(frame.events.size()-offset<sizeof(Event))return false;
        Event event{};std::memcpy(&event,frame.events.data()+offset,sizeof(Event));offset+=sizeof(Event);
        if(event.kind==Kind::Music){
            const auto bytes=filename_bytes(event);
            if(!bytes||bytes>256||bytes>frame.events.size()-offset)return false;
            const auto* name=frame.events.data()+offset;
            if(std::memchr(name,0,bytes)!=name+bytes-1)return false;
            offset+=bytes;
        }else if(event.kind==Kind::Effect||event.kind==Kind::Stop){
            if(event.first<0||event.first>=128)return false;
            if(event.kind==Kind::Effect){
                const SoundDefinition* definitions=nullptr;
                std::memcpy(&definitions,event.payload.data(),sizeof(definitions));
                if(!definitions)return false;
            }
        }else return false;
        ++count;
    }
    return true;
}
const char* AudioEvents::FailureReason()const{
    switch(failure_){
    case Failure::None:return "none";
    case Failure::CaptureClosed:return "capture closed";
    case Failure::Capacity:return "event byte limit";
    case Failure::EffectId:return "effect id";
    case Failure::Definitions:return "sound definitions";
    case Failure::StopId:return "stop id";
    case Failure::Filename:return "music filename";
    case Failure::CommitFrame:return "commit frame";
    case Failure::FinishFrame:return "finish frame";
    case Failure::Record:return "event record";
    }
    return "unknown";
}
std::size_t AudioEvents::CapturedBytes(u32 frame)const{
    const auto& slot=frames_[frame%capacity];return slot.number==frame?slot.events.size():0;
}
u32 AudioEvents::CapturedCommands(u32 frame)const{
    const auto& slot=frames_[frame%capacity];u32 count=0;
    return slot.number==frame&&scan(slot,count)?count:0;
}
void AudioEvents::word(u32 value){for(u32 i=0;i<4;++i){digest_^=(value>>(i*8))&255u;digest_*=16777619u;}}
u32 AudioEvents::PendingFrames()const{
    u32 count=0;for(const auto& frame:frames_)count+=frame.number!=Netplay::INVALID_FRAME;return count;
}
bool AudioEvents::CommitThrough(u32 confirmed,u32 last,AudioManager& output,
                                bool (*finish_frame)(void*),void* context){
    if(Failed()||IsOpen()||output.command_sink==this)return false;
    if(confirmed==Netplay::INVALID_FRAME||last==Netplay::INVALID_FRAME)return true;
    const auto end=std::min(confirmed,last);
    while(next_commit_<=end){
        auto& slot=frames_[next_commit_%capacity];
        if(slot.number!=next_commit_||!slot.closed){fail(Failure::CommitFrame);return false;}
        // Count/validate before any output. This retains the logical-command
        // digest without adding journal-visible per-frame state.
        u32 count=0;if(!scan(slot,count)){fail(Failure::Record);return false;}
        word(next_commit_);word(count);
        std::size_t offset=0;
        for(u32 index=0;index<count;++index){
            Event event{};std::memcpy(&event,slot.events.data()+offset,sizeof(Event));
            offset+=sizeof(Event);
            word(u32(event.kind));word(u32(event.first));word(u32(event.second));
            switch(event.kind){
            case Kind::Effect:{
                const SoundDefinition* definitions=nullptr;
                std::memcpy(&definitions,event.payload.data(),sizeof(definitions));
                output.queue_effect(event.first,event.second,definitions);++effects_;break;
            }
            case Kind::Stop:output.stop_effect(event.first);++stops_;break;
            case Kind::Music:{
                const auto* filename=reinterpret_cast<const char*>(slot.events.data()+offset);
                for(const char* c=filename;*c;++c){digest_^=u8(*c);digest_*=16777619u;}
                output.queue_music(event.first,event.second,filename);++music_;
                offset+=filename_bytes(event);break;
            }
            }
        }
        if(finish_frame&&!finish_frame(context)){fail(Failure::FinishFrame);return false;}
        slot.events.clear();slot.number=Netplay::INVALID_FRAME;slot.closed=false;++next_commit_;
    }
    return true;
}
}
