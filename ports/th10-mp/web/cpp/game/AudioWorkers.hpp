#pragma once
#include "AudioDevice.hpp"
namespace th10 {
struct AudioSourceLoadingEnvironment {
    const char* const* source_names;
    virtual u8* read_wave(const char* name)=0;
    virtual void missing_wave(const char* name)=0;
};
struct AudioSourceLoadingTask {
    i32 index=0;bool done=false;
    bool advance(AudioManager& manager,AudioSourceLoadingEnvironment& environment);
};
struct AudioDeviceWorkerTask {
    AudioInitializationTask initialization;
    u32 wait_milliseconds=10;bool done=false;
    bool advance(AudioDeviceEnvironment& environment);
};
struct MusicNotificationEnvironment {
    NotifiedSoundStream** current;
    virtual bool next_message(u32& message)=0;
    virtual void fill_stream(NotifiedSoundStream& stream)=0;
};
struct MusicNotificationTask {
    bool done=false;
    bool complete_wait(u32 result,MusicNotificationEnvironment& environment);
};
}
