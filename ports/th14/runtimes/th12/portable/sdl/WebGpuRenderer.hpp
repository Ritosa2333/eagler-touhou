#pragma once
#include "Renderer.hpp"
#include "Th14RenderMath.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>

// The host owns GPU objects only. All game state, batching, packed-format
// decoding, vertex layouts, matrices and draw order remain in C++.
extern "C" {
int th14_webgpu_open(float* alphaBoundaries);
void th14_webgpu_close();
void th14_webgpu_upload(uint32_t,uint32_t,uint32_t,const void*);
void th14_webgpu_depth(uint32_t,uint32_t,uint32_t,uint32_t);
void th14_webgpu_draw(const void* packet,const void* vertices,uint32_t count);
void th14_webgpu_draw_instances(const void* packet,const void* vertices,uint32_t count,const void* instances,uint32_t instanceCount);
void th14_webgpu_clear(uint32_t,uint32_t,uint32_t,uint32_t,float,uint32_t,const int32_t*,uint32_t);
void th14_webgpu_copy(uint32_t,const int32_t*,uint32_t,const int32_t*);
void th14_webgpu_read(uint32_t,void*,uint32_t);
int th14_webgpu_resample(uint32_t,const int32_t*,uint32_t,const int32_t*,const float*,uint32_t,uint32_t,const uint32_t*,const uint32_t*);
void th14_webgpu_release(uint32_t);
void th14_webgpu_present(uint32_t);
}

namespace touhou::sdl {
class WebGpuRenderer {
    Renderer& renderer;
    struct Resource {u32 version=~0u;bool rendered=false;};
    std::map<u32,Resource> resources;
    std::vector<u8> pixels;
    struct Vertex {float position[4];u32 color;float uv[2],fog,eligible;u32 specular;};
    struct Instance {std::array<float,16> clip,eye;std::array<float,4> factor;float eligible;};
    static_assert(sizeof(Instance)==148);
    // Explicit 32-bit fields; no compiler-dependent bool/enum structure ABI.
    struct Packet {std::array<u32,20> pipeline{};std::array<float,92> uniform{};};
    static_assert(sizeof(Vertex)==40 && sizeof(Packet)==448);
    static u32 word(float value){u32 result;std::memcpy(&result,&value,4);return result;}
    static std::array<u32,4> mask(PixelFormat f){
        switch(f){case PixelFormat::Bgra8:return {255,255,255,255};case PixelFormat::Bgrx8:return {255,255,255,0};
        case PixelFormat::Rgb565:return {31,63,31,0};case PixelFormat::Xrgb1555:return {31,31,31,0};
        case PixelFormat::Argb1555:return {31,31,31,1};case PixelFormat::Argb4444:return {15,15,15,15};default:return {};}
    }
    static void integers(Packet& p,u32 offset,std::initializer_list<u32> values){std::memcpy(p.uniform.data()+offset,values.begin(),values.size()*4);}
    static void color(float* out,u32 value,bool reciprocal=false){for(u32 i=0;i<4;++i){const auto shift=i==3?24:16-i*8;volatile float channel=reciprocal?float((value>>shift)&255u)*(1.f/255.f):float((value>>shift)&255u)/255.f;out[i]=channel;}}
    static u32 argument(Argument a){return u32(a.source)|(a.complement?16u:0u)|(a.alphaOnly?32u:0u);}
    void target(u32 id,u32 depth,bool utility=false){
        const auto s=renderer.resolve(renderer.owner,id);
        // Packed render targets require post-blend quantization, which this
        // backend does not yet implement. Reject them instead of losing bits.
        if(!utility&&s.format!=PixelFormat::Bgra8&&s.format!=PixelFormat::Bgrx8){std::fprintf(stderr,"WebGPU packed raster target unsupported: handle=%u format=%u\n",id,u32(s.format));std::abort();}
        prepare(id);if(depth)prepare(depth);
    }
public:
    explicit WebGpuRenderer(Renderer& r):renderer(r){}
    bool initialize(float* boundaries){return th14_webgpu_open(boundaries)!=0;}
    ~WebGpuRenderer(){th14_webgpu_close();}
    void prepare(u32 id){
        const auto s=renderer.resolve(renderer.owner,id);if(!s.handle||!s.width||!s.height)std::abort();
        auto& resource=resources[id];if(resource.version==s.version)return;
        if(s.format>=PixelFormat::Depth16){th14_webgpu_depth(id,s.width,s.height,u32(s.format));resource.version=s.version;return;}
        pixels.resize(s.width*s.height*4);
        for(u32 y=0;y<s.height;++y)for(u32 x=0;x<s.width;++x){
            const auto* input=s.data+y*s.pitch;u32 r=0,g=0,b=0,a=255;
            switch(s.format){
            case PixelFormat::Bgra8:case PixelFormat::Bgrx8:input+=x*4;b=input[0];g=input[1];r=input[2];if(s.format==PixelFormat::Bgra8)a=input[3];break;
            case PixelFormat::Bgr8:input+=x*3;b=input[0];g=input[1];r=input[2];break;
            case PixelFormat::Rgb565:case PixelFormat::Xrgb1555:case PixelFormat::Argb1555:case PixelFormat::Argb4444:{
                uint16_t n;std::memcpy(&n,input+x*2,2);
                if(s.format==PixelFormat::Argb4444){b=(n&15)*17;g=(n>>4&15)*17;r=(n>>8&15)*17;a=(n>>12)*17;}
                else{const u32 bv=n&31,gv=s.format==PixelFormat::Rgb565?(n>>5&63):(n>>5&31),rv=s.format==PixelFormat::Rgb565?(n>>11):(n>>10&31);
                    b=(bv<<3)|(bv>>2);r=(rv<<3)|(rv>>2);g=s.format==PixelFormat::Rgb565?(gv<<2)|(gv>>4):(gv<<3)|(gv>>2);if(s.format==PixelFormat::Argb1555)a=n&0x8000?255:0;}
                break;}
            case PixelFormat::Alpha8:r=g=b=255;a=input[x];break;
            case PixelFormat::Luminance8:r=g=b=input[x];break;
            case PixelFormat::LuminanceAlpha8:r=g=b=input[x*2];a=input[x*2+1];break;
            default:std::abort();
            }
            auto* out=pixels.data()+(y*s.width+x)*4;out[0]=b;out[1]=g;out[2]=r;out[3]=a;
        }
        th14_webgpu_upload(id,s.width,s.height,pixels.data());renderer.stats.uploadBytes+=s.size;resource.version=s.version;resource.rendered=false;
    }
    void issue(const State& state,Topology topology,u32 count,const void* input,const void* indices,IndexType indexType,const void* instances,u32 instanceBytes){
        target(state.target,state.depth);if(state.texture)prepare(state.texture);
        const auto& p=state.pipeline;const auto& vp=state.viewport;
        const bool screenLine=state.layout.screen&&(topology==Topology::Lines||topology==Topology::LineStrip);
        const u32 repetitions=instances?instanceBytes/68:1;
        // Only Renderer::flush's identical WorldUv strip batch is eligible.
        // Preserve the serial path for every other topology and feedback draw.
        const bool batched=instances&&repetitions>1&&instanceBytes%68==0&&
            state.layout==attributes(VertexLayout::WorldUv)&&state.stride==20&&
            topology==Topology::Strip&&count==2&&!indices&&state.texture!=state.target;
        std::vector<Instance> records;if(batched)records.reserve(repetitions);
        for(u32 repetition=0;repetition<repetitions;++repetition){
            auto world=state.matrix[0];auto factor=p.textureFactor;
            if(instances){const auto* record=static_cast<const u8*>(instances)+repetition*68;std::memcpy(world.data(),record,64);std::memcpy(&factor,record+64,4);}
            const auto eye=th14_multiply(state.matrix[1],world),clip=th14_multiply(state.matrix[2],eye);
            Packet packet;packet.pipeline={state.target,state.depth,state.texture,u32(topology),u32(p.depthTest),u32(p.depthWrite),u32(p.blend),u32(p.sourceBlend),u32(p.destinationBlend),u32(p.blendEquation),u32(p.depthCompare),u32(p.cull),p.colorMask,u32(p.minFilter),u32(p.magFilter),u32(p.addressU),u32(p.addressV),word(vp.min),word(vp.max),0};
            // Keep the packet ABI; the reserved word carries separate alpha state.
            packet.pipeline[19]=p.separateAlphaBlend?(1u|(u32(p.sourceAlphaBlend)<<1)|(u32(p.destinationAlphaBlend)<<5)|(u32(p.alphaBlendEquation)<<9)):0u;
            // X8 samples and destination-alpha factors see one. Preserve the
            // uploaded/cleared opaque alpha through every draw and mask.
            if(renderer.resolve(renderer.owner,state.target).format==PixelFormat::Bgrx8)packet.pipeline[12]&=7u;
            color(packet.uniform.data(),factor);color(packet.uniform.data()+4,p.fogColor,true);
            integers(packet,8,{u32(p.fog),u32(p.alphaTest),u32(p.alphaCompare),u32(state.texture!=0)});
            const auto* boundaries=renderer.alpha_boundaries();packet.uniform[12]=boundaries[p.alphaReference];packet.uniform[13]=p.alphaReference==255?2.f:boundaries[p.alphaReference+1];
            integers(packet,16,{u32(p.color.operation),u32(p.alpha.operation),argument(p.color.first),argument(p.color.second)});
            integers(packet,20,{argument(p.alpha.first),argument(p.alpha.second),0,0});
            std::memcpy(packet.uniform.data()+24,clip.data(),64);std::memcpy(packet.uniform.data()+40,eye.data(),64);std::memcpy(packet.uniform.data()+56,state.matrix[3].data(),64);
            const bool nativeFog=p.fog&&!p.rangeFog&&p.fogMode==FogMode::Linear&&std::isfinite(p.fogNear)&&std::isfinite(p.fogFar)&&p.fogNear!=p.fogFar;
            if(nativeFog){volatile float range=p.fogFar-p.fogNear;volatile float scale=1.f/range;volatile float bias=p.fogFar*scale;packet.uniform[72]=bias;packet.uniform[73]=scale;}
            integers(packet,76,{u32(state.layout.screen),u32(p.textureTransform),u32(nativeFog),0});
            packet.uniform[80]=float(vp.x);packet.uniform[81]=float(vp.y);packet.uniform[82]=float(vp.width);packet.uniform[83]=float(vp.height);
            // The host adds a half-pixel viewport bias. For screen-space
            // lines it must precede homogeneous clipping: shifting only
            // the viewport clips a line entering its first pixel too early.
            if(screenLine){packet.uniform[80]-=.5f;packet.uniform[81]-=.5f;}
            packet.uniform[84]=p.fogNear;packet.uniform[85]=p.fogFar;packet.uniform[86]=p.fogDensity;
            integers(packet,88,{u32(p.depthTest&&state.depth&&renderer.resolve(renderer.owner,state.depth).format==PixelFormat::Depth16),u32(p.rangeFog),u32(p.fogMode),0});
            std::vector<u32> order;const u32 n=vertex_count(topology,count);
            auto index=[&](u32 i){if(!indices)return i;u32 value=0;if(indexType==IndexType::UInt16){uint16_t v;std::memcpy(&v,static_cast<const u8*>(indices)+i*2,2);value=v;}else std::memcpy(&value,static_cast<const u8*>(indices)+i*4,4);return value;};
            if(topology==Topology::Strip||topology==Topology::Fan){for(u32 i=0;i<count;++i){order.push_back(index(topology==Topology::Fan?0:(i&1?i+1:i)));order.push_back(index(topology==Topology::Fan?i+1:(i&1?i:i+1)));order.push_back(index(i+2));}packet.pipeline[3]=u32(Topology::Triangles);}
            else if(topology==Topology::LineStrip){for(u32 i=0;i<count;++i){order.push_back(index(i));order.push_back(index(i+1));}packet.pipeline[3]=u32(Topology::Lines);}
            else for(u32 i=0;i<n;++i)order.push_back(index(i));
            std::vector<Vertex> vertices(order.size());
            for(u32 i=0;i<order.size();++i){auto& v=vertices[i];const auto* bytes=static_cast<const u8*>(input)+order[i]*state.stride;std::memcpy(v.position,bytes,state.layout.screen?16:12);if(!state.layout.screen)v.position[3]=1;
                v.color=v.specular=0xffffffff;v.uv[0]=v.uv[1]=0;v.fog=1;v.eligible=0;
                if(state.layout.diffuse!=VertexAttributes::absent)std::memcpy(&v.color,bytes+state.layout.diffuse,4);
                if(state.layout.specular!=VertexAttributes::absent)std::memcpy(&v.specular,bytes+state.layout.specular,4);
                if(state.layout.uv!=VertexAttributes::absent)std::memcpy(v.uv,bytes+state.layout.uv,8);
                if(state.layout.screen){const float rhw=v.position[3];float x=std::nearbyint((v.position[0]-float(vp.x))*256.f)*(1.f/256.f),y=std::nearbyint((v.position[1]-float(vp.y))*256.f)*(1.f/256.f);if(screenLine){x+=.5f;y+=.5f;}v.position[0]=(x/float(vp.width)*2.f-1.f)/rhw;v.position[1]=-(y/float(vp.height)*2.f-1.f)/rhw;v.position[2]/=rhw;v.position[3]=1.f/rhw;v.fog=float(v.specular>>24)/255.f;}
            }
            if(!state.layout.screen&&packet.pipeline[3]==u32(Topology::Triangles)){
                const bool quad=topology==Topology::Strip&&count==2&&state.layout==attributes(VertexLayout::WorldUv)&&!indices;
                const bool quadEligible=quad&&th14_primitive_projectable(static_cast<const float*>(input),state.stride,0,4,clip,vp);
                for(u32 i=0;i+2<vertices.size();i+=3){bool eligible=quadEligible;if(!quad){float points[12];for(u32 j=0;j<3;++j)std::memcpy(points+j*4,vertices[i+j].position,16);eligible=th14_primitive_projectable(points,16,0,3,clip,vp);}for(u32 j=0;j<3;++j)vertices[i+j].eligible=eligible?1.f:0.f;}
            }
            if(batched){
                Instance record;record.clip=clip;record.eye=eye;
                std::memcpy(record.factor.data(),packet.uniform.data(),16);
                record.eligible=vertices[0].eligible;records.push_back(record);
                if(repetition+1==repetitions){
                    th14_webgpu_draw_instances(&packet,vertices.data(),u32(vertices.size()),records.data(),u32(records.size()));
                    renderer.stats.vertexUploadBytes+=u32(vertices.size()*sizeof(Vertex)+records.size()*sizeof(Instance));
                }
            }else{
                th14_webgpu_draw(&packet,vertices.data(),u32(vertices.size()));renderer.stats.vertexUploadBytes+=u32(vertices.size()*sizeof(Vertex));
            }
        }
        resources[state.target].rendered=true;
    }
    void clear(u32 flags,u32 color,float depth,u32 stencil,const i32* rects,u32 count){const auto& s=renderer.state;target(s.target,s.depth);if(renderer.resolve(renderer.owner,s.target).format==PixelFormat::Bgrx8)color|=0xff000000u;i32 box[]{i32(s.viewport.x),i32(s.viewport.y),i32(s.viewport.x+s.viewport.width),i32(s.viewport.y+s.viewport.height)};if(!rects){rects=box;count=1;}th14_webgpu_clear(s.target,s.depth,flags,color,depth,stencil,rects,count);resources[s.target].rendered=true;}
    void copy(u32 src,const i32* rect,u32 dst,const i32* point){const auto a=renderer.resolve(renderer.owner,src),b=renderer.resolve(renderer.owner,dst);if(a.format!=b.format&&b.format!=PixelFormat::Bgra8)std::abort();prepare(src);target(dst,0,true);th14_webgpu_copy(src,rect,dst,point);auto& d=resources[dst];d.rendered=true;d.version=b.version+1;}
    bool resample(u32 src,const i32* from,u32 dst,const i32* to,const float* weights,u32 width,u32 height){if(src==dst)return false;const auto a=mask(renderer.resolve(renderer.owner,src).format),b=mask(renderer.resolve(renderer.owner,dst).format);if(!a[0]||!b[0])return false;prepare(src);target(dst,0,true);if(!th14_webgpu_resample(src,from,dst,to,weights,width,height,a.data(),b.data()))return false;auto& d=resources[dst];d.rendered=true;d.version=renderer.resolve(renderer.owner,dst).version+1;++renderer.stats.resamples;return true;}
    void read(u32 id){auto it=resources.find(id);if(it==resources.end()||!it->second.rendered)return;const auto s=renderer.resolve(renderer.owner,id);pixels.resize(s.width*s.height*4);th14_webgpu_read(id,pixels.data(),u32(pixels.size()));renderer.stats.readBytes+=s.size;
        for(u32 y=0;y<s.height;++y)for(u32 x=0;x<s.width;++x){const auto* in=pixels.data()+(y*s.width+x)*4;auto* out=s.data+y*s.pitch;u32 b=in[0],g=in[1],r=in[2],a=in[3];
            if(s.format==PixelFormat::Bgra8||s.format==PixelFormat::Bgrx8){out+=x*4;std::memcpy(out,in,4);if(s.format==PixelFormat::Bgrx8)out[3]=255;}
            else if(s.format>=PixelFormat::Rgb565&&s.format<=PixelFormat::Argb4444){uint16_t v=s.format==PixelFormat::Argb4444?(a>>4)<<12|(r>>4)<<8|(g>>4)<<4|(b>>4):s.format==PixelFormat::Rgb565?(r>>3)<<11|(g>>2)<<5|(b>>3):(s.format==PixelFormat::Argb1555&&a>=128?0x8000:0)|(r>>3)<<10|(g>>3)<<5|(b>>3);std::memcpy(out+x*2,&v,2);}
            else if(s.format==PixelFormat::Alpha8)out[x]=a;else if(s.format==PixelFormat::Luminance8)out[x]=r;else std::abort();
        }it->second.rendered=false;
    }
    void release(u32 id){th14_webgpu_release(id);resources.erase(id);}
    void present(u32 id){prepare(id);th14_webgpu_present(id);}
};
}
