#include "mmd_preview_engine.hpp"
#include "music_room.hpp"
#include "mmd_stream.hpp"
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
using namespace th03::audio;
namespace fs=std::filesystem;
std::vector<std::byte> read(const fs::path&p){std::ifstream f(p,std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Missing input");auto n=f.tellg();if(n<=0||n>40000000)throw std::runtime_error("Input size invalid");std::vector<std::byte>b(static_cast<std::size_t>(n));f.seekg(0);if(!f.read(reinterpret_cast<char*>(b.data()),n))throw std::runtime_error("Input read failed");return b;}
struct Result{std::uint32_t hash=2166136261u;std::uint64_t nonzero=0,peak=0,extremes=0;long double energy=0;MmdSoundfontStatus status;};
Result render(std::span<const std::byte> sf,std::span<const std::byte> song,unsigned seconds,std::size_t chunk,unsigned gain){
    auto created=MmdSoundfontEngine::create({.soundfont=sf});if(!created)throw std::runtime_error("SF create error="+std::to_string(unsigned(created.error().code)));auto engine=std::move(*created);
    auto loaded=engine.load_song(song);if(!loaded)throw std::runtime_error("MMD parse error="+std::to_string(unsigned(loaded.error().code))+" offset="+std::to_string(loaded.error().offset)+" cmd="+std::to_string(loaded.error().command));
    if(!engine.set_user_volume(static_cast<std::uint8_t>(gain))||!engine.play())throw std::runtime_error("MMD start failed");
    soew::MmdStream stream(engine);
    std::vector<PmdCanonicalStereoSample> pcm(chunk);Result result;const auto total=std::uint64_t(seconds)*48000;
    for(std::uint64_t done=0;done<total;){const auto n=std::size_t(std::min<std::uint64_t>(chunk,total-done));std::fill(pcm.begin(),pcm.end(),PmdCanonicalStereoSample{});const auto mixed=stream.render(std::span(pcm).first(n));if(!mixed)throw std::runtime_error("MMD render error="+std::to_string(unsigned(mixed.error().code))+" offset="+std::to_string(mixed.error().offset)+" cmd="+std::to_string(mixed.error().command));
        for(std::size_t i=0;i<n;++i)for(auto s:{pcm[i].left,pcm[i].right}){const auto norm=static_cast<long double>(s)/2147483648.0L;result.energy+=norm*norm;result.nonzero+=s!=0;result.peak=std::max(result.peak,s<0?std::uint64_t(-std::int64_t(s)):std::uint64_t(s));result.extremes+=s==std::numeric_limits<std::int32_t>::min()||s==std::numeric_limits<std::int32_t>::max();for(unsigned shift=0;shift<32;shift+=8){result.hash^=(static_cast<std::uint32_t>(s)>>shift)&255;result.hash*=16777619u;}}
        done+=n;
    }
    result.status=engine.status();engine.pause();if(!engine.status().paused)throw std::runtime_error("MMD pause failed");engine.resume();if(engine.status().paused)throw std::runtime_error("MMD resume failed");engine.stop();if(engine.status().playing)throw std::runtime_error("MMD stop failed");return result;
}
int wmain(int argc,wchar_t**argv){try{
    if(argc<4||argc>5)throw std::runtime_error("Usage: soew-mmd-probe <assets> <sf2> <seconds1..180> [gain5..25 step5]");
    const fs::path root=argv[1];const auto sf=read(argv[2]);const auto seconds=std::stoul(argv[3]);if(seconds<1||seconds>180)throw std::runtime_error("Duration invalid");const unsigned gain=argc==5?std::stoul(argv[4]):25;if(gain<5||gain>25||gain%5)throw std::runtime_error("Gain invalid");unsigned failures=0;
    std::cout<<"{\"seconds\":"<<seconds<<",\"gain\":"<<gain<<",\"audio\":\"MMD-TinySoundFont-48k-no-device\",\"tracks\":[\n";bool first=true;
    for(auto filename:soew::music_files){auto name=std::string(filename)+"MD";if(!first)std::cout<<",\n";first=false;
        try{const auto song=read(root/fs::path(name));const auto a=render(sf,song,seconds,960,gain);const auto b=render(sf,song,seconds,257,gain);if(!a.nonzero||a.hash!=b.hash||a.nonzero!=b.nonzero)throw std::runtime_error("Canonical MMD chunk determinism/silence failed");
            std::cout<<"{\"file\":\""<<name<<"\",\"bytes\":"<<song.size()<<",\"hash\":"<<a.hash<<",\"nonzero\":"<<a.nonzero<<",\"peak\":"<<a.peak<<",\"extremes\":"<<a.extremes<<",\"rms\":"<<double(std::sqrt(a.energy/(2.0L*seconds*48000)))<<",\"note_ons\":"<<a.status.note_ons<<",\"note_offs\":"<<a.status.note_offs<<",\"pending_gates\":"<<a.status.pending_gates<<",\"peak_gates\":"<<a.status.peak_gates<<",\"active_voices\":"<<a.status.active_voices<<",\"measure\":"<<a.status.measure<<",\"playing\":"<<(a.status.playing?"true":"false")<<",\"alternate_hash\":"<<b.hash<<",\"alternate_nonzero\":"<<b.nonzero<<",\"chunk_deterministic\":"<<(a.hash==b.hash?"true":"false")<<"}"<<std::flush;
        }catch(const std::exception&e){++failures;std::cout<<"{\"file\":\""<<name<<"\",\"error\":\""<<e.what()<<"\"}"<<std::flush;}
    }
    std::cout<<"\n],\"failures\":"<<failures<<"}\n";return failures?1:0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
