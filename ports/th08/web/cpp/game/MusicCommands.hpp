#pragma once
#include "WaveAudio.hpp"
namespace th08 {
struct MusicCommand {i32 opcode=0,arg=0,step=0;char text[256]{};};
static_assert(sizeof(MusicCommand)==268);
struct MusicContext {
    bool initialized=true,preload=true,wav=true,has_music=false,locked=false,has_thread=false;
    i32 master_volume=100,total_length=0;
};
struct MusicActions {
    virtual ~MusicActions()=default;
    virtual void set_volume(i32)=0;
    virtual void stop_all()=0;
    virtual i32 preload(i32 slot,const char* path)=0;
    virtual i32 load(i32 slot)=0;
    virtual i32 reset()=0;
    virtual i32 fill(bool loop)=0;
    virtual void play()=0;
    virtual void stop()=0;
    virtual void recreate_buffers()=0;
    virtual void reopen(const char* path)=0;
    virtual void request_thread_stop()=0;
    virtual bool thread_stopped()=0;
    virtual void close_music()=0;
    virtual void fade_out(float seconds)=0;
    virtual void pause()=0;
    virtual void unpause()=0;
};
class MusicCommands {
public:
    explicit MusicCommands(MusicActions& actions):actions(actions){}
    MusicCommand commands[32]{};
    char filenames[16][256]{};
    MusicContext context;
    bool enqueue(i32 opcode,i32 arg,const char* text);
    i32 process();
private:
    MusicActions& actions;
};
}
