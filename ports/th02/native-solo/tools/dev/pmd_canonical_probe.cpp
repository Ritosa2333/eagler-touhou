#include "th03/audio/pmd_opna.hpp"
#include "th03/audio/fixed_resampler.hpp"
#include "music_room.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>
namespace fs=std::filesystem;
using namespace th03::audio;
unsigned checks=0;
void check(bool ok,const char*message){++checks;if(!ok)throw std::runtime_error(message);}
std::vector<std::byte> read_song(const fs::path &path){std::ifstream f(path,std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Song missing");auto n=f.tellg();if(n<=0||n>65535)throw std::runtime_error("Song size invalid");std::vector<std::byte>b(static_cast<std::size_t>(n));f.seekg(0);if(!f.read(reinterpret_cast<char*>(b.data()),n))throw std::runtime_error("Song read failed");return b;}
void little(std::ostream &f,std::uint32_t value,unsigned n){for(unsigned i=0;i<n;++i)f.put(static_cast<char>((value>>(8*i))&255));}
struct Result{std::uint32_t hash=2166136261u;std::uint64_t frames=0,nonzero=0,peak=0,clipped=0,native_clipped=0;unsigned max_loops=0;long double energy=0;PmdOpnaStatus status;};
Result convert(std::span<const std::byte> song,unsigned seconds,std::size_t input_chunk,std::size_t output_chunk,unsigned gain,std::ostream *wav=nullptr){
    auto created=PmdOpnaEngine::create({.input_clock_hz=7987200,.fidelity=PmdOpnaFidelity::minimum});if(!created)throw std::runtime_error("Engine create failed");auto engine=std::move(*created);
    check(bool(engine.set_user_volumes(static_cast<std::uint8_t>(gain),0)),"Gain setup failed");check(bool(engine.load_song(song)),"Song rejected");check(bool(engine.play()),"Song start failed");
    PmdFixedResampler resampler;std::vector<PmdOpnaStereoSample>input(input_chunk);std::vector<PmdCanonicalStereoSample>output(output_chunk);Result result;
    const auto collect=[&](std::size_t count){for(std::size_t i=0;i<count;++i){for(auto v:{output[i].left,output[i].right}){const auto u=static_cast<std::uint32_t>(v);for(unsigned s=0;s<32;s+=8){result.hash^=(u>>s)&255;result.hash*=16777619u;}const auto mag=v<0?std::uint64_t(-std::int64_t(v)):std::uint64_t(v);result.peak=std::max(result.peak,mag);result.nonzero+=v!=0;result.clipped+=v==std::numeric_limits<std::int32_t>::min()||v==std::numeric_limits<std::int32_t>::max();const auto norm=static_cast<long double>(v)/2147483648.0L;result.energy+=norm*norm;
        if(wav){const auto pcm=std::clamp<std::int64_t>(std::int64_t(v)/65536,-32768,32767);little(*wav,static_cast<std::uint16_t>(pcm),2);}}
        ++result.frames;}};
    const auto total=std::uint64_t(seconds)*166400;
    for(std::uint64_t done=0;done<total;){const auto n=std::size_t(std::min<std::uint64_t>(input_chunk,total-done));engine.render_native(std::span(input).first(n));
        result.max_loops=std::max(result.max_loops,unsigned(engine.status().loop_count));
        for(std::size_t i=0;i<n;++i)for(auto v:{input[i].left,input[i].right})result.native_clipped+=v==std::numeric_limits<std::int32_t>::min()||v==std::numeric_limits<std::int32_t>::max();
        std::size_t at=0;while(at<n){const auto p=resampler.process(std::span(input).subspan(at,n-at),output);if(!p)throw std::runtime_error("Resampler process rejected");if(!p->input_consumed&&!p->output_written)throw std::runtime_error("Resampler stalled");at+=p->input_consumed;collect(p->output_written);}done+=n;
    }
    for(;;){const auto p=resampler.finish(output);collect(p.output_written);if(p.complete)break;if(!p.output_written)throw std::runtime_error("Resampler finish stalled");}
    check(resampler.input_samples()==total,"Native count mismatch");// finish() extends through last real sample +63 native left-span samples.
    const auto expected_frames=((total+63)*15+51)/52;
    check(result.frames==expected_frames,"48k FIR-tail frame count mismatch");
    check(resampler.finish(output).complete,"Finish not idempotent");check(!resampler.process({},output),"Input after finish accepted");
    result.status=engine.status();check(result.nonzero>0&&result.status.timer_b_count>0,"Silent or stalled output");
    check(!result.status.rhythm_rom_present&&!result.status.rhythm_pcm_present&&!result.status.effect_bank_loaded,"Unexpected rhythm/effects");
    return result;
}
int wmain(int argc,wchar_t**argv){try{
    if(argc<3||argc>6)throw std::runtime_error("Usage: soew-pmd-canonical <assets> <seconds1..180> [new.wav|-] [gain5..50 step5] [track filename; default OP.M]");
    std::string_view export_track="OP.M";
    if(argc==6){
        export_track={};
        for(auto file:soew::music_files)if(fs::path(file).wstring()==argv[5])export_track=file;
        check(!export_track.empty(),"Unknown export track");
    }
    const fs::path root=argv[1];const auto seconds=std::stoul(argv[2]);if(seconds<1||seconds>180)throw std::runtime_error("Duration invalid");
    const unsigned gain=argc>=5?std::stoul(argv[4]):50;check(gain>=5&&gain<=50&&gain%5==0,"Gain out of range");
    std::ofstream wav;if(argc>=4&&std::wstring_view(argv[3])!=L"-"){const fs::path path=argv[3];if(fs::exists(path)||!fs::is_directory(path.parent_path()))throw std::runtime_error("WAV must be new in existing directory");wav.open(path,std::ios::binary);if(!wav)throw std::runtime_error("WAV create failed");const auto bytes=(((std::uint64_t(seconds)*166400+63)*15+51)/52)*4;wav.write("RIFF",4);little(wav,36+bytes,4);wav.write("WAVEfmt ",8);little(wav,16,4);little(wav,1,2);little(wav,2,2);little(wav,48000,4);little(wav,192000,4);little(wav,4,2);little(wav,16,2);wav.write("data",4);little(wav,bytes,4);}
    std::cout<<"{\"seconds\":"<<seconds<<",\"gain_percent\":"<<gain<<",\"profile\":\"7987200-minimum-noRhythm-noEFC-noDevice-48kFIR\",\"tracks\":[\n";bool first=true;
    for(auto file:soew::music_files){const auto song=read_song(root/fs::path(file));const auto a=convert(song,seconds,1024,333,gain,(wav.is_open()&&file==export_track)?&wav:nullptr);const auto b=convert(song,seconds,257,71,gain);
        check(a.hash==b.hash&&a.frames==b.frames&&a.status.register_hash==b.status.register_hash,"Chunking/fresh-engine determinism failed");
        if(!first)std::cout<<",\n";
        first=false;
        std::cout<<"{\"file\":\""<<file<<"\",\"frames48k\":"<<a.frames<<",\"hash\":"<<a.hash<<",\"nonzero_samples\":"<<a.nonzero<<",\"peak_q31\":"<<a.peak<<",\"rms\":"<<double(std::sqrt(a.energy/(a.frames*2)))<<",\"native_extremes\":"<<a.native_clipped<<",\"canonical_extremes\":"<<a.clipped<<",\"max_loop_count\":"<<a.max_loops<<",\"final_playing\":"<<(a.status.playing?"true":"false")<<",\"chunk_deterministic\":true}"<<std::flush;
    }
    if(wav.is_open()){wav.flush();check(bool(wav),"WAV write failed");}
    std::cout<<"\n],\"checks\":"<<checks<<",\"failures\":0}\n";return 0;
}catch(const std::exception&e){std::cerr<<"ERROR "<<e.what()<<'\n';return 1;}}
