#include "Renderer.hpp"
#include "Platform.hpp"
#include "GraphicsApi.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace th12host {
namespace {
bool desktop_gl=false;
GLuint compile(GLenum type,const char* source,std::string& error){
    std::string native_source=source;
    if(desktop_gl){native_source.replace(0,native_source.find("\n"),"#version 330 core");source=native_source.c_str();}
    const GLuint s=glCreateShader(type);glShaderSource(s,1,&source,nullptr);glCompileShader(s);GLint ok=0;glGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if(!ok){char log[1024]{};glGetShaderInfoLog(s,sizeof log,nullptr,log);error=log;glDeleteShader(s);return 0;}return s;
}
// Upstream's shaders: position in 640x480 units, clip W for XYZRHW meshes and
// the 3D background, linear vertex fog, D3D ALPHAREF 1/255 GREATEREQUAL.
const char* vertex_source=R"(#version 300 es

#ifdef GL_ES
precision highp float;
#endif
layout(location=0)in vec2 position;layout(location=1)in vec2 uv;layout(location=2)in vec4 color;layout(location=3)in vec2 depthFog;
out vec2 texcoord;out vec4 tint;out float fog;
void main(){gl_Position=vec4((position.x/320.0-1.0)*depthFog.x,(1.0-position.y/240.0)*depthFog.x,0,depthFog.x);texcoord=uv;tint=color;fog=depthFog.y;})";
const char* fragment_source=R"(#version 300 es

#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D atlas;uniform vec3 fogColor;in vec2 texcoord;in vec4 tint;in float fog;out vec4 pixel;
void main(){pixel=texture(atlas,texcoord)*tint;pixel.rgb=mix(pixel.rgb,fogColor,clamp(fog,0.0,1.0));if(pixel.a<1.0/255.0)discard;})";
}

float native_pixel(float n){
    // 459e50 uses x87 nearest-even before the D3D half-pixel adjustment.
    const double floor=std::floor(double(n)),fraction=double(n)-floor;
    return float(fraction==.5?floor+(std::fabs(std::fmod(floor,2.))==1?1:0):std::round(double(n)));
}
Corners quad_corners(float x,float y,float w,float h,float angle,i32 ax,i32 ay,bool pixel){
    const double left=ax==1?0:ax==2?-double(w):-double(w)/2,top=ay==1?0:ay==2?-double(h):-double(h)/2,c=std::cos(double(angle)),s=std::sin(double(angle));
    Corners out{};
    for(int i=0;i<4;++i){const double a=i==1||i==2?left+w:left,b=i>=2?top+h:top,px=x+a*c-b*s,py=y+a*s+b*c;
        out[i][0]=pixel?native_pixel(float(px)):float(px);out[i][1]=pixel?native_pixel(float(py)):float(py);}
    return out;
}
// Original mode0:45a570 floors centred left/top, then 459e50 adds the screen
// offset and rounds nearest-even. Mode1:45af10 rotates first and never rounds.
Corners native_quad_corners(float x,float y,float w,float h,float angle,i32 ax,i32 ay,i32 mode,const float* offset){
    const auto f=[](double v){return float(v);};const float ox=offset?offset[0]:0,oy=offset?offset[1]:0;Corners out{};
    if(mode==0){
        const float left=ax==1?x:ax==2?f(double(x)-w):float(std::floor(f(double(x)-double(w)/2))),
            top=ay==1?y:ay==2?f(double(y)-h):float(std::floor(f(double(y)-double(h)/2))),
            right=ax==2?x:f(double(left)+w),bottom=ay==2?y:f(double(top)+h);
        const float l=native_pixel(f(double(left)+ox)),r=native_pixel(f(double(right)+ox)),t=native_pixel(f(double(top)+oy)),b=native_pixel(f(double(bottom)+oy));
        out[0]={l,t};out[1]={r,t};out[2]={r,b};out[3]={l,b};return out;
    }
    const float left=ax==1?0:ax==2?-w:f(-double(w)/2),top=ay==1?0:ay==2?-h:f(-double(h)/2),c=f(std::cos(double(angle))),s=f(std::sin(double(angle)));
    const float right=f(double(left)+w),bottom=f(double(top)+h);
    for(int i=0;i<4;++i){const float a=i==1||i==2?right:left,b=i>=2?bottom:top;
        out[i][0]=f(double(f(double(x)+double(a)*c-double(b)*s))+ox);out[i][1]=f(double(f(double(y)+double(a)*s+double(b)*c))+oy);}
    return out;
}

bool Renderer::initialize(u32 scale,std::string& error){
    if(!gles::load(error))return false;
#ifdef __EMSCRIPTEN__
    desktop_gl=false; // WebGL always uses GLSL ES, independently of SDL's queried attributes.
#else
    int profile=0;SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,&profile);desktop_gl=profile!=SDL_GL_CONTEXT_PROFILE_ES;
#endif
    host::log("graphics: %s",reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    scale_=scale;
    const GLuint vs=compile(GL_VERTEX_SHADER,vertex_source,error);if(!vs)return false;
    const GLuint fs=compile(GL_FRAGMENT_SHADER,fragment_source,error);if(!fs)return false;
    program=glCreateProgram();glAttachShader(program,vs);glAttachShader(program,fs);glLinkProgram(program);glDeleteShader(vs);glDeleteShader(fs);
    GLint ok=0;glGetProgramiv(program,GL_LINK_STATUS,&ok);if(!ok){char log[1024]{};glGetProgramInfoLog(program,sizeof log,nullptr,log);error=log;return false;}
    glUseProgram(program);fog_uniform=glGetUniformLocation(program,"fogColor");glUniform1i(glGetUniformLocation(program,"atlas"),0);
    glGenVertexArrays(1,&vao);glBindVertexArray(vao);glGenBuffers(1,&vbo);glBindBuffer(GL_ARRAY_BUFFER,vbo);
    const struct {GLuint slot;GLint count;size_t offset;} attributes[]{{0,2,0},{1,2,8},{2,4,16},{3,2,32}};
    for(const auto& a:attributes){glEnableVertexAttribArray(a.slot);glVertexAttribPointer(a.slot,a.count,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<const void*>(a.offset));}
    const u8 white[4]{255,255,255,255};solid_={create_texture(1,1,white,true),1,1,false,false,true};
    // Offscreen target in 640x480 units x scale.
    const GLsizei w=GLsizei(640*scale_),h=GLsizei(480*scale_);
    glGenTextures(1,&target_texture);glBindTexture(GL_TEXTURE_2D,target_texture);glTexStorage2D(GL_TEXTURE_2D,1,GL_RGBA8,w,h);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glGenFramebuffers(1,&target);glBindFramebuffer(GL_FRAMEBUFFER,target);glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,target_texture,0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE){error="offscreen target incomplete";return false;}
    // 431630: text@R entry 2 aliases the first native target. RGB: samples A=1.
    glGenTextures(1,&surface_.name);glBindTexture(GL_TEXTURE_2D,surface_.name);glTexStorage2D(GL_TEXTURE_2D,1,GL_RGB8,w,h);
    surface_.w=640;surface_.h=480;surface_.flip_y=true;surface_.repeat=true;surface_.available=false; // until the first copy
    // 4356d0/4609d0: the paused field, downsampled into text sprite 71's 128x128 corner of a 512x512 RGB565 target.
    glGenTextures(1,&pause_.name);glBindTexture(GL_TEXTURE_2D,pause_.name);glTexStorage2D(GL_TEXTURE_2D,1,GL_RGB565,512,512);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    pause_.w=512;pause_.h=512;pause_.flip_y=true;pause_.repeat=false;pause_.available=false;
    glGenFramebuffers(1,&pause_target);glBindFramebuffer(GL_FRAMEBUFFER,pause_target);glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,pause_.name,0);
    glBindFramebuffer(GL_FRAMEBUFFER,target);
    glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);
    glLineWidth(float(scale_)); // 1 original pixel; drivers clamp to their supported range
    while(glGetError()!=GL_NO_ERROR){}
    batch.reserve(60*256);return true;
}
void Renderer::shutdown(){
    if(program)glDeleteProgram(program);if(vbo)glDeleteBuffers(1,&vbo);if(vao)glDeleteVertexArrays(1,&vao);
    for(u32* t:{&target_texture,&surface_.name,&pause_.name,&solid_.name})if(*t)glDeleteTextures(1,t);
    for(u32* f:{&target,&pause_target})if(*f)glDeleteFramebuffers(1,f);
    program=vbo=vao=0;
}
u32 Renderer::create_texture(u32 w,u32 h,const u8* rgba,bool clamp){
    // Text textures are made mid-frame: draw what is queued with the texture
    // it was queued with before binding the new one (upstream flushes too).
    flush();GLuint t=0;glGenTextures(1,&t);glBindTexture(GL_TEXTURE_2D,t);glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,GLsizei(w),GLsizei(h),0,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    const GLint wrap=clamp?GL_CLAMP_TO_EDGE:GL_REPEAT; // Native4514c5/4514db defaults both texture axes to WRAP1.
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrap);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrap);
    states[t]={false,!clamp,!clamp};current=0;return t;
}
void Renderer::delete_texture(u32 name){if(!name)return;flush();if(current==name)current=0;states.erase(name);GLuint t=name;glDeleteTextures(1,&t);}
void Renderer::upload(AnmFile& file){while(upload_next(file)){}}
bool Renderer::upload_next(AnmFile& file){
    for(auto& e:file.entries)if(e.has_image&&!e.rgba.empty()&&!e.texture){e.texture=create_texture(e.texture_width,e.texture_height,e.rgba.data(),false);std::vector<u8>().swap(e.rgba);return true;}
    return false;
}
void Renderer::release(AnmFile& file){for(auto& e:file.entries)if(e.texture){GLuint t=e.texture;glDeleteTextures(1,&t);e.texture=0;}}

void Renderer::begin(Color c){
    glBindFramebuffer(GL_FRAMEBUFFER,target);glViewport(0,0,GLsizei(640*scale_),GLsizei(480*scale_));glDisable(GL_SCISSOR_TEST);
    glClearColor(c.r,c.g,c.b,c.a);glClear(GL_COLOR_BUFFER_BIT);glUseProgram(program);glBindVertexArray(vao);glBindBuffer(GL_ARRAY_BUFFER,vbo);
    draws=vertices=0;current=0;batch.clear();primitive_=Triangles;blend_=-2;surface_ready=false;material(0);fog_color(0,0,0);
    // A fresh store for this frame's vertices (the GPU may still read the last one).
    if(!stream_capacity)stream_capacity=size_t(1)<<20;glBufferData(GL_ARRAY_BUFFER,GLsizeiptr(stream_capacity),nullptr,GL_STREAM_DRAW);stream_used=0;
}
void Renderer::flush(){
    if(batch.empty())return;glBindTexture(GL_TEXTURE_2D,current);
    // Each draw appends to the frame's buffer: no new buffer store per draw,
    // and a range nothing has drawn from yet needs no wait.
    const size_t bytes=batch.size()*sizeof(Vertex);
    if(!stream_capacity||stream_used+bytes>stream_capacity){
        while(stream_capacity<bytes)stream_capacity=std::max<size_t>(stream_capacity*2,size_t(1)<<20);
        glBufferData(GL_ARRAY_BUFFER,GLsizeiptr(stream_capacity),nullptr,GL_STREAM_DRAW);stream_used=0;
    }
    glBufferSubData(GL_ARRAY_BUFFER,GLintptr(stream_used),GLsizeiptr(bytes),batch.data());
    glDrawArrays(primitive_==Triangles?GL_TRIANGLES:primitive_==Lines?GL_LINES:GL_LINE_STRIP,GLint(stream_used/sizeof(Vertex)),GLsizei(batch.size()));
    stream_used+=bytes;++draws;vertices+=u32(batch.size());batch.clear();
}
void Renderer::primitive(Primitive p){if(primitive_!=p){flush();primitive_=p;}}
void Renderer::material(i32 blend){
    if(blend_==blend)return;flush();
    static constexpr GLenum factors[7][2]{{GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA},{GL_SRC_ALPHA,GL_ONE},{GL_ZERO,GL_ONE_MINUS_SRC_COLOR},{GL_ONE,GL_ZERO},
        {GL_ONE_MINUS_DST_COLOR,GL_ONE_MINUS_SRC_ALPHA},{GL_DST_COLOR,GL_ZERO},{GL_ONE_MINUS_SRC_COLOR,GL_ONE_MINUS_SRC_ALPHA}};
    const auto& f=factors[blend>=0&&blend<7?blend:0];glBlendFunc(f[0],f[1]);blend_=blend;
}
void Renderer::texture_parameters(u32 name,bool point,bool wu,bool wv){
    // Sampling belongs to each texture, not to the last bound texture.
    auto it=states.find(name);const bool known=it!=states.end();
    if(!known||it->second.point!=point){glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,point?GL_NEAREST:GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,point?GL_NEAREST:GL_LINEAR);}
    if(!known||it->second.wu!=wu)glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wu?GL_REPEAT:GL_CLAMP_TO_EDGE);
    if(!known||it->second.wv!=wv)glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wv?GL_REPEAT:GL_CLAMP_TO_EDGE);
    states[name]={point,wu,wv};
}
void Renderer::bind(const Texture& t,bool point,i32 flags){
    if(flags<0)flags=t.address_flags;
    const bool wu=flags<0?t.repeat:(flags&1),wv=flags<0?t.repeat:(flags&2);
    if(current!=t.name||point_!=point||wrap_u!=wu||wrap_v!=wv){flush();current=t.name;point_=point;wrap_u=wu;wrap_v=wv;glBindTexture(GL_TEXTURE_2D,t.name);texture_parameters(t.name,point,wu,wv);}
}
void Renderer::push(const Vertex& v){batch.push_back(v);}
void Renderer::quad(const Corners& c,float u0,float u1,float v0,float v1,bool flip,const Color& k,float ox,float oy){
    static constexpr int order[6]{0,1,2,0,2,3};
    for(int i:order){const float u=i==1||i==2?u1:u0,v=i>=2?v1:v0;batch.push_back({c[i][0]+ox,c[i][1]+oy,u,flip?1-v:v,k.r,k.g,k.b,k.a,1,0});}
}
void Renderer::fog_color(float r,float g,float b){flush();glUniform3f(fog_uniform,r,g,b);}
void Renderer::scissor(i32 x,i32 y,i32 w,i32 h){const i32 s=i32(scale_);glScissor(x*s,(480-y-h)*s,w*s,h*s);}
void Renderer::clip(i32 mode){
    flush();if(mode==0){glDisable(GL_SCISSOR_TEST);return;}
    glEnable(GL_SCISSOR_TEST);if(mode==2)scissor(13,0,422,480);else scissor(32,16,384,448);
}
void Renderer::clear(Color c){flush();glClearColor(c.r,c.g,c.b,c.a);glClear(GL_COLOR_BUFFER_BIT);}
void Renderer::region(const Texture& t,float sx,float sy,float sw,float sh,float x,float y,float w,float h,float angle,float alpha,const Color& tint,i32 blend,const RegionOptions& o){
    if(!t.name||!t.available)return;
    material(blend);bind(t,o.point);primitive(Triangles);
    const Corners c=o.mode==0||o.mode==1?native_quad_corners(x,y,w,h,angle,o.anchor_x,o.anchor_y,o.mode,o.shake):quad_corners(x,y,w,h,angle,o.anchor_x,o.anchor_y,o.pixel);
    const float end_x=sx+sw*o.uv_scale_x,end_y=sy+sh*o.uv_scale_y;
    quad(c,sx/t.w+o.u,end_x/t.w+o.u,sy/t.h+o.v,end_y/t.h+o.v,t.flip_y,{tint.r,tint.g,tint.b,alpha});
}
void Renderer::solid_triangles(const Vertex* v,const u32* indices,u32 count,i32 blend){
    material(blend);bind(solid_,true);primitive(Triangles);for(u32 i=0;i<count;++i){Vertex p=v[indices[i]];p.u=p.v=.5f;p.depth=1;p.fog=0;batch.push_back(p);}
}
void Renderer::copy_stage_surface(){
    flush();glBindTexture(GL_TEXTURE_2D,surface_.name);
    glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,GLsizei(640*scale_),GLsizei(480*scale_));
    surface_.available=glGetError()==GL_NO_ERROR;current=0;
}
void Renderer::capture_pause_field(){
    flush();glDisable(GL_SCISSOR_TEST);const i32 s=i32(scale_);
    glBindFramebuffer(GL_READ_FRAMEBUFFER,target);glBindFramebuffer(GL_DRAW_FRAMEBUFFER,pause_target);
    glBlitFramebuffer(32*s,16*s,416*s,464*s,0,384,128,512,GL_COLOR_BUFFER_BIT,GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER,target);pause_.available=true;current=0;
}
void Renderer::present(i32 ww,i32 wh){
    flush();glDisable(GL_SCISSOR_TEST);
    // Aspect-correct fit with pure black bars (OLED friendly), like the other ports.
    const i32 tw=i32(640*scale_),th=i32(480*scale_);i32 w=ww,h=ww*3/4;if(h>wh){h=wh;w=wh*4/3;}const i32 x=(ww-w)/2,y=(wh-h)/2;
    glBindFramebuffer(GL_READ_FRAMEBUFFER,target);glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    glViewport(0,0,ww,wh);glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);
    glBlitFramebuffer(0,0,tw,th,x,y,x+w,y+h,GL_COLOR_BUFFER_BIT,GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER,target);
}
const u8* Renderer::read_pixels(){
    flush();const u32 w=640*scale_,h=480*scale_;pixels.resize(size_t(w)*h*4);std::vector<u8> raw(pixels.size());
    glBindFramebuffer(GL_READ_FRAMEBUFFER,target);glPixelStorei(GL_PACK_ALIGNMENT,1);glReadPixels(0,0,GLsizei(w),GLsizei(h),GL_RGBA,GL_UNSIGNED_BYTE,raw.data());
    for(u32 y=0;y<h;++y)std::memcpy(&pixels[size_t(y)*w*4],&raw[size_t(h-1-y)*w*4],size_t(w)*4);
    return glGetError()==GL_NO_ERROR?pixels.data():nullptr;
}
}
