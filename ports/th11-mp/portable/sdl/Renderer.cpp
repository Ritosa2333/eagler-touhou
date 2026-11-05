#include "Renderer.hpp"
#include "RasterLiveness.hpp"
#include "Shaders.hpp"
#include "ResampleShader.hpp"
#include <emscripten.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include "imgui.h"

EM_JS(int, touhou_clip_control, (), {
  const e=GL.currentContext.GLctx.getExtension('EXT_clip_control');
  if(e)e.clipControlEXT(e.LOWER_LEFT_EXT,e.ZERO_TO_ONE_EXT);return !!e;
});
namespace touhou::sdl {
namespace {
Renderer* active=nullptr;
// One SDL GL context is active per Wasm module. Cache only states owned here;
// clear() also updates the write masks so a later draw restores them correctly.
struct DrawStateCache {
 PipelineState pipeline{};Viewport viewport{};bool valid=false,has_viewport=false;
} drawState;
constexpr std::array<float,16> identity{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
// Full world + packed factor + full texture matrix. Adjacent atlas sprites
// may differ in UV transform without changing any shared draw state.
constexpr u32 instanceStride=132;
float bits(u32 n){float f;std::memcpy(&f,&n,4);return f;}
std::array<float,4> color(u32 n){return {float(n>>16&255)/255,float(n>>8&255)/255,float(n&255)/255,float(n>>24)/255};}
void replace(std::string& s,const std::string& a,const std::string& b){size_t p=0;while((p=s.find(a,p))!=std::string::npos){s.replace(p,a.size(),b);p+=b.size();}}
GLuint shader(GLenum type,const std::string& source){GLuint s=glCreateShader(type);const char* p=source.c_str();glShaderSource(s,1,&p,nullptr);glCompileShader(s);GLint ok=0;glGetShaderiv(s,GL_COMPILE_STATUS,&ok);if(!ok){char log[2048];glGetShaderInfoLog(s,sizeof(log),nullptr,log);std::fprintf(stderr,"SDL shader: %s\n",log);std::abort();}return s;}
constexpr const char* imguiVertexSource=R"(#version 300 es
precision mediump float;
uniform mat4 u_ProjMtx;
layout(location=0) in vec2 Position;
layout(location=1) in vec2 UV;
layout(location=2) in vec4 Color;
out vec2 Frag_UV;
out vec4 Frag_Color;
void main(){Frag_UV=UV;Frag_Color=Color.bgra;gl_Position=u_ProjMtx*vec4(Position.xy,0.0,1.0);}
)";
constexpr const char* imguiFragmentSource=R"(#version 300 es
precision mediump float;
in vec2 Frag_UV;
in vec4 Frag_Color;
uniform sampler2D Texture;
out vec4 Out_Color;
void main(){Out_Color=Frag_Color*texture(Texture,Frag_UV.st);}
)";

bool equal(const State& a,const State& b,bool world=false){
 if(a.layout!=b.layout||a.stride!=b.stride||a.texture!=b.texture||a.target!=b.target||a.depth!=b.depth||std::memcmp(&a.viewport,&b.viewport,sizeof(Viewport)))return false;
 if(!a.pipeline.compatible(b.pipeline,world))return false;
 if(world){for(int i=1;i<3;i++)if(a.matrix[i]!=b.matrix[i])return false;}
 else if(!a.layout.screen&&a.matrix!=b.matrix)return false;
 return true;
}
}
Renderer* current(){return active;}void set_current(Renderer* r){active=r;}
State::State(){for(auto& m:matrix)m=identity;}
Renderer::Renderer(int v,Resolve r,void* p):version(v),resolve(r),owner(p){}
bool Renderer::initialize(){
#ifdef __EMSCRIPTEN__
 vertexQuadValid=false;
#endif
 if(!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)){failure=SDL_GetError();return false;}
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_ES);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
 SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,0);SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,0);SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,0);SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
 window=SDL_CreateWindow(version==8?"Touhou 08":"Touhou 10",640,480,SDL_WINDOW_OPENGL);if(!window){failure=SDL_GetError();return false;}
 SDL_SetWindowSize(window,640,480);
 context=SDL_GL_CreateContext(window);if(!context){failure=SDL_GetError();return false;}SDL_GL_SetSwapInterval(1);drawState=DrawStateCache{};
 std::string source=vertexSource;if(version==10&&touhou_clip_control()){replace(source,"position.z*2.0-1.0","position.z");replace(source,"gl_Position.z=gl_Position.z*2.0-gl_Position.w;","");}
 vertex=shader(GL_VERTEX_SHADER,source);glGenBuffers(1,&vertices.id);glGenBuffers(1,&indices.id);glGenBuffers(1,&instances.id);
 glActiveTexture(GL_TEXTURE0);buildingGeneric=true;select(State{});generic=*program;programs.clear();buildingGeneric=false;
 State warm;for(int i=0;i<4;i++){warm.texture=i!=2;auto& p=warm.pipeline;p.alphaTest=true;p.fog=i==3;p.alphaCompare=Compare::GreaterEqual;p.color.operation=p.alpha.operation=(i==0||i==3)?ColorOperation::Multiply:ColorOperation::First;p.color.first=p.alpha.first={(i==0||i==3)?ArgumentSource::Texture:ArgumentSource::Diffuse};p.color.second=p.alpha.second={i==3?ArgumentSource::Factor:ArgumentSource::Diffuse};select(warm);}
 // Compile the game combiner families before the first frame. Rare asset
 // combinations use a prelinked uniform program instead of compiling in draw.
 for(bool texture:{false,true})for(bool alpha:{false,true})for(bool fog:{false,true})for(auto op:{ColorOperation::First,ColorOperation::Multiply})for(auto second:{ArgumentSource::Diffuse,ArgumentSource::Factor}){
   warm.texture=texture;auto& p=warm.pipeline;p.alphaTest=alpha;p.fog=fog;p.alphaCompare=Compare::GreaterEqual;
   p.color.operation=p.alpha.operation=op;p.color.first=p.alpha.first={op==ColorOperation::First?ArgumentSource::Diffuse:ArgumentSource::Texture};p.color.second=p.alpha.second={second};select(warm);
 }
 resampleProgram=glCreateProgram();const auto rv=shader(GL_VERTEX_SHADER,resampleVertex),rf=shader(GL_FRAGMENT_SHADER,resampleFragment);
 glAttachShader(resampleProgram,rv);glAttachShader(resampleProgram,rf);glLinkProgram(resampleProgram);glDeleteShader(rv);glDeleteShader(rf);
 GLint linked=0;glGetProgramiv(resampleProgram,GL_LINK_STATUS,&linked);if(!linked){failure="Resample program link failed";return false;}
 glGenVertexArrays(1,&resampleVao);glGenTextures(1,&weightTexture);
 warming=false;set_current(this);return true;
}
Renderer::~Renderer(){discard();for(auto& [id,g]:surfaces){glDeleteTextures(1,&g.texture);glDeleteFramebuffers(1,&g.framebuffer);}for(auto& [id,d]:depths)glDeleteRenderbuffers(1,&d.buffer);for(auto& [k,p]:programs)glDeleteProgram(p.id);glDeleteProgram(resampleProgram);glDeleteVertexArrays(1,&resampleVao);glDeleteTextures(1,&weightTexture);glDeleteProgram(generic.id);for(auto& entry:layouts)glDeleteVertexArrays(1,&entry.second);for(auto& entry:residentLayouts)glDeleteVertexArrays(1,&entry.second);glDeleteBuffers(1,&residentQuadVbo);glDeleteShader(vertex);glDeleteBuffers(1,&vertices.id);glDeleteBuffers(1,&indices.id);glDeleteBuffers(1,&instances.id);glDeleteProgram(imguiProgram);glDeleteVertexArrays(1,&imguiVao);glDeleteBuffers(1,&imguiVbo);glDeleteBuffers(1,&imguiEbo);glDeleteTextures(1,&imguiFontTexture);if(context)SDL_GL_DestroyContext(context);if(window)SDL_DestroyWindow(window);if(active==this)active=nullptr;SDL_Quit();}
void Renderer::bind_texture(GLuint id){if(boundTexture!=id){glBindTexture(GL_TEXTURE_2D,id);boundTexture=id;++stats.textureBinds;}}
void Renderer::bind_framebuffer(GLenum target,GLuint id){
 const bool read=target!=GL_DRAW_FRAMEBUFFER,draw=target!=GL_READ_FRAMEBUFFER;
 if((read&&readFramebuffer!=id)||(draw&&drawFramebuffer!=id)){glBindFramebuffer(target,id);++stats.framebufferBinds;if(read)readFramebuffer=id;if(draw)drawFramebuffer=id;}
}
void Renderer::draw_batch(u32 count,const void* data,u32 stride){
 if(queuedUploads){draw(Topology::Triangles,count,data,stride);return;}
 // The game already coalesced these triangles. Consume while its arena is
 // valid, without retaining a pointer to a buffer the next sprite can reuse.
 flush();++stats.calls;state.stride=stride;stats.directBytes+=count*3*stride;
 issue(state,Topology::Triangles,count,data,count*3*stride,nullptr,IndexType::UInt16,nullptr,0);
}
void Renderer::transform(MatrixKind kind,const void* p){auto& m=state.matrix[u32(kind)];if(std::memcmp(m.data(),p,64))std::memcpy(m.data(),p,64);}
void Renderer::viewport(const Viewport& v){state.viewport=v;}
void Renderer::select(const State& s){
 const auto& p=s.pipeline;auto arg=[](Argument a){return u32(a.source)|(a.complement?16u:0u)|(a.alphaOnly?32u:0u);};
 std::array<u32,10> key{u32(!!s.texture),u32(p.alphaTest),u32(p.fog),u32(p.color.operation),u32(p.alpha.operation),arg(p.color.first),arg(p.color.second),arg(p.alpha.first),arg(p.alpha.second),u32(p.alphaCompare)};auto it=programs.find(key);
 if(it==programs.end()&&!warming){program=&generic;++stats.genericBatches;if(currentProgram!=program->id){glUseProgram(program->id);currentProgram=program->id;}return;}
 if(it==programs.end()){
  ++stats.programCompiles;
  static const char* constants[]{"hasTexture","alphaTest","fogEnabled","colorOp","alphaOp","colorArg1","colorArg2","alphaArg1","alphaArg2","alphaFunc"};
  std::string source=fragmentSource,defs;if(!buildingGeneric){replace(source,"uniform bool hasTexture,alphaTest,fogEnabled;","");replace(source,"uniform int colorOp,alphaOp,colorArg1,colorArg2,alphaArg1,alphaArg2,alphaFunc;","");
  for(int i=0;i<10;i++)defs+="const "+std::string(i<3?"bool ":"int ")+constants[i]+"="+(i<3?(key[i]?"true":"false"):std::to_string(key[i]))+";\n";
  replace(source,"precision highp float;","precision highp float;\n"+defs);}
  if(version==8){replace(source,"float a=c.a*255.0","float a=floor(c.a*255.0+.5)");replace(source,"gl_FragDepth=depth16?(floor(scaled)+floor(fract(scaled)-gl_FragCoord.z+0.5))/65535.0:gl_FragCoord.z;","");}
  Program p;p.id=glCreateProgram();GLuint fs=shader(GL_FRAGMENT_SHADER,source);glAttachShader(p.id,vertex);glAttachShader(p.id,fs);glLinkProgram(p.id);glDeleteShader(fs);GLint linked=0;glGetProgramiv(p.id,GL_LINK_STATUS,&linked);if(!linked){char log[2048];glGetProgramInfoLog(p.id,sizeof(log),nullptr,log);std::fprintf(stderr,"SDL program: %s\n",log);std::abort();}
  const char* names[]{"viewport","transformed","textureTransform","instanced","world","view","projection","textureMatrix","fogEnabled","rangeFog","fogMode","fogParams","fogColor","hasTexture","factor","colorOp","colorArg1","colorArg2","alphaOp","alphaArg1","alphaArg2","alphaTest","alphaRef","alphaFunc","depth16","tex"};
  for(int i=0;i<26;i++)p.uniforms[i].location=glGetUniformLocation(p.id,names[i]);it=programs.emplace(key,p).first;
 }
 program=&it->second;if(currentProgram!=program->id){glUseProgram(program->id);currentProgram=program->id;}
}
void Renderer::uniform(int i,int n,const float* p){auto& u=program->uniforms[i];if(u.location<0)return;if(u.count==n&&!std::memcmp(u.value.data(),p,n*4))return;std::memcpy(u.value.data(),p,n*4);u.count=n;if(n==16)glUniformMatrix4fv(u.location,1,false,p);else if(n==4)glUniform4fv(u.location,1,p);else if(n==3)glUniform3fv(u.location,1,p);else glUniform1f(u.location,*p);}
void Renderer::integer(int i,int n){auto& u=program->uniforms[i];if(u.location<0)return;float f=float(n);if(u.count==1&&u.value[0]==f)return;u.count=1;u.value[0]=f;glUniform1i(u.location,n);}
u32 Renderer::upload(Stream& s,GLenum target,const void* bytes,u32 count,u32 minimum){
#ifdef __EMSCRIPTEN__
 if(&s==&vertices)vertexQuadValid=false;
#endif
 glBindBuffer(target,s.id);stats.vertexUploadBytes+=count;
#ifdef __EMSCRIPTEN__
 // Replace the backing store for each submitted batch. Updating successive
 // subranges while previous draws still reference the store stalls some mobile
 // WebGL drivers. Draw order, vertex bytes and simulation are unchanged.
 glBufferData(target,count,bytes,GL_STREAM_DRAW);stats.bufferReplacements++;return 0;
#else
 u32 offset=(s.used+3)&~3u;
 if(s.frame!=stats.frames||offset+count>s.size){s.size=std::max({s.size,minimum,count});glBufferData(target,s.size,nullptr,GL_STREAM_DRAW);stats.bufferReplacements++;s.used=offset=0;s.frame=stats.frames;}
 glBufferSubData(target,offset,count,bytes);stats.bufferSubUpdates++;s.used=offset+count;return offset;
#endif
}
Renderer::GPU& Renderer::surface(u32 h){
 auto s=resolve(owner,h);if(!s.handle||!s.width||!s.height){std::fprintf(stderr,"SDL invalid surface %u\n",h);std::abort();}auto& g=surfaces[h];if(!g.texture){glGenTextures(1,&g.texture);glGenFramebuffers(1,&g.framebuffer);bind_texture(g.texture);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);bind_framebuffer(GL_FRAMEBUFFER,g.framebuffer);glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,g.texture,0);}
 if(g.version!=s.version){
  stats.uploadBytes+=s.size;bind_texture(g.texture);glPixelStorei(GL_UNPACK_ALIGNMENT,1);GLenum format=GL_RGBA,type=GL_UNSIGNED_BYTE,internal=GL_RGBA8;
  bool rgb=s.format==PixelFormat::Bgr8||s.format==PixelFormat::Bgrx8||s.format==PixelFormat::Rgb565;u32 channels=rgb?3:4;pixels.resize(s.width*s.height*(s.format==PixelFormat::Argb4444?2:channels));
  if(s.format==PixelFormat::Argb4444){for(u32 y=0;y<s.height;y++)for(u32 x=0;x<s.width;x++){uint16_t n;std::memcpy(&n,s.data+y*s.pitch+x*2,2);n=(n<<4)|(n>>12);std::memcpy(pixels.data()+(y*s.width+x)*2,&n,2);}type=GL_UNSIGNED_SHORT_4_4_4_4;internal=GL_RGBA4;}
  else {for(u32 y=0;y<s.height;y++)for(u32 x=0;x<s.width;x++){
    const u8* p=s.data+y*s.pitch;u32 r=0,gc=0,b=0,a=255;
    if(s.format==PixelFormat::Bgra8||s.format==PixelFormat::Bgrx8){p+=x*4;b=p[0];gc=p[1];r=p[2];a=s.format==PixelFormat::Bgra8?p[3]:255;}
    else if(s.format==PixelFormat::Bgr8){p+=x*3;b=p[0];gc=p[1];r=p[2];}
    else if((s.format==PixelFormat::Rgb565||s.format==PixelFormat::Xrgb1555||s.format==PixelFormat::Argb1555)){uint16_t n;std::memcpy(&n,p+x*2,2);b=((n&31)*255+15)/31;if(s.format==PixelFormat::Rgb565){gc=((n>>5&63)*255+31)/63;r=((n>>11)*255+15)/31;}else{gc=((n>>5&31)*255+15)/31;r=((n>>10&31)*255+15)/31;a=s.format==PixelFormat::Argb1555?(n&0x8000?255:0):255;}}
    else if(s.format==PixelFormat::Alpha8){r=gc=b=255;a=p[x];}else if(s.format==PixelFormat::Luminance8)r=gc=b=p[x];else{std::fprintf(stderr,"SDL texture format %u\n",u32(s.format));std::abort();}
    u8* out=pixels.data()+(y*s.width+x)*channels;out[0]=r;out[1]=gc;out[2]=b;if(!rgb)out[3]=a;
   }format=rgb?GL_RGB:GL_RGBA;internal=s.format==PixelFormat::Rgb565?GL_RGB565:(s.format==PixelFormat::Xrgb1555||s.format==PixelFormat::Argb1555)?GL_RGB5_A1:rgb?GL_RGB8:GL_RGBA8;
  }
  if(g.version==~0u)glTexImage2D(GL_TEXTURE_2D,0,internal,s.width,s.height,0,format,type,pixels.data());else glTexSubImage2D(GL_TEXTURE_2D,0,0,0,s.width,s.height,format,type,pixels.data());g.version=s.version;g.rendered=false;
 }return g;
}
Renderer::GPU& Renderer::target(u32 id,u32 depthId){auto& g=surface(id);GLuint buffer=0;bool stencil=false;if(depthId){auto& d=depths[depthId];if(!d.buffer){auto s=resolve(owner,depthId);d.stencil=s.format==PixelFormat::Depth24Stencil8;glGenRenderbuffers(1,&d.buffer);glBindRenderbuffer(GL_RENDERBUFFER,d.buffer);glRenderbufferStorage(GL_RENDERBUFFER,d.stencil?GL_DEPTH24_STENCIL8:s.format==PixelFormat::Depth16?GL_DEPTH_COMPONENT16:GL_DEPTH_COMPONENT24,s.width,s.height);}buffer=d.buffer;stencil=d.stencil;}bind_framebuffer(GL_FRAMEBUFFER,g.framebuffer);if(g.attached!=buffer){glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,buffer);glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,stencil?buffer:0);g.attached=buffer;}return g;}
void Renderer::prepare(u32 h){flush_raster();if(queuedUploads)flush();surface(h);}
void Renderer::draw(Topology primitive,u32 count,const void* data,u32 stride,const void* index,IndexType indexFormat){
 stats.calls++;state.stride=stride;
 if(index||primitive<Topology::Triangles||primitive>Topology::Fan){flush_raster();flush();issue(state,primitive,count,data,vertex_count(primitive,count)*stride,index,indexFormat,nullptr,0);return;}
 const bool instance=version==10&&state.layout==attributes(VertexLayout::WorldUv)&&stride==20&&primitive==Topology::Strip&&count==2;
 if(instance){if(batching&&(!instancing||!equal(batchState,state,true)||quad.size()!=80||std::memcmp(quad.data(),data,80)))finish_batch();if(!batching){batchState=state;quad.assign(static_cast<const u8*>(data),static_cast<const u8*>(data)+80);batching=instancing=true;}const auto old=worlds.size();worlds.resize(old+instanceStride);std::memcpy(worlds.data()+old,state.matrix[0].data(),64);std::memcpy(worlds.data()+old+64,&state.pipeline.textureFactor,4);std::memcpy(worlds.data()+old+68,state.matrix[3].data(),64);if(worlds.size()>=1024*instanceStride)finish_batch();return;}
 if(batching&&(instancing||!equal(batchState,state)))finish_batch();if(!batching){batchState=state;batching=true;instancing=false;}
 const auto* src=static_cast<const u8*>(data);const size_t start=batchBytes.size(),size=count*3*stride;stats.copiedBytes+=size;batchBytes.resize(start+size);auto* dest=batchBytes.data()+start;
 if(primitive==Topology::Triangles)std::memcpy(dest,src,size);else if(count){std::memcpy(dest,src,3*stride);dest+=3*stride;for(u32 i=1;i<count;i++){u32 a=primitive==Topology::Fan?0:i&1?i+1:i,b=primitive==Topology::Fan?i+1:i&1?i:i+1,c=i+2;for(u32 v:{a,b,c}){std::memcpy(dest,src+v*stride,stride);dest+=stride;}}}batchCount+=count;if(batchBytes.size()>=1048576)finish_batch();
}
void Renderer::finish_batch(){
 if(!batching)return;batching=false;
 const auto primitive=instancing?Topology::Strip:Topology::Triangles;
 const u32 count=instancing?2:batchCount,size=instancing?quad.size():batchBytes.size();
 const auto* data=instancing?quad.data():batchBytes.data();
 const u32 instanceSize=instancing&&worlds.size()>instanceStride?worlds.size():0;
#ifdef __EMSCRIPTEN__
 if(deferredRaster&&size)queue_raster_draw(batchState,primitive,count,data,size,instanceSize?worlds.data():nullptr,instanceSize);
 else if(queuedUploads&&size)queue_upload_draw(batchState,primitive,count,data,size,instanceSize?worlds.data():nullptr,instanceSize);
 else
#endif
 issue(batchState,primitive,count,data,size,nullptr,IndexType::UInt16,instanceSize?worlds.data():nullptr,instanceSize);
 batchBytes.clear();worlds.clear();batchCount=0;instancing=false;
}
void Renderer::queue_upload_draw(const State& d,Topology primitive,u32 count,const void* data,u32 size,const void* instance,u32 instanceSize){
  constexpr std::size_t limit=1048576;
  auto offset=(queuedVertices.size()+d.stride-1)/d.stride*d.stride;
  if(offset+size>limit||queuedInstances.size()+instanceSize>limit||queuedDraws.size()>=1024){submit_batches();offset=0;}
  queuedVertices.resize(offset+size);std::memcpy(queuedVertices.data()+offset,data,size);
  const auto instanceOffset=queuedInstances.size();
  if(instanceSize){queuedInstances.resize(instanceOffset+instanceSize);std::memcpy(queuedInstances.data()+instanceOffset,instance,instanceSize);}
  stats.copiedBytes+=size+instanceSize;
  queuedDraws.push_back({d,primitive,count,u32(offset/d.stride),size,u32(instanceOffset),instanceSize});
  if(queuedVertices.size()>=limit||queuedInstances.size()>=limit)submit_batches();
}
void Renderer::queue_raster_draw(const State& d,Topology primitive,u32 count,const void* data,u32 size,const void* instance,u32 instanceSize){
 constexpr std::size_t limit=4*1048576;
 if(rasterCommands.size()>=8192||rasterBytes.size()+std::size_t(size)+instanceSize>limit){++rasterStats.capacityFences;flush_raster();}
 if(std::size_t(size)+instanceSize>limit||d.texture==d.target){
  // Preserve unsupported/self-sampling driver behavior at its original boundary.
  flush_raster();submit_batches();issue(d,primitive,count,data,size,nullptr,IndexType::UInt16,instance,instanceSize);return;
 }
 RasterCommand q;q.state=d;q.primitive=primitive;q.count=count;q.vertexOffset=rasterBytes.size();q.vertexBytes=size;
 rasterBytes.resize(rasterBytes.size()+size);std::memcpy(rasterBytes.data()+q.vertexOffset,data,size);
 q.instanceOffset=rasterBytes.size();q.instanceBytes=instanceSize;
 if(instanceSize){rasterBytes.resize(rasterBytes.size()+instanceSize);std::memcpy(rasterBytes.data()+q.instanceOffset,instance,instanceSize);}
 rasterCommands.push_back(q);++rasterStats.draws;
 rasterStats.peakCommands=std::max(rasterStats.peakCommands,u32(rasterCommands.size()));rasterStats.peakBytes=std::max(rasterStats.peakBytes,u32(rasterBytes.size()));
}
bool Renderer::queue_raster_clear(u32 flags,u32 color,float depth,u32 stencil,const i32* rects,u32 count){
 if(!count||count>64){flush_raster();return false;}
 if(rasterCommands.size()>=8192||rasterRects.size()+count>8192){++rasterStats.capacityFences;flush_raster();}
 RasterCommand q;q.state=state;q.clear=true;q.clearFlags=flags;q.color=color;q.clearDepth=depth;q.stencil=stencil;
 q.rectOffset=rasterRects.size();q.rectCount=count;
 for(u32 i=0;i<count;++i)rasterRects.push_back({rects[4*i],rects[4*i+1],rects[4*i+2],rects[4*i+3]});
 rasterCommands.push_back(q);++rasterStats.clears;rasterStats.peakCommands=std::max(rasterStats.peakCommands,u32(rasterCommands.size()));return true;
}
void Renderer::resolve_raster_liveness(){
 struct Resource {u32 handle,kind;RasterNeeded needed;};
 std::array<Resource,64> resources;unsigned used=0;bool valid=true;
 std::array<Surface,64> descriptions;unsigned descriptionCount=0;
 const auto describe=[&](u32 id)->Surface{
  for(unsigned i=0;i<descriptionCount;++i)if(descriptions[i].handle==id)return descriptions[i];
  const auto s=resolve(owner,id);
  if(descriptionCount==descriptions.size()||s.handle!=id||!s.width||!s.height||s.width>16384||s.height>16384){valid=false;return {};}
  descriptions[descriptionCount++]=s;return s;
 };
 const auto need=[&](u32 id,u32 kind)->RasterNeeded*{
  if(!id)return nullptr;
  for(unsigned i=0;i<used;++i)if(resources[i].handle==id&&resources[i].kind==kind)return &resources[i].needed;
  const auto s=describe(id);
  if(used==resources.size()||s.handle!=id||!s.width||!s.height||s.width>16384||s.height>16384){valid=false;return nullptr;}
  auto& r=resources[used++];r.handle=id;r.kind=kind;r.needed.full={0,0,s.width,s.height};r.needed.require_all();return &r.needed;
 };
 for(const auto& q:rasterCommands){
  for(const auto id:{q.state.target,q.state.depth,q.state.texture})if(id)describe(id);
 }
 for(auto it=rasterCommands.rbegin();it!=rasterCommands.rend()&&valid;++it){auto& q=*it;const auto& d=q.state;
  auto* color=need(d.target,0);auto* depth=need(d.depth,1);auto* stencil=need(d.depth,2);
  if(!color||!valid){valid=false;break;}
  if(q.clear){
   bool live=false;
   if(q.clearFlags&~7u){valid=false;break;}
   for(u32 i=0;i<q.rectCount;++i){const auto& r=rasterRects[q.rectOffset+i];const RasterRect rect{r[0],r[1],r[2],r[3]};
    // Negative extents are GL errors, not a dead empty clear to hide.
    if(r[2]<r[0]||r[3]<r[1]){valid=false;break;}
    for(unsigned k=0;k<3;++k){auto* n=k==0?color:k==1?depth:stencil;if(n&&(q.clearFlags&(1u<<k))){live=live||n->intersects(rect);n->subtract(rect);}}
   }
   q.needed=live;
  }else{
   const auto& v=d.viewport;RasterRect box{v.x,v.y,std::int64_t(v.x)+v.width,std::int64_t(v.y)+v.height};
   if(v.x>0x7fffffffu||v.y>0x7fffffffu||v.width>16384||v.height>16384){valid=false;break;}
   // A transformed triangle cannot rasterize outside the convex hull of its
   // submitted screen vertices. Most historical sprites are wholly inside a
   // later partial playfield clear, although their viewport is the whole FBO.
   // Restrict to finite modest coordinates and rhw exactly one; all other
   // layouts retain the conservative viewport bound. Two-pixel outward guard
   // exceeds highp float rounding in this range and preserves edge coverage.
   if(d.layout.screen&&d.stride>=16&&q.vertexBytes&&q.vertexBytes%d.stride==0&&v.x<=32768&&v.y<=32768){
    double left=32768,top=32768,right=-32768,bottom=-32768;bool bounded=true;
    for(u32 at=0;at<q.vertexBytes;at+=d.stride){
     float p[4];std::memcpy(p,rasterBytes.data()+q.vertexOffset+at,16);
     if(!std::isfinite(p[0])||!std::isfinite(p[1])||!std::isfinite(p[2])||p[3]!=1||
        std::abs(p[0])>32768||std::abs(p[1])>32768){bounded=false;break;}
     left=std::min(left,double(p[0]));right=std::max(right,double(p[0]));
     top=std::min(top,double(p[1]));bottom=std::max(bottom,double(p[1]));
    }
    if(bounded){box.left=std::max(box.left,std::int64_t(std::floor(left))-2);box.right=std::min(box.right,std::int64_t(std::ceil(right))+3);
     box.top=std::max(box.top,std::int64_t(std::floor(top))-2);box.bottom=std::min(box.bottom,std::int64_t(std::ceil(bottom))+3);}
   }
   q.needed=(d.pipeline.colorMask&&color->intersects(box))||(d.pipeline.depthWrite&&depth&&depth->intersects(box));
   if(q.needed){
    if(d.texture){auto* source=need(d.texture,0);if(source)source->require_all();}
    // Shader depth tests may make any covered pixel depend on old depth.
    if(d.pipeline.depthTest&&depth)depth->require_all();
   }
  }
 }
 if(!valid){++rasterStats.conservativeRuns;for(auto& q:rasterCommands)q.needed=true;}
}
void Renderer::flush_raster(){
 finish_batch();
 if(rasterCommands.empty())return;
 const bool deferred=deferredRaster;const State saved=state;deferredRaster=false;
 resolve_raster_liveness();++rasterStats.runs;
 for(const auto& q:rasterCommands){
  if(!q.needed){if(q.clear)++rasterStats.skippedClears;else ++rasterStats.skippedDraws;continue;}
  if(q.clear){state=q.state;clear(q.clearFlags,q.color,q.clearDepth,q.stencil,reinterpret_cast<const i32*>(rasterRects.data()+q.rectOffset),q.rectCount);}
  else {const auto* data=rasterBytes.data()+q.vertexOffset;const auto* instance=q.instanceBytes?rasterBytes.data()+q.instanceOffset:nullptr;
   if(queuedUploads)queue_upload_draw(q.state,q.primitive,q.count,data,q.vertexBytes,instance,q.instanceBytes);
   else issue(q.state,q.primitive,q.count,data,q.vertexBytes,nullptr,IndexType::UInt16,instance,q.instanceBytes);
  }
 }
 submit_batches();state=saved;deferredRaster=deferred;rasterCommands.clear();rasterBytes.clear();rasterRects.clear();
}
void Renderer::submit_batches(){
 if(queuedDraws.empty())return;
 // One immutable backing per ordered run. Never update a store already
 // referenced by an earlier draw. CPU arrays and state are owned copies.
 upload(vertices,GL_ARRAY_BUFFER,queuedVertices.data(),queuedVertices.size(),1048576);
 for(const auto& q:queuedDraws)issue_uploaded(q.state,q.primitive,q.count,queuedVertices.data()+q.first*q.state.stride,q.vertexBytes,nullptr,IndexType::UInt16,q.instanceBytes?queuedInstances.data()+q.instanceOffset:nullptr,q.instanceBytes,q.first,true);
 queuedDraws.clear();queuedVertices.clear();queuedInstances.clear();
}
void Renderer::flush(){finish_batch();submit_batches();}
void Renderer::discard(){batching=instancing=false;batchBytes.clear();worlds.clear();batchCount=0;queuedDraws.clear();queuedVertices.clear();queuedInstances.clear();rasterCommands.clear();rasterBytes.clear();rasterRects.clear();}
void Renderer::issue(const State& d,Topology primitive,u32 count,const void* data,u32 size,const void* index,IndexType indexFormat,const void* instance,u32 instanceSize){
 issue_uploaded(d,primitive,count,data,size,index,indexFormat,instance,instanceSize,0,false);
}
void Renderer::issue_uploaded(const State& d,Topology primitive,u32 count,const void* data,u32 size,const void* index,IndexType indexFormat,const void* instance,u32 instanceSize,u32 first,bool verticesUploaded){
 stats.batches++;const auto& p=d.pipeline;auto& cached=drawState.pipeline;const bool all=!drawState.valid;
 if(all||cached.dither!=p.dither)p.dither?glEnable(GL_DITHER):glDisable(GL_DITHER);
 GPU* texture=d.texture?&surface(d.texture):nullptr;auto& g=target(d.target,d.depth);const auto& v=d.viewport;
 if(!drawState.has_viewport||std::memcmp(&drawState.viewport,&v,sizeof(v))){glViewport(v.x,v.y,v.width,v.height);glDepthRangef(v.min,v.max);drawState.viewport=v;drawState.has_viewport=true;}select(d);
 float vp[]{float(v.x),float(v.y),float(v.width),float(v.height)};uniform(0,4,vp);const bool transformed=d.layout.screen;
 integer(1,transformed);integer(2,p.textureTransform);integer(3,!!instance);
 // Instance attributes own world/texture/factor. Leave their uniform caches
 // describing actual GL values so a later singleton restores them normally.
 if(!transformed){if(!instance)uniform(4,16,d.matrix[0].data());uniform(5,16,d.matrix[1].data());uniform(6,16,d.matrix[2].data());if(p.textureTransform&&!instance)uniform(7,16,d.matrix[3].data());}
 integer(8,p.fog);if(p.fog){if(!transformed){integer(9,p.rangeFog);integer(10,u32(p.fogMode));float fog[]{p.fogNear,p.fogFar,p.fogDensity};uniform(11,3,fog);}uniform(12,4,color(p.fogColor).data());}integer(13,!!texture);if(!instance)uniform(14,4,color(p.textureFactor).data());
 auto arg=[](Argument a){return int(u32(a.source)|(a.complement?16u:0u)|(a.alphaOnly?32u:0u));};
 integer(15,u32(p.color.operation));integer(16,arg(p.color.first));integer(17,arg(p.color.second));integer(18,u32(p.alpha.operation));integer(19,arg(p.alpha.first));integer(20,arg(p.alpha.second));integer(21,p.alphaTest);float alpha=float(p.alphaReference)/255;uniform(22,1,&alpha);integer(23,u32(p.alphaCompare));integer(24,version==10&&p.depthTest&&d.depth&&resolve(owner,d.depth).format==PixelFormat::Depth16);
 if(texture){
  glActiveTexture(GL_TEXTURE0);bind_texture(texture->texture);
  constexpr GLint address[]{GL_REPEAT,GL_MIRRORED_REPEAT,GL_CLAMP_TO_EDGE};GLint sampler[]{address[u32(p.addressU)],address[u32(p.addressV)],p.minFilter==Filter::Nearest?GL_NEAREST:GL_LINEAR,p.magFilter==Filter::Nearest?GL_NEAREST:GL_LINEAR};
  constexpr GLenum names[]{GL_TEXTURE_WRAP_S,GL_TEXTURE_WRAP_T,GL_TEXTURE_MIN_FILTER,GL_TEXTURE_MAG_FILTER};
  for(int i=0;i<4;i++)if(texture->sampler[i]!=sampler[i]){glTexParameteri(GL_TEXTURE_2D,names[i],sampler[i]);texture->sampler[i]=sampler[i];}
 }
 constexpr GLenum compare[]{GL_NEVER,GL_LESS,GL_EQUAL,GL_LEQUAL,GL_GREATER,GL_NOTEQUAL,GL_GEQUAL,GL_ALWAYS};
 if(all||cached.depthTest!=p.depthTest)p.depthTest?glEnable(GL_DEPTH_TEST):glDisable(GL_DEPTH_TEST);
 if(all||cached.depthWrite!=p.depthWrite)glDepthMask(p.depthWrite);
 if(all||cached.depthCompare!=p.depthCompare)glDepthFunc(compare[u32(p.depthCompare)]);
 if(all||cached.blend!=p.blend)p.blend?glEnable(GL_BLEND):glDisable(GL_BLEND);
 // Set blend parameters even while blending is disabled, so the cache always
 // describes actual GL state when a subsequent batch enables it.
 constexpr GLenum blend[]{GL_ZERO,GL_ONE,GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_DST_ALPHA,GL_ONE_MINUS_DST_ALPHA,GL_DST_COLOR,GL_ONE_MINUS_DST_COLOR,GL_SRC_ALPHA_SATURATE};
 if(all||cached.sourceBlend!=p.sourceBlend||cached.destinationBlend!=p.destinationBlend)glBlendFunc(blend[u32(p.sourceBlend)],blend[u32(p.destinationBlend)]);
 constexpr GLenum equation[]{GL_FUNC_ADD,GL_FUNC_SUBTRACT,GL_FUNC_REVERSE_SUBTRACT,GL_MIN,GL_MAX};
 if(all||cached.blendEquation!=p.blendEquation)glBlendEquation(equation[u32(p.blendEquation)]);
 if(all||cached.cull!=p.cull){if(p.cull!=Cull::None){glEnable(GL_CULL_FACE);glFrontFace(GL_CW);glCullFace(p.cull==Cull::Back?GL_BACK:GL_FRONT);}else glDisable(GL_CULL_FACE);}
 if(all||cached.colorMask!=p.colorMask)glColorMask(!!(p.colorMask&1),!!(p.colorMask&2),!!(p.colorMask&4),!!(p.colorMask&8));
 cached=p;drawState.valid=true;
 const std::array<u32,6> layoutKey{d.stride,u32(d.layout.screen),d.layout.diffuse,d.layout.specular,d.layout.uv,u32(!!instance)};
 bool resident=false;
#ifdef __EMSCRIPTEN__
 resident=!verticesUploaded&&residentUploads&&d.layout==attributes(VertexLayout::WorldUv)&&d.stride==20&&primitive==Topology::Strip&&count==2&&!index&&size==80;
 if(resident&&!residentQuadVbo)glGenBuffers(1,&residentQuadVbo);
#endif
 // Separate dynamic/resident and singleton/instanced VAOs. All eligible
 // models share ONE resident VBO, never one buffer/VAO per draw ordinal.
 auto& vao=resident?residentLayouts[{layoutKey[0],layoutKey[1],layoutKey[2],layoutKey[3],layoutKey[4],layoutKey[5],residentQuadVbo}]:layouts[layoutKey];const bool setup=!vao;
 if(setup)glGenVertexArrays(1,&vao);if(currentLayout!=vao){glBindVertexArray(vao);currentLayout=vao;}
#ifdef __EMSCRIPTEN__
 // This cache describes only the last actual contents of vertices.id. Every
 // other vertices upload invalidates it in upload(); instance/index stores
 // are independent. Rebinding remains necessary when creating another VAO.
 const bool reusable=d.layout==attributes(VertexLayout::WorldUv)&&d.stride==20&&primitive==Topology::Strip&&count==2&&!index&&size==80;
 u32 base=0;
 if(verticesUploaded){if(setup)glBindBuffer(GL_ARRAY_BUFFER,vertices.id);}
 else if(resident){
  const bool changed=!residentQuadValid||std::memcmp(residentQuad.data(),data,80)!=0;
  if(changed||setup)glBindBuffer(GL_ARRAY_BUFFER,residentQuadVbo);
  if(changed){glBufferData(GL_ARRAY_BUFFER,80,data,GL_STATIC_DRAW);std::memcpy(residentQuad.data(),data,80);residentQuadValid=true;++stats.bufferReplacements;stats.vertexUploadBytes+=80;}
 }
 else if(reusable&&vertexQuadValid&&!std::memcmp(vertexQuad.data(),data,80))glBindBuffer(GL_ARRAY_BUFFER,vertices.id);
 else {base=upload(vertices,GL_ARRAY_BUFFER,data,size,1048576);if(reusable){std::memcpy(vertexQuad.data(),data,80);vertexQuadValid=true;}}
#else
 const u32 base=upload(vertices,GL_ARRAY_BUFFER,data,size,1048576);
#endif
#ifdef __EMSCRIPTEN__
 if(setup){
#else
 {
#endif
 ++stats.layoutSetups;
glEnableVertexAttribArray(0);glVertexAttribPointer(0,transformed?4:3,GL_FLOAT,false,d.stride,reinterpret_cast<void*>(base));
 for(int j=0;j<2;j++){const int loc=j?3:1;const u32 offset=j?d.layout.specular:d.layout.diffuse;if(offset!=VertexAttributes::absent){glEnableVertexAttribArray(loc);glVertexAttribPointer(loc,4,GL_UNSIGNED_BYTE,true,d.stride,reinterpret_cast<void*>(base+offset));}else{glDisableVertexAttribArray(loc);glVertexAttrib4f(loc,1,1,1,1);}}
 if(d.layout.uv!=VertexAttributes::absent){glEnableVertexAttribArray(2);glVertexAttribPointer(2,2,GL_FLOAT,false,d.stride,reinterpret_cast<void*>(base+d.layout.uv));}else{glDisableVertexAttribArray(2);glVertexAttrib2f(2,0,0);}
 }
 constexpr GLenum modes[]{GL_POINTS,GL_LINES,GL_LINE_STRIP,GL_TRIANGLES,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN};const GLenum mode=modes[u32(primitive)];const u32 n=vertex_count(primitive,count);
 if(instance){u32 b=upload(instances,GL_ARRAY_BUFFER,instance,instanceSize,65536);
#ifdef __EMSCRIPTEN__
 if(setup){
#else
 {
#endif
 for(int i=0;i<4;i++){glEnableVertexAttribArray(4+i);glVertexAttribPointer(4+i,4,GL_FLOAT,false,instanceStride,reinterpret_cast<void*>(b+i*16));glVertexAttribDivisor(4+i,1);}glEnableVertexAttribArray(8);glVertexAttribPointer(8,4,GL_UNSIGNED_BYTE,true,instanceStride,reinterpret_cast<void*>(b+64));glVertexAttribDivisor(8,1);
 for(int i=0;i<4;i++){glEnableVertexAttribArray(9+i);glVertexAttribPointer(9+i,4,GL_FLOAT,false,instanceStride,reinterpret_cast<void*>(b+68+i*16));glVertexAttribDivisor(9+i,1);}}
 glDrawArraysInstanced(mode,first,n,instanceSize/instanceStride);}
 else if(index){u32 b=upload(indices,GL_ELEMENT_ARRAY_BUFFER,index,n*(indexFormat==IndexType::UInt16?2:4),65536);glDrawElements(mode,n,indexFormat==IndexType::UInt16?GL_UNSIGNED_SHORT:GL_UNSIGNED_INT,reinterpret_cast<void*>(b));}
 else glDrawArrays(mode,first,n);g.rendered=true;
}
void Renderer::clear(u32 flags,u32 c,float depth,u32 stencil,const i32* rects,u32 count){flush();const auto& v=state.viewport;i32 box[]{i32(v.x),i32(v.y),i32(v.x+v.width),i32(v.y+v.height)};if(!rects){rects=box;count=1;}if(deferredRaster&&queue_raster_clear(flags,c,depth,stencil,rects,count))return;auto& g=target(state.target,state.depth);glEnable(GL_SCISSOR_TEST);glColorMask(true,true,true,true);glDepthMask(true);drawState.pipeline.colorMask=15;drawState.pipeline.depthWrite=true;auto rgba=color(c);glClearColor(rgba[0],rgba[1],rgba[2],rgba[3]);glClearDepthf(depth);glClearStencil(stencil);glStencilMask(255);GLbitfield bits=(flags&1?GL_COLOR_BUFFER_BIT:0)|(flags&2?GL_DEPTH_BUFFER_BIT:0)|(flags&4?GL_STENCIL_BUFFER_BIT:0);for(u32 i=0;i<count;i++){const i32* p=rects+i*4;glScissor(p[0],p[1],p[2]-p[0],p[3]-p[1]);glClear(bits);}glDisable(GL_SCISSOR_TEST);g.rendered=true;}
void Renderer::copy(u32 src,const i32* rect,u32 dst,const i32* point){flush_raster();flush();auto& a=surface(src);auto& b=surface(dst);bind_framebuffer(GL_READ_FRAMEBUFFER,a.framebuffer);bind_framebuffer(GL_DRAW_FRAMEBUFFER,b.framebuffer);glDisable(GL_SCISSOR_TEST);glBlitFramebuffer(rect[0],rect[1],rect[2],rect[3],point[0],point[1],point[0]+rect[2]-rect[0],point[1]+rect[3]-rect[1],GL_COLOR_BUFFER_BIT,GL_NEAREST);b.rendered=true;b.version=resolve(owner,dst).version+1;}
bool Renderer::resample(u32 src,const i32* from,u32 dst,const i32* to,const float* weights,u32 width,u32 height){
 flush_raster();
 if(src==dst)return false;
 auto masks=[](PixelFormat f)->std::array<GLint,4>{switch(f){case PixelFormat::Bgra8:return {255,255,255,255};case PixelFormat::Bgrx8:return {255,255,255,0};case PixelFormat::Rgb565:return {31,63,31,0};case PixelFormat::Xrgb1555:return {31,31,31,0};case PixelFormat::Argb1555:return {31,31,31,1};case PixelFormat::Argb4444:return {15,15,15,15};default:return {};}};
 const auto a=resolve(owner,src),b=resolve(owner,dst);const auto im=masks(a.format),om=masks(b.format);if(!im[0]||!om[0])return false;
 flush();auto& input=surface(src);auto& output=target(dst,0);
 glUseProgram(resampleProgram);currentProgram=resampleProgram;glBindVertexArray(resampleVao);currentLayout=resampleVao;
 glDisable(GL_BLEND);glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);glDisable(GL_SCISSOR_TEST);glDisable(GL_DITHER);glColorMask(true,true,true,true);glDepthMask(false);
 glViewport(to[0],to[1],to[2]-to[0],to[3]-to[1]);
 glActiveTexture(GL_TEXTURE0);bind_texture(input.texture);glUniform1i(glGetUniformLocation(resampleProgram,"sourceImage"),0);
 glActiveTexture(GL_TEXTURE1);glBindTexture(GL_TEXTURE_2D,weightTexture);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
 if(weights)glTexImage2D(GL_TEXTURE_2D,0,GL_RG32F,width,height,0,GL_RG,GL_FLOAT,weights);
 else {const float zero[2]{};glTexImage2D(GL_TEXTURE_2D,0,GL_RG32F,1,1,0,GL_RG,GL_FLOAT,zero);}
 glUniform1i(glGetUniformLocation(resampleProgram,"weights"),1);glUniform1i(glGetUniformLocation(resampleProgram,"triangle"),weights!=nullptr);
 glUniform4i(glGetUniformLocation(resampleProgram,"sourceRect"),from[0],from[1],from[2]-from[0],from[3]-from[1]);
 glUniform4i(glGetUniformLocation(resampleProgram,"destinationRect"),to[0],to[1],to[2]-to[0],to[3]-to[1]);
 glUniform4iv(glGetUniformLocation(resampleProgram,"sourceMask"),1,im.data());glUniform4iv(glGetUniformLocation(resampleProgram,"destinationMask"),1,om.data());
 glDrawArrays(GL_TRIANGLES,0,3);glActiveTexture(GL_TEXTURE0);
 // Utility rendering changes masks, viewport and program. Vertex layouts remain
 // valid VAOs, while the regular draw cache must restore all pipeline state.
 drawState=DrawStateCache{};output.rendered=true;output.version=b.version+1;++stats.resamples;return true;
}
void Renderer::read(u32 id){flush_raster();flush();auto it=surfaces.find(id);if(it==surfaces.end()||!it->second.rendered)return;auto s=resolve(owner,id);auto& g=it->second;stats.readBytes+=s.size;pixels.resize(s.width*s.height*4);bind_framebuffer(GL_FRAMEBUFFER,g.framebuffer);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,s.width,s.height,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
 for(u32 y=0;y<s.height;y++)for(u32 x=0;x<s.width;x++){const auto* p=pixels.data()+(y*s.width+x)*4;u32 r=p[0],gc=p[1],b=p[2],a=p[3];auto* out=s.data+y*s.pitch;
  if(s.format==PixelFormat::Bgra8||s.format==PixelFormat::Bgrx8){out+=x*4;out[0]=b;out[1]=gc;out[2]=r;out[3]=s.format==PixelFormat::Bgra8?a:255;}else if((s.format==PixelFormat::Rgb565||s.format==PixelFormat::Xrgb1555||s.format==PixelFormat::Argb1555||s.format==PixelFormat::Argb4444)){uint16_t n=s.format==PixelFormat::Argb4444?(a>>4)<<12|(r>>4)<<8|(gc>>4)<<4|(b>>4):s.format==PixelFormat::Rgb565?(r>>3)<<11|(gc>>2)<<5|(b>>3):(s.format==PixelFormat::Argb1555&&a>=128?0x8000:0)|(r>>3)<<10|(gc>>3)<<5|(b>>3);std::memcpy(out+x*2,&n,2);}else if(s.format==PixelFormat::Alpha8)out[x]=a;else if(s.format==PixelFormat::Luminance8)out[x]=r;else std::abort();
 }g.rendered=false;
}
void Renderer::release(u32 id){flush_raster();flush();auto d=depths.find(id);if(d!=depths.end()){glDeleteRenderbuffers(1,&d->second.buffer);depths.erase(d);}auto it=surfaces.find(id);if(it==surfaces.end())return;glDeleteTextures(1,&it->second.texture);glDeleteFramebuffers(1,&it->second.framebuffer);surfaces.erase(it);boundTexture=readFramebuffer=drawFramebuffer=~0u;}
void Renderer::render_imgui(const ImDrawData* data,u32 targetHandle){
 flush_raster();
 if(!data||data->CmdListsCount<=0||data->TotalVtxCount<=0||data->TotalIdxCount<=0)return;
 // TH11's backbuffer is handle 1 and depth buffer is handle 2. The TH10
 // sentinel 0xffffffff is not a valid TH11 surface; ImGui needs no depth.
 flush();auto& output=target(targetHandle,0);
 if(!imguiProgram){
  const auto vs=shader(GL_VERTEX_SHADER,imguiVertexSource),fs=shader(GL_FRAGMENT_SHADER,imguiFragmentSource);imguiProgram=glCreateProgram();glAttachShader(imguiProgram,vs);glAttachShader(imguiProgram,fs);glLinkProgram(imguiProgram);glDeleteShader(vs);glDeleteShader(fs);
  GLint linked=0;glGetProgramiv(imguiProgram,GL_LINK_STATUS,&linked);if(!linked){char log[1024]{};glGetProgramInfoLog(imguiProgram,sizeof(log),nullptr,log);failure=std::string("ImGui program: ")+log;return;}
  imguiProjMtx=glGetUniformLocation(imguiProgram,"u_ProjMtx");imguiTexture=glGetUniformLocation(imguiProgram,"Texture");
  glGenVertexArrays(1,&imguiVao);glGenBuffers(1,&imguiVbo);glGenBuffers(1,&imguiEbo);glBindVertexArray(imguiVao);glBindBuffer(GL_ARRAY_BUFFER,imguiVbo);glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,imguiEbo);
  glEnableVertexAttribArray(0);glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(ImDrawVert),reinterpret_cast<void*>(offsetof(ImDrawVert,pos)));
  glEnableVertexAttribArray(1);glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(ImDrawVert),reinterpret_cast<void*>(offsetof(ImDrawVert,uv)));
  glEnableVertexAttribArray(2);glVertexAttribPointer(2,4,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(ImDrawVert),reinterpret_cast<void*>(offsetof(ImDrawVert,col)));
 }
 auto& io=ImGui::GetIO();if(imguiFontTexture&&io.Fonts->TexID==nullptr){glDeleteTextures(1,&imguiFontTexture);imguiFontTexture=0;}
 if(!imguiFontTexture){unsigned char* pixels=nullptr;int width=0,height=0;io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);if(!pixels||width<=0||height<=0)return;
  glGenTextures(1,&imguiFontTexture);glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,imguiFontTexture);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);glPixelStorei(GL_UNPACK_ALIGNMENT,1);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);io.Fonts->TexID=reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(imguiFontTexture));io.Fonts->ClearTexData();
 }
 bind_framebuffer(GL_FRAMEBUFFER,output.framebuffer);glViewport(0,0,640,480);glEnable(GL_BLEND);glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);glBlendFuncSeparate(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);glDisable(GL_CULL_FACE);glDisable(GL_DEPTH_TEST);glDepthMask(GL_FALSE);glEnable(GL_SCISSOR_TEST);glUseProgram(imguiProgram);glActiveTexture(GL_TEXTURE0);glBindVertexArray(imguiVao);glBindBuffer(GL_ARRAY_BUFFER,imguiVbo);glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,imguiEbo);glUniform1i(imguiTexture,0);
 const ImVec2 clipOffset=data->DisplayPos,clipScale=data->FramebufferScale;const float displayWidth=data->DisplaySize.x*clipScale.x,displayHeight=data->DisplaySize.y*clipScale.y,left=data->DisplayPos.x,right=left+data->DisplaySize.x,top=data->DisplayPos.y,bottom=top+data->DisplaySize.y;
 // The backbuffer is presented with a vertical flip (see commit()), so draw
 // ImGui pre-flipped: mirror the projection Y and skip the GL-origin scissor
 // conversion below. The two flips compose to identity for the overlay.
 const float projection[4][4]={{2.0f/(right-left),0,0,0},{0,2.0f/(bottom-top),0,0},{0,0,-1,0},{(right+left)/(left-right),(top+bottom)/(top-bottom),0,1}};glUniformMatrix4fv(imguiProjMtx,1,GL_FALSE,&projection[0][0]);
 glBufferData(GL_ARRAY_BUFFER,GLsizeiptr(data->TotalVtxCount*sizeof(ImDrawVert)),nullptr,GL_STREAM_DRAW);glBufferData(GL_ELEMENT_ARRAY_BUFFER,GLsizeiptr(data->TotalIdxCount*sizeof(ImDrawIdx)),nullptr,GL_STREAM_DRAW);GLsizeiptr vertexOffset=0,indexOffset=0;
 for(int listIndex=0;listIndex<data->CmdListsCount;++listIndex){const auto* list=data->CmdLists[listIndex];const GLsizeiptr vertexBytes=GLsizeiptr(list->VtxBuffer.Size*sizeof(ImDrawVert)),indexBytes=GLsizeiptr(list->IdxBuffer.Size*sizeof(ImDrawIdx));glBufferSubData(GL_ARRAY_BUFFER,vertexOffset,vertexBytes,list->VtxBuffer.Data);glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,indexOffset,indexBytes,list->IdxBuffer.Data);
  for(int commandIndex=0;commandIndex<list->CmdBuffer.Size;++commandIndex){const auto& command=list->CmdBuffer[commandIndex];if(command.UserCallback){if(command.UserCallback==ImDrawCallback_ResetRenderState){glUseProgram(imguiProgram);glBindVertexArray(imguiVao);glUniformMatrix4fv(imguiProjMtx,1,GL_FALSE,&projection[0][0]);}else command.UserCallback(list,&command);continue;}
   ImVec4 clip{(command.ClipRect.x-clipOffset.x)*clipScale.x,(command.ClipRect.y-clipOffset.y)*clipScale.y,(command.ClipRect.z-clipOffset.x)*clipScale.x,(command.ClipRect.w-clipOffset.y)*clipScale.y};if(clip.x>=clip.z||clip.y>=clip.w||clip.z<=0||clip.w<=0||clip.x>=displayWidth||clip.y>=displayHeight)continue;
   const GLint x=GLint(std::floor(std::max(clip.x,0.0f))),y=GLint(std::floor(std::max(clip.y,0.0f)));const GLsizei width=GLsizei(std::ceil(std::min(clip.z,displayWidth))-x),height=GLsizei(std::ceil(std::min(clip.w,displayHeight))-y);if(width<=0||height<=0)continue;glScissor(x,y,width,height);
   const GLuint texture=command.TextureId?GLuint(reinterpret_cast<uintptr_t>(command.TextureId)):imguiFontTexture;glBindTexture(GL_TEXTURE_2D,texture);const auto base=vertexOffset+GLsizeiptr(command.VtxOffset*sizeof(ImDrawVert));glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(ImDrawVert),reinterpret_cast<void*>(base+offsetof(ImDrawVert,pos)));glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(ImDrawVert),reinterpret_cast<void*>(base+offsetof(ImDrawVert,uv)));glVertexAttribPointer(2,4,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(ImDrawVert),reinterpret_cast<void*>(base+offsetof(ImDrawVert,col)));glDrawElements(GL_TRIANGLES,GLsizei(command.ElemCount),sizeof(ImDrawIdx)==2?GL_UNSIGNED_SHORT:GL_UNSIGNED_INT,reinterpret_cast<void*>(indexOffset+GLsizeiptr(command.IdxOffset*sizeof(ImDrawIdx))));
  }vertexOffset+=vertexBytes;indexOffset+=indexBytes;
 }
 // ImGui is an end-of-frame overlay on the existing GPU backbuffer.  Keep the
 // GPU copy synchronized with the current CPU revision: advancing the private
 // GPU version here makes surface() treat the unchanged CPU pixels as newer on
 // the next paused/menu frame and upload them over the overlay/game image.
 output.rendered=true;drawState=DrawStateCache{};currentProgram=0;currentLayout=0;boundTexture=readFramebuffer=drawFramebuffer=~0u;glDepthMask(GL_TRUE);
}
void Renderer::present(u32 id){flush();pending=id;stats.frames++;if(!defer)commit();}
bool Renderer::commit(){if(!pending)return false;flush_raster();auto s=resolve(owner,pending);auto& g=surface(pending);pending=0;bind_framebuffer(GL_READ_FRAMEBUFFER,g.framebuffer);bind_framebuffer(GL_DRAW_FRAMEBUFFER,0);glDisable(GL_SCISSOR_TEST);glBlitFramebuffer(0,0,s.width,s.height,0,480,640,0,GL_COLOR_BUFFER_BIT,GL_NEAREST);SDL_GL_SwapWindow(window);stats.presentations++;return true;}
}
extern "C" {
const touhou::sdl::Statistics* sdl_stats(){static touhou::sdl::Statistics zero{};return touhou::sdl::current()?&touhou::sdl::current()->stats:&zero;}
const touhou::sdl::Renderer::RasterStatistics* sdl_raster_stats(){static touhou::sdl::Renderer::RasterStatistics zero{};return touhou::sdl::current()?&touhou::sdl::current()->rasterStats:&zero;}
void sdl_defer(int enabled){if(auto* r=touhou::sdl::current())r->defer=enabled;}
int sdl_commit(){return touhou::sdl::current()&&touhou::sdl::current()->commit();}
const char* sdl_error(){return touhou::sdl::current()?touhou::sdl::current()->error():SDL_GetError();}
int sdl_version(){return SDL_GetVersion();}
double sdl_ticks(){return double(SDL_GetTicksNS())/1000000.0;}
__attribute__((export_name("sdl_canvas"))) void sdl_canvas(const char* selector){SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR,selector);}
__attribute__((export_name("sdl_pump"))) void sdl_pump(){SDL_Event event;while(SDL_PollEvent(&event)){};}
}
