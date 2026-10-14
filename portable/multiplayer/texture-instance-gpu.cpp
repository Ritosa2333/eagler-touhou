// Real SDL/GLES Renderer fixture. One identical translation unit is linked
// separately with the pinned reference Renderer and the candidate Renderer.
#include "Renderer.hpp"
#include "RasterLiveness.hpp"
#include <emscripten.h>
#include <array>
#include <cstdio>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace touhou::sdl;
namespace {
constexpr u32 size=512,clearColor=0xff132537;
struct Image{Surface surface;std::vector<u8> bytes;};
std::map<u32,Image> images;
std::unique_ptr<Renderer> renderer;
std::vector<u8> result;
std::string failure;
std::array<u32,24> metadata{};
u32 requested=0;bool queueMode=false,rasterMode=false;
const char* names[]={
    "singleton-uniform","existing-same-uv-instancing","atlas-third-column",
    "full-texture-matrices","ordered-alpha-overlap","fog-depth-overlap",
    "shared-state-boundaries","original-quad-boundary","vao-singleton-transitions",
    "instance-limit-1023","instance-limit-1024","instance-limit-1025",
    "clear-read-target-resource-boundaries","texture-transform-disabled","generic-program",
    "uniform-cache-return-to-instance-values","fresh-vao-quad-cache-and-screen-invalidation",
    "resident-vertices-exact-change-and-order","resident-instances-and-singleton-vao",
    "resident-shared-model-quad-across-many-draws",
    "queued-owner-data-and-ordered-state","queued-mixed-strides-and-instance-first",
    "queued-command-bound-and-discard","raster-overwritten-draw",
    "raster-required-texture-read","raster-overwritten-texture-reader",
    "raster-partial-clear-preserves-outside","raster-color-clear-preserves-depth",
    "raster-capacity-fence","raster-screen-bounds-in-partial-clear",
    "raster-screen-bounds-crossing-partial-clear","raster-nonunit-rhw-fallback"
};
struct Vertex{float x,y,z,u,v;};
static_assert(sizeof(Vertex)==20,"exact original WorldUv stride");
const Vertex quad[]={{-1,-1,0,0,0},{1,-1,0,1,0},{-1,1,0,0,1},{1,1,0,1,1}};
static_assert(sizeof quad==80,"exact original instancing quad");
Surface resolve_image(void*,u32 id){
    const auto it=images.find(id);return it==images.end()?Surface{}:it->second.surface;
}
void image(u32 id,PixelFormat format,u32 width=size,u32 height=size){
    auto& im=images[id];const u32 bpp=format==PixelFormat::Depth16?2:4;
    im.bytes.assign(width*height*bpp,0);
    im.surface={id,width,height,format,width*bpp,im.bytes.data(),u32(im.bytes.size()),0};
}
void paint_atlas(u32 id,u32 seed){
    auto& im=images.at(id);
    for(u32 y=0;y<im.surface.height;++y)for(u32 x=0;x<im.surface.width;++x){
        const u32 offset=(y*im.surface.width+x)*4;
        im.bytes[offset]=u8((x*7+y*3+seed*11)&255);
        im.bytes[offset+1]=u8((y*5+((x/8)^seed)*29)&255);
        im.bytes[offset+2]=u8((x*3+y*11+seed*47)&255);
        im.bytes[offset+3]=u8(48+((x/4+y/4+seed)*17)%208);
    }
    ++im.surface.version;
}
bool gpu_ok(const char* phase){
    const auto error=glGetError();
    if(error!=GL_NO_ERROR){failure=std::string(phase)+" GL error "+std::to_string(error);return false;}
    return true;
}
void reset_state(u32 target=1){
    renderer->state=State{};auto& s=renderer->state;
    s.target=target;s.depth=target==5?6:2;s.texture=3;
    s.viewport={0,0,size,size,0,1};s.layout=attributes(VertexLayout::WorldUv);
    auto& p=s.pipeline;
    p.textureTransform=true;p.color.operation=p.alpha.operation=ColorOperation::Multiply;
    p.color.first=p.alpha.first={ArgumentSource::Texture};
    p.color.second=p.alpha.second={ArgumentSource::Factor};
    p.textureFactor=0xffffffff;p.depthTest=false;p.depthWrite=false;
    p.minFilter=p.magFilter=Filter::Nearest;p.addressU=p.addressV=Address::Repeat;
}
void texture_matrix(u32 i,bool full=false){
    auto& m=renderer->state.matrix[3];m={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    m[0]=.21875f;m[5]=.4375f;m[8]=float(i%4)*.25f+.015625f;m[9]=float((i/4)%2)*.5f+.03125f;
    if(full){
        m[0]=.3125f+float(i%3)*.03125f;m[1]=-.0625f;m[2]=.125f;m[3]=.25f;
        m[4]=.09375f;m[5]=-.34375f;m[6]=.1875f;m[7]=-.25f;
        m[8]=float(i%5)*.1875f-.125f;m[9]=float(i%3)*.3125f+.0625f;m[10]=.75f;m[11]=.5f;
        m[12]=.375f;m[13]=-.5f;m[14]=.625f;m[15]=.875f;
    }
}
void world(u32 i,u32 columns,u32 rows,bool overlap=false){
    auto& m=renderer->state.matrix[0];m={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    m[0]=overlap?.58f:.86f/float(columns);m[5]=overlap?.58f:.86f/float(rows);
    m[12]=overlap?float(int(i%3)-1)*.0625f:-.95f+(float(i%columns)+.5f)*1.9f/float(columns);
    m[13]=overlap?float(int(i%4)-2)*.046875f:-.95f+(float(i/columns)+.5f)*1.9f/float(rows);
    m[14]=.125f+float(i%8)*.09375f;
}
void factor(u32 i){renderer->state.pipeline.textureFactor=(u32(96+(i%5)*31)<<24)|((37+i*23)%256)<<16|((83+i*17)%256)<<8|((151+i*31)%256);}
void draw(u32 i,u32 columns=4,u32 rows=2,bool full=false,bool overlap=false,const Vertex* vertices=quad){
    texture_matrix(i,full);world(i,columns,rows,overlap);factor(i);
    renderer->draw(Topology::Strip,2,vertices,sizeof(Vertex));++requested;
}
void capture(u32 target){
    renderer->read(target);const auto& im=images.at(target);result.insert(result.end(),im.bytes.begin(),im.bytes.end());
}
void screen_triangle(){
    struct ScreenVertex{float x,y,z,w;u32 color;float u,v;};
    static_assert(sizeof(ScreenVertex)==28,"real ScreenColorUv stride");
    const ScreenVertex vertices[]={{40,40,.2f,1,0x8f123456,0,0},{160,40,.2f,1,0x8fabcdef,1,0},{40,140,.2f,1,0x8f876543,0,1}};
    renderer->state.layout=attributes(VertexLayout::ScreenColorUv);
    renderer->draw_batch(1,vertices,sizeof(ScreenVertex));++requested;
    renderer->state.layout=attributes(VertexLayout::WorldUv);
}
void checkpoint(u32 phase){
    if(rasterMode)renderer->read(1);
    renderer->flush();metadata[12+phase*2]=renderer->stats.bufferReplacements;
    metadata[13+phase*2]=renderer->stats.vertexUploadBytes;metadata[20+phase]=renderer->stats.layoutSetups;
}
bool initialize(){
    RasterNeeded needed;needed.full={0,0,32,32};needed.require_all();
    std::array<bool,1024> live;live.fill(true);u32 random=714;
    for(unsigned i=0;i<600;++i){
        random=random*1664525u+1013904223u;const i32 x=random%32;
        random=random*1664525u+1013904223u;const i32 y=random%32;
        const RasterRect cut{x,y,x+2,y+3};needed.subtract(cut);
        for(i32 yy=y;yy<std::min(y+3,32);++yy)for(i32 xx=x;xx<std::min(x+2,32);++xx)live[yy*32+xx]=false;
        for(i32 yy=0;yy<32;++yy)for(i32 xx=0;xx<32;++xx)if(live[yy*32+xx]&&!needed.intersects({xx,yy,xx+1,yy+1})){failure="region subtraction lost live pixel";return false;}
    }
    needed.subtract(needed.full);if(needed.count){failure="full clear did not retire live region";return false;}
    failure.clear();SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR,"#screen");
    renderer=std::make_unique<Renderer>(10,resolve_image,nullptr);
    if(!renderer->initialize()){failure=renderer->error();return false;}
    image(1,PixelFormat::Bgra8);image(2,PixelFormat::Depth16);
    image(3,PixelFormat::Bgra8,64,64);image(4,PixelFormat::Bgra8,64,64);
    image(5,PixelFormat::Bgra8);image(6,PixelFormat::Depth24);
    paint_atlas(3,1);paint_atlas(4,7);reset_state();
    GLint attributes=0;glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&attributes);
    if(attributes<13){failure="GPU exposes fewer than 13 vertex attributes";return false;}
    metadata[11]=u32(attributes);return gpu_ok("initialize");
}
bool run(u32 id){
    if(!renderer||id>=sizeof(names)/sizeof(names[0])){failure="invalid fixture invocation";return false;}
    failure.clear();result.clear();requested=0;renderer->discard();paint_atlas(3,1);paint_atlas(4,7);
    renderer->residentUploads=id>=17;
    renderer->queuedUploads=queueMode;
    renderer->deferredRaster=rasterMode;renderer->rasterStats={};
    for(u32 i=12;i<metadata.size();++i)metadata[i]=0;
    reset_state(5);renderer->clear(3,clearColor,1,0);
    reset_state();renderer->clear(3,clearColor,1,0);
    if(id==15){
        // Establish known non-quad VBO contents without a private cache reset.
        screen_triangle();renderer->clear(3,clearColor,1,0);reset_state();requested=0;
    }
    renderer->stats={};
    if(!gpu_ok("case start"))return false;
    switch(id){
    case 0:draw(0,1,1,true);break;
    case 1:
        for(u32 i=0;i<8;++i){texture_matrix(0);world(i,4,2);factor(i);renderer->draw(Topology::Strip,2,quad,20);++requested;}break;
    case 2:for(u32 i=0;i<8;++i)draw(i);break;
    case 3:for(u32 i=0;i<8;++i)draw(i,4,2,true);break;
    case 4:
        renderer->state.pipeline.blend=true;renderer->state.pipeline.sourceBlend=BlendFactor::SourceAlpha;
        renderer->state.pipeline.destinationBlend=BlendFactor::InverseSourceAlpha;
        for(u32 i=0;i<12;++i)draw(i,4,3,true,true);break;
    case 5:{
        auto& p=renderer->state.pipeline;p.depthTest=p.depthWrite=p.fog=p.rangeFog=true;
        p.depthCompare=Compare::LessEqual;p.fogMode=FogMode::Linear;p.fogNear=.125f;p.fogFar=.875f;p.fogColor=0xff765432;
        for(u32 i=0;i<12;++i)draw(i,4,3,false,true);break;}
    case 6:
        for(u32 boundary=0;boundary<16;++boundary){
            reset_state();auto& s=renderer->state;auto& p=s.pipeline;
            switch(boundary){
            case 1:s.texture=4;break;
            case 2:s.matrix[1][12]=.03125f;break;
            case 3:s.matrix[2][0]=.9375f;break;
            case 4:s.viewport={8,8,496,496,.0625f,.9375f};break;
            case 5:p.minFilter=p.magFilter=Filter::Linear;break;
            case 6:p.addressU=Address::Mirror;p.addressV=Address::Clamp;break;
            case 7:p.fog=true;p.fogMode=FogMode::Linear;p.fogNear=.125f;p.fogFar=.875f;p.fogColor=0xff123456;break;
            case 8:p.depthWrite=p.depthTest=true;p.depthCompare=Compare::Less;break;
            case 9:p.blend=true;p.sourceBlend=BlendFactor::One;p.destinationBlend=BlendFactor::One;break;
            case 10:p.blend=true;p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::InverseSourceAlpha;p.blendEquation=BlendEquation::ReverseSubtract;break;
            case 11:p.colorMask=7;break;
            case 12:p.alphaTest=true;p.alphaReference=99;p.alphaCompare=Compare::GreaterEqual;break;
            case 13:p.textureTransform=false;break;
            case 14:s.target=5;s.depth=6;break;
            case 15:p.cull=Cull::Front;break;
            }
            for(u32 j=0;j<2;++j)draw(boundary*2+j,8,4,true);
        }
        break;
    case 7:
        for(u32 group=0;group<4;++group){
            Vertex changed[4];std::memcpy(changed,quad,sizeof quad);changed[0].u=float(group)*.0625f;
            for(u32 j=0;j<2;++j)draw(group*2+j,4,2,true,false,changed);
        }break;
    case 8:{
        draw(0);renderer->flush();
        for(u32 i=1;i<5;++i)draw(i);
        struct ScreenVertex{float x,y,z,w;u32 color;float u,v;};
        const ScreenVertex screen[]={ {40,40,.2f,1,0x8f123456,0,0},{160,40,.2f,1,0x8fabcdef,1,0},{40,140,.2f,1,0x8f876543,0,1} };
        renderer->state.layout=attributes(VertexLayout::ScreenColorUv);renderer->draw_batch(1,screen,sizeof(ScreenVertex));++requested;
        renderer->state.layout=attributes(VertexLayout::WorldUv);
        for(u32 i=5;i<8;++i)draw(i);
        renderer->flush();draw(2,4,2,true);break;}
    case 9:case 10:case 11:{
        const u32 count=id==9?1023:id==10?1024:1025;
        for(u32 i=0;i<count;++i)draw(i,33,32);break;}
    case 12:{
        for(u32 i=0;i<8;++i)draw(i);
        capture(1); // A real read flushes pending geometry before mutation.
        paint_atlas(3,4);
        const i32 rectangle[]={0,0,256,256};renderer->clear(3,0xff395775,1,0,rectangle,1);
        for(u32 i=0;i<8;++i)draw(i,4,2,true);
        capture(1);
        reset_state(5);renderer->state.texture=1;
        for(u32 i=0;i<4;++i)draw(i,2,2,true);
        break;}
    case 13:
        renderer->state.pipeline.textureTransform=false;for(u32 i=0;i<8;++i)draw(i,4,2,true);break;
    case 14:
        renderer->state.pipeline.color.operation=ColorOperation::Add;
        renderer->state.pipeline.alpha.operation=ColorOperation::Second;
        renderer->state.pipeline.blend=true;renderer->state.pipeline.sourceBlend=BlendFactor::SourceAlpha;
        renderer->state.pipeline.destinationBlend=BlendFactor::InverseSourceAlpha;
        for(u32 i=0;i<8;++i)draw(i,4,2,true);break;
    case 15:
        draw(0,4,2,true);checkpoint(0); // Uniforms hold A.
        draw(1,4,2,true);draw(2,4,2,true);checkpoint(1);capture(1);
        // Erase the prior image so accidentally drawing A again cannot hide
        // behind the old B pixels. The singleton equals the first instance B.
        renderer->clear(3,clearColor,1,0);draw(1,4,2,true);checkpoint(2);break;
    case 16:
        // The runner executes this case FIRST in each newly initialized module.
        // The instance buffer remains bound before the new singleton VAO:
        // a reused quad must still bind vertices.id before its attrib pointers.
        draw(1,4,2,true);draw(2,4,2,true);checkpoint(0);capture(1);
        renderer->clear(3,clearColor,1,0);draw(1,4,2,true);checkpoint(1);capture(1);
        screen_triangle();checkpoint(2);capture(1);
        // The 84-byte direct upload invalidates the previous 80-byte quad.
        renderer->clear(3,clearColor,1,0);
        draw(3,4,2,true);draw(4,4,2,true);checkpoint(3);break;
    case 17:
        for(u32 phase=0;phase<4;++phase){
            reset_state();renderer->clear(3,clearColor,1,0);
            Vertex changed[4];std::memcpy(changed,quad,sizeof quad);
            if(phase>=2)changed[0].x+=.25f;
            if(phase==3)screen_triangle();
            draw(0,1,1,false,false,changed);
            if(phase!=3)screen_triangle();
            checkpoint(phase);capture(1);
        }break;
    case 18:
        for(u32 phase=0;phase<4;++phase){
            reset_state();renderer->clear(3,clearColor,1,0);
            draw(phase<2?1:3,4,2,true);
            if(phase<3)draw(2,4,2,true);
            checkpoint(phase);capture(1);
        }break;
    case 19:
        for(u32 phase=0;phase<4;++phase){
            reset_state();renderer->clear(3,clearColor,1,0);
            for(u32 i=0;i<65;++i){
                Vertex changed[4];std::memcpy(changed,quad,sizeof quad);
                if(phase==2&&i==10)changed[0].y+=.25f;
                draw(i,13,5,false,false,changed);renderer->flush();
            }
            checkpoint(phase);capture(1);
        }break;
    case 20:{
        struct V{float x,y,z,w;u32 color;float u,v;};
        renderer->state.layout=attributes(VertexLayout::ScreenColorUv);
        for(u32 i=0;i<48;++i){
            renderer->state.texture=3+(i%2);
            const float x=16+float(i%8)*58,y=16+float(i/8)*72;
            V v[]={{x,y,.2f,1,0xaf123456,0,0},{x+52,y,.2f,1,0xbfabcdfe,1,0},{x,y+64,.2f,1,0xcf876543,0,1}};
            renderer->draw_batch(1,v,sizeof(V));++requested;
            std::memset(v,0,sizeof v);
        }
        checkpoint(0);break;
    }
    case 21:
        draw(1,4,2,true);draw(2,4,2,true);
        screen_triangle();draw(3,4,2,true);draw(4,4,2,true);
        checkpoint(0);break;
    case 22:{
        for(u32 i=0;i<1025;++i){renderer->state.texture=3+(i%2);screen_triangle();}
        checkpoint(0);capture(1);
        reset_state();renderer->draw(Topology::Strip,2,quad,sizeof(Vertex));++requested;
        renderer->discard();checkpoint(1);break;
    }
    case 23:
        draw(0);renderer->clear(3,clearColor,1,0);draw(1);break;
    case 24:case 25:
        reset_state(5);draw(0);
        reset_state();renderer->state.texture=5;draw(0,1,1);
        reset_state(5);renderer->clear(3,clearColor,1,0);draw(1);
        if(id==25){reset_state();renderer->clear(3,clearColor,1,0);draw(2);}
        break;
    case 26:{
        draw(0,1,1);const i32 rect[]={0,0,256,512};renderer->clear(3,0xff891234,1,0,rect,1);
        renderer->state.viewport={0,0,256,512,0,1};draw(1,1,1);break;
    }
    case 27:
        renderer->state.pipeline.colorMask=0;renderer->state.pipeline.depthWrite=true;renderer->state.pipeline.depthTest=true;renderer->state.pipeline.depthCompare=Compare::Less;
        draw(0,1,1);renderer->clear(1,0xff112233,1,0);renderer->state.pipeline.colorMask=15;draw(1,1,1);break;
    case 28:
        for(u32 i=0;i<4200;++i){renderer->clear(3,clearColor,1,0);draw(i%8,4,2);}
        break;
    case 29:case 30:case 31:{
        struct V{float x,y,z,w;u32 color;float u,v;};
        V v[]={{40,40,.2f,1,0xaf123456,0,0},{160,40,.2f,1,0xbfabcdfe,1,0},{40,140,.2f,1,0xcf876543,0,1}};
        if(id==30)v[1].x=310;
        if(id==31)v[1].w=.75f;
        renderer->state.layout=attributes(VertexLayout::ScreenColorUv);
        renderer->draw_batch(1,v,sizeof(V));++requested;
        const i32 rect[]={0,0,256,256};renderer->clear(3,clearColor,1,0,rect,1);
        reset_state();draw(1,4,2);break;
    }
    }
    renderer->flush();capture(1);capture(5);
    if(!gpu_ok("case draw/read"))return false;
    const auto& s=renderer->stats;
    metadata[0]=id;metadata[1]=requested;metadata[2]=s.calls;metadata[3]=s.batches;
    metadata[4]=s.vertexUploadBytes;metadata[5]=s.bufferReplacements;metadata[6]=s.bufferSubUpdates;
    metadata[7]=s.layoutSetups;metadata[8]=s.genericBatches;metadata[9]=u32(result.size());metadata[10]=0;
    const u8 bg[]={u8(clearColor),u8(clearColor>>8),u8(clearColor>>16),u8(clearColor>>24)};
    for(std::size_t i=0;i<result.size();i+=4)if(std::memcmp(result.data()+i,bg,4))++metadata[10];
    if(!metadata[10]||s.calls!=requested){failure="empty fixture image or missing real draw";return false;}
    return true;
}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int ti_initialize(){return initialize();}
EMSCRIPTEN_KEEPALIVE void ti_queue_mode(u32 enabled){queueMode=enabled!=0;}
EMSCRIPTEN_KEEPALIVE void ti_raster_mode(u32 enabled){rasterMode=enabled!=0;}
EMSCRIPTEN_KEEPALIVE const void* ti_raster_stats(){return &renderer->rasterStats;}
EMSCRIPTEN_KEEPALIVE u32 ti_case_count(){return sizeof(names)/sizeof(names[0]);}
EMSCRIPTEN_KEEPALIVE const char* ti_case_name(u32 id){return id<ti_case_count()?names[id]:"invalid";}
EMSCRIPTEN_KEEPALIVE int ti_run(u32 id){return run(id);}
EMSCRIPTEN_KEEPALIVE const u8* ti_pixels(){return result.data();}
EMSCRIPTEN_KEEPALIVE u32 ti_bytes(){return u32(result.size());}
EMSCRIPTEN_KEEPALIVE const u32* ti_metadata(){return metadata.data();}
EMSCRIPTEN_KEEPALIVE const char* ti_error(){return failure.c_str();}
EMSCRIPTEN_KEEPALIVE void ti_shutdown(){renderer.reset();images.clear();}
}
