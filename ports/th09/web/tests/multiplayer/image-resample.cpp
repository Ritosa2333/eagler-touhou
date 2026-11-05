#include "../../cpp/game/ImageResample.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
using namespace th09;
using touhou::graphics::PixelFormat;

// Frozen pre-optimization authored image filter. Its owning table copies and
// fresh accumulator provide an independent byte oracle for cache eviction and
// workspace reuse; this test never loads or invokes an original executable.
namespace reference {
struct Channels {u32 bytes;std::array<u32,4> masks,shifts;};
Channels channels(PixelFormat p){switch(p){case PixelFormat::Bgra8:return {4,{255,255,255,255},{16,8,0,24}};case PixelFormat::Bgr8:return {3,{255,255,255,0},{16,8,0,0}};case PixelFormat::Rgb565:return {2,{31,63,31,0},{11,5,0,0}};case PixelFormat::Argb1555:return {2,{31,31,31,1},{10,5,0,15}};case PixelFormat::Argb4444:return {2,{15,15,15,15},{8,4,0,12}};default:return {};}}
struct Weight {u32 destination;float value;};
using Weights=std::vector<std::vector<Weight>>;
const Weights& weights(u32 source,u32 destination){
    static std::map<std::pair<u32,u32>,Weights> cache;const auto key=std::make_pair(source,destination);auto found=cache.find(key);if(found!=cache.end())return found->second;
    if(cache.size()>=16)cache.clear();Weights result(source);const float scale=float(destination)/float(source),half_step=.5f/scale;u32 last=0;float accumulated=0;
    for(u32 sample=0;sample<source;++sample){const auto emit=[&](){if(accumulated>.000009999999747378752f)result[sample].push_back({last,accumulated});};
        for(u32 half=0;half<2;++half){const float origin=float(half)+float(sample)-.5f,lower=origin*scale,upper=scale+lower;
            for(i32 pixel=i32(std::floor(lower));float(pixel)<upper;++pixel){float start=float(pixel),end=1.f+start;
                const u32 index=pixel<0?u32(pixel)+destination:u32(pixel)>=destination?u32(pixel)-destination:u32(pixel);
                if(index!=last){emit();accumulated=0;last=index;}start=std::max(start,lower);end=std::min(end,upper);
                float weight=(end+start)*half_step-origin;if(half)weight=1.f-weight;accumulated=(end-start)*weight+accumulated;
            }
        }emit();accumulated=0;
    }
    return cache.emplace(key,std::move(result)).first->second;
}
bool valid(const TextureImage& i,const ImageRect& r,Channels c){return c.bytes&&r.left>=0&&r.top>=0&&r.right>r.left&&r.bottom>r.top&&u32(r.right)<=i.width&&u32(r.bottom)<=i.height&&i.pitch>=i.width*c.bytes&&i.pixels.size()>=size_t(i.pitch)*i.height;}
float truncate_positive(double value){float n=float(value);if(double(n)>value){u32 bits;std::memcpy(&bits,&n,4);--bits;std::memcpy(&n,&bits,4);}return n;}
bool triangle(TextureImage& out,const ImageRect& d,const TextureImage& in,const ImageRect& s){
    const auto ic=channels(in.format),oc=channels(out.format);if(!valid(out,d,oc)||!valid(in,s,ic))return false;
    const u32 w=d.right-d.left,h=d.bottom-d.top,sw=s.right-s.left,sh=s.bottom-s.top;if(u64(w)*h>0x1000000)return false;
    const auto horizontal=weights(sw,w),vertical=weights(sh,h);std::vector<float> pixels(size_t(w)*h*4);
    float levels[4][256]{};for(u32 c=0;c<4;++c){const auto mask=ic.masks[c];if(!mask){levels[c][0]=1;continue;}const float unit=1.f/float(mask);for(u32 n=0;n<=mask;++n)levels[c][n]=float(n)*unit;}
    for(u32 y=0;y<sh;++y)for(u32 x=0;x<sw;++x){u32 packed=0;std::memcpy(&packed,in.pixels.data()+(s.top+y)*in.pitch+(s.left+x)*ic.bytes,ic.bytes);float rgba[4];for(u32 c=0;c<4;++c)rgba[c]=levels[c][(packed>>ic.shifts[c])&ic.masks[c]];
        if(w==sw&&h==sh){std::memcpy(pixels.data()+(y*w+x)*4,rgba,16);continue;}
        for(const auto& wy:vertical[y])for(const auto& wx:horizontal[x]){const float weight=wx.value*wy.value;auto* p=pixels.data()+(wy.destination*w+wx.destination)*4;for(u32 c=0;c<4;++c)p[c]=weight*rgba[c]+p[c];}
    }
    for(u32 y=0;y<h;++y)for(u32 x=0;x<w;++x){u32 packed=0;const auto* p=pixels.data()+(y*w+x)*4;for(u32 c=0;c<4;++c){const auto mask=oc.masks[c];const float v=std::clamp(p[c],0.f,1.f);const auto n=u32(truncate_positive(double(truncate_positive(double(v)*mask))+.5));packed|=std::min(n,mask)<<oc.shifts[c];}std::memcpy(out.pixels.data()+(d.top+y)*out.pitch+(d.left+x)*oc.bytes,&packed,oc.bytes);}
    return true;
}
}

namespace {
u32 cases=0;
[[noreturn]] void fail(const char* detail){
    std::fprintf(stderr,"Image resample case %u: %s\n",cases,detail);std::exit(1);
}
u32 next(u32& seed){seed=seed*1664525u+1013904223u;return seed;}
u32 pixel_bytes(PixelFormat format){
    switch(format){case PixelFormat::Bgra8:return 4;case PixelFormat::Bgr8:return 3;default:return 2;}
}
TextureImage image(u32 width,u32 height,PixelFormat format,u32 padding,u32 seed){
    TextureImage out;out.width=width;out.height=height;out.format=format;out.pitch=width*pixel_bytes(format)+padding;
    out.pixels.resize(size_t(out.pitch)*height);for(auto& byte:out.pixels)byte=u8(next(seed)>>24);return out;
}
void identical(const TextureImage& actual,const TextureImage& expected){
    if(actual.width!=expected.width||actual.height!=expected.height||actual.pitch!=expected.pitch||actual.format!=expected.format||actual.pixels.size()!=expected.pixels.size())fail("image metadata changed");
    if(actual.pixels!=expected.pixels){
        size_t at=0;while(at<actual.pixels.size()&&actual.pixels[at]==expected.pixels[at])++at;
        std::fprintf(stderr,"First different byte: %zu (%u != %u)\n",at,unsigned(actual.pixels[at]),unsigned(expected.pixels[at]));fail("output differs from frozen filter");
    }
}
void compare(const TextureImage& source,const ImageRect& s,const TextureImage& destination,const ImageRect& d,bool accepted=true){
    ++cases;const auto before=source.pixels;auto expected=destination,actual=destination;
    if(reference::triangle(expected,d,source,s)!=accepted)fail("unexpected reference acceptance");
    if(ImageResample::triangle(actual,d,source,s)!=accepted)fail("unexpected filter acceptance");
    identical(actual,expected);if(source.pixels!=before)fail("source pixels changed");
    if(!accepted)identical(actual,destination);
}
void compare_size(u32 sw,u32 sh,u32 w,u32 h,PixelFormat source_format,PixelFormat destination_format,u32 seed){
    // Nonzero rectangle offsets and odd pitch padding also compare every
    // untouched border/padding byte, not only the resampled rectangle.
    const auto source=image(sw+3,sh+4,source_format,7,seed);
    const auto destination=image(w+5,h+3,destination_format,11,seed^0x9e3779b9u);
    compare(source,{1,2,i32(sw)+1,i32(sh)+2},destination,{2,1,i32(w)+2,i32(h)+1});
}
}

int main(){
    // These must be the first calls: 15 unique square axis pairs fill 15 cache
    // entries. The next call inserts horizontal entry 16, then vertical entry
    // 17 clears the cache before any horizontal samples are consumed.
    for(u32 n=2;n<=16;++n)compare_size(n,n,n+1,n+1,PixelFormat::Argb1555,PixelFormat::Bgra8,n);
    compare_size(17,18,18,19,PixelFormat::Argb1555,PixelFormat::Bgra8,0x31415926);
    compare_size(17,18,18,19,PixelFormat::Argb1555,PixelFormat::Bgra8,0x27182818);

    constexpr PixelFormat formats[]={PixelFormat::Bgra8,PixelFormat::Bgr8,PixelFormat::Rgb565,PixelFormat::Argb1555,PixelFormat::Argb4444};
    constexpr u32 sizes[][4]={{1,1,1,1},{1,1,7,5},{7,5,1,1},{3,5,8,9},{8,9,3,5},{13,7,13,7},{2,31,19,3},{31,2,3,19},{64,33,17,9}};
    u32 seed=0x12345678;
    for(const auto source_format:formats)for(const auto destination_format:formats)for(const auto& size:sizes)
        compare_size(size[0],size[1],size[2],size[3],source_format,destination_format,next(seed));

    // Equal-size conversion exactly at the retained-workspace threshold,
    // beyond it, then back; compare all pixels and untouched padding bytes.
    for(const u32 w:{512u,513u,512u})compare_size(w,512,w,512,PixelFormat::Argb1555,PixelFormat::Bgra8,next(seed));

    // Large text-shaped calls, repeated hot dimensions, and smaller/larger
    // intervening calls catch stale accumulation after capacity reuse.
    constexpr u32 reused[][4]={{1024,130,512,64},{2,3,1,2},{1024,130,512,64},{258,32,128,15},{1,1,1,1},{258,32,128,15}};
    for(const auto& size:reused)compare_size(size[0],size[1],size[2],size[3],PixelFormat::Argb1555,PixelFormat::Bgra8,next(seed));
    for(u32 trial=0;trial<128;++trial){
        const u32 sw=1+next(seed)%37,sh=1+next(seed)%37,w=1+next(seed)%37,h=1+next(seed)%37;
        compare_size(sw,sh,w,h,formats[trial%5],formats[(trial/5)%5],next(seed));
    }

    // All source samples must still be consumed before an overlapping
    // destination rectangle can overwrite them.
    for(const auto format:formats){
        ++cases;auto actual=image(20,16,format,5,next(seed)),expected=actual;
        const ImageRect s{1,2,15,11},d{4,1,13,13};
        if(!reference::triangle(expected,d,expected,s)||!ImageResample::triangle(actual,d,actual,s))fail("in-place image rejected");
        identical(actual,expected);
    }

    auto source=image(12,10,PixelFormat::Bgra8,3,next(seed));
    auto destination=image(16,12,PixelFormat::Argb1555,5,next(seed));
    const ImageRect s{1,1,9,8},d{2,1,12,10};
    compare(source,{-1,1,9,8},destination,d,false);
    compare(source,{1,1,1,8},destination,d,false);
    compare(source,s,destination,{2,1,17,10},false);
    source.pitch=source.width*4-1;compare(source,s,destination,d,false);
    source=image(12,10,PixelFormat::Bgra8,3,next(seed));source.pixels.pop_back();compare(source,s,destination,d,false);
    source=image(12,10,PixelFormat::Bgra8,3,next(seed));source.format=static_cast<PixelFormat>(255);compare(source,s,destination,d,false);
    std::printf("TH09 image resample: %u byte-exact cache/workspace/format cases PASS\n",cases);
}
