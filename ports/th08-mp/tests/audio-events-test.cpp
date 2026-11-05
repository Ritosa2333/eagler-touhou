#include "AudioEvents.hpp"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace th08;
using namespace th08::multiplayer;
struct Output:AudioEventOutput {
    std::vector<AudioEvent> events;u32 reject=~u32(0);
    bool apply_audio_event(const AudioEvent& event)override{
        if(events.size()==reject)return false;events.push_back(event);return true;
    }
};
AudioEvent event(AudioEventKind kind,i32 first=0,const char* path=""){
    AudioEvent result;result.kind=kind;result.first=first;std::strcpy(result.text,path);return result;
}
void record(AudioEvents& queue,u32 frame,const AudioEvent& value){
    assert(queue.BeginFrame(frame));assert(queue.Append(value));assert(queue.EndFrame());
}
int main(){
    AudioEvents queue;queue.Reset();Output output;
    record(queue,0,event(AudioEventKind::Sound,14));
    record(queue,1,event(AudioEventKind::WaveCommand,2,"wrong.wav"));
    const auto discarded=queue.FrameDigest(1);
    assert(queue.CommitThrough(AudioEvents::invalid_frame,1,output)&&output.events.empty());
    assert(queue.CommitThrough(0,1,output)&&output.events.size()==1);
    assert(!queue.DiscardFrom(0)&&queue.NextCommit()==1&&!queue.Failed());
    assert(queue.DiscardFrom(1));
    record(queue,1,event(AudioEventKind::WaveCommand,2,"corrected.wav"));
    assert(queue.FrameDigest(1)!=discarded);
    record(queue,2,event(AudioEventKind::MidiStop));
    record(queue,3,event(AudioEventKind::MidiFile,0,"corrected.mid"));
    record(queue,4,event(AudioEventKind::MidiStart));
    assert(queue.CommitThrough(4,3,output)&&queue.NextCommit()==4&&output.events.size()==4);
    assert(!std::strcmp(output.events[1].text,"corrected.wav"));
    assert(output.events[2].kind==AudioEventKind::MidiStop&&output.events[3].kind==AudioEventKind::MidiFile);
    assert(queue.CommitThrough(4,4,output)&&output.events.back().kind==AudioEventKind::MidiStart);
    const auto digest=queue.CommittedDigest();
    assert(queue.CommitThrough(4,4,output)&&output.events.size()==5&&queue.CommittedDigest()==digest);
    assert(queue.PendingFrames()==0&&queue.CommittedEvents()==5);
    queue.Reset(100);
    for(u32 frame=100;frame<100+AudioEvents::capacity;++frame)record(queue,frame,event(AudioEventKind::TickFades));
    assert(!queue.BeginFrame(100+AudioEvents::capacity)&&queue.Failed());
    queue.Reset();assert(queue.BeginFrame(0));
    for(u32 i=0;i<AudioEvents::events_per_frame;++i)assert(queue.Append(event(AudioEventKind::Sound,1)));
    assert(!queue.Append(event(AudioEventKind::Sound,2))&&queue.Failed());
    queue.Reset();assert(queue.BeginFrame(0));auto invalid=event(AudioEventKind::Fade);
    invalid.value=std::numeric_limits<float>::quiet_NaN();assert(!queue.Append(invalid));
    queue.Reset();assert(queue.BeginFrame(0));invalid=event(AudioEventKind::WaveCommand);
    std::memset(invalid.text,'x',sizeof(invalid.text));assert(!queue.Append(invalid));
    queue.Reset();record(queue,0,event(AudioEventKind::Sound,1));
    assert(!queue.CommitThrough(1,1,output));
    queue.Reset();Output broken;broken.reject=0;record(queue,0,event(AudioEventKind::Sound,1));
    assert(!queue.CommitThrough(0,0,broken)&&queue.Failed()&&broken.events.empty());
    assert(!queue.CommitThrough(0,0,broken));
    queue.Reset();broken.reject=1;assert(queue.BeginFrame(0));
    assert(queue.Append(event(AudioEventKind::Sound,1))&&queue.Append(event(AudioEventKind::Sound,2))&&queue.EndFrame());
    assert(!queue.CommitThrough(0,0,broken)&&broken.events.size()==1);
    assert(!queue.CommitThrough(0,0,broken)&&broken.events.size()==1);
    std::puts("TH08 confirmed audio event ordering, replacement, bounds and failure fences: PASS");
}
