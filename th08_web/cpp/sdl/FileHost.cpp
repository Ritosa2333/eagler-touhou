#ifdef TH_NATIVE_PLATFORM
#include "PlatformHost.hpp"
#include "../platform/PlatformDevices.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <dirent.h>
#include <ctime>
#include <cctype>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include "../../../portable/sdl/third_party/stb_image.h"
EM_JS(void, th08_save_changed, (), { if(Module['runtimeFileChanged'])Module['runtimeFileChanged'](); });
EM_JS(void, th08_replay_error, (), { if(Module['runtimeNotice'])Module['runtimeNotice']('无法读取这份永夜抄录像。'); });
namespace th08 {
namespace {u32 eagler_music_source=1;}
extern "C" u32 sdl_music_source_mode(){return eagler_music_source;}
extern "C" __attribute__((export_name("sdl_music_source"))) void sdl_music_source(u32 mode){eagler_music_source=mode==2?2:1;}
namespace {
struct ArchiveFile final:ArchiveSource {
    SDL_IOStream* file=SDL_IOFromFile("/game/th08.dat","rb");
    ~ArchiveFile(){if(file)SDL_CloseIO(file);}
    u32 size()const override{const auto n=file?SDL_GetIOSize(file):-1;return n>0&&n<=128*1024*1024?u32(n):0;}
    bool read(u32 offset,u8* bytes,u32 count)override{return file&&SDL_SeekIO(file,offset,SDL_IO_SEEK_SET)==offset&&SDL_ReadIO(file,bytes,count)==count;}
};
bool save_name(const std::string& name){
    if(name=="score.dat"||name=="th08.cfg")return true;
    if(name.rfind("replay/th8_",0)||name.size()<16||name.size()>24||name.substr(name.size()-4)!=".rpy")return false;
    const auto id=name.substr(11,name.size()-15);if(id.size()!=2&&(id.size()!=6||id.substr(0,2)!="ud"))return false;
    for(char c:id)if(!std::isalnum(static_cast<unsigned char>(c)))return false;return true;
}
}
bool sdl_read_file(const char* name,std::vector<u8>& result){size_t size=0;auto* p=SDL_LoadFile(name,&size);if(!p)return false;result.assign(static_cast<u8*>(p),static_cast<u8*>(p)+size);SDL_free(p);return true;}
bool sdl_load_assets(BrowserRuntime& r){
    std::vector<u8> bytes;if(!r.mount_archive(std::make_unique<ArchiveFile>())||!r.native_fonts())return false;
    // OGG replaces the PCM archive, but the original configuration reader
    // still uses this format marker to select its WAV-shaped music owner.
    const u32 music_header[]{fourcc('Z','W','A','V'),1,0x800,0};
    if(!r.put("thbgm.dat",reinterpret_cast<const u8*>(music_header),sizeof(music_header)))return false;
    for(const auto* directory:{"/savesth08","/savesth08/replay"}){
        auto* dir=opendir(directory);if(!dir)continue;while(auto* entry=readdir(dir)){
            const auto relative=std::string(directory==std::string("/savesth08")?"":"replay/")+entry->d_name;
            if(save_name(relative)&&sdl_read_file((std::string(directory)+"/"+entry->d_name).c_str(),bytes))r.put(relative.c_str(),bytes.data(),bytes.size());
        }closedir(dir);
    }
    GameConfiguration configuration;const auto config=r.read("th08.cfg"),wave=r.read_prefix("thbgm.dat",16);
    if(!load_configuration(configuration,config.data(),config.size(),wave.empty()?nullptr:wave.data(),wave.size()))return false;
    r.app.library.force_16bit=configuration.options&4;return true;
}
bool sdl_prepare_asset(BrowserRuntime& r,u32 index){
    if(index>=r.resources().size())return false;const auto& name=r.resources()[index].name;const auto& bytes=r.file(name.c_str());
    if(bytes.empty()&&r.resources()[index].size)return false;
    const auto dot=name.rfind('.');const auto ext=dot==std::string::npos?"":name.substr(dot);
    // Endings are not expanded into dozens of RGBA images at startup. Their
    // compressed source stays available and is decoded when the scene needs it.
    if(name=="title00.png"||name=="select00.png"||name=="music.jpg"||name=="th08logo.jpg")return sdl_decode_image(r,name.c_str(),bytes);
    if(name=="title01.anm"||name=="resulttext.anm"||name=="result00.anm"||name=="music00.anm")return r.app.library.preload(bytes.data(),bytes.size());
    return true;
}
bool sdl_decode_image(BrowserRuntime& r,const char* name,const std::vector<u8>& bytes){int width=0,height=0,channels=0;auto* rgba=stbi_load_from_memory(bytes.data(),bytes.size(),&width,&height,&channels,4);if(!rgba)return false;
    const bool result=r.put_image(name,width,height,rgba,u32(width)*height*4);stbi_image_free(rgba);return result;}
bool sdl_decode_rgba(const u8* bytes,u32 size,u32& width,u32& height,std::vector<u8>& rgba){if(!bytes||!size)return false;int w=0,h=0,channels=0;auto* pixels=stbi_load_from_memory(bytes,int(size),&w,&h,&channels,4);if(!pixels)return false;
    width=u32(w);height=u32(h);rgba.assign(pixels,pixels+size_t(w)*h*4);stbi_image_free(pixels);return true;}
struct SDLFiles final:FileDevice {
 bool save(const char* path,const u8* bytes,u32 size)override{const auto name=BrowserRuntime::path(path);if(!save_name(name))return false;
    auto* stream=SDL_IOFromFile(("/savesth08/"+name).c_str(),"wb");if(!stream)return false;
    const auto written=SDL_WriteIO(stream,bytes,size);const bool closed=SDL_CloseIO(stream);if(written!=size||!closed)return false;th08_save_changed();return true;}
 void calendar(char* date,char* stamp)override{const auto now=std::time(nullptr);const auto* local=std::localtime(&now);if(!local)return;std::strftime(date,6,"%m/%d",local);std::strftime(stamp,20,"%y/%m/%d %H:%M:%S",local);}
 u32 milliseconds()override{return sdl_game_time();}u16 supplemental_input()override{return 0;}
 void replay_error()override{th08_replay_error();}
};
FileDevice& file_device(){static SDLFiles files;return files;}

}
#endif
