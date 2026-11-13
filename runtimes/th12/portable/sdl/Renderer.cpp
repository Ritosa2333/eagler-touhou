#include "Renderer.hpp"
#include "Shaders.hpp"
#include "ResampleShader.hpp"
#include "AlphaConversion.hpp"
#include "Th14RenderMath.hpp"
#ifdef TH14_WEBGPU_BACKEND
#if !defined(PORT_TITLE) || PORT_TITLE != 14
#error TH14_WEBGPU_BACKEND is limited to TH14
#endif
#include "WebGpuRenderer.hpp"
#endif
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
 PipelineState pipeline{};Viewport viewport{};bool valid=false,has_viewport=false;
} drawState;
constexpr std::array<float,16> identity{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
float bits(u32 n){float f;std::memcpy(&f,&n,4);return f;}
std::array<float,4> color(u32 n){return {float(n>>16&255)/255,float(n>>8&255)/255,float(n&255)/255,float(n>>24)/255};}
void replace(std::string& s,const std::string& a,const std::string& b){size_t p=0;while((p=s.find(a,p))!=std::string::npos){s.replace(p,a.size(),b);p+=b.size();}}
GLuint shader(GLenum type,const std::string& source){GLuint s=glCreateShader(type);const char* p=source.c_str();glShaderSource(s,1,&p,nullptr);glCompileShader(s);GLint ok=0;glGetShaderiv(s,GL_COMPILE_STATUS,&ok);if(!ok){char log[2048];glGetShaderInfoLog(s,sizeof(log),nullptr,log);std::fprintf(stderr,"SDL shader: %s\n",log);std::abort();}return s;}

bool equal(const State& a,const State& b,bool world=false){
 if(a.layout!=b.layout||a.stride!=b.stride||a.texture!=b.texture||a.target!=b.target||a.depth!=b.depth||std::memcmp(&a.viewport,&b.viewport,sizeof(Viewport)))return false;
 if(!a.pipeline.compatible(b.pipeline,world))return false;
 if(world){for(int i=1;i<4;i++)if(a.matrix[i]!=b.matrix[i])return false;}
 else if(!a.layout.screen&&a.matrix!=b.matrix)return false;
 return true;
}
}
Renderer* current(){return active;}void set_current(Renderer* r){active=r;}
State::State(){for(auto& m:matrix)m=identity;}
Renderer::Renderer(int v,Resolve r,void* p):version(v),resolve(r),owner(p){}
bool Renderer::initialize(){
#ifdef TH14_WEBGPU_BACKEND
 if(!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)){failure=SDL_GetError();return false;}
 window=SDL_CreateWindow("Touhou 14",640,480,0);if(!window){failure=SDL_GetError();return false;}
 webGpu=new WebGpuRenderer(*this);if(!webGpu->initialize(alphaBoundaries.data())){failure="WebGPU initialization failed";return false;}
 set_current(this);return true;
#endif
 if(!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS)){failure=SDL_GetError();return false;}
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_ES);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
 SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,0);SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,0);SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,0);SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
 window=SDL_CreateWindow(version==8?"Touhou 08":"Touhou 10",640,480,SDL_WINDOW_OPENGL);if(!window){failure=SDL_GetError();return false;}
 SDL_SetWindowSize(window,640,480);
 context=SDL_GL_CreateContext(window);if(!context){failure=SDL_GetError();return false;}
#if !defined(__EMSCRIPTEN__) || !defined(PORT_TITLE) || PORT_TITLE != 14
 SDL_GL_SetSwapInterval(1);
#endif
 // TH14's Asyncify-safe host owns requestAnimationFrame scheduling. SDL's
 // Emscripten swap interval instead configures emscripten_set_main_loop,
 // which this host never creates; its deferred first-pump call would fail.
 drawState=DrawStateCache{};
 if(version==10&&!measure_alpha_boundaries(alphaBoundaries)){failure="RGBA8 alpha conversion measurement failed";return false;}
 std::string source=vertexSource;
#if defined(PORT_TITLE) && PORT_TITLE == 14
 if(version==10){replace(source,"uniform vec3 fogParams;","uniform vec3 fogParams;uniform vec3 fogLinear;");replace(source,"(fogParams.y-z)/(fogParams.y-fogParams.x)","(fogLinear.z>0.5?fogLinear.x-z*fogLinear.y:(fogParams.y-z)/(fogParams.y-fogParams.x))");}
#endif
#if defined(PORT_TITLE) && PORT_TITLE == 14
 // Retail screen vertices are snapped to an eight-bit subpixel grid. Doing
 // that before the clip-space conversion preserves midpoint neighbors that
 // otherwise collapse onto a tie during the viewport divide. Native HAL
 // shape and varying-RHW boundary fixtures establish round-to-even here.
 replace(source,"vec2 xy=(position.xy-viewport.xy+vec2(.5))/viewport.zw;",
   "vec2 screen=roundEven((position.xy-viewport.xy)*256.0)*(1.0/256.0);vec2 xy=(screen+vec2(.5))/viewport.zw;");
#endif
 if(version==10&&touhou_clip_control()){replace(source,"position.z*2.0-1.0","position.z");replace(source,"gl_Position.z=gl_Position.z*2.0-gl_Position.w;","");}
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
Renderer::~Renderer(){discard();
#ifdef TH14_WEBGPU_BACKEND
 if(webGpu){delete webGpu;if(window)SDL_DestroyWindow(window);if(active==this)active=nullptr;SDL_Quit();return;}
#endif
for(auto& [id,g]:surfaces){glDeleteTextures(1,&g.texture);glDeleteFramebuffers(1,&g.framebuffer);}for(auto& [id,d]:depths)glDeleteRenderbuffers(1,&d.buffer);for(auto& [k,p]:programs)glDeleteProgram(p.id);glDeleteProgram(resampleProgram);glDeleteVertexArrays(1,&resampleVao);glDeleteTextures(1,&weightTexture);glDeleteProgram(generic.id);for(auto& entry:layouts)glDeleteVertexArrays(1,&entry.second);glDeleteShader(vertex);glDeleteBuffers(1,&vertices.id);glDeleteBuffers(1,&indices.id);glDeleteBuffers(1,&instances.id);if(context)SDL_GL_DestroyContext(context);if(window)SDL_DestroyWindow(window);if(active==this)active=nullptr;SDL_Quit();}
void Renderer::bind_texture(GLuint id){if(boundTexture!=id){glBindTexture(GL_TEXTURE_2D,id);boundTexture=id;++stats.textureBinds;}}
void Renderer::bind_framebuffer(GLenum target,GLuint id){
 const bool read=target!=GL_DRAW_FRAMEBUFFER,draw=target!=GL_READ_FRAMEBUFFER;
 if((read&&readFramebuffer!=id)||(draw&&drawFramebuffer!=id)){glBindFramebuffer(target,id);++stats.framebufferBinds;if(read)readFramebuffer=id;if(draw)drawFramebuffer=id;}
}
void Renderer::draw_batch(u32 count,const void* data,u32 stride){
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
  // Match byte alpha after the measured RGBA8 conversion. Float32 arithmetic
  // alone differs at rounding boundaries on some devices.
  if(version==10){replace(source,"uniform float alphaRef;","uniform float alphaRef;\nuniform vec4 alphaBoundary[64];\nfloat alpha_boundary(int n){return alphaBoundary[n/4][n%4];}");replace(source,"float a=c.a*255.0,ref=floor(alphaRef*255.0+.5);","int byteAlpha=clamp(int(floor(c.a*255.0+.5)),0,255);if(c.a<alpha_boundary(byteAlpha))--byteAlpha;else if(byteAlpha<255&&c.a>=alpha_boundary(byteAlpha+1))++byteAlpha;float a=float(byteAlpha),ref=floor(alphaRef*255.0+.5);");}
  if(version==8)replace(source,"float a=c.a*255.0","float a=floor(c.a*255.0+.5)");
  if(version==8)replace(source,"gl_FragDepth=depth16?(floor(scaled)+floor(fract(scaled)-gl_FragCoord.z+0.5))/65535.0:gl_FragCoord.z;","");
#if defined(PORT_TITLE) && PORT_TITLE == 14
  // Installed D3D9 D16 conversion: truncate to twenty fractional bits,
  // then round the sixteen-bit result to nearest with ties downward.
  if(version==10){replace(source,"float scaled=gl_FragCoord.z*65536.0;","float quantized=floor(gl_FragCoord.z*1048576.0)/1048576.0;float scaled=quantized*65536.0;");replace(source,"floor(fract(scaled)-gl_FragCoord.z+0.5)","ceil(fract(scaled)-quantized-0.5)");}
#endif
#if defined(PORT_TITLE) && PORT_TITLE == 14
  if(version==10)replace(source,"c.rgb=mix(fogColor.rgb,c.rgb,clamp(vFog,0.0,1.0));","{float f=clamp(vFog,0.0,1.0);vec3 rest=fogColor.rgb-fogColor.rgb*f;c.rgb=c.rgb*f+rest;}");
#endif
  Program p;p.id=glCreateProgram();GLuint fs=shader(GL_FRAGMENT_SHADER,source);glAttachShader(p.id,vertex);glAttachShader(p.id,fs);glLinkProgram(p.id);glDeleteShader(fs);GLint linked=0;glGetProgramiv(p.id,GL_LINK_STATUS,&linked);if(!linked){char log[2048];glGetProgramInfoLog(p.id,sizeof(log),nullptr,log);std::fprintf(stderr,"SDL program: %s\n",log);std::abort();}
  if(version==10){glUseProgram(p.id);glUniform4fv(glGetUniformLocation(p.id,"alphaBoundary[0]"),64,alphaBoundaries.data());glUseProgram(currentProgram);}
#if defined(PORT_TITLE) && PORT_TITLE == 14
  const char* names[]{"viewport","transformed","textureTransform","instanced","world","view","projection","textureMatrix","fogEnabled","rangeFog","fogMode","fogParams","fogColor","hasTexture","factor","colorOp","colorArg1","colorArg2","alphaOp","alphaArg1","alphaArg2","alphaTest","alphaRef","alphaFunc","depth16","tex","clipMatrix","fogLinear"};
  for(int i=0;i<28;i++)p.uniforms[i].location=glGetUniformLocation(p.id,names[i]);it=programs.emplace(key,p).first;
#else
  const char* names[]{"viewport","transformed","textureTransform","instanced","world","view","projection","textureMatrix","fogEnabled","rangeFog","fogMode","fogParams","fogColor","hasTexture","factor","colorOp","colorArg1","colorArg2","alphaOp","alphaArg1","alphaArg2","alphaTest","alphaRef","alphaFunc","depth16","tex"};
  for(int i=0;i<26;i++)p.uniforms[i].location=glGetUniformLocation(p.id,names[i]);it=programs.emplace(key,p).first;
#endif
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
void Renderer::prepare(u32 h){
#ifdef TH14_WEBGPU_BACKEND
 if(webGpu){webGpu->prepare(h);return;}
#endif
 surface(h);
}
void Renderer::draw(Topology primitive,u32 count,const void* data,u32 stride,const void* index,IndexType indexFormat){
 stats.calls++;state.stride=stride;
 if(index||primitive<Topology::Triangles||primitive>Topology::Fan){flush();issue(state,primitive,count,data,vertex_count(primitive,count)*stride,index,indexFormat,nullptr,0);return;}
 const bool instance=version==10&&state.layout==attributes(VertexLayout::WorldUv)&&stride==20&&primitive==Topology::Strip&&count==2;
 if(instance){if(batching&&(!instancing||!equal(batchState,state,true)||quad.size()!=80||std::memcmp(quad.data(),data,80)))flush();if(!batching){batchState=state;quad.assign(static_cast<const u8*>(data),static_cast<const u8*>(data)+80);batching=instancing=true;}const auto old=worlds.size();worlds.resize(old+68);std::memcpy(worlds.data()+old,state.matrix[0].data(),64);std::memcpy(worlds.data()+old+64,&state.pipeline.textureFactor,4);if(worlds.size()>=1024*68)flush();return;}
 if(batching&&(instancing||!equal(batchState,state)))flush();if(!batching){batchState=state;batching=true;instancing=false;}
 const auto* src=static_cast<const u8*>(data);const size_t start=batchBytes.size(),size=count*3*stride;stats.copiedBytes+=size;batchBytes.resize(start+size);auto* dest=batchBytes.data()+start;
 if(primitive==Topology::Triangles)std::memcpy(dest,src,size);else if(count){std::memcpy(dest,src,3*stride);dest+=3*stride;for(u32 i=1;i<count;i++){u32 a=primitive==Topology::Fan?0:i&1?i+1:i,b=primitive==Topology::Fan?i+1:i&1?i:i+1,c=i+2;for(u32 v:{a,b,c}){std::memcpy(dest,src+v*stride,stride);dest+=stride;}}}batchCount+=count;if(batchBytes.size()>=1048576)flush();
}
void Renderer::flush(){if(!batching)return;batching=false;if(instancing)issue(batchState,Topology::Strip,2,quad.data(),quad.size(),nullptr,IndexType::UInt16,worlds.size()>68?worlds.data():nullptr,worlds.size()>68?worlds.size():0);else issue(batchState,Topology::Triangles,batchCount,batchBytes.data(),batchBytes.size(),nullptr,IndexType::UInt16,nullptr,0);batchBytes.clear();worlds.clear();batchCount=0;instancing=false;}
void Renderer::discard(){batching=instancing=false;batchBytes.clear();worlds.clear();batchCount=0;}
void Renderer::issue(const State& d,Topology primitive,u32 count,const void* data,u32 size,const void* index,IndexType indexFormat,const void* instance,u32 instanceSize,bool clippedGeometry){
 stats.batches++;
#ifdef TH14_WEBGPU_BACKEND
 if(webGpu){webGpu->issue(d,primitive,count,data,index,indexFormat,instance,instanceSize);return;}
#endif
#if defined(PORT_TITLE) && PORT_TITLE == 14
 if(!clippedGeometry&&!index&&!d.layout.screen&&d.layout==attributes(VertexLayout::WorldUv)&&d.stride==20){
  if(instance){
   bool needsClip=false;const auto* records=static_cast<const u8*>(instance);
   for(u32 i=0;i<instanceSize/68;i++){State one=d;std::memcpy(one.matrix[0].data(),records+i*68,64);std::vector<Th14ClipVertex> temp;if(th14_clip_world_uv(one,primitive,count,data,temp)==1){needsClip=true;break;}}
   if(needsClip){for(u32 i=0;i<instanceSize/68;i++){State one=d;std::memcpy(one.matrix[0].data(),records+i*68,64);std::memcpy(&one.pipeline.textureFactor,records+i*68+64,4);std::vector<Th14ClipVertex> temp;if(th14_clip_world_uv(one,primitive,count,data,temp)==1)issue(one,primitive,count,data,size,nullptr,indexFormat,nullptr,0);else issue(d,primitive,count,data,size,nullptr,indexFormat,records+i*68,68);}return;}
  }else{
   std::vector<Th14ClipVertex> clipped;const int status=th14_clip_world_uv(d,primitive,count,data,clipped);
   if(status==1){if(!clipped.empty()){State one=d;one.stride=88;one.layout.uv=44;issue(one,Topology::Triangles,u32(clipped.size()/3),clipped.data(),u32(clipped.size()*88),nullptr,indexFormat,nullptr,0,true);}return;}
  }
 }
#endif
 const auto& p=d.pipeline;auto& cached=drawState.pipeline;const bool all=!drawState.valid;
 if(all||cached.dither!=p.dither)p.dither?glEnable(GL_DITHER):glDisable(GL_DITHER);
 GPU* texture=d.texture?&surface(d.texture):nullptr;auto& g=target(d.target,d.depth);const auto& v=d.viewport;
 if(!drawState.has_viewport||std::memcmp(&drawState.viewport,&v,sizeof(v))){glViewport(v.x,v.y,v.width,v.height);glDepthRangef(v.min,v.max);drawState.viewport=v;drawState.has_viewport=true;}select(d);
 float vp[]{float(v.x),float(v.y),float(v.width),float(v.height)};uniform(0,4,vp);const bool transformed=d.layout.screen;
#if defined(PORT_TITLE) && PORT_TITLE == 14
 if(!transformed&&!instance){auto clip=th14_clip(d.matrix[0],d.matrix[1],d.matrix[2]);uniform(26,16,clip.data());}
#endif
 integer(1,transformed);integer(2,p.textureTransform);integer(3,!!instance);
#if defined(PORT_TITLE) && PORT_TITLE == 14
 const bool nativeFog=version==10&&p.fog&&!p.rangeFog&&p.fogMode==FogMode::Linear&&std::isfinite(p.fogNear)&&std::isfinite(p.fogFar)&&p.fogNear!=p.fogFar;
 if(nativeFog){const auto eye=th14_multiply(d.matrix[1],d.matrix[0]);uniform(4,16,eye.data());uniform(5,16,identity.data());uniform(6,16,d.matrix[2].data());uniform(7,16,d.matrix[3].data());}
 else {for(int i=0;i<4;i++)uniform(4+i,16,d.matrix[i].data());}
 float fogLinear[3]{};if(nativeFog){volatile float range=p.fogFar-p.fogNear;volatile float scale=1.f/range;volatile float bias=p.fogFar*scale;fogLinear[0]=bias;fogLinear[1]=scale;fogLinear[2]=1.f;}uniform(27,3,fogLinear);
#else
 for(int i=0;i<4;i++)uniform(4+i,16,d.matrix[i].data());
#endif
 integer(8,p.fog);integer(9,p.rangeFog);integer(10,u32(p.fogMode));float fog[]{p.fogNear,p.fogFar,p.fogDensity};uniform(11,3,fog);
#if defined(PORT_TITLE) && PORT_TITLE == 14
 if(version==10){auto fogColor=color(p.fogColor);constexpr float reciprocal=1.f/255.f;for(int i=0;i<3;++i){volatile float channel=float((p.fogColor>>(16-i*8))&255u)*reciprocal;fogColor[i]=channel;}uniform(12,4,fogColor.data());}else uniform(12,4,color(p.fogColor).data());
#else
 uniform(12,4,color(p.fogColor).data());
#endif
integer(13,!!texture);uniform(14,4,color(p.textureFactor).data());
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
 if(all||cached.sourceBlend!=p.sourceBlend||cached.destinationBlend!=p.destinationBlend||cached.separateAlphaBlend!=p.separateAlphaBlend||cached.sourceAlphaBlend!=p.sourceAlphaBlend||cached.destinationAlphaBlend!=p.destinationAlphaBlend){
  if(p.separateAlphaBlend)glBlendFuncSeparate(blend[u32(p.sourceBlend)],blend[u32(p.destinationBlend)],blend[u32(p.sourceAlphaBlend)],blend[u32(p.destinationAlphaBlend)]);
  else glBlendFunc(blend[u32(p.sourceBlend)],blend[u32(p.destinationBlend)]);
 }
 constexpr GLenum equation[]{GL_FUNC_ADD,GL_FUNC_SUBTRACT,GL_FUNC_REVERSE_SUBTRACT,GL_MIN,GL_MAX};
 if(all||cached.blendEquation!=p.blendEquation||cached.separateAlphaBlend!=p.separateAlphaBlend||cached.alphaBlendEquation!=p.alphaBlendEquation){
  if(p.separateAlphaBlend)glBlendEquationSeparate(equation[u32(p.blendEquation)],equation[u32(p.alphaBlendEquation)]);
  else glBlendEquation(equation[u32(p.blendEquation)]);
 }
 if(all||cached.cull!=p.cull){if(p.cull!=Cull::None){glEnable(GL_CULL_FACE);glFrontFace(GL_CW);glCullFace(p.cull==Cull::Back?GL_BACK:GL_FRONT);}else glDisable(GL_CULL_FACE);}
 if(all||cached.colorMask!=p.colorMask)glColorMask(!!(p.colorMask&1),!!(p.colorMask&2),!!(p.colorMask&4),!!(p.colorMask&8));
 cached=p;drawState.valid=true;
 const std::array<u32,6> layoutKey{d.stride,u32(d.layout.screen),d.layout.diffuse,d.layout.specular,d.layout.uv,u32(!!instance)};
 auto& vao=layouts[layoutKey];const bool setup=!vao;
 if(setup)glGenVertexArrays(1,&vao);if(currentLayout!=vao){glBindVertexArray(vao);currentLayout=vao;}
 
#if defined(PORT_TITLE) && PORT_TITLE == 14
 if(!instance){
  if(clippedGeometry){glDisableVertexAttribArray(13);glVertexAttrib1f(13,-1.f);glVertexAttribDivisor(13,0);}
  else if(!transformed&&!index&&primitive==Topology::Triangles){
   const u32 submitted=vertex_count(primitive,count);std::vector<float> eligibility(submitted,0.f);const auto clip=th14_clip(d.matrix[0],d.matrix[1],d.matrix[2]);
   for(u32 i=0;i+2<submitted;i+=3)if(th14_primitive_projectable(static_cast<const float*>(data),d.stride,i,3,clip,d.viewport))eligibility[i]=eligibility[i+1]=eligibility[i+2]=1.f;
   const u32 flagBase=upload(instances,GL_ARRAY_BUFFER,eligibility.data(),u32(eligibility.size()*4),65536);glEnableVertexAttribArray(13);glVertexAttribPointer(13,1,GL_FLOAT,false,4,reinterpret_cast<void*>(flagBase));glVertexAttribDivisor(13,0);
  }else{
   glDisableVertexAttribArray(13);float eligible=0.f;
   // A non-instanced four-vertex strip is the same complete quad tested by
   // the instanced path. Preserve its projection rounding as well.
   if(!transformed&&!index&&primitive==Topology::Strip&&count==2){const auto clip=th14_clip(d.matrix[0],d.matrix[1],d.matrix[2]);eligible=th14_primitive_projectable(static_cast<const float*>(data),d.stride,0,4,clip,d.viewport)?1.f:0.f;}
   glVertexAttrib1f(13,eligible);glVertexAttribDivisor(13,0);
  }
 }
#endif
const u32 base=upload(vertices,GL_ARRAY_BUFFER,data,size,1048576);
 if(clippedGeometry){
  glEnableVertexAttribArray(13);glVertexAttribPointer(13,1,GL_FLOAT,false,88,reinterpret_cast<void*>(base+84));glVertexAttribDivisor(13,0);
  glEnableVertexAttribArray(14);glVertexAttribPointer(14,4,GL_FLOAT,false,88,reinterpret_cast<void*>(base+56));glVertexAttribDivisor(14,0);
  glEnableVertexAttribArray(15);glVertexAttribPointer(15,3,GL_FLOAT,false,88,reinterpret_cast<void*>(base+72));glVertexAttribDivisor(15,0);
 }
#ifdef __EMSCRIPTEN__
 if(setup){
#else
 {
#endif
 ++stats.layoutSetups;
glEnableVertexAttribArray(0);glVertexAttribPointer(0,(transformed||clippedGeometry)?4:3,GL_FLOAT,false,d.stride,reinterpret_cast<void*>(base+(clippedGeometry?28:0)));
 for(int j=0;j<2;j++){const int loc=j?3:1;const u32 offset=j?d.layout.specular:d.layout.diffuse;if(clippedGeometry&&j){glEnableVertexAttribArray(loc);glVertexAttribPointer(loc,1,GL_FLOAT,false,88,reinterpret_cast<void*>(base+52));}else if(offset!=VertexAttributes::absent){glEnableVertexAttribArray(loc);glVertexAttribPointer(loc,4,GL_UNSIGNED_BYTE,true,d.stride,reinterpret_cast<void*>(base+offset));}else{glDisableVertexAttribArray(loc);glVertexAttrib4f(loc,1,1,1,1);}}
 if(d.layout.uv!=VertexAttributes::absent){glEnableVertexAttribArray(2);glVertexAttribPointer(2,2,GL_FLOAT,false,d.stride,reinterpret_cast<void*>(base+d.layout.uv));}else{glDisableVertexAttribArray(2);glVertexAttrib2f(2,0,0);}
 }
 constexpr GLenum modes[]{GL_POINTS,GL_LINES,GL_LINE_STRIP,GL_TRIANGLES,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN};const GLenum mode=modes[u32(primitive)];const u32 n=vertex_count(primitive,count);
 if(instance){
#if defined(PORT_TITLE) && PORT_TITLE == 14
 // Keep each original world/factor record and append its derived clip matrix.
 // The queue remains 68B; only the submitted instance buffer gains the matrix.
 const u32 records=instanceSize/68;std::vector<u8> expanded(records*136);
 const auto* input=static_cast<const u8*>(instance);
 for(u32 record=0;record<records;++record){
  std::array<float,16> model{};std::memcpy(model.data(),input+record*68,64);
  auto clip=th14_clip(model,d.matrix[1],d.matrix[2]);
  std::memcpy(expanded.data()+record*136,input+record*68,68);
  if(nativeFog){const auto eye=th14_multiply(d.matrix[1],model);std::memcpy(expanded.data()+record*136,eye.data(),64);}
  std::memcpy(expanded.data()+record*136+68,clip.data(),64);
  const float eligible=th14_primitive_projectable(static_cast<const float*>(data),d.stride,0,4,clip,d.viewport)?1.f:0.f;std::memcpy(expanded.data()+record*136+132,&eligible,4);
 }
 u32 b=upload(instances,GL_ARRAY_BUFFER,expanded.data(),u32(expanded.size()),65536);
#else
 u32 b=upload(instances,GL_ARRAY_BUFFER,instance,instanceSize,65536);
#endif
#ifdef __EMSCRIPTEN__
 if(setup){
#else
 {
#endif
#if defined(PORT_TITLE) && PORT_TITLE == 14
 for(int i=0;i<4;i++){glEnableVertexAttribArray(4+i);glVertexAttribPointer(4+i,4,GL_FLOAT,false,136,reinterpret_cast<void*>(b+i*16));glVertexAttribDivisor(4+i,1);glEnableVertexAttribArray(9+i);glVertexAttribPointer(9+i,4,GL_FLOAT,false,136,reinterpret_cast<void*>(b+68+i*16));glVertexAttribDivisor(9+i,1);}glEnableVertexAttribArray(8);glVertexAttribPointer(8,4,GL_UNSIGNED_BYTE,true,136,reinterpret_cast<void*>(b+64));glVertexAttribDivisor(8,1);glEnableVertexAttribArray(13);glVertexAttribPointer(13,1,GL_FLOAT,false,136,reinterpret_cast<void*>(b+132));glVertexAttribDivisor(13,1);}
#else
 for(int i=0;i<4;i++){glEnableVertexAttribArray(4+i);glVertexAttribPointer(4+i,4,GL_FLOAT,false,68,reinterpret_cast<void*>(b+i*16));glVertexAttribDivisor(4+i,1);}glEnableVertexAttribArray(8);glVertexAttribPointer(8,4,GL_UNSIGNED_BYTE,true,68,reinterpret_cast<void*>(b+64));glVertexAttribDivisor(8,1);}
#endif
 glDrawArraysInstanced(mode,0,n,instanceSize/68);}
 else if(index){u32 b=upload(indices,GL_ELEMENT_ARRAY_BUFFER,index,n*(indexFormat==IndexType::UInt16?2:4),65536);glDrawElements(mode,n,indexFormat==IndexType::UInt16?GL_UNSIGNED_SHORT:GL_UNSIGNED_INT,reinterpret_cast<void*>(b));}
 else glDrawArrays(mode,0,n);g.rendered=true;

}
void Renderer::clear(u32 flags,u32 c,float depth,u32 stencil,const i32* rects,u32 count){flush();
#ifdef TH14_WEBGPU_BACKEND
 if(webGpu){webGpu->clear(flags,c,depth,stencil,rects,count);return;}
#endif
auto& g=target(state.target,state.depth);const auto& v=state.viewport;i32 box[]{i32(v.x),i32(v.y),i32(v.x+v.width),i32(v.y+v.height)};if(!rects){rects=box;count=1;}glEnable(GL_SCISSOR_TEST);glColorMask(true,true,true,true);glDepthMask(true);drawState.pipeline.colorMask=15;drawState.pipeline.depthWrite=true;auto rgba=color(c);glClearColor(rgba[0],rgba[1],rgba[2],rgba[3]);glClearDepthf(depth);glClearStencil(stencil);glStencilMask(255);GLbitfield bits=(flags&1?GL_COLOR_BUFFER_BIT:0)|(flags&2?GL_DEPTH_BUFFER_BIT:0)|(flags&4?GL_STENCIL_BUFFER_BIT:0);for(u32 i=0;i<count;i++){const i32* p=rects+i*4;glScissor(p[0],p[1],p[2]-p[0],p[3]-p[1]);glClear(bits);}glDisable(GL_SCISSOR_TEST);g.rendered=true;}
void Renderer::copy(u32 src,const i32* rect,u32 dst,const i32* point){flush();
#ifdef TH14_WEBGPU_BACKEND
 if(webGpu){webGpu->copy(src,rect,dst,point);return;}
#endif
auto& a=surface(src);auto& b=surface(dst);bind_framebuffer(GL_READ_FRAMEBUFFER,a.framebuffer);bind_framebuffer(GL_DRAW_FRAMEBUFFER,b.framebuffer);glDisable(GL_SCISSOR_TEST);glBlitFramebuffer(rect[0],rect[1],rect[2],rect[3],point[0],point[1],point[0]+rect[2]-rect[0],point[1]+rect[3]-rect[1],GL_COLOR_BUFFER_BIT,GL_NEAREST);b.rendered=true;b.version=resolve(owner,dst).version+1;}
bool Renderer::resample(u32 src,const i32* from,u32 dst,const i32* to,const float* weights,u32 width,u32 height){
#ifdef TH14_WEBGPU_BACKEND
 if(webGpu){flush();return webGpu->resample(src,from,dst,to,weights,width,height);}
#endif
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
void Renderer::read(u32 id){flush();
#ifdef TH14_WEBGPU_BACKEND
 if(webGpu){webGpu->read(id);return;}
#endif
auto it=surfaces.find(id);if(it==surfaces.end()||!it->second.rendered)return;auto s=resolve(owner,id);auto& g=it->second;stats.readBytes+=s.size;pixels.resize(s.width*s.height*4);bind_framebuffer(GL_FRAMEBUFFER,g.framebuffer);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,s.width,s.height,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
 for(u32 y=0;y<s.height;y++)for(u32 x=0;x<s.width;x++){const auto* p=pixels.data()+(y*s.width+x)*4;u32 r=p[0],gc=p[1],b=p[2],a=p[3];auto* out=s.data+y*s.pitch;
  if(s.format==PixelFormat::Bgra8||s.format==PixelFormat::Bgrx8){out+=x*4;out[0]=b;out[1]=gc;out[2]=r;out[3]=s.format==PixelFormat::Bgra8?a:255;}else if((s.format==PixelFormat::Rgb565||s.format==PixelFormat::Xrgb1555||s.format==PixelFormat::Argb1555||s.format==PixelFormat::Argb4444)){uint16_t n=s.format==PixelFormat::Argb4444?(a>>4)<<12|(r>>4)<<8|(gc>>4)<<4|(b>>4):s.format==PixelFormat::Rgb565?(r>>3)<<11|(gc>>2)<<5|(b>>3):(s.format==PixelFormat::Argb1555&&a>=128?0x8000:0)|(r>>3)<<10|(gc>>3)<<5|(b>>3);std::memcpy(out+x*2,&n,2);}else if(s.format==PixelFormat::Alpha8)out[x]=a;else if(s.format==PixelFormat::Luminance8)out[x]=r;else std::abort();
 }g.rendered=false;
}
void Renderer::release(u32 id){flush();
#ifdef TH14_WEBGPU_BACKEND
 if(webGpu){webGpu->release(id);return;}
#endif
auto d=depths.find(id);if(d!=depths.end()){glDeleteRenderbuffers(1,&d->second.buffer);depths.erase(d);}auto it=surfaces.find(id);if(it==surfaces.end())return;glDeleteTextures(1,&it->second.texture);glDeleteFramebuffers(1,&it->second.framebuffer);surfaces.erase(it);boundTexture=readFramebuffer=drawFramebuffer=~0u;}
void Renderer::present(u32 id){flush();pending=id;stats.frames++;if(!defer)commit();}
bool Renderer::commit(){if(!pending)return false;
#ifdef TH14_WEBGPU_BACKEND
 if(webGpu){const auto id=pending;pending=0;webGpu->present(id);++stats.presentations;return true;}
#endif
auto s=resolve(owner,pending);auto& g=surface(pending);pending=0;bind_framebuffer(GL_READ_FRAMEBUFFER,g.framebuffer);bind_framebuffer(GL_DRAW_FRAMEBUFFER,0);glDisable(GL_SCISSOR_TEST);
#if defined(PORT_TITLE) && PORT_TITLE == 14
glBlitFramebuffer(0,0,s.width,s.height,0,s.height,s.width,0,GL_COLOR_BUFFER_BIT,GL_NEAREST);
#else
glBlitFramebuffer(0,0,s.width,s.height,0,480,640,0,GL_COLOR_BUFFER_BIT,GL_NEAREST);
#endif
SDL_GL_SwapWindow(window);stats.presentations++;return true;}
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
