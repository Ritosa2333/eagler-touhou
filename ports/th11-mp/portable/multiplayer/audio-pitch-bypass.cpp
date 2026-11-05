// Independent O2 PCM oracle; never calls SDL or changes production AudioDevice.
// Uses the production Archive reader, sound tables, stb_vorbis and pinned miniaudio.
// Original vs candidate differs only by MA_SOUND_FLAG_NO_PITCH at attachment.
#define MA_NO_DEVICE_IO
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_ENCODING
#define MA_NO_THREADING
#include "../sdl/third_party/stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "../sdl/third_party/miniaudio.h"
#include "../../web/cpp/game/Archive.hpp"
#include "../../web/cpp/game/SoundEffects.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

namespace {
using namespace th11;
unsigned checks=0,cases=0;
void require(bool okay,const char* what){++checks;if(!okay){std::fprintf(stderr,"FAIL %u: %s\n",checks,what);std::exit(1);}}
u32 word(const u8* p){return u32(p[0])|(u32(p[1])<<8)|(u32(p[2])<<16)|(u32(p[3])<<24);}
u16 half(const u8* p){return u16(p[0])|u16(p[1])<<8;}
std::vector<u8> file(const std::string& path){
    auto* f=std::fopen(path.c_str(),"rb");require(f!=nullptr,("open "+path).c_str());
    require(std::fseek(f,0,SEEK_END)==0,"seek file end");const long length=std::ftell(f);
    require(length>0&&length<=256*1024*1024,"bounded resource file");std::rewind(f);
    std::vector<u8> bytes(static_cast<size_t>(length));require(std::fread(bytes.data(),1,bytes.size(),f)==bytes.size(),"read resource");std::fclose(f);return bytes;
}
void raw_file(const std::string& path,const std::vector<float>& pcm){
    auto* f=std::fopen(path.c_str(),"wb");require(f!=nullptr,"create PCM artifact");
    require(std::fwrite(pcm.data(),sizeof(float),pcm.size(),f)==pcm.size(),"write PCM artifact");require(std::fclose(f)==0,"close PCM artifact");
}
std::vector<u8> resource(Archive& archive,const char* name){
    for(u32 n=0;n<archive.entries.size();++n)if(archive.entries[n].name==name){std::vector<u8> out;require(archive.read(n,out),"decode archive resource");return out;}
    require(false,("missing archive entry "+std::string(name)).c_str());return {};
}
float gain(i32 value){return std::pow(10.f,float(value)/2000.f);}
float pan_value(i32 value){return value>=0?1.f-std::pow(10.f,-float(value)/2000.f):std::pow(10.f,float(value)/2000.f)-1.f;}
struct Voice {
    ma_decoder decoder{};ma_sound sound{};bool decoded=false,attached=false;ma_uint64 frames=0;
    ~Voice(){if(attached)ma_sound_uninit(&sound);if(decoded)ma_decoder_uninit(&decoder);}
};
struct Engine {
    ma_engine value{};bool initialized=false;bool bypass=false;double mix_ms=0;
    std::vector<std::unique_ptr<Voice>> voices;
    explicit Engine(bool flag):bypass(flag){
        auto config=ma_engine_config_init();config.noDevice=MA_TRUE;config.channels=2;config.sampleRate=44100;config.defaultVolumeSmoothTimeInPCMFrames=0;
        require(ma_engine_init(&config,&value)==MA_SUCCESS,"initialize real noDevice engine");initialized=true;
    }
    ~Engine(){voices.clear();if(initialized)ma_engine_uninit(&value);}
    Voice& add(const std::vector<u8>& bytes,u32 rate,u32 channels,bool loop,i32 db=0,i32 pan=0){
        auto voice=std::make_unique<Voice>();auto config=ma_decoder_config_init(ma_format_f32,channels,rate);
        require(ma_decoder_init_memory(bytes.data(),bytes.size(),&config,&voice->decoder)==MA_SUCCESS,"initialize real decoder");voice->decoded=true;
        require(ma_decoder_get_length_in_pcm_frames(&voice->decoder,&voice->frames)==MA_SUCCESS&&voice->frames>0,"decoded PCM length");
        const u32 flags=MA_SOUND_FLAG_NO_SPATIALIZATION|(bypass?MA_SOUND_FLAG_NO_PITCH:0);
        require(ma_sound_init_from_data_source(&value,&voice->decoder,flags,nullptr,&voice->sound)==MA_SUCCESS,"attach voice");voice->attached=true;
        // Verify the actual library branch, including the automatic unequal-rate override.
        require(bool(voice->sound.engineNode.isPitchDisabled)==(bypass&&rate==44100),"NO_PITCH versus required rate conversion");
        ma_sound_set_looping(&voice->sound,loop?MA_TRUE:MA_FALSE);
        ma_sound_set_pan_mode(&voice->sound,ma_pan_mode_balance);ma_sound_set_pan(&voice->sound,pan_value(pan));
        ma_sound_set_volume(&voice->sound,gain(db));voices.push_back(std::move(voice));return *voices.back();
    }
    void start(){for(auto& v:voices)require(ma_sound_start(&v->sound)==MA_SUCCESS,"start voice");}
    void stop(){for(auto& v:voices)require(ma_sound_stop(&v->sound)==MA_SUCCESS,"stop voice");}
    void seek(ma_uint64 at){for(auto& v:voices)require(ma_sound_seek_to_pcm_frame(&v->sound,at)==MA_SUCCESS,"seek voice");}
    void mix(u32 frames,std::vector<float>& target){
        const auto offset=target.size();target.resize(offset+size_t(frames)*2);ma_uint64 read=0;
        const auto begin=std::chrono::steady_clock::now();
        const auto result=ma_engine_read_pcm_frames(&value,target.data()+offset,frames,&read);
        mix_ms+=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count();
        require(result==MA_SUCCESS&&read==frames,"complete native PCM block");
        for(size_t i=offset;i<target.size();++i)require(std::isfinite(target[i]),"finite unclipped mixer PCM");
        // Production clamp is deliberately applied only after checking the raw mixer.
        for(size_t i=offset;i<target.size();++i)target[i]=std::clamp(target[i],-1.f,1.f);
    }
};
struct Segment {size_t first=0,frames=0;bool playing=true;std::array<float,2> expected_first{};};
struct Compare {
    std::string name,out;Engine old{false},candidate{true};
    std::vector<float> a,b;std::vector<Segment> segments;std::array<float,2> history{};
    bool equal_rate=true,natural_end=false;u64 first_mismatch=~u64(0),last_mismatch=0;
    explicit Compare(std::string n,const std::string& output):name(std::move(n)),out(output){}
    void block(u32 count,bool playing=true){
        Segment segment;segment.first=a.size()/2;segment.frames=count;segment.playing=playing;segment.expected_first=playing?history:std::array<float,2>{};
        // Alternate order so the timing receipt does not always favor the second engine.
        if(segments.size()%2){candidate.mix(count,b);old.mix(count,a);}else{old.mix(count,a);candidate.mix(count,b);}
        if(playing)history={b[b.size()-2],b.back()};segments.push_back(segment);
    }
    void report(){
        require(a.size()==b.size()&&!a.empty(),"equal PCM output lengths");
        u64 raw_different=0,bit_different=0,model_different=0,eof_cut_samples=0;double peak=0,ea=0,eb=0;
        long first_a=-1,first_b=-1;size_t final_nonzero=0;bool any_nonzero=false;
        for(size_t n=0;n<a.size();++n)if(a[n]!=0||b[n]!=0){final_nonzero=n/2;any_nonzero=true;}
        FILE* differences=std::fopen((out+"/"+name+".differences.jsonl").c_str(),"wb");require(differences!=nullptr,"create sample differences");
        for(size_t n=0;n<a.size();++n){
            require(std::isfinite(a[n])&&std::isfinite(b[n]),"finite retained PCM");ea+=double(a[n])*a[n];eb+=double(b[n])*b[n];
            if(first_a<0&&a[n]!=0)first_a=long(n/2);if(first_b<0&&b[n]!=0)first_b=long(n/2);
            const double delta=std::abs(double(a[n])-b[n]);peak=std::max(peak,delta);
            if(std::memcmp(&a[n],&b[n],sizeof(float)))++bit_different;
            if(a[n]!=b[n]){if(!raw_different)first_mismatch=n;last_mismatch=n;++raw_different;}
        }
        // This is a zero-tolerance, sample-by-sample mathematical model, not an
        // RMS/tolerance waiver. At unity rate the old linear stage retains one
        // input frame. Stop leaves its history intact; seek does not reset it.
        for(const auto& s:segments)for(size_t frame=0;frame<s.frames;++frame)for(size_t ch=0;ch<2;++ch){
            const size_t at=(s.first+frame)*2+ch;
            const float expected=!equal_rate?b[at]:!s.playing?0.f:frame?b[at-2]:s.expected_first[ch];
            if(a[at]!=expected){
                // Finite nonlooping playback does not drain the old resampler's
                // one retained final frame after the data source reaches end.
                const bool final_cut=natural_end&&any_nonzero&&at/2==final_nonzero+1&&a[at]==0&&b[at]==0;
                if(final_cut)++eof_cut_samples;else ++model_different;
                std::fprintf(differences,"{\"sample\":%llu,\"frame\":%llu,\"channel\":%u,\"original\":%.9g,\"candidate\":%.9g,\"model\":%.9g,\"kind\":\"%s\"}\n",
                    (unsigned long long)at,(unsigned long long)(at/2),unsigned(ch),a[at],b[at],expected,final_cut?"undrained-one-frame-tail":"unexplained");
            }
        }
        require(std::fclose(differences)==0,"close sample differences");
        raw_file(out+"/"+name+".original.f32",a);raw_file(out+"/"+name+".no-pitch.f32",b);
        const bool energy_required=name=="dense-56-looping"||name=="synthetic-48000-rate-guard"||name=="bgm-loop-seam";
        const bool energy_okay=!energy_required||(ea>0&&eb>0);
        const bool okay=model_different==0&&energy_okay;
        std::printf("{\"case\":\"%s\",\"frames\":%llu,\"rateModel\":\"%s\",\"rawNumericDifferences\":%llu,\"rawBitDifferences\":%llu,\"firstDifferentSample\":%lld,\"lastDifferentSample\":%lld,\"modelDifferences\":%llu,\"undrainedTailSamples\":%llu,\"firstNonzeroOriginal\":%ld,\"firstNonzeroCandidate\":%ld,\"peakAbsoluteDifference\":%.17g,\"energyOriginal\":%.17g,\"energyCandidate\":%.17g,\"originalMixMs\":%.9g,\"candidateMixMs\":%.9g,\"passed\":%s}\n",
            name.c_str(),(unsigned long long)(a.size()/2),equal_rate?"one-PCM-frame-history":"identical-real-rate-conversion",
            (unsigned long long)raw_different,(unsigned long long)bit_different,raw_different?(long long)first_mismatch:-1LL,raw_different?(long long)last_mismatch:-1LL,
            (unsigned long long)model_different,(unsigned long long)eof_cut_samples,first_a,first_b,peak,ea,eb,old.mix_ms,candidate.mix_ms,okay?"true":"false");std::fflush(stdout);
        ++cases;require(energy_okay,("required nonzero PCM energy in "+name).c_str());
        require(okay,("unexplained PCM difference in "+name).c_str());
    }
};
std::vector<u8> unequal_rate_wav(){
    // Explicit synthetic guard, never represented as a retail asset.
    const u32 frames=8192,bytes=frames*4;std::vector<u8> wav(44+bytes);
    auto w16=[&](size_t at,u16 v){wav[at]=u8(v);wav[at+1]=u8(v>>8);};
    auto w32=[&](size_t at,u32 v){for(unsigned n=0;n<4;++n)wav[at+n]=u8(v>>(n*8));};
    std::memcpy(wav.data(),"RIFF",4);w32(4,36+bytes);std::memcpy(wav.data()+8,"WAVEfmt ",8);w32(16,16);w16(20,1);w16(22,2);
    w32(24,48000);w32(28,192000);w16(32,4);w16(34,16);std::memcpy(wav.data()+36,"data",4);w32(40,bytes);
    for(u32 n=0;n<frames;++n){w16(44+n*4,u16(i16((i32(n%127)-63)*384)));w16(46+n*4,u16(i16((i32(n%79)-39)*512)));}
    return wav;
}
}
int main(int argc,char** argv){
    require(argc==5,"usage: oracle archive.dat bgm.ogg bgm_stem output_directory");
    auto archive_bytes=file(argv[1]);Archive archive;require(archive.open(archive_bytes.data(),u32(archive_bytes.size())),"production THA1 archive reader");
    std::array<std::vector<u8>,46> samples;for(u32 n=0;n<samples.size();++n)samples[n]=resource(archive,sound_samples[n]);
    const auto formats=resource(archive,"thbgm.fmt");const auto music=file(argv[2]);const std::string stem=argv[3],output=argv[4];
    const u8* fmt=nullptr;const std::string wav_name=stem+".wav";
    for(size_t n=0;n+52<=formats.size();n+=52)if(std::strncmp(reinterpret_cast<const char*>(formats.data()+n),wav_name.c_str(),16)==0){fmt=formats.data()+n;break;}
    require(fmt!=nullptr,"real BGM fmt record");const u32 rate=word(fmt+36),channels=half(fmt+34),align=half(fmt+44);
    require(rate&&channels&&align,"real BGM format");const u64 intro=word(fmt+24)/align,end=word(fmt+28)/align;
    require(intro<end,"real BGM loop frame interval");
    std::printf("{\"inventory\":{\"wavSamples\":46,\"fixedSfxIds\":56,\"music\":\"%s\",\"musicRate\":%u,\"musicChannels\":%u,\"loopIntro\":%llu,\"loopEnd\":%llu,\"syntheticRateGuard\":48000,\"pcmFormat\":\"f32-le-interleaved-stereo-44100\",\"sameRateExpectedAdvanceFrames\":1,\"filterRemoved\":false}}\n",stem.c_str(),rate,channels,(unsigned long long)intro,(unsigned long long)end);
    for(u32 id=0;id<56;++id){
        Compare c("sfx-"+std::to_string(id),output);const auto& def=sound_definitions[id];const i32 pan=i32(id%3)*800-800;
        c.old.add(samples[def.sample],44100,2,true,def.volume,pan);c.candidate.add(samples[def.sample],44100,2,true,def.volume,pan);
        c.old.start();c.candidate.start();for(u32 frames:{1u,7u,511u,1024u,3u,2551u})c.block(frames);c.report();
    }
    {
        Compare c("sfx-seek-stop-restart",output);const auto& def=sound_definitions[10];
        c.old.add(samples[def.sample],44100,2,true,def.volume);c.candidate.add(samples[def.sample],44100,2,true,def.volume);
        c.old.start();c.candidate.start();c.block(1024);
        c.old.seek(0);c.candidate.seek(0);c.block(513);
        c.old.stop();c.candidate.stop();c.block(64,false);
        c.old.seek(17);c.candidate.seek(17);c.old.start();c.candidate.start();c.block(1);c.block(2048);c.report();
    }
    {
        Compare c("sfx-natural-end",output);const auto& def=sound_definitions[10];
        const auto length=c.old.add(samples[def.sample],44100,2,false,def.volume).frames;c.candidate.add(samples[def.sample],44100,2,false,def.volume);
        require(length<=44100*60,"bounded one-shot natural end");c.natural_end=true;c.old.start();c.candidate.start();c.block(u32(length+2048));
        require(ma_sound_at_end(&c.old.voices.front()->sound)&&ma_sound_at_end(&c.candidate.voices.front()->sound),"both one-shots reach end");c.report();
    }
    {
        Compare c("dense-56-looping",output);
        for(u32 id=0;id<56;++id){const auto& def=sound_definitions[id];const i32 pan=i32(id%3)*800-800;
            c.old.add(samples[def.sample],44100,2,true,def.volume,pan);c.candidate.add(samples[def.sample],44100,2,true,def.volume,pan);}
        c.old.start();c.candidate.start();for(unsigned n=0;n<32;++n)c.block(1024);c.report();
    }
    for(bool seam:{false,true}){
        Compare c(seam?"bgm-loop-seam":"bgm-real-start",output);c.equal_rate=rate==44100;
        for(auto* engine:{&c.old,&c.candidate}){
            auto& v=engine->add(music,rate,channels,true,-400);require(v.frames==end,"real OGG PCM length matches fmt");
            require(ma_data_source_set_loop_point_in_pcm_frames(&v.decoder,intro,end)==MA_SUCCESS,"real fmt loop points");
            if(seam)engine->seek(end>128?end-128:intro);engine->start();
        }
        c.block(1);c.block(4095);c.block(4096);c.report();
    }
    {
        const auto wav=unequal_rate_wav();Compare c("synthetic-48000-rate-guard",output);c.equal_rate=false;
        c.old.add(wav,48000,2,true,-400,700);c.candidate.add(wav,48000,2,true,-400,700);c.old.start();c.candidate.start();
        c.block(1024);c.old.seek(4000);c.candidate.seek(4000);c.block(6144);c.report();
    }
    std::printf("PASS AUDIO NO_PITCH PCM oracle: %u cases, %u checks\n",cases,checks);return 0;
}
