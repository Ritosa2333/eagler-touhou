#pragma once
#include "../game/Archive.hpp"
#include "../game/GameResources.hpp"
#include <SDL3/SDL.h>
#include <map>
namespace th09::sdl {
bool read_file(const char*,std::vector<u8>&);
class Assets final:public ResourceReader,private ArchiveSource {
    SDL_IOStream* file=nullptr;u32 length=0;Archive archive;std::map<std::string,std::vector<u8>> cache;u32 cached=0;
    std::string prepared_name;std::vector<u8> packed_bytes,prepared_bytes;std::unique_ptr<LzssStream> decoder;bool prepared_ready=false;
    u32 size()const override{return length;}
    bool read(u32 offset,u8* output,u32 count)override;
public:
    std::string error;
    ~Assets(){if(file)SDL_CloseIO(file);}
    bool open(const char* path);
    i32 prepare(const char*)override;void cancel_prepare()override;
    bool read(const char* name,std::vector<u8>&)override;
    const std::vector<ArchiveEntry>& entries()const{return archive.contents();}
};
}
