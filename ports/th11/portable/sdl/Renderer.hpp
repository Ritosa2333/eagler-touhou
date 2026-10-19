#pragma once
#include <SDL3/SDL.h>
#include "GraphicsState.hpp"
#include <GLES3/gl3.h>
#include <array>
#include <cstdint>
#include <map>
#include <vector>
#include <string>

struct ImDrawData;

// Semantic GLES renderer shared by the two games. Matrices and vertices stay
// in C++; only resource decoding knows the original file format codes.
namespace touhou::sdl {
using u32=uint32_t;using i32=int32_t;using u8=uint8_t;
using namespace touhou::graphics;
struct Surface {u32 handle=0,width=0,height=0;PixelFormat format=PixelFormat::Bgra8;u32 pitch=0;u8* data=nullptr;u32 size=0,version=0;};
struct Viewport {u32 x=0,y=0,width=640,height=480;float min=0,max=1;};
struct Statistics {u32 calls=0,batches=0,uploadBytes=0,readBytes=0,frames=0,presentations=0,bufferReplacements=0,bufferSubUpdates=0,vertexUploadBytes=0,directBytes=0,copiedBytes=0,layoutSetups=0,textureBinds=0,framebufferBinds=0,programCompiles=0,genericBatches=0,resamples=0;};
struct State {
    PipelineState pipeline{};
    std::array<std::array<float,16>,4> matrix{};
    Viewport viewport{};u32 texture=0,target=0,depth=0,stride=0;VertexAttributes layout{};
    State();
};
class Renderer : public StateCommands {
public:
    using Resolve=Surface(*)(void*,u32);
    Statistics stats{};State state{};bool defer=false;
    Renderer(int version,Resolve,void*);~Renderer();
    bool initialize();void flush();void discard();bool commit();
    PipelineState& pipeline() override { return state.pipeline; }
    void transform(MatrixKind,const void*);void viewport(const Viewport&);
    void draw(Topology primitive,u32 count,const void*,u32 stride,const void* indices=nullptr,IndexType indexFormat=IndexType::UInt16);
    void draw_batch(u32 count,const void*,u32 stride);
    void clear(u32 flags,u32 color,float depth,u32 stencil,const i32* rects=nullptr,u32 count=0);
    void copy(u32 source,const i32* rect,u32 target,const i32* point);
    bool resample(u32,const i32*,u32,const i32*,const float*,u32,u32);
    void read(u32);void release(u32);void present(u32);void prepare(u32);
    void render_imgui(const ImDrawData*,u32 target);
    const char* error()const{return failure.c_str();}
    int version;Resolve resolve;void* owner;
private:
    struct GPU {GLuint texture=0,framebuffer=0;u32 version=~0u;bool rendered=false;GLuint attached=~0u;std::array<GLint,4> sampler{};};
    struct Depth {GLuint buffer=0;bool stencil=false;};
    struct Uniform {GLint location=-1;std::array<float,16> value{};int count=-1;};
    struct Program {GLuint id=0;std::array<Uniform,28> uniforms{};};
    struct Stream {GLuint id=0;u32 size=0,used=0,frame=~0u;};
    SDL_Window* window=nullptr;SDL_GLContext context=nullptr;
    std::map<u32,GPU> surfaces;std::map<u32,Depth> depths;std::map<std::array<u32,10>,Program> programs;
    GLuint vertex=0,currentProgram=0,boundTexture=~0u,readFramebuffer=~0u,drawFramebuffer=~0u;
    std::map<std::array<u32,6>,GLuint> layouts;GLuint currentLayout=0;
    GLuint resampleProgram=0,resampleVao=0,weightTexture=0;
    GLuint imguiProgram=0,imguiVao=0,imguiVbo=0,imguiEbo=0,imguiFontTexture=0;
    GLint imguiProjMtx=-1,imguiTexture=-1;
    Program generic{};bool warming=true,buildingGeneric=false;Program* program=nullptr;
    Stream vertices{},indices{},instances{};State batchState{};
    std::vector<u8> batchBytes,quad,worlds,pixels;bool batching=false,instancing=false;
    u32 batchCount=0,pending=0;std::string failure;
    GPU& surface(u32);GPU& target(u32,u32);void issue(const State&,Topology,u32,const void*,u32,const void*,IndexType,const void*,u32);
    void bind_texture(GLuint);void bind_framebuffer(GLenum,GLuint);
    void select(const State&);void uniform(int,int,const float*);void integer(int,int);
    u32 upload(Stream&,GLenum,const void*,u32,u32);
};
Renderer* current();void set_current(Renderer*);
}

extern "C" {
__attribute__((export_name("sdl_stats"))) const touhou::sdl::Statistics* sdl_stats();
__attribute__((export_name("sdl_defer"))) void sdl_defer(int);
__attribute__((export_name("sdl_commit"))) int sdl_commit();
__attribute__((export_name("sdl_error"))) const char* sdl_error();
__attribute__((export_name("sdl_version"))) int sdl_version();
__attribute__((export_name("sdl_ticks"))) double sdl_ticks();
}
