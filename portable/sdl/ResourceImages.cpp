#include "ResourceImages.hpp"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_MAX_DIMENSIONS 8192
#include "third_party/stb_image.h"
namespace th12host {
bool apply_texture_overrides(AnmFile& file,std::string& error){
    for(size_t i=0;i<file.entries.size();++i){
        const auto path=host::paths().data_file(("overrides/"+file.name+"-"+std::to_string(i)+".png").c_str());
        if(!host::exists(path))continue;std::vector<u8> bytes;
        if(!host::read_file(path,bytes,64*1024*1024)){error="Cannot read texture override";return false;}
        int width=0,height=0,channels=0;
        if(!stbi_info_from_memory(bytes.data(),int(bytes.size()),&width,&height,&channels)||width<=0||height<=0||width>8192||height>8192){error="Invalid PNG texture override";return false;}
        auto* pixels=stbi_load_from_memory(bytes.data(),int(bytes.size()),&width,&height,&channels,4);
        if(!pixels){error="Cannot decode PNG texture override";return false;}
        auto& entry=file.entries[i];entry.has_image=true;entry.texture_width=unsigned(width);entry.texture_height=unsigned(height);
        entry.rgba.assign(pixels,pixels+size_t(width)*height*4);stbi_image_free(pixels);
    }
    return true;
}
}
