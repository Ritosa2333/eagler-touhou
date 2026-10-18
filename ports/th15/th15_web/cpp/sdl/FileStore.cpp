#include "FileStore.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
namespace th15::sdl {
namespace {bool exists(const std::string& path){struct stat info{};return ::stat(path.c_str(),&info)==0;}bool is_directory(const std::string& path){struct stat info{};return ::stat(path.c_str(),&info)==0&&S_ISDIR(info.st_mode);}}
bool FileStore::filename(const std::string& value)noexcept{return !value.empty()&&value!="."&&value!=".."&&value.size()<=255&&value.find_first_of("/\\") == std::string::npos&&value.find(char(0))==std::string::npos;}
bool FileStore::read(const std::string& path,std::vector<u8>& out,u32 limit,bool& present){out.clear();present=exists(path);if(!present)return true;SDL_IOStream* file=SDL_IOFromFile(path.c_str(),"rb");if(!file){error="Unable to read player file: "+path;return false;}const auto size=SDL_GetIOSize(file);if(size<0||u64(size)>limit){SDL_CloseIO(file);error="Player file outside allowed size: "+path;return false;}out.resize(size_t(size));const bool valid=SDL_ReadIO(file,out.data(),out.size())==out.size();const bool closed=SDL_CloseIO(file);if(!valid||!closed){error="Unable to finish reading player file: "+path;return false;}return true;}
bool FileStore::write(const std::string& path,const u8* bytes,u32 size){const auto temporary=path+".tmp";SDL_IOStream* file=SDL_IOFromFile(temporary.c_str(),"wb");if(!file){error="Unable to create player file: "+path;return false;}const bool written=SDL_WriteIO(file,bytes,size)==size;const bool closed=SDL_CloseIO(file);if(!written||!closed||std::rename(temporary.c_str(),path.c_str())!=0){error="Unable to finish player file: "+path;return false;}return true;}
bool FileStore::initialize(RecordStore& records,GameConfig& config,Rng& random){if(!is_directory(directory)){if(::mkdir(directory.c_str(),0777)!=0){error="Unable to create save directory";return false;}}const auto replays=directory+"/replay";if(!is_directory(replays)&&::mkdir(replays.c_str(),0777)!=0){error="Unable to create replay directory";return false;}const auto autosaves=directory+"/autosave";if(!is_directory(autosaves)&&::mkdir(autosaves.c_str(),0777)!=0){error="Unable to create checkpoint directory";return false;}std::vector<u8> bytes;bool present=false;records.reset(random);if(!read(directory+"/scoreth15.dat",bytes,16*1024*1024,present))return false;if(present&&!records.open(bytes.data(),bytes.size())){error="Invalid player score file: "+records.error;return false;}const auto& f=records.export_blocks();for(const auto& c:f.characters)saved_score.insert(saved_score.end(),c.begin(),c.end());saved_score.insert(saved_score.end(),f.settings.begin(),f.settings.end());if(!read(directory+"/th15.cfg",bytes,1024,present))return false;config.reset();if(present&&!config.open(bytes.data(),bytes.size())){error="Invalid player configuration";return false;}saved_config.assign(config.bytes.begin(),config.bytes.end());records.checkpoint_remove=[this](i32 c,i32 d){return remove_checkpoint(c,d);};return true;}
bool FileStore::save(RecordStore& records,const GameConfig& config){const auto& f=records.export_blocks();std::vector<u8> state;for(const auto& c:f.characters)state.insert(state.end(),c.begin(),c.end());state.insert(state.end(),f.settings.begin(),f.settings.end());if(state!=saved_score||!exists(directory+"/scoreth15.dat")){std::vector<u8> encoded;if(!records.save(encoded)){error="Unable to encode player records: "+records.error;return false;}if(!write(directory+"/scoreth15.dat",encoded.data(),encoded.size()))return false;saved_score=std::move(state);}const std::vector<u8> settings(config.bytes.begin(),config.bytes.end());if(settings!=saved_config||!exists(directory+"/th15.cfg")){if(!write(directory+"/th15.cfg",settings.data(),settings.size()))return false;saved_config=settings;}return true;}
bool FileStore::replay(const std::string& name,std::shared_ptr<Replay>& out){out.reset();if(!filename(name)){error="Invalid replay filename";return false;}std::vector<u8> bytes;bool present=false;if(!read(directory+"/replay/"+name,bytes,64*1024*1024,present))return false;if(!present)return true;out=std::make_shared<Replay>();if(!out->open(bytes.data(),bytes.size())){error="Invalid replay: "+out->error();out.reset();return false;}return true;}
bool FileStore::replay_slot(i32 slot,std::shared_ptr<Replay>& out){if(slot<0||slot>=99){error="Replay slot outside range";return false;}char name[32];std::snprintf(name,sizeof name,"th15_%.2d.rpyx",slot+1);if(exists(directory+"/replay/"+name))return replay(name,out);std::snprintf(name,sizeof name,"th15_%.2d.rpy",slot+1);return replay(name,out);}
bool FileStore::save_replay(i32 slot,ReplayRecording& recording,const std::array<char,9>& name,const ReplayExportDetails& details){if(slot<0||slot>=99||name[8]){error="Invalid replay slot or name";return false;}if(!recording.write(name.data(),details,true)){error=recording.error();return false;}const bool touch=recording.uses_touch();char file[32];std::snprintf(file,sizeof file,"th15_%.2d.%s",slot+1,touch?"rpyx":"rpy");const auto& bytes=recording.output();if(!write(directory+"/replay/"+file,bytes.data(),bytes.size()))return false;std::snprintf(file,sizeof file,"th15_%.2d.%s",slot+1,touch?"rpy":"rpyx");const auto previous=directory+"/replay/"+file;if(exists(previous)&&std::remove(previous.c_str())!=0){error="Unable to remove replaced replay format";return false;}return true;}
bool FileStore::catalog(ReplayCatalog& out){
    out.clear();const auto path=directory+"/replay";DIR* dir=opendir(path.c_str());
    if(!dir){error="Unable to list player replay directory";return false;}
    std::vector<std::string> users;
    while(auto* e=readdir(dir)){
        const std::string name=e->d_name;
        const auto suffix=name.size()==15?name.substr(11):name.size()==16?name.substr(11):"";
        if(filename(name)&&name.compare(0,7,"th15_ud")==0&&(suffix==".rpy"||suffix==".rpyx"))users.push_back(name);
    }
    closedir(dir);std::sort(users.begin(),users.end());
    const auto load=[&](u32 slot,const std::string& name){std::vector<u8> bytes;bool present=false;
        if(!read(path+"/"+name,bytes,64*1024*1024,present))return false;
        if(present)out.set(slot,name,bytes.data(),bytes.size());return true;};
    // Native slots 01..25 retain empty rows; user files occupy the next two pages.
    for(u32 slot=0;slot<25;slot++){char name[32];std::snprintf(name,sizeof name,"th15_%.2u.rpyx",slot+1);
        if(!exists(path+"/"+name))std::snprintf(name,sizeof name,"th15_%.2u.rpy",slot+1);
        if(!load(slot,name))return false;}
    u32 slot=25;
    for(const auto& name:users){
        // A continuous-touch replay replaces the ordinary replay with the same basename.
        if(name.size()==15&&std::binary_search(users.begin(),users.end(),name+"x"))continue;
        if(!load(slot,name))return false;if(++slot==75)break;
    }
    return true;
}
bool FileStore::remove_checkpoint(i32 character,i32 difficulty){if(character<0||character>=4||difficulty<0||difficulty>=5){error="Checkpoint selection outside range";return false;}char name[64];std::snprintf(name,sizeof name,"/autosave/save%d_%d.dat",character,difficulty);const auto path=directory+name;if(exists(path)&&std::remove(path.c_str())!=0){error="Unable to remove player checkpoint";return false;}return true;}
}

namespace th15::sdl {
namespace {std::string checkpoint_name(i32 c,i32 d){return "/autosave/save"+std::to_string(c)+"_"+std::to_string(d)+".dat";}}
bool FileStore::checkpoint(i32 c,i32 d,std::vector<u8>& bytes,bool& present){error.clear();if(c<0||c>=4||d<0||d>=5){error="Checkpoint selection outside range";return false;}return read(directory+checkpoint_name(c,d),bytes,CheckpointFile::max_payload+0x60,present);}
bool FileStore::checkpoint_header(i32 c,i32 d,CheckpointHeader& header,bool& present){error.clear();present=false;header={};if(c<0||c>=4||d<0||d>=5){error="Checkpoint selection outside range";return false;}const auto path=directory+checkpoint_name(c,d);if(!exists(path))return true;SDL_IOStream* stream=SDL_IOFromFile(path.c_str(),"rb");if(!stream){error="Unable to read checkpoint header";return false;}std::array<u8,0x60> bytes{};const auto size=SDL_ReadIO(stream,bytes.data(),bytes.size());const bool closed=SDL_CloseIO(stream);if(!closed){error="Unable to finish checkpoint header read";return false;}present=size==bytes.size()&&header.open(bytes.data(),bytes.size());return true;}
bool FileStore::save_checkpoint(i32 c,i32 d,const std::vector<u8>& bytes){error.clear();CheckpointHeader header;if(c<0||c>=4||d<0||d>=5||bytes.size()>CheckpointFile::max_payload+0x60||!header.open(bytes.data(),bytes.size())||header.character()!=c||header.difficulty()!=d){error="Invalid checkpoint file selection/header";return false;}return write(directory+checkpoint_name(c,d),bytes.data(),bytes.size());}
}
