// Exercise the real SDL replay codec without SDL, a window or retail assets.
#include "Archive.hpp"
#include "Replay.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>

namespace th12host::host {
FILE* open_file(const std::string& path,const char* mode){return std::fopen(path.c_str(),mode);}
bool exists(const std::string& path){return std::filesystem::exists(path);}
std::vector<std::string> list_files(const std::string& folder){
    std::vector<std::string> names;
    for(const auto& entry:std::filesystem::directory_iterator(folder))names.push_back(entry.path().filename().string());
    return names;
}
bool read_file(const std::string& path,std::vector<u8>& data,u64 limit){
    std::ifstream file(path,std::ios::binary|std::ios::ate);if(!file)return false;
    const auto size=file.tellg();if(size<0||u64(size)>limit)return false;
    data.resize(size_t(size));file.seekg(0);return bool(file.read(reinterpret_cast<char*>(data.data()),size));
}
}
namespace {
using namespace th12host;
void word(std::vector<u8>& bytes,size_t offset,u32 value){std::memcpy(bytes.data()+offset,&value,4);}
std::vector<u8> packed(const std::vector<u8>& raw){
    const auto plain=lzss_literals(raw);std::vector<u8> first(plain.size()),second(plain.size()),file(36);
    resource_encrypt(plain.data(),first.data(),u32(plain.size()),{0x7d,0x3a,0x40,0xffffffffu});
    resource_encrypt(first.data(),second.data(),u32(first.size()),{0x5e,0xe1,0x800,0xffffffffu});
    word(file,0,0x72323174);file[4]=4;word(file,28,u32(second.size()));word(file,32,u32(raw.size()));
    file.insert(file.end(),second.begin(),second.end());return file;
}
void bounds(){
    for(size_t size=112;size<272;++size){
        std::vector<u8> raw(size);word(raw,88,1);
        Replay replay;std::string error;
        assert(!parse_replay(packed(raw),replay,error));
        assert(error=="truncated stage header");
    }
    // A complete first stage followed by only part of a second header.
    std::vector<u8> raw(112+160+7+159);word(raw,88,2);raw[112]=1;
    word(raw,116,1);word(raw,120,7);
    Replay replay;std::string error;
    assert(!parse_replay(packed(raw),replay,error));assert(error=="truncated stage header");
    // A legal stream with 31 playable frames and its end marker still decodes.
    ReplayRecorder recorder(0,1,2,false,{});ReplayInitial initial;initial.power=100;
    recorder.begin_stage(1,0x3456,initial);
    for(u32 i=0;i<31;++i)recorder.push(i?0x81:0,i==1?0x81:0,0,i==30?59:60);
    assert(parse_replay(recorder.encode("REPLAY",1770000000,1234,0,1,0),replay,error));
    assert(replay.stages.size()==1&&replay.stage(1)->frames==32&&replay.stage(1)->fps_count==2);
    assert(replay.input(1,31)->end());assert(!replay.input(1,32));
    std::puts("truncated headers rejected; valid recording round-trip passed");
}
void fps(){
    Replay replay;ReplayStage stage;stage.number=1;stage.frames=91;stage.fps_count=4;
    replay.stages.push_back(stage);replay.data={60,59,58,57};
    for(u32 cursor=0;cursor<=91;++cursor)std::printf("%d\n",replay.recorded_fps(1,cursor));
    assert(replay.recorded_fps(2,0)==-1);
    assert(replay.recorded_fps(1,92)==-1);
}
}
int main(int argc,char** argv){
    if(argc!=2)return 2;
    if(std::string(argv[1])=="bounds")bounds();else if(std::string(argv[1])=="fps")fps();else return 2;
}
