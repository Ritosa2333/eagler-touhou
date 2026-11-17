#include "ReplayCompression.hpp"

#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES
#include "../third_party/miniz/miniz.c"

namespace th09::replay_compression {
std::vector<u8> deflate(const u8* data, u32 size){
    mz_ulong length=mz_compressBound(size);
    std::vector<u8> output(length);
    if(mz_compress2(output.data(),&length,data,size,MZ_BEST_COMPRESSION)!=MZ_OK)return {};
    output.resize(length);
    return output;
}
bool inflate(const u8* data, u32 size, u8* output, u32 output_size){
    mz_ulong consumed=size,written=output_size;
    return mz_uncompress2(output,&written,data,&consumed)==MZ_OK&&written==output_size&&consumed==size;
}
}
