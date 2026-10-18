#include "TriangleCoefficients.hpp"
#include <cmath>
#include <cstdlib>
namespace th10 {
namespace {
Extended integer(u32 value){auto n=Extended::from_int(static_cast<i32>(value));if(value&0x80000000u)n=n+number(4294967296.0f);return n;}
}
// D3DX 9.31 triangle integration. The two halves of each source triangle are
// integrated separately; contributions meeting at the center are accumulated
// before the tiny-weight threshold is applied.
u8* TriangleCoefficients::create(u32 source_size,u32 destination_size,bool wrap) noexcept {
    if(!source_size||!destination_size||source_size>65536||destination_size>65536)return nullptr;
    const float destination=integer(destination_size).to_float(),source=integer(source_size).to_float();
    const auto ratio=number(destination)/integer(source_size);
    const float scale=ratio.to_float(),half_step=(number(0.5f)/ratio).to_float();
    const u32 capacity=4+(source_size+4)*36+destination_size*32;
    auto* result=static_cast<u8*>(std::malloc(capacity));if(!result)return nullptr;
    u32 used=4,last=0;float accumulated=0;
    const auto emit=[&](){if(number(0.000009999999747378752f)<number(accumulated)){
        if(used+8>capacity)return false;const FilterWeight weight{last,accumulated};std::memcpy(result+used,&weight,8);used+=8;
    }return true;};
    for(u32 sample=0;sample<source_size;++sample){
        const u32 begin=used;used+=4;const float center=integer(sample).to_float();
        for(u32 half=0;half<2;++half){
            const auto origin_wide=integer(half)+number(center)-number(0.5f);const float origin=origin_wide.to_float();
            auto lower_wide=origin_wide*number(scale);float lower=lower_wide.to_float(),upper=(number(scale)+lower_wide).to_float();
            if(!wrap){if(lower_wide<number(0)){lower_wide=number(0);lower=0;}if(number(destination)<number(upper))upper=destination;}
            i32 pixel=Extended::from_double(std::floor(lower_wide.to_float())).truncate_int();
            for(auto start=Extended::from_int(pixel);start<number(upper);start=Extended::from_int(++pixel)){
                float end=(number(1)+start).to_float();const u32 index=pixel<0?static_cast<u32>(pixel)+destination_size:static_cast<u32>(pixel)>=destination_size?static_cast<u32>(pixel)-destination_size:static_cast<u32>(pixel);
                if(index!=last){if(!emit()){std::free(result);return nullptr;}accumulated=0;last=index;}
                if(start<number(lower))start=number(lower);if(number(upper)<number(end))end=upper;
                Extended weight;
                if(!wrap&&number(origin)<number(0))weight=number(1);
                else if(!wrap&&!(number(origin)+number(1)<number(source)))weight=number(0);
                else weight=(number(end)+start)*number(half_step)-number(origin);
                if(half)weight=number(1)-weight;
                accumulated=((number(end)-start)*weight+number(accumulated)).to_float();
            }
        }
        if(!emit()){std::free(result);return nullptr;}accumulated=0;const u32 length=used-begin;std::memcpy(result+begin,&length,4);
    }
    std::memcpy(result,&used,4);return result;
}
}
