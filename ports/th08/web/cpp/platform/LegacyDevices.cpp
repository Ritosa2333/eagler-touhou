#include "LegacyDeviceAbi.hpp"
#ifndef TH_NATIVE_PLATFORM
#include "PlatformDevices.hpp"
#include "BrowserRuntime.hpp"
namespace th08 {
extern "C" __attribute__((import_module("th08_device"),import_name("call"))) i32 browser_call(u32,u32,u32,u32);
namespace {
u32 address(const void* p){return u32(reinterpret_cast<uintptr_t>(p));}
i32 call(BrowserCall op,u32 a=0,u32 b=0,u32 c=0){return browser_call(u32(op),a,b,c);}
#ifndef TH_SDL3
struct LegacyGraphics final:ZunGraphics {
 void flush()override{call(BrowserCall::Flush);}void texture(u32 h)override{call(BrowserCall::Texture,h);}
 void render_state(u32 key,u32 value)override{call(BrowserCall::RenderState,key,value);}void stage_state(u32 key,u32 value)override{call(BrowserCall::StageState,key,value);}
 void transform(u32 kind,const Matrix4& matrix)override{call(BrowserCall::Transform,kind,address(&matrix));}void viewport(const Viewport& view)override{call(BrowserCall::Viewport,address(&view));}
 void draw(Primitive p,VertexFormat f,const void* v,u32 n)override{const BrowserDraw d{u32(p),u32(f),address(v),n};call(BrowserCall::Draw,address(&d));}
 void clear(u32 flags,u32 color,float depth,u32 stencil)override{const BrowserClear c{flags,color,depth,stencil};call(BrowserCall::Clear,address(&c));}
 bool present(u32 h)override{return call(BrowserCall::Present,h)==0;}void read(u32 h)override{call(BrowserCall::Readback,h);}void discard()override{call(BrowserCall::Discard);}
 void copy(u32 source,const TextureRect& rect,u32 target,i32 x,i32 y)override{const BrowserCopy c{source,target,rect,x,y};call(BrowserCall::Copy,address(&c));}
};
#endif
struct LegacySound final:SoundDevice {
 i32 create_pcm(u32 h,const WaveFormat& f,const u8* p,u32 n)override{const BrowserPcm pcm{h,address(p),n,f};return call(BrowserCall::Pcm,address(&pcm));}
 i32 music_format(u32 h,const BgmFormat& f,bool loop)override{return call(BrowserCall::MusicFormat,h,address(&f),loop);}
 i32 stop(u32 h)override{return call(BrowserCall::AudioStop,h);}i32 position(u32 h,u32 n)override{return call(BrowserCall::AudioPosition,h,n);}
 i32 pan(u32 h,i32 n)override{return call(BrowserCall::AudioPan,h,u32(n));}i32 volume(u32 h,i32 n)override{return call(BrowserCall::AudioVolume,h,u32(n));}
 i32 play(u32 h,u32 n,u32 flags)override{return call(BrowserCall::AudioPlay,h,n,flags);}void release(u32 h)override{call(BrowserCall::AudioRelease,h);}
 void midi_open()override{call(BrowserCall::MidiOpen);}void midi_close(u32 time)override{call(BrowserCall::MidiClose,0,0,time);}void midi_message(const u8* p,u32 n,u32 time)override{call(BrowserCall::MidiData,address(p),n,time);}
};
struct LegacyFiles final:FileDevice {
 bool save(const char* p,const u8* bytes,u32 n)override{return call(BrowserCall::Save,address(p),address(bytes),n)==0;}
 void calendar(char* date,char* stamp)override{call(BrowserCall::Calendar,address(date),address(stamp));}
 u32 milliseconds()override{return call(BrowserCall::Milliseconds);}u16 supplemental_input()override{return call(BrowserCall::Input);}
 void replay_error()override{call(BrowserCall::Error,1);}
};
}
#ifndef TH_SDL3
ZunGraphics& graphics_device(){static LegacyGraphics graphics;return graphics;}
#endif
SoundDevice& sound_device(){static LegacySound sound;return sound;}
FileDevice& file_device(){static LegacyFiles files;return files;}
}
#endif
