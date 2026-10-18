#include "TriangleFilter.hpp"
namespace th10 {
namespace {bool ordinary(float value){u32 bits;std::memcpy(&bits,&value,4);bits&=0x7fffffff;return bits<0x7f800000&&(!bits||bits>=0x800000);}}
FilterProgress TriangleFilter::accumulate(const u8* column,const u8* end,const FilterWeight* y_begin,const FilterWeight* y_end,FilteredRow* rows,const float* source,u32 source_width) noexcept {
    for(u32 i=0;i<source_width*4;++i)if(!ordinary(source[i]))return {column,source};
    while(column<end){u32 bytes;std::memcpy(&bytes,column,4);const auto* next=column+bytes;if(next>=end)break;
        for(auto* y=y_begin;y<y_end;++y){auto* row=rows[y->index].rgba;for(auto* x=reinterpret_cast<const FilterWeight*>(column+4);reinterpret_cast<const u8*>(x)<next;++x){const auto weight=number(x->weight)*number(y->weight);auto* pixel=row+x->index*4;for(u32 channel=0;channel<4;++channel)pixel[channel]=(weight*number(source[channel])+number(pixel[channel])).to_float();}}
        column=next;source+=4;
    }
    // The final source column is left for the platform, preserving its final
    // floating status and continuation values at this development boundary.
    return {column,source};
}
bool TriangleFilter::saturate(float* rgba,u32 first,u32 width,const float* minimum) noexcept {
    if(!width)return true;
    for(u32 i=first*4;i<(width-1)*4;++i)if(!ordinary(rgba[i]))return false;
    for(u32 i=first*4;i<(width-1)*4;++i){const float value=rgba[i],low=minimum[i&3];rgba[i]=value<low?low:value<1?value:1;}
    return true;
}
}
