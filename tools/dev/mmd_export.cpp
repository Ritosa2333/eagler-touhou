#ifdef SOEW_LEGACY_MIDI
#include "th03/audio/mmd_soundfont.hpp"
#else
#include "mmd_preview_engine.hpp"
#endif
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
using namespace th03::audio;
namespace fs=std::filesystem;
std::vector<std::byte> read(const fs::path &p){std::ifstream f(p,std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Missing input");auto n=f.tellg();if(n<1||n>40000000)throw std::runtime_error("Bad size");std::vector<std::byte>b(static_cast<std::size_t>(n));f.seekg(0);if(!f.read(reinterpret_cast<char*>(b.data()),n))throw std::runtime_error("Read failed");return b;}
void le(std::ostream&f,std::uint32_t n,int count){for(int i=0;i<count;++i)f.put(char((n>>(8*i))&255));}
int wmain(int argc,wchar_t**argv){try{
    if(argc!=7)throw std::runtime_error("Usage: exporter sf2 song seconds gain new.wav tag");
    const auto sf=read(argv[1]),song=read(argv[2]);const auto seconds=std::stoul(argv[3]),gain=std::stoul(argv[4]);if(seconds<1||seconds>180||gain>100||gain%5)throw std::runtime_error("Bad duration/gain");const fs::path path=argv[5];if(fs::exists(path)||!fs::is_directory(path.parent_path()))throw std::runtime_error("Export must be a new file");
    auto made=MmdSoundfontEngine::create({.soundfont=sf});if(!made)throw std::runtime_error("Create failed");auto &e=*made;if(!e.set_user_volume(static_cast<std::uint8_t>(gain))||!e.load_song(song)||!e.play())throw std::runtime_error("Start failed");
    const auto total=seconds*48000;std::ofstream f(path,std::ios::binary);const auto bytes=total*4;f.write("RIFF",4);le(f,36+bytes,4);f.write("WAVEfmt ",8);le(f,16,4);le(f,1,2);le(f,2,2);le(f,48000,4);le(f,192000,4);le(f,4,2);le(f,16,2);f.write("data",4);le(f,bytes,4);
    std::array<PmdCanonicalStereoSample,256> pcm{};long double energy=0;std::uint64_t extremes=0,peak=0;
    for(std::size_t done=0;done<total;){const auto n=std::min<std::size_t>(pcm.size(),total-done);pcm.fill({});if(!e.render_mix(std::span(pcm).first(n)))throw std::runtime_error("Render failed");for(std::size_t i=0;i<n;++i)for(auto v:{pcm[i].left,pcm[i].right}){const auto norm=static_cast<long double>(v)/2147483648.0L;energy+=norm*norm;extremes+=v==INT32_MIN||v==INT32_MAX;peak=std::max(peak,v<0?std::uint64_t(-std::int64_t(v)):std::uint64_t(v));le(f,static_cast<std::uint16_t>(std::clamp<std::int64_t>(std::int64_t(v)/65536,-32768,32767)),2);}done+=n;}
    f.flush();if(!f)throw std::runtime_error("Export write failed");std::wcout<<argv[6]<<L" frames="<<total<<L" rms="<<double(std::sqrt(energy/(2*total)))<<L" peak="<<peak<<L" extremes="<<extremes;
#ifndef SOEW_LEGACY_MIDI
    const auto s=e.status();std::wcout<<L" note_ons="<<s.note_ons<<L" offs="<<s.note_offs<<L" pending="<<s.pending_gates<<L" peak_gates="<<s.peak_gates<<L" voices="<<s.active_voices;
#endif
    std::wcout<<L"\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
