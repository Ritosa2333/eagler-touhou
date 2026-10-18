#pragma once
#include "../game/GameApplication.hpp"
#include "../game/WaveAudio.hpp"
namespace th08 {
// Game-facing device interfaces. Operation numbers and machine-word payloads
// belong only to LegacyDevices.cpp, used by the original comparison harness.
struct ZunGraphics
#ifdef TH_NATIVE_PLATFORM
 : StateCommands
#endif
{
    virtual ~ZunGraphics()=default;
    virtual void prepare_texture(u32){}
    virtual bool resample(u32,const TextureRect&,u32,const TextureRect&,bool){return false;}
    virtual void flush()=0;virtual void texture(u32)=0;
#ifndef TH_NATIVE_PLATFORM
    virtual void render_state(u32,u32)=0;virtual void stage_state(u32,u32)=0;
#endif
    virtual void transform(MatrixParameter,const Matrix4&)=0;virtual void viewport(const Viewport&)=0;
    virtual void draw(Primitive,VertexFormat,const void*,u32)=0;
    virtual void clear(u32,u32,float,u32)=0;virtual bool present(u32)=0;
    virtual void read(u32)=0;virtual void discard()=0;
    virtual void copy(u32,const TextureRect&,u32,i32,i32)=0;
};
struct SoundDevice {
    virtual ~SoundDevice()=default;
    virtual i32 create_pcm(u32,const WaveFormat&,const u8*,u32)=0;
    virtual i32 music_format(u32,const BgmFormat&,bool)=0;
    virtual i32 stop(u32)=0;virtual i32 position(u32,u32)=0;
    virtual i32 pan(u32,i32)=0;virtual i32 volume(u32,i32)=0;
    virtual i32 play(u32,u32,u32)=0;virtual void release(u32)=0;
    virtual void midi_open()=0;virtual void midi_close(u32)=0;
    virtual void midi_message(const u8*,u32,u32)=0;
};
struct FileDevice {
    virtual ~FileDevice()=default;
    virtual bool save(const char*,const u8*,u32)=0;
    virtual void calendar(char*,char*)=0;virtual u32 milliseconds()=0;
    virtual u16 supplemental_input()=0;virtual void replay_error()=0;
};
ZunGraphics& graphics_device();SoundDevice& sound_device();FileDevice& file_device();
}
