#include <SDL3/SDL.h>
#include <emscripten.h>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "../../../portable/sdl/third_party/stb_image.h"
#include "../game/Types.hpp"
using th10::u32;using th10::i32;using th10::u8;
extern "C" SDL_IOStream* th10_music_stream();
EM_JS(void, browser_save_changed, (), { Module['runtimeFileChanged']?.(); });
namespace {
std::string save_root="/savesth10/jp";u32 next=1;
std::map<u32,SDL_IOStream*> handles;
std::set<u32> writers;
std::string normalize(const char* value){
    if(!value||!*value)return {};
    std::string out,part;auto flush=[&](){if(part=="..")return false;if(!part.empty()&&part!="."){if(!out.empty())out+='/';out+=part;}part.clear();return true;};
    for(const auto* p=value;*p;p++){char c=*p;if(c==':')return {};if(c=='/'||c=='\\'){if(!flush())return {};}else part+=c>='A'&&c<='Z'?char(c+32):c;}
    if(!flush())return {};return out;
}
void parents(const std::string& path){for(size_t i=1;i<path.size();i++)if(path[i]=='/')mkdir(path.substr(0,i).c_str(),0777);}
bool match(const char* pat,const char* s){
    if(std::strcmp(pat,"*.*")==0)pat="*";
    const char* star=nullptr;const char* retry=nullptr;
    while(*s){if(*pat=='?'||*pat==*s){++pat;++s;}else if(*pat=='*'){star=pat++;retry=s;}else if(star){pat=star+1;s=++retry;}else return false;}
    while(*pat=='*')++pat;return !*pat;
}
}
extern "C" {
__attribute__((export_name("sdl_file_open"))) u32 browser_open(const char* raw,u32 write){const auto name=normalize(raw);if(name.empty())return ~0u;SDL_IOStream* f=nullptr;
    if(write){const auto path=save_root+"/"+name;parents(path);f=SDL_IOFromFile(path.c_str(),"wb");}
    else if(name=="thbgm.dat")f=th10_music_stream();
    else {f=SDL_IOFromFile((save_root+"/"+name).c_str(),"rb");if(!f)f=SDL_IOFromFile(("/game/"+name).c_str(),"rb");}
    if(!f)return ~0u;const auto id=next++;handles[id]=f;if(write)writers.insert(id);return id;
}
__attribute__((export_name("sdl_file_close"))) void browser_close(u32 id){auto it=handles.find(id);if(it!=handles.end()){SDL_CloseIO(it->second);handles.erase(it);if(writers.erase(id))browser_save_changed();}}
u32 browser_size(u32 id){auto it=handles.find(id);return it==handles.end()?~0u:u32(SDL_GetIOSize(it->second));}
__attribute__((export_name("sdl_file_seek"))) u32 browser_seek(u32 id,i32 offset,u32 origin){auto it=handles.find(id);return it==handles.end()||origin>2?~0u:u32(SDL_SeekIO(it->second,offset,static_cast<SDL_IOWhence>(origin)));}
__attribute__((export_name("sdl_file_read"))) u32 browser_read(u32 id,u8* out,u32 size){auto it=handles.find(id);return it==handles.end()?0:SDL_ReadIO(it->second,out,size);}
u32 browser_write(u32 id,const u8* in,u32 size){auto it=handles.find(id);return it==handles.end()?0:SDL_WriteIO(it->second,in,size);}
u32 browser_list(const char* directory,const char* pattern,u32 index,char* out,u32 capacity){
    const auto dir=normalize(directory);std::vector<std::string> names;
    for(const auto& root:{std::string("/game"),save_root})if(auto* d=opendir((root+"/"+dir).c_str())){
        while(auto* e=readdir(d)){const std::string name=e->d_name;if(name=="."||name==".."||!match(pattern,name.c_str()))continue;
            struct stat st{};if(!stat((root+"/"+dir+"/"+name).c_str(),&st)&&S_ISREG(st.st_mode))names.push_back(name);
        }closedir(d);
    }
    std::sort(names.begin(),names.end());names.erase(std::unique(names.begin(),names.end()),names.end());
    if(index>=names.size()||names[index].size()+1>capacity)return 0;std::memcpy(out,names[index].c_str(),names[index].size()+1);return 1;
}
__attribute__((export_name("sdl_files_root"))) void sdl_files_root(u32 chinese){save_root=chinese?"/savesth10/chs":"/savesth10/jp";parents(save_root+"/replay/");}
__attribute__((export_name("sdl_file_handles"))) u32 sdl_file_handles(){return handles.size();}
}
namespace th10 {
// Raw host-file read used by RuntimeOverride to reach /thcrap/th10/<relative>
// pack entries without going through the archive writer/reader bookkeeping.
bool sdl_read_file(const char* path,std::vector<u8>& out){
    if(!path||!*path)return false;
    std::size_t size=0;void* bytes=SDL_LoadFile(path,&size);
    if(!bytes)return false;
    out.assign(static_cast<u8*>(bytes),static_cast<u8*>(bytes)+size);
    SDL_free(bytes);return true;
}
// Decodes a thcrap texture-override PNG into tightly packed RGBA8. The ANM
// compositor re-encodes it to the surface's format, mirroring th08's port.
bool sdl_decode_rgba(const u8* bytes,u32 size,u32& width,u32& height,std::vector<u8>& rgba){
    if(!bytes||!size)return false;int w=0,h=0,channels=0;
    auto* pixels=stbi_load_from_memory(bytes,int(size),&w,&h,&channels,4);
    if(!pixels)return false;width=static_cast<u32>(w);height=static_cast<u32>(h);
    rgba.assign(pixels,pixels+static_cast<std::size_t>(w)*static_cast<std::size_t>(h)*4);
    stbi_image_free(pixels);return true;
}
}
