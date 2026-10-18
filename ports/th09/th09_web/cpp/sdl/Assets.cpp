#include "Assets.hpp"
#include "../game/ResourceCrypt.hpp"
#include "../game/RuntimeOverride.hpp"
#include <limits>
namespace th09::sdl {
bool read_file(const char* name,std::vector<u8>& out){
    SDL_IOStream* file=SDL_IOFromFile(name,"rb");if(!file)return false;const Sint64 size=SDL_GetIOSize(file);
    if(size<0||u64(size)>std::numeric_limits<u32>::max()){SDL_CloseIO(file);return false;}
    out.resize(size_t(size));const bool result=out.empty()||SDL_ReadIO(file,out.data(),out.size())==out.size();SDL_CloseIO(file);return result;
}
bool Assets::open(const char* path){
    cancel_prepare();cache.clear();cached=0;archive.close();if(file)SDL_CloseIO(file);file=SDL_IOFromFile(path,"rb");if(!file){error=SDL_GetError();return false;}
    const auto n=SDL_GetIOSize(file);if(n<=0||n>0xffffffffll){error="Invalid TH09 archive size";return false;}length=u32(n);
    if(!archive.open(*this)){error="Invalid TH09 resource archive";return false;}error.clear();return true;
}
bool Assets::read(u32 offset,u8* output,u32 count){return file&&offset<=length&&count<=length-offset&&SDL_SeekIO(file,offset,SDL_IO_SEEK_SET)==offset&&SDL_ReadIO(file,output,count)==count;}
bool Assets::read(const char* name,std::vector<u8>& out){
    if(!name)return false;const char* short_name=name;for(const char* p=name;*p;++p)if(*p=='/'||*p=='\\')short_name=p+1;
    // thcrap's virtual file wins over DAT entries, with Japanese DAT fallback.
    if(RuntimeOverride::Read(name,out))return true;
    if(prepared_ready&&prepared_name==short_name){out=std::move(prepared_bytes);cancel_prepare();return true;}
    const auto found=cache.find(short_name);if(found!=cache.end()){out=found->second;return true;}if(archive.read(short_name,out)){if(out.size()<=512*1024&&cached+out.size()<=12*1024*1024){cached+=out.size();cache.emplace(short_name,out);}return true;}error=std::string("Missing TH09 resource: ")+short_name;return false;
}
void Assets::cancel_prepare(){prepared_name.clear();packed_bytes.clear();prepared_bytes.clear();decoder.reset();prepared_ready=false;}
i32 Assets::prepare(const char* name){
    if(!name)return -1;if(cache.find(name)!=cache.end())return 1;if(prepared_name!=name){cancel_prepare();prepared_name=name;u32 size=0;if(!archive.packed(name,packed_bytes,size)){cancel_prepare();return -1;}prepared_bytes.resize(size);decoder=std::make_unique<LzssStream>();return 0;}
    if(prepared_ready)return 1;if(!decoder||!decoder->step(packed_bytes.data(),u32(packed_bytes.size()),prepared_bytes.data(),u32(prepared_bytes.size()))){cancel_prepare();return -1;}
    if(!decoder->done())return 0;if(decoder->size()!=prepared_bytes.size()||!resource_unwrap(prepared_bytes)){cancel_prepare();return -1;}prepared_ready=true;decoder.reset();packed_bytes.clear();return 1;
}

}
