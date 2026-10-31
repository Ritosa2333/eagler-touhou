#include "Renderer.hpp"
#include "Shaders.hpp"
#include "ResampleShader.hpp"
#include <emscripten.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>

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
 PipelineState pipeline{};Viewport viewport{};bool valid=false,has_viewport=false,depthEnabled=false,blendEnabled=false;
} drawState;
constexpr u32 instanceStride=132;
constexpr std::array<float,16> identity{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
float bits(u32 n){float f;std::memcpy(&f,&n,4);return f;}
std::array<float,4> color(u32 n){return {float(n>>16&255)/255,float(n>>8&255)/255,float(n&255)/255,float(n>>24)/255};}
void replace(std::string& s,const std::string& a,const std::string& b){size_t p=0;while((p=s.find(a,p))!=std::string::npos){s.replace(p,a.size(),b);p+=b.size();}}
GLuint shader(GLenum type,const std::string& source){GLuint s=glCreateShader(type);const char* p=source.c_str();glShaderSource(s,1,&p,nullptr);glCompileShader(s);GLint ok=0;glGetShaderiv(s,GL_COMPILE_STATUS,&ok);if(!ok){char log[2048];glGetShaderInfoLog(s,sizeof(log),nullptr,log);std::fprintf(stderr,"SDL shader: %s\n",log);std::abort();}return s;}

bool equal(const State& a,const State& b,bool world=false){
 if(a.layout!=b.layout||a.stride!=b.stride||a.texture!=b.texture||a.target!=b.target||a.depth!=b.depth||std::memcmp(&a.viewport,&b.viewport,sizeof(Viewport)))return false;
 if(!a.pipeline.compatible(b.pipeline,world))return false;
 // Instanced quads carry their world and texture matrices. View/projection
 // remain shared, and adjacent primitives retain their original draw order.
 if(world){for(int i=1;i<3;i++)if(a.matrix[i]!=b.matrix[i])return false;}
 else if(!a.layout.screen&&a.matrix!=b.matrix)return false;
 return true;
}
}
Renderer* current(){return active;}void set_current(Renderer* r){active=r;}
State::State(){for(auto& m:matrix)m=identity;}
Renderer::Renderer(int v,Resolve r,void* p):version(v),resolve(r),owner(p){}
bool Renderer::initialize(){
 if(!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)){failure=SDL_GetError();return false;}
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_ES);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
 SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,0);SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,0);SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,0);SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
 window=SDL_CreateWindow(version==8?"Touhou 08":"Touhou 10",640,480,SDL_WINDOW_OPENGL);if(!window){failure=SDL_GetError();return false;}
 SDL_SetWindowSize(window,640,480);
 context=SDL_GL_CreateContext(window);if(!context){failure=SDL_GetError();return false;}SDL_GL_SetSwapInterval(1);drawState=DrawStateCache{};
 std::string source=vertexSource;if(version==10&&touhou_clip_control()){replace(source,"position.z*2.0-1.0","position.z");replace(source,"gl_Position.z=gl_Position.z*2.0-gl_Position.w;","");}
 vertex=shader(GL_VERTEX_SHADER,source);glGenBuffers(1,&vertices.id);glGenBuffers(1,&indices.id);glGenBuffers(1,&instances.id);
 glActiveTexture(GL_TEXTURE0);buildingGeneric=true;select(State{});generic=*program;programs.clear();buildingGeneric=false;
 auto warm_program=[this](State& s){select(s);s.pipeline.depthTest=true;select(s);s.pipeline.depthTest=false;};
 State warm;for(int i=0;i<4;i++){warm.texture=i!=2;auto& p=warm.pipeline;p.alphaTest=true;p.fog=i==3;p.alphaCompare=Compare::GreaterEqual;p.color.operation=p.alpha.operation=(i==0||i==3)?ColorOperation::Multiply:ColorOperation::First;p.color.first=p.alpha.first={(i==0||i==3)?ArgumentSource::Texture:ArgumentSource::Diffuse};p.color.second=p.alpha.second={i==3?ArgumentSource::Factor:ArgumentSource::Diffuse};warm_program(warm);}
 // Compile the game combiner families before the first frame. Rare asset
 // combinations use a prelinked uniform program instead of compiling in draw.
 for(bool texture:{false,true})for(bool alpha:{false,true})for(bool fog:{false,true})for(auto op:{ColorOperation::First,ColorOperation::Multiply})for(auto second:{ArgumentSource::Diffuse,ArgumentSource::Factor}){
   warm.texture=texture;auto& p=warm.pipeline;p.alphaTest=alpha;p.fog=fog;p.alphaCompare=Compare::GreaterEqual;
   p.color.operation=p.alpha.operation=op;p.color.first=p.alpha.first={op==ColorOperation::First?ArgumentSource::Diffuse:ArgumentSource::Texture};p.color.second=p.alpha.second={second};warm_program(warm);
 }
 resampleProgram=glCreateProgram();const auto rv=shader(GL_VERTEX_SHADER,resampleVertex),rf=shader(GL_FRAGMENT_SHADER,resampleFragment);
 glAttachShader(resampleProgram,rv);glAttachShader(resampleProgram,rf);glLinkProgram(resampleProgram);glDeleteShader(rv);glDeleteShader(rf);
 GLint linked=0;glGetProgramiv(resampleProgram,GL_LINK_STATUS,&linked);if(!linked){failure="Resample program link failed";return false;}
 glGenVertexArrays(1,&resampleVao);glGenTextures(1,&weightTexture);
 warming=false;set_current(this);return true;
}
Renderer::~Renderer(){discard();if(context){SDL_GL_MakeCurrent(window,context);for(auto& [id,g]:surfaces){glDeleteTextures(1,&g.texture);glDeleteFramebuffers(1,&g.framebuffer);}for(auto& [id,d]:depths)glDeleteRenderbuffers(1,&d.buffer);for(auto& [k,p]:programs)glDeleteProgram(p.id);glDeleteProgram(resampleProgram);glDeleteVertexArrays(1,&resampleVao);glDeleteTextures(1,&weightTexture);glDeleteProgram(generic.id);for(auto& entry:layouts)glDeleteVertexArrays(1,&entry.second);glDeleteShader(vertex);glDeleteBuffers(1,&vertices.id);glDeleteBuffers(1,&indices.id);glDeleteBuffers(1,&instances.id);glDeleteBuffers(1,&quadIndices);SDL_GL_DestroyContext(context);}if(window)SDL_DestroyWindow(window);if(active==this)active=nullptr;SDL_Quit();}
void Renderer::bind_texture(GLuint id){if(boundTexture!=id){glBindTexture(GL_TEXTURE_2D,id);boundTexture=id;++stats.textureBinds;}}
void Renderer::bind_framebuffer(GLenum target,GLuint id){
 const bool read=target!=GL_DRAW_FRAMEBUFFER,draw=target!=GL_READ_FRAMEBUFFER;
 if((read&&readFramebuffer!=id)||(draw&&drawFramebuffer!=id)){glBindFramebuffer(target,id);++stats.framebufferBinds;if(read)readFramebuffer=id;if(draw)drawFramebuffer=id;}
}
void Renderer::draw_batch(u32 count,const void* data,u32 stride){
 // The game already coalesced these triangles. Consume while its arena is
 // valid, without retaining a pointer to a buffer the next sprite can reuse.
 flush_batch();++stats.calls;state.stride=stride;stats.directBytes+=count*3*stride;
 issue(state,Topology::Triangles,count,data,count*3*stride,nullptr,IndexType::UInt16,nullptr,0);
}
void Renderer::draw_quads(u32 count,const void* data,u32 stride){
 if(!count)return;flush_batch();++stats.calls;state.stride=stride;stats.directBytes+=count*4*stride;
 issue(state,Topology::Triangles,count*2,data,count*4*stride,nullptr,IndexType::UInt32,nullptr,0,true);
}
void Renderer::transform(MatrixKind kind,const void* p){auto& m=state.matrix[u32(kind)];if(std::memcmp(m.data(),p,64))std::memcpy(m.data(),p,64);}
void Renderer::viewport(const Viewport& v){state.viewport=v;}
void Renderer::select(const State& s){
 const auto& p=s.pipeline;auto arg=[](Argument a){return u32(a.source)|(a.complement?16u:0u)|(a.alphaOnly?32u:0u);};
 std::array<u32,10> key{u32(!!s.texture),u32(p.alphaTest),u32(p.fog),u32(p.color.operation),u32(p.alpha.operation),arg(p.color.first),arg(p.color.second),arg(p.alpha.first),arg(p.alpha.second),u32(p.alphaCompare)|(u32(version==10&&p.depthTest&&(p.depthWrite||p.depthCompare!=Compare::Always))<<8)};auto it=programs.find(key);
 if(it==programs.end()&&!warming){program=&generic;++stats.genericBatches;if(currentProgram!=program->id){glUseProgram(program->id);currentProgram=program->id;}return;}
 if(it==programs.end()){
  ++stats.programCompiles;
  static const char* constants[]{"hasTexture","alphaTest","fogEnabled","colorOp","alphaOp","colorArg1","colorArg2","alphaArg1","alphaArg2","alphaFunc"};
  std::string source=fragmentSource,defs;if(!buildingGeneric){replace(source,"uniform bool hasTexture,alphaTest,fogEnabled;","");replace(source,"uniform int colorOp,alphaOp,colorArg1,colorArg2,alphaArg1,alphaArg2,alphaFunc;","");
  for(int i=0;i<10;i++)defs+="const "+std::string(i<3?"bool ":"int ")+constants[i]+"="+(i<3?(key[i]?"true":"false"):std::to_string(i==9?key[i]&255:key[i]))+";\n";
  replace(source,"precision highp float;","precision highp float;\n"+defs);}
  if(!buildingGeneric&&!(key[9]&256))replace(source,"gl_FragDepth=depth16?(floor(scaled)+floor(fract(scaled)-gl_FragCoord.z+0.5))/65535.0:gl_FragCoord.z;","");
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
void Renderer::changed(u32 id,u32 previous,const std::array<u32,4>& region){
 auto it=surfaces.find(id);if(it==surfaces.end())return;auto& g=it->second;
 if(region[0]>=region[2]||region[1]>=region[3]){g.dirty={};g.dirtyBase=~0u;return;}
 if(g.version==previous){g.dirty=region;g.dirtyBase=previous;}
 else if(g.dirtyBase==g.version&&g.dirty[0]<g.dirty[2]&&g.dirty[1]<g.dirty[3]){g.dirty[0]=std::min(g.dirty[0],region[0]);g.dirty[1]=std::min(g.dirty[1],region[1]);g.dirty[2]=std::max(g.dirty[2],region[2]);g.dirty[3]=std::max(g.dirty[3],region[3]);}
 else {g.dirty={};g.dirtyBase=~0u;}
}
Renderer::GPU& Renderer::surface(u32 h){
 auto s=resolve(owner,h);if(!s.handle||!s.width||!s.height){std::fprintf(stderr,"SDL invalid surface %u\n",h);std::abort();}auto& g=surfaces[h];if(!g.texture){glGenTextures(1,&g.texture);glGenFramebuffers(1,&g.framebuffer);bind_texture(g.texture);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);bind_framebuffer(GL_FRAMEBUFFER,g.framebuffer);glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,g.texture,0);}
 // Merge every known CPU edit since the last upload. Unknown edits retain
 // the full-image path so overlapping writes cannot be lost.
 const auto& r=g.dirty;
 if(g.version!=~0u&&g.version==g.dirtyBase&&g.version!=s.version&&s.renderScale==1&&s.format==PixelFormat::Argb4444&&r[0]<r[2]&&r[1]<r[3]&&r[2]<=s.width&&r[3]<=s.height){
  const u32 w=r[2]-r[0],h=r[3]-r[1];pixels.resize(w*h*2);
  for(u32 y=0;y<h;y++)for(u32 x=0;x<w;x++){uint16_t n;std::memcpy(&n,s.data+(y+r[1])*s.pitch+(x+r[0])*2,2);n=(n<<4)|(n>>12);std::memcpy(pixels.data()+(y*w+x)*2,&n,2);}
  bind_texture(g.texture);glPixelStorei(GL_UNPACK_ALIGNMENT,1);glTexSubImage2D(GL_TEXTURE_2D,0,r[0],r[1],w,h,GL_RGBA,GL_UNSIGNED_SHORT_4_4_4_4,pixels.data());stats.uploadBytes+=w*h*2;g.version=s.version;g.rendered=false;
 }else if(g.version!=s.version){
  stats.uploadBytes+=s.size;bind_texture(g.texture);glPixelStorei(GL_UNPACK_ALIGNMENT,1);GLenum format=GL_RGBA,type=GL_UNSIGNED_BYTE,internal=GL_RGBA8;
  bool rgb=s.format==PixelFormat::Bgr8||s.format==PixelFormat::Bgrx8||s.format==PixelFormat::Rgb565;u32 channels=rgb?3:4;pixels.resize(s.width*s.height*(s.format==PixelFormat::Argb4444?2:channels));
  if(s.format==PixelFormat::Argb4444){for(u32 y=0;y<s.height;y++)for(u32 x=0;x<s.width;x++){uint16_t n;std::memcpy(&n,s.data+y*s.pitch+x*2,2);n=(n<<4)|(n>>12);std::memcpy(pixels.data()+(y*s.width+x)*2,&n,2);}type=GL_UNSIGNED_SHORT_4_4_4_4;internal=GL_RGBA4;}
  else {for(u32 y=0;y<s.height;y++)for(u32 x=0;x<s.width;x++){
    const u8* p=s.data+y*s.pitch;u32 r=0,gc=0,b=0,a=255;
    if(s.format==PixelFormat::Bgra8||s.format==PixelFormat::Bgrx8){p+=x*4;b=p[0];gc=p[1];r=p[2];a=s.format==PixelFormat::Bgra8?p[3]:255;}
    else if(s.format==PixelFormat::Bgr8){p+=x*3;b=p[0];gc=p[1];r=p[2];}
    else if((s.format==PixelFormat::Rgb565||s.format==PixelFormat::Xrgb1555||s.format==PixelFormat::Argb1555)){uint16_t n;std::memcpy(&n,p+x*2,2);b=((n&31)*255+15)/31;if(s.format==PixelFormat::Rgb565){gc=((n>>5&63)*255+31)/63;r=((n>>11)*255+15)/31;}else{gc=((n>>5&31)*255+15)/31;r=((n>>10&31)*255+15)/31;a=s.format==PixelFormat::Argb1555?(n&0x8000?255:0):255;}}
    else if(s.format==PixelFormat::Argb8332){uint16_t n;std::memcpy(&n,p+x*2,2);r=((n>>5&7)*255+3)/7;gc=((n>>2&7)*255+3)/7;b=(n&3)*85;a=n>>8;}
    else if(s.format==PixelFormat::Alpha8){r=gc=b=255;a=p[x];}else if(s.format==PixelFormat::Luminance8)r=gc=b=p[x];else{std::fprintf(stderr,"SDL texture format %u\n",u32(s.format));std::abort();}
    u8* out=pixels.data()+(y*s.width+x)*channels;out[0]=r;out[1]=gc;out[2]=b;if(!rgb)out[3]=a;
   }format=rgb?GL_RGB:GL_RGBA;internal=s.format==PixelFormat::Rgb565?GL_RGB565:(s.format==PixelFormat::Xrgb1555||s.format==PixelFormat::Argb1555)?GL_RGB5_A1:rgb?GL_RGB8:GL_RGBA8;
  }
  const u32 scale=s.renderScale;
  if(scale>1){const auto logical=pixels;const u32 bytes=s.format==PixelFormat::Argb4444?2:channels;pixels.resize(size_t(s.width)*s.height*scale*scale*bytes);for(u32 y=0;y<s.height*scale;y++)for(u32 x=0;x<s.width*scale;x++)std::memcpy(pixels.data()+(size_t(y)*s.width*scale+x)*bytes,logical.data()+(size_t(y/scale)*s.width+x/scale)*bytes,bytes);}
  if(g.version==~0u)glTexImage2D(GL_TEXTURE_2D,0,internal,s.width*scale,s.height*scale,0,format,type,pixels.data());else glTexSubImage2D(GL_TEXTURE_2D,0,0,0,s.width*scale,s.height*scale,format,type,pixels.data());g.version=s.version;g.rendered=false;
 }g.dirty={};g.dirtyBase=~0u;return g;
}
Renderer::GPU& Renderer::target(u32 id,u32 depthId){auto& g=surface(id);
 // An opted-in draw/clear that cannot use depth only needs the color target.
 // Leave an existing attachment intact, but do not attach the shared depth
 // image to every 2D intermediate surface. That forces needless tile stores
 // and reloads on mobile GPUs even with depth testing disabled.
 if(depthId==~0u){bind_framebuffer(GL_FRAMEBUFFER,g.framebuffer);return g;}
 GLuint buffer=0;bool stencil=false;if(depthId){auto& d=depths[depthId];if(!d.buffer){auto s=resolve(owner,depthId);d.stencil=s.format==PixelFormat::Depth24Stencil8;glGenRenderbuffers(1,&d.buffer);glBindRenderbuffer(GL_RENDERBUFFER,d.buffer);glRenderbufferStorage(GL_RENDERBUFFER,d.stencil?GL_DEPTH24_STENCIL8:s.format==PixelFormat::Depth16?GL_DEPTH_COMPONENT16:GL_DEPTH_COMPONENT24,s.width*s.renderScale,s.height*s.renderScale);}buffer=d.buffer;stencil=d.stencil;}bind_framebuffer(GL_FRAMEBUFFER,g.framebuffer);if(g.attached!=buffer){glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,buffer);glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,stencil?buffer:0);g.attached=buffer;}return g;}
void Renderer::prepare(u32 h){surface(h);}
void Renderer::draw(Topology primitive,u32 count,const void* data,u32 stride,const void* index,IndexType indexFormat){
 stats.calls++;state.stride=stride;
 // Cancellation trails are many independent line strips with the same state.
 // Expand only their connectivity into ordered line pairs, then concatenate
 // adjacent compatible trails. Every original segment and endpoint is kept.
 if(staged_uploads&&!index&&(primitive==Topology::Lines||primitive==Topology::LineStrip)){
  if(!count)return;
  if(batching&&(instancing||batchTopology!=Topology::Lines||!equal(batchState,state)))flush_batch();
  if(!batching){batchState=state;batching=true;instancing=false;batchTopology=Topology::Lines;}
  const auto* source=static_cast<const u8*>(data);const size_t start=batchBytes.size(),size=size_t(count)*2*stride;
  batchBytes.resize(start+size);auto* destination=batchBytes.data()+start;u32 kept=0;
  const auto& p=state.pipeline;
  // Faded trail history remains in the simulation. Two zero-alpha endpoints
  // either fail the exact diffuse-alpha test or contribute zero under the
  // selected blend with depth writes disabled. Texture/factor alpha is excluded.
  const bool diffuseAlpha=!state.texture||p.alpha.operation==ColorOperation::Disabled||
   (p.alpha.operation==ColorOperation::First&&p.alpha.first.source==ArgumentSource::Diffuse&&!p.alpha.first.complement);
  const bool alphaReject=p.alphaTest&&(p.alphaCompare==Compare::Greater||(p.alphaCompare==Compare::GreaterEqual&&p.alphaReference>0));
  const bool zeroContribution=!p.depthWrite&&p.blend&&!p.separateAlphaBlend&&p.sourceBlend==BlendFactor::SourceAlpha&&
   (p.destinationBlend==BlendFactor::One||p.destinationBlend==BlendFactor::InverseSourceAlpha)&&
   (p.blendEquation==BlendEquation::Add||p.blendEquation==BlendEquation::ReverseSubtract);
  const bool invisible=state.layout.diffuse!=VertexAttributes::absent&&diffuseAlpha&&(alphaReject||zeroContribution);
  for(u32 i=0;i<count;i++){
   const auto* pair=source+size_t(primitive==Topology::Lines?i*2:i)*stride;
   if(invisible&&!pair[state.layout.diffuse+3]&&!pair[stride+state.layout.diffuse+3])continue;
   std::memcpy(destination+size_t(kept++)*2*stride,pair,2*stride);
  }
  const size_t copied=size_t(kept)*2*stride;batchBytes.resize(start+copied);stats.copiedBytes+=copied;batchCount+=kept;
  if(!batchCount)batching=false;else if(batchBytes.size()>=1048576)flush_batch();return;
 }
 if(index||primitive<Topology::Triangles||primitive>Topology::Fan){flush_batch();issue(state,primitive,count,data,vertex_count(primitive,count)*stride,index,indexFormat,nullptr,0);return;}
 const bool instance=version==10&&state.layout==attributes(VertexLayout::WorldUv)&&stride==20&&primitive==Topology::Strip&&count==2;
 if(instance){if(batching&&(!instancing||!equal(batchState,state,true)||quad.size()!=80||std::memcmp(quad.data(),data,80)))flush_batch();if(!batching){batchState=state;quad.assign(static_cast<const u8*>(data),static_cast<const u8*>(data)+80);batching=instancing=true;}const auto old=worlds.size();worlds.resize(old+instanceStride);std::memcpy(worlds.data()+old,state.matrix[0].data(),64);std::memcpy(worlds.data()+old+64,&state.pipeline.textureFactor,4);std::memcpy(worlds.data()+old+68,state.matrix[3].data(),64);if(worlds.size()>=1024*instanceStride)flush_batch();return;}
 if(batching&&(instancing||batchTopology==Topology::Lines||!equal(batchState,state)))flush_batch();if(!batching){batchState=state;batching=true;instancing=false;batchTopology=Topology::Triangles;}
 // Keep an isolated long strip/fan in its original topology. If a following
 // draw joins the batch, normalize it backwards before appending, preserving
 // triangle order and winding without retaining caller-owned vertex memory.
 if(version==10&&!batchCount&&count>=16&&(primitive==Topology::Strip||primitive==Topology::Fan)){
  const auto* src=static_cast<const u8*>(data);const size_t size=(count+2)*stride;
  batchTopology=primitive;batchBytes.assign(src,src+size);stats.copiedBytes+=size;batchCount=count;if(size>=1048576)flush_batch();return;
 }
 // Compatible long ribbons previously expanded both strips into triangle
 // lists as soon as the second ribbon arrived. Keep the shared vertices and
 // join with zero-area triangles. An even starting index preserves winding
 // and every original triangle's vertices (including the provoking vertex).
 if(staged_uploads&&batchTopology==Topology::Strip&&primitive==Topology::Strip&&count>=16){
  const size_t oldVertices=batchBytes.size()/stride,bridge=2+(oldVertices&1),added=(bridge+count+2)*stride,start=batchBytes.size();
  batchBytes.resize(start+added);auto* output=batchBytes.data()+start;
  for(size_t i=0;i<bridge-1;i++)std::memcpy(output+i*stride,batchBytes.data()+start-stride,stride);
  std::memcpy(output+(bridge-1)*stride,data,stride);
  std::memcpy(output+bridge*stride,data,size_t(count+2)*stride);
  batchCount=u32(oldVertices+bridge+count);stats.copiedBytes+=added;
  if(batchBytes.size()>=1048576)flush_batch();return;
 }
 if(batchTopology!=Topology::Triangles){
  batchScratch.resize(3*stride);batchBytes.resize(size_t(batchCount)*3*stride);
  for(u32 i=batchCount;i-->0;){const u32 a=batchTopology==Topology::Fan?0:i&1?i+1:i,b=batchTopology==Topology::Fan?i+1:i&1?i:i+1,c=i+2;u32 j=0;
   for(u32 v:{a,b,c}){std::memcpy(batchScratch.data()+j*stride,batchBytes.data()+v*stride,stride);++j;}
   std::memcpy(batchBytes.data()+size_t(i)*3*stride,batchScratch.data(),3*stride);
  }stats.copiedBytes+=batchCount*3*stride;batchTopology=Topology::Triangles;
 }
 const auto* src=static_cast<const u8*>(data);const size_t start=batchBytes.size(),size=count*3*stride;stats.copiedBytes+=size;batchBytes.resize(start+size);auto* dest=batchBytes.data()+start;
 if(primitive==Topology::Triangles)std::memcpy(dest,src,size);else if(count){std::memcpy(dest,src,3*stride);dest+=3*stride;for(u32 i=1;i<count;i++){u32 a=primitive==Topology::Fan?0:i&1?i+1:i,b=primitive==Topology::Fan?i+1:i&1?i:i+1,c=i+2;for(u32 v:{a,b,c}){std::memcpy(dest,src+v*stride,stride);dest+=stride;}}}batchCount+=count;if(batchBytes.size()>=1048576)flush_batch();
}
void Renderer::flush_batch(){if(!batching)return;batching=false;if(instancing)issue(batchState,Topology::Strip,2,quad.data(),quad.size(),nullptr,IndexType::UInt16,(additive_batches||worlds.size()>instanceStride)?worlds.data():nullptr,(additive_batches||worlds.size()>instanceStride)?worlds.size():0);else issue(batchState,batchTopology,batchCount,batchBytes.data(),batchBytes.size(),nullptr,IndexType::UInt16,nullptr,0);batchBytes.clear();worlds.clear();batchCount=0;instancing=false;}
void Renderer::flush(){flush_batch();drain();}
void Renderer::drain(){
 if(queuedDraws.empty())return;
 if(additive_batches){
  // Nonnegative additive color commutes on normalized 8-bit targets. No draw
  // may read existing depth. A span may have only one compatible depth-writing
  // group, whose internal primitive order remains unchanged after grouping.
  const auto additive=[this](const QueuedDraw& q){const auto& p=q.state.pipeline;const auto f=resolve(owner,q.state.target).format;return (!p.depthTest||p.depthCompare==Compare::Always)&&!p.dither&&p.blend&&!p.separateAlphaBlend&&p.blendEquation==BlendEquation::Add&&p.destinationBlend==BlendFactor::One&&(p.sourceBlend==BlendFactor::One||p.sourceBlend==BlendFactor::SourceAlpha)&&(f==PixelFormat::Bgra8||f==PixelFormat::Bgrx8);};
  const auto compatible=[this](const QueuedDraw& first,const QueuedDraw& q){const bool instances=q.instanceSize&&first.instanceSize;if(q.quads!=first.quads||q.topology!=first.topology||bool(q.instanceSize)!=bool(first.instanceSize)||!equal(q.state,first.state,instances))return false;if(instances)return q.size==first.size&&!std::memcmp(vertexArena.data()+q.offset,vertexArena.data()+first.offset,q.size);return q.topology==Topology::Triangles||q.topology==Topology::Lines;};
  // Keep the grouping plan in reusable flat arenas. The previous nested
  // vectors allocated a tiny heap block for every retained draw on every
  // flush, including spans too short to merge. Dense effects amplified that
  // allocator traffic without reducing any rendering work.
  const size_t endLink=queuedDraws.size();mergeGroups.clear();mergeGroups.reserve(endLink);mergeNext.resize(endLink);bool merged=false;
  const auto retain=[&](size_t first,size_t last){for(size_t i=first;i<last;i++){mergeNext[i]=endLink;mergeGroups.push_back({i,i});}};
  for(size_t at=0;at<queuedDraws.size();){
   if(!additive(queuedDraws[at])){retain(at,at+1);++at;continue;}
   size_t end=at+1,writer=queuedDraws[at].state.pipeline.depthTest&&queuedDraws[at].state.pipeline.depthWrite?at:queuedDraws.size();
   while(end<queuedDraws.size()&&queuedDraws[end].state.target==queuedDraws[at].state.target&&queuedDraws[end].state.depth==queuedDraws[at].state.depth&&additive(queuedDraws[end])){const auto& q=queuedDraws[end];if(q.state.pipeline.depthTest&&q.state.pipeline.depthWrite){if(writer!=queuedDraws.size()&&!compatible(queuedDraws[writer],q))break;writer=end;}++end;}
   if(end-at<16){retain(at,end);at=end;continue;}
   const size_t firstGroup=mergeGroups.size();bool spanMerged=false;
   for(size_t i=at;i<end;i++){const auto& q=queuedDraws[i];size_t group=firstGroup;for(;group<mergeGroups.size();group++)if(compatible(queuedDraws[mergeGroups[group].first],q))break;
    mergeNext[i]=endLink;
    if(group==mergeGroups.size())mergeGroups.push_back({i,i});
    else{auto& g=mergeGroups[group];mergeNext[g.last]=i;g.last=i;spanMerged=true;}
   }
   u32 depthGroups=0;for(size_t i=firstGroup;i<mergeGroups.size();i++){const auto& p=queuedDraws[mergeGroups[i].first].state.pipeline;if(p.depthTest&&p.depthWrite)++depthGroups;}
   if(depthGroups>1){mergeGroups.resize(firstGroup);retain(at,end);at=end;continue;}
   merged|=spanMerged;at=end;
  }
  if(merged){
   // Pack only referenced data, instead of uploading both the old and merged
   // copies. Reuse both arenas across frames; every source range is copied once.
   orderedDraws.clear();orderedDraws.reserve(mergeGroups.size());size_t vertexBytes=0,instanceBytes=0;
   for(const auto& group:mergeGroups){const auto& first=queuedDraws[group.first];const size_t alignment=first.state.stride*(first.quads?4:1);vertexBytes=(vertexBytes+alignment-1)/alignment*alignment;if(first.instanceSize){vertexBytes+=first.size;for(size_t i=group.first;i!=endLink;i=mergeNext[i])instanceBytes+=queuedDraws[i].instanceSize;}else for(size_t i=group.first;i!=endLink;i=mergeNext[i])vertexBytes+=queuedDraws[i].size;}
   compactVertices.resize(vertexBytes);compactInstances.resize(instanceBytes);size_t vertexCursor=0,instanceCursor=0;
   for(const auto& group:mergeGroups){auto q=queuedDraws[group.first];const size_t alignment=q.state.stride*(q.quads?4:1);vertexCursor=(vertexCursor+alignment-1)/alignment*alignment;const size_t vertexStart=vertexCursor,instanceStart=instanceCursor;
    if(q.instanceSize){std::memcpy(compactVertices.data()+vertexCursor,vertexArena.data()+q.offset,q.size);vertexCursor+=q.size;for(size_t i=group.first;i!=endLink;i=mergeNext[i]){const auto& source=queuedDraws[i];std::memcpy(compactInstances.data()+instanceCursor,instanceArena.data()+source.instanceOffset,source.instanceSize);instanceCursor+=source.instanceSize;}}
    else{q.count=0;for(size_t i=group.first;i!=endLink;i=mergeNext[i]){const auto& source=queuedDraws[i];std::memcpy(compactVertices.data()+vertexCursor,vertexArena.data()+source.offset,source.size);vertexCursor+=source.size;q.count+=source.count;}}
    q.offset=u32(vertexStart);q.size=u32(vertexCursor-vertexStart);q.instanceOffset=u32(instanceStart);q.instanceSize=u32(instanceCursor-instanceStart);orderedDraws.push_back(q);
   }
   stats.copiedBytes+=vertexBytes+instanceBytes;queuedDraws.swap(orderedDraws);vertexArena.swap(compactVertices);instanceArena.swap(compactInstances);
  }
 }
 // Upload each contiguous drawing span once, then consume its original draw
 // order and state. Target changes, clears, copies and CPU edits end a span.
 // This retains full glBufferData replacement on WebGL (no bufferSubData).
 upload(vertices,GL_ARRAY_BUFFER,vertexArena.data(),u32(vertexArena.size()),1048576);
 const auto instanceBatches=std::count_if(queuedDraws.begin(),queuedDraws.end(),[](const QueuedDraw& q){return q.instanceSize!=0;});
 // Large cancellation bursts alternate world sparks and trails. For those
 // spans, replace the instance store once as well; small spans retain their
 // fixed attribute offsets, which avoid unnecessary pointer setup.
 const bool sharedInstances=instanceBatches>=16;if(sharedInstances)upload(instances,GL_ARRAY_BUFFER,instanceArena.data(),u32(instanceArena.size()),65536);
 draining=true;
 for(const auto& q:queuedDraws){drawOffset=q.offset;instanceOffset=sharedInstances?q.instanceOffset:~0u;issue(q.state,q.topology,q.count,nullptr,q.size,nullptr,IndexType::UInt16,q.instanceSize?instanceArena.data()+q.instanceOffset:nullptr,q.instanceSize,q.quads);}
 draining=false;queuedDraws.clear();vertexArena.clear();instanceArena.clear();
}
void Renderer::discard(){batching=instancing=false;batchBytes.clear();worlds.clear();queuedDraws.clear();vertexArena.clear();instanceArena.clear();batchCount=0;}
void Renderer::issue(const State& d,Topology primitive,u32 count,const void* data,u32 size,const void* index,IndexType indexFormat,const void* instance,u32 instanceSize,bool quads){
 // Textured geometry and concatenated cancellation lines share the span.
 // Keep untextured fills as explicit barriers: they include the health-bar
 // fill path, whose immediate rasterization is covered by pixel regressions.
 if(staged_uploads&&!draining&&!index&&(d.layout.uv!=VertexAttributes::absent||primitive==Topology::Lines)){
  const size_t alignment=d.stride*(quads?4:1);const u32 offset=u32((vertexArena.size()+alignment-1)/alignment*alignment),io=u32(instanceArena.size());
  vertexArena.resize(size_t(offset)+size);if(size)std::memcpy(vertexArena.data()+offset,data,size);
  instanceArena.resize(size_t(io)+instanceSize);if(instanceSize)std::memcpy(instanceArena.data()+io,instance,instanceSize);
  queuedDraws.push_back({d,primitive,count,offset,size,io,instanceSize,quads});stats.copiedBytes+=size+instanceSize;
  if(vertexArena.size()+instanceArena.size()>=4*1048576)drain();return;
 }
 if(!draining)drain();
 stats.batches++;const auto& p=d.pipeline;auto& cached=drawState.pipeline;const bool all=!drawState.valid;
 if(all||cached.dither!=p.dither)p.dither?glEnable(GL_DITHER):glDisable(GL_DITHER);
 GPU* texture=d.texture?&surface(d.texture):nullptr;const bool consumesDepth=p.depthTest&&(p.depthWrite||p.depthCompare!=Compare::Always);auto& g=target(d.target,lazy_depth_clears&&!consumesDepth?~0u:d.depth);const auto& v=d.viewport;
 if(lazy_depth_clears&&p.depthTest&&(p.depthWrite||p.depthCompare!=Compare::Always))flush_depth(d.depth,d.target);
 const auto scale=resolve(owner,d.target).renderScale;const Viewport physical{v.x*scale,v.y*scale,v.width*scale,v.height*scale,v.min,v.max};
 if(!drawState.has_viewport||std::memcmp(&drawState.viewport,&physical,sizeof(physical))){glViewport(physical.x,physical.y,physical.width,physical.height);glDepthRangef(v.min,v.max);drawState.viewport=physical;drawState.has_viewport=true;}select(d);
 float vp[]{float(v.x),float(v.y),float(v.width),float(v.height)};uniform(0,4,vp);const bool transformed=d.layout.screen;
 integer(1,transformed);integer(2,p.textureTransform);integer(3,!!instance);
 // Screen vertices already contain their final positions and UVs. Instanced
 // world sprites supply world/texture matrices and tint through attributes.
 // Do not upload unused matrices after every intervening background draw.
 if(!transformed){for(int i=0;i<4;i++)if(!(instance&&(i==0||i==3))&&(i!=3||p.textureTransform))uniform(4+i,16,d.matrix[i].data());}
 integer(8,p.fog);if(p.fog){integer(9,p.rangeFog);integer(10,u32(p.fogMode));float fog[]{p.fogNear,p.fogFar,p.fogDensity};uniform(11,3,fog);uniform(12,4,color(p.fogColor).data());}integer(13,!!texture);if(!instance)uniform(14,4,color(p.textureFactor).data());
 auto arg=[](Argument a){return int(u32(a.source)|(a.complement?16u:0u)|(a.alphaOnly?32u:0u));};
 integer(15,u32(p.color.operation));integer(16,arg(p.color.first));integer(17,arg(p.color.second));integer(18,u32(p.alpha.operation));integer(19,arg(p.alpha.first));integer(20,arg(p.alpha.second));integer(21,p.alphaTest);float alpha=float(p.alphaReference)/255;uniform(22,1,&alpha);integer(23,u32(p.alphaCompare));integer(24,version==10&&p.depthTest&&d.depth&&resolve(owner,d.depth).format==PixelFormat::Depth16);
 if(texture){
  bind_texture(texture->texture);
  constexpr GLint address[]{GL_REPEAT,GL_MIRRORED_REPEAT,GL_CLAMP_TO_EDGE};GLint sampler[]{address[u32(p.addressU)],address[u32(p.addressV)],p.minFilter==Filter::Nearest?GL_NEAREST:GL_LINEAR,p.magFilter==Filter::Nearest?GL_NEAREST:GL_LINEAR};
  constexpr GLenum names[]{GL_TEXTURE_WRAP_S,GL_TEXTURE_WRAP_T,GL_TEXTURE_MIN_FILTER,GL_TEXTURE_MAG_FILTER};
  for(int i=0;i<4;i++)if(texture->sampler[i]!=sampler[i]){glTexParameteri(GL_TEXTURE_2D,names[i],sampler[i]);texture->sampler[i]=sampler[i];}
 }
 constexpr GLenum compare[]{GL_NEVER,GL_LESS,GL_EQUAL,GL_LEQUAL,GL_GREATER,GL_NOTEQUAL,GL_GEQUAL,GL_ALWAYS};
 // An Always test with writes disabled cannot affect any pixel or depth.
 // Disable the attachment test for HUD, bullets and fullscreen composition.
 const auto depth_enabled=[this](const PipelineState& s){return s.depthTest&&(!staged_uploads||s.depthWrite||s.depthCompare!=Compare::Always);};
 const bool depthEnabled=depth_enabled(p);if(all||drawState.depthEnabled!=depthEnabled)depthEnabled?glEnable(GL_DEPTH_TEST):glDisable(GL_DEPTH_TEST);drawState.depthEnabled=depthEnabled;
 if(all||cached.depthWrite!=p.depthWrite)glDepthMask(p.depthWrite);
 if(all||cached.depthCompare!=p.depthCompare)glDepthFunc(compare[u32(p.depthCompare)]);
 const auto blend_enabled=[this](const PipelineState& s){return s.blend&&(!staged_uploads||!(s.sourceBlend==BlendFactor::One&&s.destinationBlend==BlendFactor::Zero&&s.blendEquation==BlendEquation::Add&&(!s.separateAlphaBlend||(s.sourceAlphaBlend==BlendFactor::One&&s.destinationAlphaBlend==BlendFactor::Zero&&s.alphaBlendEquation==BlendEquation::Add))));};
 const bool blendEnabled=blend_enabled(p);if(all||drawState.blendEnabled!=blendEnabled)blendEnabled?glEnable(GL_BLEND):glDisable(GL_BLEND);drawState.blendEnabled=blendEnabled;
 // Set blend parameters even while blending is disabled, so the cache always
 // describes actual GL state when a subsequent batch enables it.
 constexpr GLenum blend[]{GL_ZERO,GL_ONE,GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_DST_ALPHA,GL_ONE_MINUS_DST_ALPHA,GL_DST_COLOR,GL_ONE_MINUS_DST_COLOR,GL_SRC_ALPHA_SATURATE};
 if(all||cached.sourceBlend!=p.sourceBlend||cached.destinationBlend!=p.destinationBlend||cached.separateAlphaBlend!=p.separateAlphaBlend||(p.separateAlphaBlend&&(cached.sourceAlphaBlend!=p.sourceAlphaBlend||cached.destinationAlphaBlend!=p.destinationAlphaBlend)))glBlendFuncSeparate(blend[u32(p.sourceBlend)],blend[u32(p.destinationBlend)],blend[u32(p.separateAlphaBlend?p.sourceAlphaBlend:p.sourceBlend)],blend[u32(p.separateAlphaBlend?p.destinationAlphaBlend:p.destinationBlend)]);
 constexpr GLenum equation[]{GL_FUNC_ADD,GL_FUNC_SUBTRACT,GL_FUNC_REVERSE_SUBTRACT,GL_MIN,GL_MAX};
 if(all||cached.blendEquation!=p.blendEquation||cached.separateAlphaBlend!=p.separateAlphaBlend||(p.separateAlphaBlend&&cached.alphaBlendEquation!=p.alphaBlendEquation))glBlendEquationSeparate(equation[u32(p.blendEquation)],equation[u32(p.separateAlphaBlend?p.alphaBlendEquation:p.blendEquation)]);
 if(all||cached.cull!=p.cull){if(p.cull!=Cull::None){glEnable(GL_CULL_FACE);glFrontFace(GL_CW);glCullFace(p.cull==Cull::Back?GL_BACK:GL_FRONT);}else glDisable(GL_CULL_FACE);}
 if(all||cached.colorMask!=p.colorMask)glColorMask(!!(p.colorMask&1),!!(p.colorMask&2),!!(p.colorMask&4),!!(p.colorMask&8));
 cached=p;drawState.valid=true;
 const std::array<u32,6> layoutKey{d.stride,u32(d.layout.screen),d.layout.diffuse,d.layout.specular,d.layout.uv,u32(!!instance)};
 auto& vao=layouts[layoutKey];const bool setup=!vao;
 if(setup)glGenVertexArrays(1,&vao);if(currentLayout!=vao){glBindVertexArray(vao);currentLayout=vao;}
 const u32 base=draining?0:upload(vertices,GL_ARRAY_BUFFER,data,size,1048576);
 if(draining&&setup)glBindBuffer(GL_ARRAY_BUFFER,vertices.id);
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
 // Keep instance attributes at offset zero. WebGL2 has no base-instance draw;
 // changing nine attribute pointers for every small world batch costs more
 // than replacing this compact instance buffer. The vertex arena stays shared.
 if(instance){u32 b=draining&&instanceOffset!=~0u?instanceOffset:upload(instances,GL_ARRAY_BUFFER,instance,instanceSize,65536);
#ifdef __EMSCRIPTEN__
 if(setup||instanceBases[vao]!=b){
#else
 {
#endif
 if(draining)glBindBuffer(GL_ARRAY_BUFFER,instances.id);instanceBases[vao]=b;
 for(int i=0;i<4;i++){glEnableVertexAttribArray(4+i);glVertexAttribPointer(4+i,4,GL_FLOAT,false,instanceStride,reinterpret_cast<void*>(b+i*16));glVertexAttribDivisor(4+i,1);glEnableVertexAttribArray(9+i);glVertexAttribPointer(9+i,4,GL_FLOAT,false,instanceStride,reinterpret_cast<void*>(b+68+i*16));glVertexAttribDivisor(9+i,1);}glEnableVertexAttribArray(8);glVertexAttribPointer(8,4,GL_UNSIGNED_BYTE,true,instanceStride,reinterpret_cast<void*>(b+64));glVertexAttribDivisor(8,1);}
 glDrawArraysInstanced(mode,draining?drawOffset/d.stride:0,n,instanceSize/instanceStride);}
 else if(quads){
  // Four vertices per sprite, with exactly the original two triangles and
  // provoking vertices. A reusable index table also addresses each aligned
  // sprite span without changing any VAO attribute pointers on WebGL2.
  const u32 first=draining?drawOffset/d.stride/4:0,required=first+count/2;
  if(!quadIndices)glGenBuffers(1,&quadIndices);glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,quadIndices);
  if(required>quadIndexCapacity){quadIndexCapacity=std::max(required,std::max(1024u,quadIndexCapacity*2));std::vector<u32> table(size_t(quadIndexCapacity)*6);for(u32 i=0;i<quadIndexCapacity;i++){const u32 v=i*4;const u32 row[]={v,v+1,v+2,v+1,v+2,v+3};std::copy(std::begin(row),std::end(row),table.begin()+size_t(i)*6);}glBufferData(GL_ELEMENT_ARRAY_BUFFER,table.size()*sizeof(u32),table.data(),GL_STATIC_DRAW);stats.vertexUploadBytes+=table.size()*sizeof(u32);++stats.bufferReplacements;}
  glDrawElements(GL_TRIANGLES,count*3,GL_UNSIGNED_INT,reinterpret_cast<void*>(size_t(first)*6*sizeof(u32)));
 }
 else if(index){u32 b=upload(indices,GL_ELEMENT_ARRAY_BUFFER,index,n*(indexFormat==IndexType::UInt16?2:4),65536);glDrawElements(mode,n,indexFormat==IndexType::UInt16?GL_UNSIGNED_SHORT:GL_UNSIGNED_INT,reinterpret_cast<void*>(b));}
 else glDrawArrays(mode,draining?drawOffset/d.stride:0,n);g.rendered=true;
}
void Renderer::queue_depth(float value,const i32* rects,u32 count){
 const auto color=resolve(owner,state.target),depth=resolve(owner,state.depth);
 auto& pending=depths[state.depth].pending;
 const i32 width=i32(std::min(color.width*color.renderScale,depth.width*depth.renderScale)),height=i32(std::min(color.height*color.renderScale,depth.height*depth.renderScale));
 const auto covers=[](const std::array<i32,4>& a,const std::array<i32,4>& b){return a[0]<=b[0]&&a[1]<=b[1]&&a[2]>=b[2]&&a[3]>=b[3];};
 for(u32 i=0;i<count;i++){
  const auto* p=rects+i*4;std::array<i32,4> r{std::clamp(i32(p[0]*color.renderScale),0,width),std::clamp(i32(p[1]*color.renderScale),0,height),std::clamp(i32(p[2]*color.renderScale),0,width),std::clamp(i32(p[3]*color.renderScale),0,height)};
  if(r[0]>=r[2]||r[1]>=r[3])continue;
  // Only combine equal values: an intervening different-value partial clear
  // can invalidate the coverage of an older rectangle.
  if(std::all_of(pending.begin(),pending.end(),[&](const auto& q){return q.value==value;})){
   if(std::any_of(pending.begin(),pending.end(),[&](const auto& q){return covers(q.rect,r);}))continue;
  }
  pending.erase(std::remove_if(pending.begin(),pending.end(),[&](const auto& q){return covers(r,q.rect);}),pending.end());
  pending.push_back({state.target,r,value});
 }
}
void Renderer::flush_depth(u32 id,u32 preferredTarget){
 auto it=depths.find(id);if(it==depths.end()||it->second.pending.empty())return;
 const GLuint read=readFramebuffer,draw=drawFramebuffer;
 glDepthMask(true);drawState.pipeline.depthWrite=true;
 for(const auto& clear:it->second.pending){
  // Rectangles were already clipped in physical attachment coordinates.
  // Reuse the current compatible target, avoiding a round trip to the old
  // color buffer solely to clear the SAME shared depth attachment.
  const auto& r=clear.rect;u32 color=clear.target;
  if(preferredTarget){const auto s=resolve(owner,preferredTarget);if(r[2]<=i32(s.width*s.renderScale)&&r[3]<=i32(s.height*s.renderScale))color=preferredTarget;}
  target(color,id);const auto s=resolve(owner,color);
  const bool full=r[0]==0&&r[1]==0&&r[2]==i32(s.width*s.renderScale)&&r[3]==i32(s.height*s.renderScale);
  if(full)glDisable(GL_SCISSOR_TEST);else{glEnable(GL_SCISSOR_TEST);glScissor(r[0],r[1],r[2]-r[0],r[3]-r[1]);}
  glClearDepthf(clear.value);glClear(GL_DEPTH_BUFFER_BIT);
 }
 it->second.pending.clear();glDisable(GL_SCISSOR_TEST);
 bind_framebuffer(GL_READ_FRAMEBUFFER,read==~0u?0:read);bind_framebuffer(GL_DRAW_FRAMEBUFFER,draw==~0u?0:draw);
}
void Renderer::clear(u32 flags,u32 c,float depth,u32 stencil,const i32* rects,u32 count){
 flush();const auto& v=state.viewport;
 i32 box[]{i32(v.x),i32(v.y),i32(v.x+v.width),i32(v.y+v.height)};if(!rects){rects=box;count=1;}
 if(lazy_depth_clears&&(flags&2)&&state.depth){queue_depth(depth,rects,count);flags&=~2u;}
 if(!flags)return;
 // A deferred depth-only clear must not switch framebuffer or attach depth.
 // Its eventual consumer binds the correct color/depth pair in flush_depth.
 auto& g=target(state.target,lazy_depth_clears&&!(flags&6)?~0u:state.depth);
 // Only touch masks/clear values consumed by this operation. In particular,
 // a depth-only compositor clear must not invalidate the color-mask cache.
 if(flags&1){glColorMask(true,true,true,true);drawState.pipeline.colorMask=15;const auto rgba=color(c);glClearColor(rgba[0],rgba[1],rgba[2],rgba[3]);}
 if(flags&2){glDepthMask(true);drawState.pipeline.depthWrite=true;glClearDepthf(depth);}
 if(flags&4){glClearStencil(stencil);glStencilMask(255);}
 const GLbitfield bits=(flags&1?GL_COLOR_BUFFER_BIT:0)|(flags&2?GL_DEPTH_BUFFER_BIT:0)|(flags&4?GL_STENCIL_BUFFER_BIT:0);
 const auto surface=resolve(owner,state.target);const auto scale=surface.renderScale;
 const bool full=count==1&&rects[0]==0&&rects[1]==0&&rects[2]==i32(surface.width)&&rects[3]==i32(surface.height);
 if(full){glDisable(GL_SCISSOR_TEST);glClear(bits);}
 else{glEnable(GL_SCISSOR_TEST);for(u32 i=0;i<count;i++){const i32* p=rects+i*4;glScissor(p[0]*scale,p[1]*scale,(p[2]-p[0])*scale,(p[3]-p[1])*scale);glClear(bits);}glDisable(GL_SCISSOR_TEST);}
 g.rendered=true;
}
void Renderer::copy(u32 src,const i32* rect,u32 dst,const i32* point){flush();auto& a=surface(src);auto& b=surface(dst);bind_framebuffer(GL_READ_FRAMEBUFFER,a.framebuffer);bind_framebuffer(GL_DRAW_FRAMEBUFFER,b.framebuffer);glDisable(GL_SCISSOR_TEST);const auto as=resolve(owner,src).renderScale,bs=resolve(owner,dst).renderScale;glBlitFramebuffer(rect[0]*as,rect[1]*as,rect[2]*as,rect[3]*as,point[0]*bs,point[1]*bs,(point[0]+rect[2]-rect[0])*bs,(point[1]+rect[3]-rect[1])*bs,GL_COLOR_BUFFER_BIT,GL_NEAREST);b.rendered=true;b.version=resolve(owner,dst).version+1;}
bool Renderer::resample(u32 src,const i32* from,u32 dst,const i32* to,const float* weights,u32 width,u32 height){
 if(src==dst)return false;
 auto masks=[](PixelFormat f)->std::array<GLint,4>{switch(f){case PixelFormat::Argb8332:return {7,7,3,255};case PixelFormat::Bgra8:return {255,255,255,255};case PixelFormat::Bgrx8:return {255,255,255,0};case PixelFormat::Rgb565:return {31,63,31,0};case PixelFormat::Xrgb1555:return {31,31,31,0};case PixelFormat::Argb1555:return {31,31,31,1};case PixelFormat::Argb4444:return {15,15,15,15};default:return {};}};
 const auto a=resolve(owner,src),b=resolve(owner,dst);const auto im=masks(a.format),om=masks(b.format);if(!im[0]||!om[0])return false;
 flush();auto& input=surface(src);auto& output=target(dst,0);
 glUseProgram(resampleProgram);currentProgram=resampleProgram;glBindVertexArray(resampleVao);currentLayout=resampleVao;
 glDisable(GL_BLEND);glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);glDisable(GL_SCISSOR_TEST);glDisable(GL_DITHER);glColorMask(true,true,true,true);glDepthMask(false);
 glViewport(to[0]*b.renderScale,to[1]*b.renderScale,(to[2]-to[0])*b.renderScale,(to[3]-to[1])*b.renderScale);
 glActiveTexture(GL_TEXTURE0);bind_texture(input.texture);glUniform1i(glGetUniformLocation(resampleProgram,"sourceImage"),0);
 glActiveTexture(GL_TEXTURE1);glBindTexture(GL_TEXTURE_2D,weightTexture);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
 if(weights)glTexImage2D(GL_TEXTURE_2D,0,GL_RG32F,width,height,0,GL_RG,GL_FLOAT,weights);
 else {const float zero[2]{};glTexImage2D(GL_TEXTURE_2D,0,GL_RG32F,1,1,0,GL_RG,GL_FLOAT,zero);}
 glUniform1i(glGetUniformLocation(resampleProgram,"weights"),1);glUniform1i(glGetUniformLocation(resampleProgram,"triangle"),weights!=nullptr);glUniform2i(glGetUniformLocation(resampleProgram,"renderScale"),a.renderScale,b.renderScale);
 glUniform4i(glGetUniformLocation(resampleProgram,"sourceRect"),from[0],from[1],from[2]-from[0],from[3]-from[1]);
 glUniform4i(glGetUniformLocation(resampleProgram,"destinationRect"),to[0],to[1],to[2]-to[0],to[3]-to[1]);
 glUniform4iv(glGetUniformLocation(resampleProgram,"sourceMask"),1,im.data());glUniform4iv(glGetUniformLocation(resampleProgram,"destinationMask"),1,om.data());
 glDrawArrays(GL_TRIANGLES,0,3);glActiveTexture(GL_TEXTURE0);
 // Utility rendering changes masks, viewport and program. Vertex layouts remain
 // valid VAOs, while the regular draw cache must restore all pipeline state.
 drawState=DrawStateCache{};output.rendered=true;output.version=b.version+1;++stats.resamples;return true;
}
void Renderer::read(u32 id){flush();auto it=surfaces.find(id);if(it==surfaces.end()||!it->second.rendered)return;auto s=resolve(owner,id);auto& g=it->second;stats.readBytes+=s.size;pixels.resize(size_t(s.width)*s.height*s.renderScale*s.renderScale*4);bind_framebuffer(GL_FRAMEBUFFER,g.framebuffer);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,s.width*s.renderScale,s.height*s.renderScale,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
 for(u32 y=0;y<s.height;y++)for(u32 x=0;x<s.width;x++){const auto scale=s.renderScale;const auto* p=pixels.data()+((size_t(y)*scale+scale/2)*s.width*scale+x*scale+scale/2)*4;u32 r=p[0],gc=p[1],b=p[2],a=p[3];auto* out=s.data+y*s.pitch;
  if(s.format==PixelFormat::Argb8332){const uint16_t value=uint16_t((a<<8)|(((r*7+127)/255)<<5)|(((gc*7+127)/255)<<2)|((b*3+127)/255));std::memcpy(out+x*2,&value,2);}else if(s.format==PixelFormat::Bgra8||s.format==PixelFormat::Bgrx8){out+=x*4;out[0]=b;out[1]=gc;out[2]=r;out[3]=s.format==PixelFormat::Bgra8?a:255;}else if((s.format==PixelFormat::Rgb565||s.format==PixelFormat::Xrgb1555||s.format==PixelFormat::Argb1555||s.format==PixelFormat::Argb4444)){uint16_t n=s.format==PixelFormat::Argb4444?(a>>4)<<12|(r>>4)<<8|(gc>>4)<<4|(b>>4):s.format==PixelFormat::Rgb565?(r>>3)<<11|(gc>>2)<<5|(b>>3):(s.format==PixelFormat::Argb1555&&a>=128?0x8000:0)|(r>>3)<<10|(gc>>3)<<5|(b>>3);std::memcpy(out+x*2,&n,2);}else if(s.format==PixelFormat::Alpha8)out[x]=a;else if(s.format==PixelFormat::Luminance8)out[x]=r;else std::abort();
 }g.rendered=false;
}
void Renderer::release(u32 id){flush();for(auto& [depth,value]:depths)if(depth!=id&&std::any_of(value.pending.begin(),value.pending.end(),[&](const auto& c){return c.target==id;}))flush_depth(depth);auto d=depths.find(id);if(d!=depths.end()){glDeleteRenderbuffers(1,&d->second.buffer);depths.erase(d);}auto it=surfaces.find(id);if(it==surfaces.end())return;glDeleteTextures(1,&it->second.texture);glDeleteFramebuffers(1,&it->second.framebuffer);surfaces.erase(it);boundTexture=readFramebuffer=drawFramebuffer=~0u;}
void Renderer::present(u32 id){flush();pending=id;stats.frames++;if(!defer)commit();}
bool Renderer::commit(){if(!pending)return false;auto s=resolve(owner,pending);auto& g=surface(pending);pending=0;bind_framebuffer(GL_READ_FRAMEBUFFER,g.framebuffer);bind_framebuffer(GL_DRAW_FRAMEBUFFER,0);glDisable(GL_SCISSOR_TEST);int width=640,height=480;SDL_GetWindowSizeInPixels(window,&width,&height);glBlitFramebuffer(0,0,s.width*s.renderScale,s.height*s.renderScale,0,height,width,0,GL_COLOR_BUFFER_BIT,GL_NEAREST);SDL_GL_SwapWindow(window);stats.presentations++;return true;}
}
extern "C" {
const touhou::sdl::Statistics* sdl_stats(){static touhou::sdl::Statistics zero{};return touhou::sdl::current()?&touhou::sdl::current()->stats:&zero;}
void sdl_defer(int enabled){if(auto* r=touhou::sdl::current())r->defer=enabled;}
int sdl_commit(){return touhou::sdl::current()&&touhou::sdl::current()->commit();}
const char* sdl_error(){return touhou::sdl::current()?touhou::sdl::current()->error():SDL_GetError();}
int sdl_version(){return SDL_GetVersion();}
double sdl_ticks(){return double(SDL_GetTicksNS())/1000000.0;}
__attribute__((export_name("sdl_canvas"))) void sdl_canvas(const char* selector){SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR,selector);}
__attribute__((export_name("sdl_pump"))) void sdl_pump(){SDL_Event event;while(SDL_PollEvent(&event)){};}
}
