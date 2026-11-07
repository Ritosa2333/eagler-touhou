#include "FileStore.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
namespace th17::sdl {
namespace {bool exists(const std::string& path){struct stat info{};return ::stat(path.c_str(),&info)==0;}bool directory_exists(const std::string& path){struct stat info{};return ::stat(path.c_str(),&info)==0&&S_ISDIR(info.st_mode);}}
bool FileStore::filename(const std::string& name)noexcept{return !name.empty()&&name!="."&&name!=".."&&name.size()<=255&&name.find_first_of("/\\")==std::string::npos&&name.find(char(0))==std::string::npos;}
bool FileStore::read(const std::string& path,std::vector<u8>& bytes,u32 limit,bool& present){
 bytes.clear();present=exists(path);if(!present)return true;SDL_IOStream* file=SDL_IOFromFile(path.c_str(),"rb");if(!file){error="Unable to read player file: "+path;return false;}const auto size=SDL_GetIOSize(file);
 if(size<0||u64(size)>limit){SDL_CloseIO(file);error="Player file outside allowed size: "+path;return false;}bytes.resize(size_t(size));const bool ok=SDL_ReadIO(file,bytes.data(),bytes.size())==bytes.size(),closed=SDL_CloseIO(file);if(!ok||!closed){error="Unable to finish reading player file: "+path;return false;}return true;
}
bool FileStore::write(const std::string& path,const u8* bytes,u32 size){
 const auto temporary=path+".tmp";SDL_IOStream* file=SDL_IOFromFile(temporary.c_str(),"wb");if(!file){error="Unable to create player file: "+path;return false;}const bool written=SDL_WriteIO(file,bytes,size)==size,closed=SDL_CloseIO(file);if(!written||!closed||std::rename(temporary.c_str(),path.c_str())!=0){error="Unable to finish player file: "+path;return false;}return true;
}
bool FileStore::initialize(RecordStore& records,GameConfig& config,Rng& random){
 if(!directory_exists(directory)&&::mkdir(directory.c_str(),0777)!=0){error="Unable to create save directory";return false;}const auto replays=directory+"/replay";if(!directory_exists(replays)&&::mkdir(replays.c_str(),0777)!=0){error="Unable to create replay directory";return false;}
 records.reset(random);std::vector<u8> bytes;bool present=false;if(!read(directory+"/scoreth17.dat",bytes,16*1024*1024,present))return false;if(present&&!records.open(bytes.data(),bytes.size())){error="Invalid player records: "+records.error;return false;}
 const auto& blocks=records.export_blocks();for(const auto& bank:blocks.characters)saved_score.insert(saved_score.end(),bank.begin(),bank.end());saved_score.insert(saved_score.end(),blocks.settings.begin(),blocks.settings.end());
 config.reset();if(!read(directory+"/th17.cfg",bytes,1024,present))return false;if(present&&!config.open(bytes.data(),bytes.size())){error="Invalid TH17 player configuration";return false;}saved_config.assign(config.bytes.begin(),config.bytes.end());return true;
}
bool FileStore::save(RecordStore& records,const GameConfig& config){
 const auto& blocks=records.export_blocks();std::vector<u8> state;for(const auto& bank:blocks.characters)state.insert(state.end(),bank.begin(),bank.end());state.insert(state.end(),blocks.settings.begin(),blocks.settings.end());
 if(state!=saved_score||!exists(directory+"/scoreth17.dat")){std::vector<u8> bytes;if(!records.save(bytes)){error=records.error;return false;}if(!write(directory+"/scoreth17.dat",bytes.data(),bytes.size()))return false;saved_score=std::move(state);}
 const std::vector<u8> settings(config.bytes.begin(),config.bytes.end());if(settings!=saved_config||!exists(directory+"/th17.cfg")){if(!write(directory+"/th17.cfg",settings.data(),settings.size()))return false;saved_config=settings;}return true;
}
bool FileStore::replay(const std::string& name,std::shared_ptr<Replay>& result){
 result.reset();if(!filename(name)){error="Invalid replay filename";return false;}std::vector<u8> bytes;bool present=false;if(!read(directory+"/replay/"+name,bytes,64*1024*1024,present))return false;if(!present)return true;auto value=std::make_shared<Replay>();if(!value->open(bytes.data(),bytes.size())){error="Invalid replay: "+value->error();return false;}result=std::move(value);return true;
}
bool FileStore::replay_slot(i32 slot,std::shared_ptr<Replay>& result){if(slot<0||slot>=25){error="Replay slot outside original range";return false;}char name[32];std::snprintf(name,sizeof name,"th17_%.2d.rpyx",slot+1);if(exists(directory+"/replay/"+name))return replay(name,result);std::snprintf(name,sizeof name,"th17_%.2d.rpy",slot+1);return replay(name,result);}
bool FileStore::save_replay(i32 slot,ReplayRecording& recording,const std::array<char,9>& name,const ReplayExportDetails& details){
 if(slot<0||slot>=25||name[8]){error="Invalid replay slot or name";return false;}if(!recording.write(name.data(),details,true)){error=recording.error();return false;}const bool touch=recording.uses_touch();char file[32];std::snprintf(file,sizeof file,"th17_%.2d.%s",slot+1,touch?"rpyx":"rpy");const auto& bytes=recording.output();if(!write(directory+"/replay/"+file,bytes.data(),bytes.size()))return false;
 std::snprintf(file,sizeof file,"th17_%.2d.%s",slot+1,touch?"rpy":"rpyx");const auto previous=directory+"/replay/"+file;if(exists(previous)&&std::remove(previous.c_str())!=0){error="Unable to remove replaced replay format";return false;}return true;
}
bool FileStore::catalog(ReplayCatalog& catalog){
 catalog.clear();const auto path=directory+"/replay";DIR* dir=opendir(path.c_str());if(!dir){error="Unable to list player replay directory";return false;}std::vector<std::string> users;
 while(auto* entry=readdir(dir)){const std::string name=entry->d_name;const auto suffix=name.size()==15||name.size()==16?name.substr(11):"";if(filename(name)&&name.compare(0,7,"th17_ud")==0&&(suffix==".rpy"||suffix==".rpyx"))users.push_back(name);}closedir(dir);std::sort(users.begin(),users.end());
 auto load=[&](u32 slot,const std::string& name){std::vector<u8> bytes;bool present=false;if(!read(path+"/"+name,bytes,64*1024*1024,present))return false;if(present)catalog.set(slot,name,bytes.data(),bytes.size());return true;};
 for(u32 slot=0;slot<25;slot++){char name[32];std::snprintf(name,sizeof name,"th17_%.2u.rpyx",slot+1);if(!exists(path+"/"+name))std::snprintf(name,sizeof name,"th17_%.2u.rpy",slot+1);if(!load(slot,name))return false;}
 u32 slot=25;for(const auto& name:users){if(name.size()==15&&std::binary_search(users.begin(),users.end(),name+"x"))continue;if(!load(slot,name))return false;if(++slot==75)break;}return true;
}
}
