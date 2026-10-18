#include "ImageResample.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <memory>
namespace th09 {
namespace {
struct Channels {u32 bytes;std::array<u32,4> masks,shifts;};
Channels channels(touhou::graphics::PixelFormat p){using touhou::graphics::PixelFormat;switch(p){case PixelFormat::Bgra8:return {4,{255,255,255,255},{16,8,0,24}};case PixelFormat::Bgr8:return {3,{255,255,255,0},{16,8,0,0}};case PixelFormat::Rgb565:return {2,{31,63,31,0},{11,5,0,0}};case PixelFormat::Argb1555:return {2,{31,31,31,1},{10,5,0,15}};case PixelFormat::Argb4444:return {2,{15,15,15,15},{8,4,0,12}};default:return {};}}
struct Weight {u32 destination;float value;};
using Weights=std::vector<std::vector<Weight>>;
std::shared_ptr<const Weights> weights(u32 source,u32 destination){
    static std::map<std::pair<u32,u32>,std::shared_ptr<const Weights>> cache;const auto key=std::make_pair(source,destination);auto found=cache.find(key);if(found!=cache.end())return found->second;
    if(cache.size()>=16)cache.clear();auto result=std::make_shared<Weights>(source);const float scale=float(destination)/float(source),half_step=.5f/scale;u32 last=0;float accumulated=0;
    for(u32 sample=0;sample<source;++sample){const auto emit=[&](){if(accumulated>.000009999999747378752f)(*result)[sample].push_back({last,accumulated});};
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
}
bool ImageResample::triangle(TextureImage& out,const ImageRect& d,const TextureImage& in,const ImageRect& s){
    const auto ic=channels(in.format),oc=channels(out.format);if(!valid(out,d,oc)||!valid(in,s,ic))return false;
    const u32 w=d.right-d.left,h=d.bottom-d.top,sw=s.right-s.left,sh=s.bottom-s.top;if(u64(w)*h>0x1000000)return false;
    // Each axis retains its immutable table if preparing the other axis evicts
    // the bounded cache. Cache hits do not copy the nested weight vectors.
    const auto horizontal=weights(sw,w),vertical=weights(sh,h);
    // Like the weight cache, this workspace is used by the serial image path
    // without callbacks. Keep up to 4 MiB for text; whole-texture replacements
    // use temporary storage so a large mod image cannot pin its peak buffer.
    constexpr size_t retained_limit=1u<<20;const size_t count=size_t(w)*h*4;
    static std::vector<float> workspace;std::vector<float> temporary;
    auto& pixels=count<=retained_limit?workspace:temporary;
    // Construct exact-size storage when growing, avoiding geometric growth
    // past the retained limit. Clear every active float on all reuse paths.
    if(count>pixels.capacity())std::vector<float>(count).swap(pixels);else pixels.assign(count,0.f);
    float levels[4][256]{};for(u32 c=0;c<4;++c){const auto mask=ic.masks[c];if(!mask){levels[c][0]=1;continue;}const float unit=1.f/float(mask);for(u32 n=0;n<=mask;++n)levels[c][n]=float(n)*unit;}
    for(u32 y=0;y<sh;++y)for(u32 x=0;x<sw;++x){u32 packed=0;std::memcpy(&packed,in.pixels.data()+(s.top+y)*in.pitch+(s.left+x)*ic.bytes,ic.bytes);float rgba[4];for(u32 c=0;c<4;++c)rgba[c]=levels[c][(packed>>ic.shifts[c])&ic.masks[c]];
        if(w==sw&&h==sh){std::memcpy(pixels.data()+(y*w+x)*4,rgba,16);continue;}
        for(const auto& wy:(*vertical)[y])for(const auto& wx:(*horizontal)[x]){const float weight=wx.value*wy.value;auto* p=pixels.data()+(wy.destination*w+wx.destination)*4;for(u32 c=0;c<4;++c)p[c]=weight*rgba[c]+p[c];}
    }
    for(u32 y=0;y<h;++y)for(u32 x=0;x<w;++x){u32 packed=0;const auto* p=pixels.data()+(y*w+x)*4;for(u32 c=0;c<4;++c){const auto mask=oc.masks[c];const float v=std::clamp(p[c],0.f,1.f);const auto n=u32(truncate_positive(double(truncate_positive(double(v)*mask))+.5));packed|=std::min(n,mask)<<oc.shifts[c];}std::memcpy(out.pixels.data()+(d.top+y)*out.pitch+(d.left+x)*oc.bytes,&packed,oc.bytes);}
    return true;
}
}
