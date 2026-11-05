// Prospective SFX cache oracle. Real WAV input; no original executable or game oracle.
#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
#define MINIAUDIO_IMPLEMENTATION
#include "../../../portable/sdl/third_party/miniaudio.h"
#include "../../cpp/game/SoundEffects.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

static void require(bool condition,const char* message){
    if(!condition){std::fprintf(stderr,"audio-sfx-pcm: %s\n",message);std::exit(2);}
}
struct Sample{
    std::vector<unsigned char> wav;void* pcm=nullptr;ma_uint64 frames=0;
    ma_uint32 sourceRate=0,sourceChannels=0;ma_format sourceFormat=ma_format_unknown;
    ~Sample(){ma_free(pcm,nullptr);}
    Sample()=default;Sample(const Sample&)=delete;Sample& operator=(const Sample&)=delete;
};
struct Reader{
    ma_decoder decoder{};ma_audio_buffer buffer{};bool cached;
    Reader(const Sample& sample,bool useCache):cached(useCache){
        if(cached){auto c=ma_audio_buffer_config_init(ma_format_f32,2,sample.frames,sample.pcm,nullptr);c.sampleRate=44100;
            require(ma_audio_buffer_init(&c,&buffer)==MA_SUCCESS,"PCM buffer init");}
        else {auto c=ma_decoder_config_init(ma_format_f32,2,44100);
            require(ma_decoder_init_memory(sample.wav.data(),sample.wav.size(),&c,&decoder)==MA_SUCCESS,"reference decoder init");}
    }
    ~Reader(){if(cached)ma_audio_buffer_uninit(&buffer);else ma_decoder_uninit(&decoder);}
    ma_data_source* source(){return cached?static_cast<ma_data_source*>(&buffer):static_cast<ma_data_source*>(&decoder);}
};
struct Comparison{
    const char* name;ma_uint64 reads=0,values=0,differingValues=0,frameCountMismatches=0,statusMismatches=0,cursorMismatches=0;
    float maximumAbsoluteError=0;std::string first;
    explicit Comparison(const char* n):name(n){}
    void note(const std::string& where,const char* kind){
        if(first.empty())first=where+":"+kind;
    }
    void pcm(const float* a,const float* b,std::size_t n,const std::string& where){
        ++reads;values+=n;
        for(std::size_t i=0;i<n;++i){ma_uint32 x,y;std::memcpy(&x,a+i,4);std::memcpy(&y,b+i,4);
            require(std::isfinite(a[i])&&std::isfinite(b[i]),"nonfinite mixed PCM");
            if(x!=y){++differingValues;maximumAbsoluteError=std::max(maximumAbsoluteError,std::fabs(a[i]-b[i]));note(where,"pcm["+std::to_string(i)+"]");}}
    }
    void note(const std::string& where,const std::string& kind){note(where,kind.c_str());}
    bool passed()const{return !differingValues&&!frameCountMismatches&&!statusMismatches&&!cursorMismatches;}
    void print()const{
        std::printf("{\"name\":\"%s\",\"passed\":%s,\"reads\":%llu,\"comparedFloatValues\":%llu,\"differingValues\":%llu,\"frameCountMismatches\":%llu,\"statusMismatches\":%llu,\"cursorMismatches\":%llu,\"maximumAbsoluteError\":%.9g,\"firstMismatch\":\"%s\"}",
            name,passed()?"true":"false",(unsigned long long)reads,(unsigned long long)values,(unsigned long long)differingValues,
            (unsigned long long)frameCountMismatches,(unsigned long long)statusMismatches,(unsigned long long)cursorMismatches,
            double(maximumAbsoluteError),first.c_str());
    }
};
static ma_uint64 readPair(Reader& a,Reader& b,ma_uint64 frames,Comparison& check,const std::string& where){
    std::array<float,2048> x{},y{};ma_uint64 nx=0,ny=0;
    require(frames<=1024,"bounded read");
    const auto rx=ma_data_source_read_pcm_frames(a.source(),x.data(),frames,&nx);
    const auto ry=ma_data_source_read_pcm_frames(b.source(),y.data(),frames,&ny);
    require((rx==MA_SUCCESS||rx==MA_AT_END)&&(ry==MA_SUCCESS||ry==MA_AT_END),"source read error");
    if(rx!=ry){++check.statusMismatches;check.note(where,"read-status");}
    if(nx!=ny){++check.frameCountMismatches;check.note(where,"frames");}
    check.pcm(x.data(),y.data(),std::size_t(frames)*2,where);
    ma_uint64 cx=0,cy=0;require(ma_data_source_get_cursor_in_pcm_frames(a.source(),&cx)==MA_SUCCESS,"reference cursor");
    require(ma_data_source_get_cursor_in_pcm_frames(b.source(),&cy)==MA_SUCCESS,"buffer cursor");
    if(cx!=cy){++check.cursorMismatches;check.note(where,"cursor");}
    return std::max(nx,ny);
}
static void seekPair(Reader& a,Reader& b,ma_uint64 frame,Comparison& check,const std::string& where){
    const auto x=ma_data_source_seek_to_pcm_frame(a.source(),frame),y=ma_data_source_seek_to_pcm_frame(b.source(),frame);
    if(x!=y){++check.statusMismatches;check.note(where,"seek-status");}
    require((x==MA_SUCCESS||x==MA_AT_END)&&(y==MA_SUCCESS||y==MA_AT_END),"seek error");
}
struct Voice{
    Reader reader;ma_sound sound{};bool attached=false;
    Voice(ma_engine& engine,const Sample& sample,bool cached):reader(sample,cached){
        require(ma_sound_init_from_data_source(&engine,reader.source(),MA_SOUND_FLAG_NO_SPATIALIZATION|MA_SOUND_FLAG_NO_PITCH,nullptr,&sound)==MA_SUCCESS,"sound attach");attached=true;
    }
    ~Voice(){if(attached)ma_sound_uninit(&sound);}
};
struct Mixer:th09::SoundOutput{
    ma_engine engine{};std::array<std::unique_ptr<Voice>,54> voices;
    th09::SoundEffects effects{*this};
    Mixer(const std::array<Sample,39>& samples,bool cached){
        auto c=ma_engine_config_init();c.noDevice=MA_TRUE;c.channels=2;c.sampleRate=44100;c.defaultVolumeSmoothTimeInPCMFrames=0;
        require(ma_engine_init(&c,&engine)==MA_SUCCESS,"engine init");
        for(unsigned i=0;i<voices.size();++i){require(th09::sound_definitions[i].sample>=0&&th09::sound_definitions[i].sample<39,"voice mapping");
            voices[i]=std::make_unique<Voice>(engine,samples[th09::sound_definitions[i].sample],cached);effects.buffers[i]=i+1;}
    }
    ~Mixer(){for(auto& v:voices)v.reset();ma_engine_uninit(&engine);}
    ma_sound* sound(unsigned n){require(n&&n<=voices.size(),"voice index");return &voices[n-1]->sound;}
    void sound_stop(th09::u32 n)override{ma_sound_stop(sound(n));}
    void sound_position(th09::u32 n,th09::u32 frame)override{ma_sound_seek_to_pcm_frame(sound(n),frame);}
    void sound_pan(th09::u32 n,th09::i32 value)override{ma_sound_set_pan_mode(sound(n),ma_pan_mode_balance);ma_sound_set_pan(sound(n),value>=0?1.f-std::pow(10.f,-float(value)/2000.f):std::pow(10.f,float(value)/2000.f)-1.f);}
    void sound_volume(th09::u32 n,th09::i32 value)override{ma_sound_set_volume(sound(n),std::pow(10.f,float(value)/2000.f));}
    void sound_play(th09::u32 n)override{ma_sound_start(sound(n));}
    void command(unsigned step){
        if(step==0){for(unsigned i=0;i<54;++i){effects.enqueue(i,int(i%7)*650-1950);if(i%12==11)effects.process();}effects.process();}
        // Exercise real SoundEffects batching, shared-sample voice independence,
        // stop/seek(0)/pan/volume/start, saturation, silence, and retrigger.
        if(step<96&&step%3==0){effects.master_volume=step%4==0?100:63;
            for(unsigned i=0;i<12;++i){const int id=int((step*7+i*11)%54);effects.enqueue(id,int(i%5)*700-1400);}
            effects.process();}
        if(step==48)for(unsigned i=1;i<=54;i+=3)sound_stop(i);
        if(step==50)for(unsigned i=1;i<=54;i+=3)sound_play(i);
    }
    void read(std::array<float,2048>& pcm){
        ma_uint64 n=0;require(ma_engine_read_pcm_frames(&engine,pcm.data(),1024,&n)==MA_SUCCESS&&n==1024,"mixed read");
    }
};
static void mixPairs(const std::array<Sample,39>& samples,bool nonzero,Comparison& preClamp,Comparison& postClamp){
    Mixer reference(samples,false),candidate(samples,true);
    for(unsigned step=0;step<256;++step){
        reference.command(step);candidate.command(step);
        if(nonzero&&(step==16||step==47||step==75))for(unsigned i=0;i<54;++i){
            const auto frames=samples[th09::sound_definitions[i].sample].frames;
            const auto target=ma_uint64(step==16?17:step==47?frames/2:frames-1);
            reference.sound_position(i+1,th09::u32(target));candidate.sound_position(i+1,th09::u32(target));
            reference.sound_play(i+1);candidate.sound_play(i+1);
        }
        std::array<float,2048> a{},b{};reference.read(a);candidate.read(b);
        const auto where=std::string(nonzero?"mixed-nonzero/":"mixed-zero/")+std::to_string(step);
        preClamp.pcm(a.data(),b.data(),a.size(),where);
        for(auto& v:a)v=std::clamp(v,-1.f,1.f);for(auto& v:b)v=std::clamp(v,-1.f,1.f);
        postClamp.pcm(a.data(),b.data(),a.size(),where);
        for(unsigned i=1;i<=54;++i){ma_uint64 x=0,y=0;
            require(ma_sound_get_cursor_in_pcm_frames(reference.sound(i),&x)==MA_SUCCESS,"mixed reference cursor");
            require(ma_sound_get_cursor_in_pcm_frames(candidate.sound(i),&y)==MA_SUCCESS,"mixed cached cursor");
            if(x!=y){++preClamp.cursorMismatches;preClamp.note(where,"voice-cursor");}}
    }
}
int main(){
    std::array<Sample,39> samples;ma_uint64 wavBytes=0,pcmBytes=0;double decodeMs=0;unsigned rate22050=0,rate44100=0;
    for(unsigned i=0;i<39;++i){
        auto& sample=samples[i];const auto filename=std::string("/fixtures/")+th09::sound_samples[i];
        FILE* f=std::fopen(filename.c_str(),"rb");require(f!=nullptr,"actual WAV fixture missing");
        require(std::fseek(f,0,SEEK_END)==0,"fixture seek");const auto size=std::ftell(f);require(size>0,"fixture size");
        std::rewind(f);sample.wav.resize(std::size_t(size));require(std::fread(sample.wav.data(),1,sample.wav.size(),f)==sample.wav.size(),"fixture read");std::fclose(f);wavBytes+=sample.wav.size();
        ma_decoder decoder{};auto config=ma_decoder_config_init(ma_format_f32,2,44100);
        require(ma_decoder_init_memory(sample.wav.data(),sample.wav.size(),&config,&decoder)==MA_SUCCESS,"actual WAV decoder");
        require(ma_data_source_get_data_format(decoder.pBackend,&sample.sourceFormat,&sample.sourceChannels,&sample.sourceRate,nullptr,0)==MA_SUCCESS,"source format");
        ma_decoder_uninit(&decoder);rate22050+=sample.sourceRate==22050;rate44100+=sample.sourceRate==44100;
        const auto start=std::chrono::steady_clock::now();
        require(ma_decode_memory(sample.wav.data(),sample.wav.size(),&config,&sample.frames,&sample.pcm)==MA_SUCCESS&&sample.frames,"full decode");
        decodeMs+=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();pcmBytes+=sample.frames*8;
    }
    Comparison continuous("source-continuous-1024"),restart("source-seek0-retrigger"),nonzero("source-arbitrary-seek-and-tail");
    for(unsigned i=0;i<39;++i){
        const auto& s=samples[i];const auto name=std::string(th09::sound_samples[i]);
        {Reader a(s,false),b(s,true);ma_uint64 n=0;unsigned reads=0;
            do{n=readPair(a,b,1024,continuous,name+"/"+std::to_string(reads++));require(reads<4096,"bounded continuous sample");}while(n);
            readPair(a,b,1024,continuous,name+"/after-end");seekPair(a,b,0,restart,name+"/after-end-restart");
            for(unsigned k=0;k<6;++k)readPair(a,b,1024,restart,name+"/after-end-restart/"+std::to_string(k));
        }
        {Reader a(s,false),b(s,true);
            for(unsigned k=0;k<12;++k){const ma_uint64 frames=k%3==0?127:k%3==1?513:1024;
                readPair(a,b,frames,restart,name+"/prime/"+std::to_string(k));seekPair(a,b,0,restart,name+"/seek0/"+std::to_string(k));
                readPair(a,b,1024,restart,name+"/restart/"+std::to_string(k));}
        }
        const ma_uint64 positions[]={1,2,17,511,512,1023,1024,s.frames/2,s.frames/2+1,s.frames-3,s.frames-1,s.frames};
        for(const auto position:positions){Reader a(s,false),b(s,true);
            readPair(a,b,513,nonzero,name+"/seek-prime");const auto where=name+"/seek="+std::to_string(position);
            seekPair(a,b,position,nonzero,where);
            for(unsigned k=0;k<4;++k)readPair(a,b,1024,nonzero,where+"/"+std::to_string(k));
            seekPair(a,b,0,restart,name+"/nonzero-to-zero");
            for(unsigned k=0;k<2;++k)readPair(a,b,1024,restart,name+"/nonzero-to-zero/"+std::to_string(k));
        }
    }
    Comparison mixZero("mixed54-seek0-before-clamp"),mixZeroClamped("mixed54-seek0-after-clamp"),
        mixSeek("mixed54-nonzero-before-clamp"),mixSeekClamped("mixed54-nonzero-after-clamp");
    mixPairs(samples,false,mixZero,mixZeroClamped);mixPairs(samples,true,mixSeek,mixSeekClamped);
    const Comparison* all[]={&continuous,&restart,&nonzero,&mixZero,&mixZeroClamped,&mixSeek,&mixSeekClamped};
    bool passed=true;for(auto* c:all)passed=passed&&c->passed();
    std::printf("{\"passed\":%s,\"samples\":39,\"voices\":54,\"sourceRate22050\":%u,\"sourceRate44100\":%u,\"wavBytes\":%llu,\"decodedPcmBytes\":%llu,\"componentFullDecodeElapsedMs\":%.9g,\"comparisons\":[",
        passed?"true":"false",rate22050,rate44100,(unsigned long long)wavBytes,(unsigned long long)pcmBytes,decodeMs);
    for(unsigned i=0;i<7;++i){if(i)std::printf(",");all[i]->print();}
    std::printf("],\"sampleFrames\":[");for(unsigned i=0;i<39;++i){if(i)std::printf(",");std::printf("{\"name\":\"%s\",\"frames\":%llu,\"sourceRate\":%u}",th09::sound_samples[i],(unsigned long long)samples[i].frames,samples[i].sourceRate);}
    std::printf("]}\n");return passed?0:1;
}
