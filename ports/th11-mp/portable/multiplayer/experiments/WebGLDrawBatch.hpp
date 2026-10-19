#pragma once
// Archived r36 experiment: not included or enabled in the production renderer.
// Internal bridge batching only: the SAME GLES calls, arguments and order.
// It is bounded to one issue_uploaded transaction. Uploads and GL object
// allocation are fences; no pointer to caller-owned memory survives a scope.
#if defined(__EMSCRIPTEN__) && !defined(__wasm64__)
#include <emscripten.h>
#include <array>
#include <cstring>
#include <initializer_list>
#include <cstdint>
EM_JS_DEPS(touhou_gl_draw_commands,
 "glUniform1i,glUniform1f,glUniform3fv,glUniform4fv,glUniformMatrix4fv,glEnable,glDisable,glActiveTexture,glBindTexture,glTexParameteri,glDepthMask,glDepthFunc,glBlendFunc,glBlendEquation,glFrontFace,glCullFace,glColorMask,glBindVertexArray,glEnableVertexAttribArray,glDisableVertexAttribArray,glVertexAttribPointer,glVertexAttribDivisor,glVertexAttrib4f,glVertexAttrib2f,glDrawArrays,glDrawArraysInstanced,glDrawElements");
EM_JS(void,touhou_gl_draw_commands,(const unsigned* data,unsigned count),{
 const start=data>>>2,end=start+count;let at=start;
 // Resolve views per invocation: a previous upload may have grown memory.
 const u=HEAPU32,f=HEAPF32;
 if(end>u.length)throw new RangeError('draw command buffer outside memory');
 while(at<end){const op=u[at++],a=u[at++],b=u[at++],c=u[at++],d=u[at++],e=u[at++],g=u[at++];
  switch(op){
   case 1:_glUniform1i(a|0,b|0);break;
   case 2:_glUniform1f(a|0,f[at-5]);break;
   case 3:_glUniform3fv(a|0,1,at*4);at+=3;break;
   case 4:_glUniform4fv(a|0,1,at*4);at+=4;break;
   case 5:_glUniformMatrix4fv(a|0,1,b,at*4);at+=16;break;
   case 6:_glEnable(a);break;
   case 7:_glDisable(a);break;
   case 8:_glActiveTexture(a);break;
   case 9:_glBindTexture(a,b);break;
   case 10:_glTexParameteri(a,b,c|0);break;
   case 11:_glDepthMask(a);break;
   case 12:_glDepthFunc(a);break;
   case 13:_glBlendFunc(a,b);break;
   case 14:_glBlendEquation(a);break;
   case 15:_glFrontFace(a);break;
   case 16:_glCullFace(a);break;
   case 17:_glColorMask(a,b,c,d);break;
   case 18:_glBindVertexArray(a);break;
   case 19:_glEnableVertexAttribArray(a);break;
   case 20:_glDisableVertexAttribArray(a);break;
   case 21:_glVertexAttribPointer(a,b|0,c,d,e|0,g);break;
   case 22:_glVertexAttribDivisor(a,b);break;
   case 23:_glVertexAttrib4f(a,f[at-5],f[at-4],f[at-3],f[at-2]);break;
   case 24:_glVertexAttrib2f(a,f[at-5],f[at-4]);break;
   case 25:_glDrawArrays(a,b|0,c|0);break;
   case 26:_glDrawArraysInstanced(a,b|0,c|0,d|0);break;
   case 27:_glDrawElements(a,b|0,c,d);break;
   default:throw new Error('invalid draw command');
  }
 }
 if(at!==end)throw new Error('invalid draw command length');
});
namespace touhou::sdl::glbatch {
inline std::array<unsigned,2048> commands{};
inline unsigned count=0;inline bool active=false;
inline void flush(){if(count){touhou_gl_draw_commands(commands.data(),count);count=0;}}
inline unsigned bits(float value){unsigned out;std::memcpy(&out,&value,4);return out;}
inline void emit(unsigned op,std::initializer_list<unsigned> arguments,const float* values=nullptr,unsigned length=0){
 if(count+7+length>commands.size())flush();
 auto* out=commands.data()+count;out[0]=op;unsigned i=1;for(const auto x:arguments)out[i++]=x;
 for(;i<7;++i)out[i]=0;
 if(length)std::memcpy(out+7,values,length*4);count+=7+length;
}
struct Scope{
 bool enabled;
 explicit Scope(bool on):enabled(on){if(enabled){flush();active=true;}}
 ~Scope(){if(enabled){flush();active=false;}}
};
inline void Uniform1i(GLint a,GLint b){if(active)emit(1,{unsigned(a),unsigned(b)});else glUniform1i(a,b);}
inline void Uniform1f(GLint a,GLfloat b){if(active)emit(2,{unsigned(a),bits(b)});else glUniform1f(a,b);}
inline void Uniform3fv(GLint a,GLsizei n,const GLfloat* v){if(active&&n==1)emit(3,{unsigned(a)},v,3);else{flush();glUniform3fv(a,n,v);}}
inline void Uniform4fv(GLint a,GLsizei n,const GLfloat* v){if(active&&n==1)emit(4,{unsigned(a)},v,4);else{flush();glUniform4fv(a,n,v);}}
inline void UniformMatrix4fv(GLint a,GLsizei n,GLboolean t,const GLfloat* v){if(active&&n==1)emit(5,{unsigned(a),unsigned(t)},v,16);else{flush();glUniformMatrix4fv(a,n,t,v);}}
#define TH11_GL_BATCH_ONE(name,type,op) inline void name(type a){if(active)emit(op,{unsigned(a)});else gl##name(a);}
TH11_GL_BATCH_ONE(Enable,GLenum,6)
TH11_GL_BATCH_ONE(Disable,GLenum,7)
TH11_GL_BATCH_ONE(ActiveTexture,GLenum,8)
TH11_GL_BATCH_ONE(DepthMask,GLboolean,11)
TH11_GL_BATCH_ONE(DepthFunc,GLenum,12)
TH11_GL_BATCH_ONE(BlendEquation,GLenum,14)
TH11_GL_BATCH_ONE(FrontFace,GLenum,15)
TH11_GL_BATCH_ONE(CullFace,GLenum,16)
TH11_GL_BATCH_ONE(BindVertexArray,GLuint,18)
TH11_GL_BATCH_ONE(EnableVertexAttribArray,GLuint,19)
TH11_GL_BATCH_ONE(DisableVertexAttribArray,GLuint,20)
#undef TH11_GL_BATCH_ONE
inline void BindTexture(GLenum a,GLuint b){if(active)emit(9,{a,b});else glBindTexture(a,b);}
inline void TexParameteri(GLenum a,GLenum b,GLint c){if(active)emit(10,{a,b,unsigned(c)});else glTexParameteri(a,b,c);}
inline void BlendFunc(GLenum a,GLenum b){if(active)emit(13,{a,b});else glBlendFunc(a,b);}
inline void ColorMask(GLboolean a,GLboolean b,GLboolean c,GLboolean d){if(active)emit(17,{a,b,c,d});else glColorMask(a,b,c,d);}
inline void VertexAttribPointer(GLuint a,GLint b,GLenum c,GLboolean d,GLsizei e,const void* p){if(active)emit(21,{a,unsigned(b),c,d,unsigned(e),unsigned(reinterpret_cast<std::uintptr_t>(p))});else glVertexAttribPointer(a,b,c,d,e,p);}
inline void VertexAttribDivisor(GLuint a,GLuint b){if(active)emit(22,{a,b});else glVertexAttribDivisor(a,b);}
inline void VertexAttrib4f(GLuint a,GLfloat b,GLfloat c,GLfloat d,GLfloat e){if(active)emit(23,{a,bits(b),bits(c),bits(d),bits(e)});else glVertexAttrib4f(a,b,c,d,e);}
inline void VertexAttrib2f(GLuint a,GLfloat b,GLfloat c){if(active)emit(24,{a,bits(b),bits(c)});else glVertexAttrib2f(a,b,c);}
inline void DrawArrays(GLenum a,GLint b,GLsizei c){if(active)emit(25,{a,unsigned(b),unsigned(c)});else glDrawArrays(a,b,c);}
inline void DrawArraysInstanced(GLenum a,GLint b,GLsizei c,GLsizei d){if(active)emit(26,{a,unsigned(b),unsigned(c),unsigned(d)});else glDrawArraysInstanced(a,b,c,d);}
inline void DrawElements(GLenum a,GLsizei b,GLenum c,const void* p){if(active)emit(27,{a,unsigned(b),c,unsigned(reinterpret_cast<std::uintptr_t>(p))});else glDrawElements(a,b,c,p);}
// Every operation that consumes a transient source or produces an object ID
// executes immediately, after preceding state commands have been submitted.
inline void BindBuffer(GLenum a,GLuint b){flush();glBindBuffer(a,b);}
inline void BufferData(GLenum a,GLsizeiptr b,const void* p,GLenum c){flush();glBufferData(a,b,p,c);}
inline void GenBuffers(GLsizei n,GLuint* p){flush();glGenBuffers(n,p);}
inline void GenVertexArrays(GLsizei n,GLuint* p){flush();glGenVertexArrays(n,p);}
}
#define glUniform1i touhou::sdl::glbatch::Uniform1i
#define glUniform1f touhou::sdl::glbatch::Uniform1f
#define glUniform3fv touhou::sdl::glbatch::Uniform3fv
#define glUniform4fv touhou::sdl::glbatch::Uniform4fv
#define glUniformMatrix4fv touhou::sdl::glbatch::UniformMatrix4fv
#define glEnable touhou::sdl::glbatch::Enable
#define glDisable touhou::sdl::glbatch::Disable
#define glActiveTexture touhou::sdl::glbatch::ActiveTexture
#define glBindTexture touhou::sdl::glbatch::BindTexture
#define glTexParameteri touhou::sdl::glbatch::TexParameteri
#define glDepthMask touhou::sdl::glbatch::DepthMask
#define glDepthFunc touhou::sdl::glbatch::DepthFunc
#define glBlendFunc touhou::sdl::glbatch::BlendFunc
#define glBlendEquation touhou::sdl::glbatch::BlendEquation
#define glFrontFace touhou::sdl::glbatch::FrontFace
#define glCullFace touhou::sdl::glbatch::CullFace
#define glColorMask touhou::sdl::glbatch::ColorMask
#define glBindVertexArray touhou::sdl::glbatch::BindVertexArray
#define glEnableVertexAttribArray touhou::sdl::glbatch::EnableVertexAttribArray
#define glDisableVertexAttribArray touhou::sdl::glbatch::DisableVertexAttribArray
#define glVertexAttribPointer touhou::sdl::glbatch::VertexAttribPointer
#define glVertexAttribDivisor touhou::sdl::glbatch::VertexAttribDivisor
#define glVertexAttrib4f touhou::sdl::glbatch::VertexAttrib4f
#define glVertexAttrib2f touhou::sdl::glbatch::VertexAttrib2f
#define glDrawArrays touhou::sdl::glbatch::DrawArrays
#define glDrawArraysInstanced touhou::sdl::glbatch::DrawArraysInstanced
#define glDrawElements touhou::sdl::glbatch::DrawElements
#define glBindBuffer touhou::sdl::glbatch::BindBuffer
#define glBufferData touhou::sdl::glbatch::BufferData
#define glGenBuffers touhou::sdl::glbatch::GenBuffers
#define glGenVertexArrays touhou::sdl::glbatch::GenVertexArrays
#else
namespace touhou::sdl::glbatch {struct Scope{explicit Scope(bool){}};}
#endif
