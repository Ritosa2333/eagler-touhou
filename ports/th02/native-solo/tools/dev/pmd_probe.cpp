#include "th03/audio/pmd_opna.hpp"
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
using namespace th03::audio;
namespace fs=std::filesystem;
unsigned checks=0;
void check(bool ok,const char *message){++checks;if(!ok)throw std::runtime_error(message);}
std::vector<std::byte> read(const fs::path&p){
    std::ifstream f(p,std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Song missing: "+p.filename().string());
    const auto size=f.tellg();if(size<=0||size>65535)throw std::runtime_error("Song size out of range");
    std::vector<std::byte>b(static_cast<std::size_t>(size));f.seekg(0);if(!f.read(reinterpret_cast<char*>(b.data()),size))throw std::runtime_error("Song read failed");return b;
}
PmdOpnaEngine engine(){auto e=PmdOpnaEngine::create({.input_clock_hz=7987200,.fidelity=PmdOpnaFidelity::minimum});if(!e)throw std::runtime_error("Engine create failed");return std::move(*e);}
struct Metrics{
    std::uint32_t hash=2166136261u;
    std::uint64_t nonzero=0,peak=0,clipped=0;
    long double sum_square=0;
    PmdOpnaStatus status;
};
Metrics render(PmdOpnaEngine &e,unsigned seconds){
    Metrics m;const auto initial=e.status().samples_rendered;
    const auto frames=std::uint64_t(seconds)*e.status().native_sample_rate;
    std::array<PmdOpnaStereoSample,1024> buffer{};
    for(std::uint64_t done=0;done<frames;){const auto count=std::size_t(std::min<std::uint64_t>(buffer.size(),frames-done));
        e.render_native(std::span(buffer).first(count));
        for(std::size_t i=0;i<count;++i)for(auto sample:{buffer[i].left,buffer[i].right}){
            const auto value=static_cast<std::uint32_t>(sample);
            for(unsigned shift=0;shift<32;shift+=8){m.hash^=(value>>shift)&255;m.hash*=16777619u;}
            const auto magnitude=sample<0?std::uint64_t(-std::int64_t(sample)):std::uint64_t(sample);
            m.peak=std::max(m.peak,magnitude);m.nonzero+=sample!=0;
            m.clipped+=sample==std::numeric_limits<std::int32_t>::max()||sample==std::numeric_limits<std::int32_t>::min();
            const auto normalized=static_cast<long double>(sample)/2147483648.0L;m.sum_square+=normalized*normalized;
        }done+=count;
    }
    m.status=e.status();check(m.status.samples_rendered-initial==frames,"Exact PCM frame count failed");return m;
}
void start(PmdOpnaEngine&e,std::span<const std::byte>b){auto loaded=e.load_song(b);if(!loaded)throw std::runtime_error("PMD load failed code="+std::to_string(unsigned(loaded.error().code)));if(!e.play())throw std::runtime_error("PMD play failed");}
int wmain(int argc,wchar_t**argv){try{
    if(argc<2||argc>3)throw std::runtime_error("Usage: soew-pmd-probe <original-assets> [seconds 1..30]");
    const fs::path root=argv[1];const unsigned seconds=argc==3?std::stoul(argv[2]):3;
    if(seconds<1||seconds>30)throw std::runtime_error("Duration out of range");
    std::cout<<"{\"profile\":\"7987200Hz minimum native166400 no-rhythm no-EFC no-device\",\"seconds\":"<<seconds<<",\"tracks\":[\n";
    bool first=true;
    for(const auto name:soew::music_files){const auto bytes=read(root/fs::path(name));auto a=engine();start(a,bytes);auto m=render(a,seconds);auto b=engine();start(b,bytes);const auto n=render(b,seconds);
        check(m.status.song_loaded&&m.status.playing,"Song did not remain playing in window");
        check(m.status.native_sample_rate==166400,"Native sample rate mismatch");check(m.nonzero>0,"Silent PCM");
        check(m.status.chip_clocks>0&&m.status.timer_b_count>0&&m.status.register_writes>0,"Sequencer did not advance");
        check(!m.status.rhythm_rom_present&&!m.status.rhythm_pcm_present&&!m.status.effect_bank_loaded&&!m.status.active_effect,"Unexpected rhythm or effects");
        check(m.hash==n.hash&&m.peak==n.peak&&m.nonzero==n.nonzero&&m.status.register_hash==n.status.register_hash&&m.status.timer_b_count==n.status.timer_b_count,"Independent engine determinism failed");
        a.pause();check(a.status().paused,"Pause state failed");a.resume();check(!a.status().paused,"Resume state failed");a.stop();check(!a.status().playing,"Stop state failed");
        check(bool(a.play()),"Fresh restart failed");const auto restarted=render(a,seconds);check(restarted.hash==m.hash,"Restart PCM determinism failed");
        if(!first)std::cout<<",\n";
        first=false;
        std::uint64_t reduced_clipped=0,reduced_peak=0;
        if(m.clipped){auto reduced=engine();check(bool(reduced.set_user_volumes(50,0)),"Reduced gain setup failed");start(reduced,bytes);const auto quiet=render(reduced,seconds);reduced_clipped=quiet.clipped;reduced_peak=quiet.peak;check(quiet.clipped==0,"Reduced gain still saturates in bounded window");}
        const auto rms=std::sqrt(m.sum_square/(2.0L*seconds*m.status.native_sample_rate));
        std::cout<<"{\"file\":\""<<name<<"\",\"bytes\":"<<bytes.size()<<",\"frames\":"<<m.status.samples_rendered<<",\"pcm_fnv1a32\":"<<m.hash<<",\"nonzero_samples\":"<<m.nonzero<<",\"peak_abs_q31\":"<<m.peak<<",\"rms_normalized\":"<<double(rms)<<",\"clipped_extreme_samples\":"<<m.clipped<<",\"reduced50_tested\":"<<(m.clipped?"true":"false")<<",\"reduced50_clipped\":"<<reduced_clipped<<",\"reduced50_peak\":"<<reduced_peak<<",\"register_writes\":"<<m.status.register_writes<<",\"register_hash\":"<<m.status.register_hash<<",\"timer_b_count\":"<<m.status.timer_b_count<<",\"loops\":"<<unsigned(m.status.loop_count)<<",\"deterministic\":true}"<<std::flush;
    }
    auto invalid=engine();check(!invalid.load_song({}),"Empty song accepted");const std::array<std::byte,4> bad{};check(!invalid.load_song(bad),"Malformed song accepted");
    std::cout<<"\n],\"checks\":"<<checks<<",\"failures\":0}\n";return 0;
}catch(const std::exception&e){std::cerr<<"ERROR "<<e.what()<<" checks="<<checks<<'\n';return 1;}}
