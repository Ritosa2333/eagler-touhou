#include "AudioWorkers.hpp"
namespace th10 {
bool AudioSourceLoadingTask::advance(AudioManager& manager,AudioSourceLoadingEnvironment& env){
    if(done)return true;
    while(index<37){
        if(manager.load_stop==2){done=true;return true;}
        auto* bytes=env.read_wave(env.source_names[index]);manager.pending_waves[index]=bytes;
        if(!bytes){env.missing_wave(env.source_names[index]);done=true;return true;}++index;
    }
    done=manager.load_stop!=0;return done;
}
bool AudioDeviceWorkerTask::advance(AudioDeviceEnvironment& env){
    if(done)return true;auto& manager=*env.global_manager;
    if(!initialization.advance(manager,manager.startup_window,env)){wait_milliseconds=10;return false;}
    if(!manager.load_stop){wait_milliseconds=1;return false;}
    manager.loading_done=1;done=true;return true;
}
bool MusicNotificationTask::complete_wait(u32 result,MusicNotificationEnvironment& env){
    auto* stream=*env.current;if(!stream)done=true;
    if(result==0){if(stream&&stream->active){stream->notification_busy=1;env.fill_stream(**env.current);(*env.current)->notification_busy=0;}}
    else if(result==1){u32 message;while(env.next_message(message))if(message==0x12)done=true;}
    return done;
}
}
