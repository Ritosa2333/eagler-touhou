// Scalar vs standard Wasm SIMD gate. No PCM cache or decoder replacement.
// This translation unit owns the same miniaudio/stb implementation as AudioDevice.
#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
#include "../../../portable/sdl/third_party/stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "../../../portable/sdl/third_party/miniaudio.h"
#include "../../cpp/game/SoundEffects.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#if defined(MA_SUPPORT_SSE2) || defined(MA_SUPPORT_NEON)
#error This gate must retain the generic miniaudio implementation.
#endif
#define EXPORT(name) extern "C" __attribute__((export_name(name)))
namespace {
void require(bool ok,const char* why){if(!ok){std::fprintf(stderr,"audio-simd gate: %s\n",why);std::abort();}}
std::vector<unsigned char> file(const std::string& name){
    FILE* f=std::fopen(name.c_str(),"rb");require(f,"fixture open");
    require(!std::fseek(f,0,SEEK_END),"fixture end");const long n=std::ftell(f);require(n>0,"fixture size");std::rewind(f);
    std::vector<unsigned char> bytes(std::size_t(n),0);require(std::fread(bytes.data(),1,bytes.size(),f)==bytes.size(),"fixture read");std::fclose(f);return bytes;
}
unsigned word(const unsigned char* p){unsigned v;std::memcpy(&v,p,4);return v;}
unsigned shortword(const unsigned char* p){unsigned short v;std::memcpy(&v,p,2);return v;}
float volume(int db){return std::pow(10.f,float(db)/2000.f);}
struct Music {std::string path;unsigned channels,rate,total,intro;};
std::array<std::vector<unsigned char>,39> samples;
std::vector<Music> tracks;
bool loaded=false;
struct Voice {
    ma_decoder decoder{};ma_sound sound{};bool decoded=false,attached=false;ma_uint64 length=0;
    Voice(unsigned sample){
        const auto& bytes=samples.at(sample);auto c=ma_decoder_config_init(ma_format_f32,2,44100);
        require(ma_decoder_init_memory(bytes.data(),bytes.size(),&c,&decoder)==MA_SUCCESS,"WAV decoder");
        decoded=true;require(ma_decoder_get_length_in_pcm_frames(&decoder,&length)==MA_SUCCESS,"WAV length");
    }
    Voice(const Music& m){
        auto c=ma_decoder_config_init(ma_format_f32,m.channels,m.rate);
        require(ma_decoder_init_file(m.path.c_str(),&c,&decoder)==MA_SUCCESS,"OGG pull decoder");decoded=true;
        require(ma_decoder_get_length_in_pcm_frames(&decoder,&length)==MA_SUCCESS&&length==m.total,"OGG exact length");
        require(ma_data_source_set_loop_point_in_pcm_frames(&decoder,m.intro,m.total)==MA_SUCCESS,"OGG loop points");
    }
    void attach(ma_engine& engine){
        require(ma_sound_init_from_data_source(&engine,&decoder,MA_SOUND_FLAG_NO_SPATIALIZATION|MA_SOUND_FLAG_NO_PITCH,nullptr,&sound)==MA_SUCCESS,"sound attach");
        attached=true;
    }
    ~Voice(){if(attached)ma_sound_uninit(&sound);if(decoded)ma_decoder_uninit(&decoder);}
};
struct Mixer:th09::SoundOutput {
    ma_engine engine{};std::array<std::unique_ptr<Voice>,54> voices;std::unique_ptr<Voice> music;
    th09::SoundEffects effects{*this};bool hasSfx;
    Mixer(bool sfx,int musicIndex):hasSfx(sfx){
        auto c=ma_engine_config_init();c.noDevice=MA_TRUE;c.channels=2;c.sampleRate=44100;c.defaultVolumeSmoothTimeInPCMFrames=0;
        require(ma_engine_init(&c,&engine)==MA_SUCCESS,"engine init");
        if(sfx)for(unsigned i=0;i<54;++i){voices[i]=std::make_unique<Voice>(unsigned(th09::sound_definitions[i].sample));voices[i]->attach(engine);effects.buffers[i]=i+1;}
        if(musicIndex>=0){music=std::make_unique<Voice>(tracks.at(unsigned(musicIndex)));music->attach(engine);ma_sound_set_looping(&music->sound,MA_TRUE);
            ma_sound_set_volume(&music->sound,volume(th09::SoundEffects::adjusted_volume(0,100,true)));ma_sound_start(&music->sound);}
    }
    ~Mixer(){music.reset();for(auto& v:voices)v.reset();ma_engine_uninit(&engine);}
    ma_sound* sound(unsigned n){require(n&&n<=54&&voices[n-1],"voice index");return &voices[n-1]->sound;}
    void sound_stop(th09::u32 n)override{ma_sound_stop(sound(n));}
    void sound_position(th09::u32 n,th09::u32 frame)override{ma_sound_seek_to_pcm_frame(sound(n),frame);}
    void sound_pan(th09::u32 n,th09::i32 value)override{ma_sound_set_pan_mode(sound(n),ma_pan_mode_balance);ma_sound_set_pan(sound(n),value>=0?1.f-std::pow(10.f,-float(value)/2000.f):std::pow(10.f,float(value)/2000.f)-1.f);}
    void sound_volume(th09::u32 n,th09::i32 value)override{ma_sound_set_volume(sound(n),volume(value));}
    void sound_play(th09::u32 n)override{ma_sound_start(sound(n));}
    void command(unsigned step,bool nonzero){
        require(hasSfx,"SFX command in music-only case");
        if(step==0){for(unsigned i=0;i<54;++i){effects.enqueue(i,int(i%7)*650-1950);if(i%12==11)effects.process();}effects.process();}
        if(step<96&&step%3==0){effects.master_volume=step%4==0?100:63;
            for(unsigned i=0;i<12;++i)effects.enqueue(int((step*7+i*11)%54),int(i%5)*700-1400);effects.process();}
        if(step==48)for(unsigned i=1;i<=54;i+=3)sound_stop(i);
        if(step==50)for(unsigned i=1;i<=54;i+=3)sound_play(i);
        if(nonzero&&(step==16||step==47||step==75))for(unsigned i=0;i<54;++i){
            const auto length=voices[i]->length;const auto target=step==16?17:step==47?length/2:length-1;
            sound_position(i+1,unsigned(target));sound_play(i+1);
        }
    }
};
std::unique_ptr<Voice> reader;
std::unique_ptr<Mixer> mixer;
std::array<float,2048> pcm{};
std::array<unsigned,4096> state{};
unsigned stateWords=0,mode=0,caseIndex=0,requested=1024;
ma_uint64 framesRead=0;
ma_result lastResult=MA_SUCCESS;
void push(unsigned x){require(stateWords<state.size(),"state capacity");state[stateWords++]=x;}
void push64(ma_uint64 x){push(unsigned(x));push(unsigned(x>>32));}
void pushFloat(float f){unsigned u;std::memcpy(&u,&f,4);push(u);}
void observeVoice(Voice& v){
    ma_uint64 cursor=0,length=0;
    push(unsigned(ma_decoder_get_cursor_in_pcm_frames(&v.decoder,&cursor)));push64(cursor);
    push(unsigned(ma_decoder_get_length_in_pcm_frames(&v.decoder,&length)));push64(length);
    ma_format format;ma_uint32 channels=0,rate=0;
    push(unsigned(ma_decoder_get_data_format(&v.decoder,&format,&channels,&rate,nullptr,0)));push(unsigned(format));push(channels);push(rate);
    push(v.attached);
    if(v.attached){ma_uint64 sc=0;push(unsigned(ma_sound_get_cursor_in_pcm_frames(&v.sound,&sc)));push64(sc);
        push(ma_sound_is_playing(&v.sound));push(ma_sound_at_end(&v.sound));push(ma_sound_is_looping(&v.sound));
        pushFloat(ma_sound_get_volume(&v.sound));pushFloat(ma_sound_get_pan(&v.sound));pushFloat(ma_sound_get_pitch(&v.sound));}
}
}
EXPORT("gate_compiled_simd") unsigned compiledSimd(){
#ifdef __wasm_simd128__
    return 1;
#else
    return 0;
#endif
}
EXPORT("gate_init") unsigned initialize(){
    require(!loaded,"init only once");
    for(unsigned i=0;i<39;++i)samples[i]=file(std::string("/fixtures/")+th09::sound_samples[i]);
    const auto fmt=file("/fixtures/thbgm.fmt");
    // AudioDevice scans complete 52-byte records. The real fixture also has
    // a 17-byte zero tail; retain it and validate, rather than invent a record.
    const auto completeBytes=fmt.size()/52*52;
    for(auto n=completeBytes;n<fmt.size();++n)require(fmt[n]==0,"nonzero music layout tail");
    for(unsigned n=0;n+52<=fmt.size();n+=52){const auto* p=fmt.data()+n;char name[17]{};std::memcpy(name,p,16);
        std::string stem(name);require(stem.size()>4&&stem.substr(stem.size()-4)==".wav","music name");stem.resize(stem.size()-4);
        const unsigned align=shortword(p+44);require(align&&word(p+36)&&shortword(p+34),"music format");
        tracks.push_back({std::string("/music/")+stem+".ogg",shortword(p+34),word(p+36),word(p+28)/align,word(p+24)/align});
    }
    require(tracks.size()==19,"actual music count");loaded=true;return 0;
}
// 0: standalone WAV, 1: standalone OGG, 2: 54 SFX plus optional music, 3: music-only engine.
EXPORT("gate_open") unsigned openCase(unsigned kind,int which){
    require(loaded,"not initialized");reader.reset();mixer.reset();mode=kind;caseIndex=unsigned(which);lastResult=MA_SUCCESS;framesRead=0;
    if(kind==0)reader=std::make_unique<Voice>(unsigned(which));
    else if(kind==1)reader=std::make_unique<Voice>(tracks.at(unsigned(which)));
    else if(kind==2||kind==3)mixer=std::make_unique<Mixer>(kind==2,which);
    else require(false,"invalid case kind");
    return 0;
}
EXPORT("gate_seek") int seekFrame(unsigned frame){
    require(reader!=nullptr,"standalone seek");lastResult=ma_decoder_seek_to_pcm_frame(&reader->decoder,frame);return lastResult;
}
EXPORT("gate_music_action") int musicAction(unsigned action,int value){
    require(mixer&&mixer->music,"music action without voice");auto& s=mixer->music->sound;
    switch(action){
    case 0:lastResult=ma_sound_stop(&s);break;
    case 1:lastResult=ma_sound_start(&s);break;
    case 2:lastResult=ma_sound_seek_to_pcm_frame(&s,unsigned(value));break;
    case 3:ma_sound_set_volume(&s,volume(value));lastResult=MA_SUCCESS;break;
    case 4:ma_sound_set_pan_mode(&s,ma_pan_mode_balance);ma_sound_set_pan(&s,float(value)/1000.f);lastResult=MA_SUCCESS;break;
    default:require(false,"music action");
    }return lastResult;
}
EXPORT("gate_command") unsigned command(unsigned step,unsigned nonzero){require(mixer!=nullptr,"mixer command");mixer->command(step,nonzero!=0);return 0;}
EXPORT("gate_prepare") unsigned prepare(unsigned frames){require(frames&&frames<=1024,"bounded read");requested=frames;pcm.fill(0.f);framesRead=0;lastResult=MA_SUCCESS;return 0;}
EXPORT("gate_read") int readFrames(){
    if(reader)lastResult=ma_decoder_read_pcm_frames(&reader->decoder,pcm.data(),requested,&framesRead);
    else {require(mixer!=nullptr,"read without source");lastResult=ma_engine_read_pcm_frames(&mixer->engine,pcm.data(),requested,&framesRead);}
    return lastResult;
}
EXPORT("gate_clamp") unsigned clampFrames(){for(auto& value:pcm)value=std::clamp(value,-1.f,1.f);return 0;}
EXPORT("gate_pcm") const float* pcmData(){return pcm.data();}
EXPORT("gate_clamped") const float* clampedData(){return pcm.data();}
EXPORT("gate_frames") unsigned readCount(){return unsigned(framesRead);}
EXPORT("gate_length") unsigned length(){return reader?unsigned(reader->length):mixer&&mixer->music?unsigned(mixer->music->length):0;}
EXPORT("gate_cursor") unsigned cursor(){ma_uint64 value=0;require(reader||(mixer&&mixer->music),"cursor source");
    const auto result=reader?ma_decoder_get_cursor_in_pcm_frames(&reader->decoder,&value):ma_sound_get_cursor_in_pcm_frames(&mixer->music->sound,&value);
    require(result==MA_SUCCESS,"cursor query");return unsigned(value);}
EXPORT("gate_at_end") int atEnd(){return MA_AT_END;}
EXPORT("gate_observe") unsigned observe(){
    stateWords=0;push(1);push(mode);push(caseIndex);push(unsigned(lastResult));push64(framesRead);push(requested);
    if(reader)observeVoice(*reader);
    if(mixer){push64(ma_engine_get_time_in_pcm_frames(&mixer->engine));push(mixer->hasSfx);push(mixer->music!=nullptr);
        if(mixer->music)observeVoice(*mixer->music);
        if(mixer->hasSfx){for(auto& v:mixer->voices)observeVoice(*v);
            const auto& e=mixer->effects;push(e.initialized);push(e.enabled);push(unsigned(e.master_volume));
            for(auto v:e.buffers)push(v);for(auto v:e.state.metadata)push(unsigned(v));for(auto v:e.state.indices)push(unsigned(v));
            for(auto v:e.state.counts)push(unsigned(v));for(const auto& row:e.state.pans)for(auto v:row)push(unsigned(v));}
    }return stateWords;
}
EXPORT("gate_state") const unsigned* stateData(){return state.data();}
EXPORT("gate_close") unsigned closeCase(){reader.reset();mixer.reset();return 0;}
