#include "../th10_web/cpp/multiplayer/AudioEvents.hpp"
#include <cassert>
#include <string>
#include <vector>

using namespace th10;
using namespace th10::multiplayer;
namespace {
struct Observed final:AudioCommandSink {
    std::vector<std::string> events;
    void capture_effect(i32 id,i32 pan,const SoundDefinition*) noexcept override{
        events.push_back("effect:"+std::to_string(id)+":"+std::to_string(pan));
    }
    void capture_stop(i32 id) noexcept override{events.push_back("stop:"+std::to_string(id));}
    void capture_music(i32 kind,i32 value,const char* name) override{
        events.push_back("music:"+std::to_string(kind)+":"+std::to_string(value)+":"+name);
    }
};
SoundDefinition definitions[128]{};
bool tick(void* context){++*static_cast<unsigned*>(context);return true;}
}
int main(){
    AudioEvents events;AudioManager manager{};Observed observer;unsigned ticks=0;
    manager.command_sink=&events;
    assert(events.BeginFrame(0));
    manager.queue_effect(3,123,definitions);manager.stop_effect(4);
    assert(events.EndFrame());
    assert(events.BeginFrame(1));
    manager.queue_music(2,1,"wrong.wav");assert(events.EndFrame());
    manager.command_sink=&observer;
    assert(events.CommitThrough(Netplay::INVALID_FRAME,1,manager,tick,&ticks));
    assert(observer.events.empty()&&ticks==0);
    assert(events.DiscardFrom(1));
    manager.command_sink=&events;
    assert(events.BeginFrame(1));
    char name[]="correct.wav";manager.queue_music(2,2,name);name[0]='X';
    assert(!events.CommitThrough(1,1,manager,tick,&ticks));
    assert(events.EndFrame());manager.command_sink=&observer;
    assert(events.CommitThrough(0,1,manager,tick,&ticks));
    assert(ticks==1&&observer.events.size()==2&&events.NextCommit()==1);
    assert(events.CommitThrough(1,1,manager,tick,&ticks));
    assert(ticks==2&&observer.events==std::vector<std::string>({
        "effect:3:123","stop:4","music:2:2:correct.wav"}));
    assert(events.EffectsCommitted()==1&&events.StopsCommitted()==1&&events.MusicCommitted()==1);
    const auto digest=events.Digest();
    assert(events.CommitThrough(1,1,manager,tick,&ticks));
    assert(ticks==2&&events.Digest()==digest);
    assert(!events.DiscardFrom(0)&&!events.BeginFrame(1));
    for(u32 frame=2;frame<200;++frame){
        manager.command_sink=&events;assert(events.BeginFrame(frame));
        manager.queue_effect(i32(frame%12),i32(frame),definitions);
        assert(events.EndFrame());manager.command_sink=&observer;
        assert(events.CommitThrough(frame,frame,manager,tick,&ticks));
    }
    assert(ticks==200&&events.NextCommit()==200&&events.PendingFrames()==0);
    events.Reset();manager.command_sink=&events;
    for(u32 frame=0;frame<AudioEvents::capacity;++frame){
        assert(events.BeginFrame(frame));assert(events.EndFrame());
    }
    assert(!events.BeginFrame(u32(AudioEvents::capacity)));
    assert(events.DiscardFrom(0));assert(events.BeginFrame(0));
    for(u32 i=0;i<=AudioEvents::events_per_frame;++i)manager.stop_effect(1);
    assert(events.Failed()&&!events.EndFrame());
    manager.command_sink=&observer;
    assert(!events.CommitThrough(0,0,manager,tick,&ticks));
    events.Reset();manager.command_sink=&events;assert(events.BeginFrame(0));
    char long_name[257]{};for(u32 i=0;i<256;++i)long_name[i]='a';
    manager.queue_music(2,0,long_name);assert(events.Failed());
}
