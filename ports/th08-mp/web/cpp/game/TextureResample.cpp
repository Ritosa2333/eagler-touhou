#include "TextureResample.hpp"
#include "TriangleCoefficients.hpp"
#include <cstdlib>
#include <array>
#include <memory>
namespace th08 {
namespace {
constexpr i32 invalid=static_cast<i32>(0x8876086cu),no_memory=static_cast<i32>(0x8007000eu);
u32 word(const u8* p){u32 n;std::memcpy(&n,p,4);return n;}
struct Channels {u32 masks[4],shifts[4];};
Channels channels(PixelFormat f){return {{f.red_mask,f.green_mask,f.blue_mask,f.alpha_mask},{f.red_shift,f.green_shift,f.blue_shift,f.alpha_shift}};}
// Positive, finite values only. Reproduce a single-precision x87 store with
// truncation while WebAssembly itself uses round-to-nearest conversions.
float truncate_positive(double exact){
    float rounded=static_cast<float>(exact);
    if(static_cast<double>(rounded)>exact){u32 bits;std::memcpy(&bits,&rounded,4);--bits;std::memcpy(&rounded,&bits,4);}
    return rounded;
}
struct FreeWeights{void operator()(u8* p)const{std::free(p);}};
struct WeightTable{
    const u8* data;bool owned;
    ~WeightTable(){if(owned)std::free(const_cast<u8*>(data));}
};
WeightTable weights(u32 from,u32 to,bool wrap){
    struct Entry{u32 from=0,to=0;bool wrap=false;Precision precision{};Rounding rounding{};u64 age=0;std::unique_ptr<u8,FreeWeights> data;};
    // Text lines repeatedly use the same two axis tables. Bound the cache to
    // 512 KiB and key the original arithmetic mode as well as the geometry.
    static std::array<Entry,8> cache;static u64 age=0;
    const auto precision=arithmetic_precision();const auto rounding=arithmetic_rounding();
    for(auto& e:cache)if(e.data&&e.from==from&&e.to==to&&e.wrap==wrap&&e.precision==precision&&e.rounding==rounding){e.age=++age;return {e.data.get(),false};}
    auto* p=TriangleCoefficients::create(from,to,wrap);if(!p||word(p)>65536)return {p,true};
    auto* compact=static_cast<u8*>(std::realloc(p,word(p)));if(!compact)return {p,true};p=compact;
    auto* oldest=&cache[0];for(auto& e:cache)if(e.age<oldest->age)oldest=&e;
    oldest->from=from;oldest->to=to;oldest->wrap=wrap;oldest->precision=precision;oldest->rounding=rounding;oldest->age=++age;oldest->data.reset(p);
    return {p,false};
}
}
bool TextureResample::valid(const PixelSurface& image,const TextureRect& r) noexcept {
    const auto f=TexturePixels::describe(image.format);
    return f.bytes&&image.pixels&&r.left>=0&&r.top>=0&&r.right>r.left&&r.bottom>r.top&&
        u32(r.right)<=image.width&&u32(r.bottom)<=image.height&&u64(image.width)*f.bytes<=image.pitch;
}
i32 TextureResample::point(PixelSurface& output,const TextureRect& dest,const PixelSurface& input,const TextureRect& source) noexcept {
    if(!valid(output,dest)||!valid(input,source))return invalid;
    const auto in=TexturePixels::describe(input.format),out=TexturePixels::describe(output.format);
    const u32 w=dest.right-dest.left,h=dest.bottom-dest.top,sw=source.right-source.left,sh=source.bottom-source.top;
    const auto c=channels(out);u32 mask=0;for(u32 i=0;i<4;++i)mask|=c.masks[i]<<c.shifts[i];
    for(u32 y=0;y<h;++y)for(u32 x=0;x<w;++x){
        auto* to=output.pixels+(dest.top+y)*output.pitch+(dest.left+x)*out.bytes;
        const auto* from=input.pixels+(source.top+u64(y)*sh/h)*input.pitch+(source.left+u64(x)*sw/w)*in.bytes;
        TexturePixels::convert(to,output.format,from,input.format,1);
        if(w!=sw||h!=sh){u32 pixel=0;std::memcpy(&pixel,to,out.bytes);pixel&=mask;std::memcpy(to,&pixel,out.bytes);}
    }
    return 0;
}
i32 TextureResample::triangle(PixelSurface& output,const TextureRect& dest,const PixelSurface& input,const TextureRect& source,bool wrap_x,bool wrap_y,bool dither) noexcept {
    if(!valid(output,dest)||!valid(input,source))return invalid;
    const auto in=TexturePixels::describe(input.format),out=TexturePixels::describe(output.format);
    const auto ic=channels(in),oc=channels(out);
    const u32 w=dest.right-dest.left,h=dest.bottom-dest.top,sw=source.right-source.left,sh=source.bottom-source.top;
    if(w==sw&&h==sh&&(!dither||input.format==output.format))return point(output,dest,input,source);
    if(u64(w)*h>0x1000000u)return no_memory;
    const auto horizontal_table=weights(sw,w,wrap_x),vertical_table=weights(sh,h,wrap_y);
    const auto* horizontal=horizontal_table.data;const auto* vertical=vertical_table.data;
    auto* pixels=static_cast<float*>(std::calloc(size_t(w)*h*4,sizeof(float)));
    const auto release=[&](){std::free(pixels);};
    if(!horizontal||!vertical||!pixels){release();return no_memory;}
    float levels[4][256];for(u32 c=0;c<4;++c){const auto mask=ic.masks[c];if(!mask){levels[c][0]=1;continue;}const float unit=1.0f/mask;for(u32 n=0;n<=mask;++n)levels[c][n]=(Extended::from_int(n)*number(unit)).to_float();}
    const auto precision=arithmetic_precision();const auto rounding=arithmetic_rounding();
    // The game's 24-bit, nearest-even mode is exactly native f32 arithmetic
    // here: channel values and positive filter weights stay in normal range.
    // Keep the software x87 path for every other precision/rounding setting.
    const bool native_accumulation=precision==Precision::Single&&rounding==Rounding::NearestEven;
    const auto* y_block=vertical+4;
    for(u32 sy=0;sy<sh;++sy){
        const auto* source_row=input.pixels+(source.top+sy)*input.pitch+source.left*in.bytes;
        const auto* y_end=y_block+word(y_block);const auto* x_block=horizontal+4;
        for(u32 sx=0;sx<sw;++sx){
            u32 packed=0;std::memcpy(&packed,source_row+sx*in.bytes,in.bytes);float rgba[4];
            for(u32 c=0;c<4;++c)rgba[c]=levels[c][(packed>>ic.shifts[c])&ic.masks[c]];
            const auto* x_end=x_block+word(x_block);
            if(w==sw&&h==sh)std::memcpy(pixels+(sy*w+sx)*4,rgba,16);
            else for(const auto* y=reinterpret_cast<const FilterWeight*>(y_block+4);reinterpret_cast<const u8*>(y)<y_end;++y)
                for(const auto* x=reinterpret_cast<const FilterWeight*>(x_block+4);reinterpret_cast<const u8*>(x)<x_end;++x){
                    auto* pixel=pixels+(y->index*w+x->index)*4;
                    if(native_accumulation){
                        const float weight=x->weight*y->weight;
                        for(u32 c=0;c<4;++c){const float contribution=weight*rgba[c];pixel[c]=contribution+pixel[c];}
                    }else{
                        const auto weight=number(x->weight)*number(y->weight);
                        for(u32 c=0;c<4;++c)pixel[c]=(weight*number(rgba[c])+number(pixel[c])).to_float();
                    }
                }
            x_block=x_end;
        }
        y_block=y_end;
    }
    // The original row writers temporarily select truncation for their x87
    // multiply, bias and integer conversion, preserving the precision setting.
    arithmetic_mode(precision,Rounding::TowardZero);
    for(u32 y=0;y<h;++y){
        const auto* values=pixels+y*w*4;auto* row=output.pixels+(dest.top+y)*output.pitch+dest.left*out.bytes;
        for(u32 x=0;x<w;++x){
            static constexpr u8 thresholds[4][4]={{31,15,27,11},{7,23,3,19},{25,9,29,13},{1,17,5,21}};
            const float bias=dither?thresholds[y&3][x&3]/32.0f:.5f;u32 packed=0;
            for(u32 c=0;c<4;++c){const auto max=oc.masks[c];const float v=values[x*4+c]<0?0:values[x*4+c]<1?values[x*4+c]:1;
                const auto n=precision==Precision::Single
                    ?static_cast<i32>(truncate_positive(static_cast<double>(truncate_positive(static_cast<double>(v)*max))+bias))
                    :number((number(v)*Extended::from_int(max)+number(bias)).to_float()).truncate_int();
                packed|=u32(n<0?0:n>i32(max)?max:n)<<oc.shifts[c];
            }
            std::memcpy(row+x*out.bytes,&packed,out.bytes);
        }
    }
    arithmetic_mode(precision,rounding);release();return 0;
}
}
