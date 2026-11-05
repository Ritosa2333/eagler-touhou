#pragma once
#include "../game/GameApplication.hpp"
#include "../game/WaveAudio.hpp"
#ifndef TH_NATIVE_PLATFORM
namespace th08 {
// Synchronous device calls consume their pointed-to bytes before returning.
// The browser owns WebGL/WebAudio/IndexedDB objects, never game state machines.
enum class BrowserCall:u32 {Flush=1,Texture,RenderState,StageState,Transform,Viewport,Draw,Clear,Present,Readback,Copy,Discard,
    Save=20,Calendar,Milliseconds,Input,Error,
    Pcm=40,MusicFormat,AudioStop,AudioPosition,AudioPan,AudioVolume,AudioPlay,AudioRelease,
    MidiOpen=60,MidiClose,MidiData};
struct BrowserDraw {u32 primitive,format,vertices,count;};
struct BrowserClear {u32 flags,color;float depth;u32 stencil;};
struct BrowserCopy {u32 source,target;TextureRect rect;i32 x,y;};
struct BrowserPcm {u32 handle,data,size;WaveFormat format;};
}
#endif
