#include "Archive.hpp"
#include "AnmResource.hpp"
#include <emscripten.h>

namespace {
th17::Archive archive;
std::vector<th17::u8> output;
th17::Lzss decoder;
th17::AnmResource animation;
th17::u32 written=0;
}

extern "C" {
EMSCRIPTEN_KEEPALIVE int open_archive(const th17::u8* data,th17::u32 size){
    archive={};
    return archive.open(data,size)?int(archive.entries.size()):-1;
}
EMSCRIPTEN_KEEPALIVE int read_archive(th17::u32 index){
    return archive.read(index,output)?int(output.size()):-1;
}
EMSCRIPTEN_KEEPALIVE const char* archive_name(th17::u32 index){return archive.entries[index].name.c_str();}
EMSCRIPTEN_KEEPALIVE const th17::u8* output_bytes(){return output.data();}
EMSCRIPTEN_KEEPALIVE th17::u8* history_bytes(){return decoder.dictionary;}
EMSCRIPTEN_KEEPALIVE th17::u32 written_bytes(){return written;}
EMSCRIPTEN_KEEPALIVE int decode(const th17::u8* input,th17::u32 size,th17::u8* result,th17::u32 capacity){
    return decoder.decode(input,size,result,capacity,written);
}
EMSCRIPTEN_KEEPALIVE int open_animation(const th17::u8* data,th17::u32 size){
    animation={};
    return animation.open(data,size)?int(animation.alpha_bounds.size()):-1;
}
EMSCRIPTEN_KEEPALIVE const void* animation_bounds(){return animation.alpha_bounds.data();}
}
