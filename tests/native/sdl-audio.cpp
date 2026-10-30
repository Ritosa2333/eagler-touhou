// Production mixer and archive reader with generated PCM. SDL device IO is
// disabled by the test header; samples are read through Audio::mix directly.
#include "Audio.hpp"
#include "Archive.hpp"
#include "RuntimeOptions.hpp"
#include <cassert>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace th12host::host {
Paths& paths(){static Paths value;return value;}
FILE* open_file(const std::string& path,const char* mode){return std::fopen(path.c_str(),mode);}
bool exists(const std::string& path){return std::filesystem::exists(path);}
void log(const char*,...){}
std::vector<std::string> list_files(const std::string& path){std::vector<std::string> out;for(const auto& entry:std::filesystem::directory_iterator(path))out.push_back(entry.path().filename().string());return out;}
bool read_file(const std::string& path,std::vector<u8>& bytes,u64 limit){
    std::ifstream file(path,std::ios::binary|std::ios::ate);if(!file)return false;
    const auto length=file.tellg();if(length<0||u64(length)>limit)return false;
    bytes.resize(size_t(length));file.seekg(0);return bool(file.read(reinterpret_cast<char*>(bytes.data()),length));
}
}
using namespace th12host;
namespace {
template<class T>void put(std::vector<u8>& bytes,size_t at,T value){std::memcpy(bytes.data()+at,&value,sizeof value);}
void file(const std::string& name,const std::vector<u8>& bytes){std::ofstream out(host::paths().data_file(name.c_str()),std::ios::binary);out.write(reinterpret_cast<const char*>(bytes.data()),bytes.size());assert(out.good());}
std::vector<u8> wave(){
    std::vector<u8> bytes(44+4096*4);std::memcpy(bytes.data(),"RIFF",4);put<u32>(bytes,4,u32(bytes.size()-8));std::memcpy(bytes.data()+8,"WAVEfmt ",8);
    put<u32>(bytes,16,16);put<u16>(bytes,20,1);put<u16>(bytes,22,2);put<u32>(bytes,24,44100);put<u32>(bytes,28,44100*4);put<u16>(bytes,32,4);put<u16>(bytes,34,16);
    std::memcpy(bytes.data()+36,"data",4);put<u32>(bytes,40,4096*4);
    for(size_t n=44;n<bytes.size();n+=4){put<i16>(bytes,n,8192);put<i16>(bytes,n+2,-8192);}return bytes;
}
double peak(Audio& audio){float pcm[1024]{};assert(audio.mix(pcm,512));double out=0;for(float sample:pcm){assert(std::isfinite(sample));out=std::max(out,std::abs(double(sample)));}return out;}
}
int main(int argc,char** argv){
    assert(argc==2);host::paths().data=argv[1];
    file("default.ecl",{0});file("effect.wav",wave());
    std::filesystem::create_directory(host::paths().data_file("music"));file("music/track.ogg",wave());
    std::vector<u8> format(52);std::memcpy(format.data(),"track.wav",9);put<u32>(format,28,4096*4);put<u16>(format,32,1);put<u16>(format,34,2);put<u32>(format,36,44100);put<u16>(format,44,4);put<u16>(format,46,16);file("thbgm.fmt",format);
    Archive archive;assert(archive.open(host::paths().data));ExeTables tables;for(auto& sound:tables.sounds)sound.id=-1;tables.sounds[0]={0,"effect.wav",0,false};
    Audio audio;assert(audio.initialize(archive,tables));audio.set_volumes(100,100);
    for(bool full:{false,true}){
        runtime_options.full_music=full;runtime_options.music=true;
        file("music/track.ogg",{'b','a','d'});audio.play_music(0);
        assert(!audio.error.empty());assert(peak(audio)<1e-6);
        audio.effect(0,0,false);assert(peak(audio)>.1);audio.stop_effects();
        audio.pause_music(true);file("music/track.ogg",wave());audio.retry_music();
        assert(audio.error.empty());assert(audio.music_paused());assert(peak(audio)<1e-6);
        audio.pause_music(false);assert(peak(audio)>.1);
        audio.pause_music(true);assert(peak(audio)<1e-6);audio.pause_music(false);assert(peak(audio)>.1);
        runtime_options.music=false;audio.retry_music();assert(peak(audio)<1e-6);
        audio.effect(0,0,false);assert(peak(audio)>.1);audio.stop_effects();assert(peak(audio)<1e-6);
        runtime_options.music=true;audio.retry_music();assert(peak(audio)>.1);
        audio.fade_music(.1);audio.update(.1);assert(peak(audio)<1e-6);
    }
    std::puts("SDL audio: PCM output, pause, no-BGM with effects, re-enable, stream/full and fade passed");
}
