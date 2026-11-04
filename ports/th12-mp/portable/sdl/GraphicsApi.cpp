#define TH12_GL_NO_ALIASES
#include "GraphicsApi.hpp"
#include <SDL3/SDL.h>
namespace th12host::gles {
PFNGLATTACHSHADERPROC AttachShader=nullptr;
PFNGLBINDBUFFERPROC BindBuffer=nullptr;
PFNGLBINDFRAMEBUFFERPROC BindFramebuffer=nullptr;
PFNGLBINDTEXTUREPROC BindTexture=nullptr;
PFNGLBINDVERTEXARRAYPROC BindVertexArray=nullptr;
PFNGLBLENDFUNCPROC BlendFunc=nullptr;
PFNGLBLITFRAMEBUFFERPROC BlitFramebuffer=nullptr;
PFNGLBUFFERDATAPROC BufferData=nullptr;
PFNGLBUFFERSUBDATAPROC BufferSubData=nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC CheckFramebufferStatus=nullptr;
PFNGLCLEARPROC Clear=nullptr;
PFNGLCLEARCOLORPROC ClearColor=nullptr;
PFNGLCOMPILESHADERPROC CompileShader=nullptr;
PFNGLCOPYTEXSUBIMAGE2DPROC CopyTexSubImage2D=nullptr;
PFNGLCREATEPROGRAMPROC CreateProgram=nullptr;
PFNGLCREATESHADERPROC CreateShader=nullptr;
PFNGLDELETEBUFFERSPROC DeleteBuffers=nullptr;
PFNGLDELETEFRAMEBUFFERSPROC DeleteFramebuffers=nullptr;
PFNGLDELETEPROGRAMPROC DeleteProgram=nullptr;
PFNGLDELETESHADERPROC DeleteShader=nullptr;
PFNGLDELETETEXTURESPROC DeleteTextures=nullptr;
PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays=nullptr;
PFNGLDISABLEPROC Disable=nullptr;
PFNGLDRAWARRAYSPROC DrawArrays=nullptr;
PFNGLENABLEPROC Enable=nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray=nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC FramebufferTexture2D=nullptr;
PFNGLGENBUFFERSPROC GenBuffers=nullptr;
PFNGLGENFRAMEBUFFERSPROC GenFramebuffers=nullptr;
PFNGLGENTEXTURESPROC GenTextures=nullptr;
PFNGLGENVERTEXARRAYSPROC GenVertexArrays=nullptr;
PFNGLGETERRORPROC GetError=nullptr;
PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog=nullptr;
PFNGLGETPROGRAMIVPROC GetProgramiv=nullptr;
PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog=nullptr;
PFNGLGETSHADERIVPROC GetShaderiv=nullptr;
PFNGLGETSTRINGPROC GetString=nullptr;
PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation=nullptr;
PFNGLLINEWIDTHPROC LineWidth=nullptr;
PFNGLLINKPROGRAMPROC LinkProgram=nullptr;
PFNGLPIXELSTOREIPROC PixelStorei=nullptr;
PFNGLREADPIXELSPROC ReadPixels=nullptr;
PFNGLSCISSORPROC Scissor=nullptr;
PFNGLSHADERSOURCEPROC ShaderSource=nullptr;
PFNGLTEXIMAGE2DPROC TexImage2D=nullptr;
PFNGLTEXPARAMETERIPROC TexParameteri=nullptr;
PFNGLTEXSTORAGE2DPROC TexStorage2D=nullptr;
PFNGLUNIFORM1IPROC Uniform1i=nullptr;
PFNGLUNIFORM3FPROC Uniform3f=nullptr;
PFNGLUSEPROGRAMPROC UseProgram=nullptr;
PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer=nullptr;
PFNGLVIEWPORTPROC Viewport=nullptr;
bool load(std::string& error){
#ifdef __EMSCRIPTEN__
    AttachShader=&::glAttachShader;
#else
    AttachShader=reinterpret_cast<PFNGLATTACHSHADERPROC>(SDL_GL_GetProcAddress("glAttachShader"));
#endif
    if(!AttachShader){error="Missing GLES 3 entry point: glAttachShader";return false;}
#ifdef __EMSCRIPTEN__
    BindBuffer=&::glBindBuffer;
#else
    BindBuffer=reinterpret_cast<PFNGLBINDBUFFERPROC>(SDL_GL_GetProcAddress("glBindBuffer"));
#endif
    if(!BindBuffer){error="Missing GLES 3 entry point: glBindBuffer";return false;}
#ifdef __EMSCRIPTEN__
    BindFramebuffer=&::glBindFramebuffer;
#else
    BindFramebuffer=reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(SDL_GL_GetProcAddress("glBindFramebuffer"));
#endif
    if(!BindFramebuffer){error="Missing GLES 3 entry point: glBindFramebuffer";return false;}
#ifdef __EMSCRIPTEN__
    BindTexture=&::glBindTexture;
#else
    BindTexture=reinterpret_cast<PFNGLBINDTEXTUREPROC>(SDL_GL_GetProcAddress("glBindTexture"));
#endif
    if(!BindTexture){error="Missing GLES 3 entry point: glBindTexture";return false;}
#ifdef __EMSCRIPTEN__
    BindVertexArray=&::glBindVertexArray;
#else
    BindVertexArray=reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(SDL_GL_GetProcAddress("glBindVertexArray"));
#endif
    if(!BindVertexArray){error="Missing GLES 3 entry point: glBindVertexArray";return false;}
#ifdef __EMSCRIPTEN__
    BlendFunc=&::glBlendFunc;
#else
    BlendFunc=reinterpret_cast<PFNGLBLENDFUNCPROC>(SDL_GL_GetProcAddress("glBlendFunc"));
#endif
    if(!BlendFunc){error="Missing GLES 3 entry point: glBlendFunc";return false;}
#ifdef __EMSCRIPTEN__
    BlitFramebuffer=&::glBlitFramebuffer;
#else
    BlitFramebuffer=reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(SDL_GL_GetProcAddress("glBlitFramebuffer"));
#endif
    if(!BlitFramebuffer){error="Missing GLES 3 entry point: glBlitFramebuffer";return false;}
#ifdef __EMSCRIPTEN__
    BufferData=&::glBufferData;
#else
    BufferData=reinterpret_cast<PFNGLBUFFERDATAPROC>(SDL_GL_GetProcAddress("glBufferData"));
#endif
    if(!BufferData){error="Missing GLES 3 entry point: glBufferData";return false;}
#ifdef __EMSCRIPTEN__
    BufferSubData=&::glBufferSubData;
#else
    BufferSubData=reinterpret_cast<PFNGLBUFFERSUBDATAPROC>(SDL_GL_GetProcAddress("glBufferSubData"));
#endif
    if(!BufferSubData){error="Missing GLES 3 entry point: glBufferSubData";return false;}
#ifdef __EMSCRIPTEN__
    CheckFramebufferStatus=&::glCheckFramebufferStatus;
#else
    CheckFramebufferStatus=reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(SDL_GL_GetProcAddress("glCheckFramebufferStatus"));
#endif
    if(!CheckFramebufferStatus){error="Missing GLES 3 entry point: glCheckFramebufferStatus";return false;}
#ifdef __EMSCRIPTEN__
    Clear=&::glClear;
#else
    Clear=reinterpret_cast<PFNGLCLEARPROC>(SDL_GL_GetProcAddress("glClear"));
#endif
    if(!Clear){error="Missing GLES 3 entry point: glClear";return false;}
#ifdef __EMSCRIPTEN__
    ClearColor=&::glClearColor;
#else
    ClearColor=reinterpret_cast<PFNGLCLEARCOLORPROC>(SDL_GL_GetProcAddress("glClearColor"));
#endif
    if(!ClearColor){error="Missing GLES 3 entry point: glClearColor";return false;}
#ifdef __EMSCRIPTEN__
    CompileShader=&::glCompileShader;
#else
    CompileShader=reinterpret_cast<PFNGLCOMPILESHADERPROC>(SDL_GL_GetProcAddress("glCompileShader"));
#endif
    if(!CompileShader){error="Missing GLES 3 entry point: glCompileShader";return false;}
#ifdef __EMSCRIPTEN__
    CopyTexSubImage2D=&::glCopyTexSubImage2D;
#else
    CopyTexSubImage2D=reinterpret_cast<PFNGLCOPYTEXSUBIMAGE2DPROC>(SDL_GL_GetProcAddress("glCopyTexSubImage2D"));
#endif
    if(!CopyTexSubImage2D){error="Missing GLES 3 entry point: glCopyTexSubImage2D";return false;}
#ifdef __EMSCRIPTEN__
    CreateProgram=&::glCreateProgram;
#else
    CreateProgram=reinterpret_cast<PFNGLCREATEPROGRAMPROC>(SDL_GL_GetProcAddress("glCreateProgram"));
#endif
    if(!CreateProgram){error="Missing GLES 3 entry point: glCreateProgram";return false;}
#ifdef __EMSCRIPTEN__
    CreateShader=&::glCreateShader;
#else
    CreateShader=reinterpret_cast<PFNGLCREATESHADERPROC>(SDL_GL_GetProcAddress("glCreateShader"));
#endif
    if(!CreateShader){error="Missing GLES 3 entry point: glCreateShader";return false;}
#ifdef __EMSCRIPTEN__
    DeleteBuffers=&::glDeleteBuffers;
#else
    DeleteBuffers=reinterpret_cast<PFNGLDELETEBUFFERSPROC>(SDL_GL_GetProcAddress("glDeleteBuffers"));
#endif
    if(!DeleteBuffers){error="Missing GLES 3 entry point: glDeleteBuffers";return false;}
#ifdef __EMSCRIPTEN__
    DeleteFramebuffers=&::glDeleteFramebuffers;
#else
    DeleteFramebuffers=reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(SDL_GL_GetProcAddress("glDeleteFramebuffers"));
#endif
    if(!DeleteFramebuffers){error="Missing GLES 3 entry point: glDeleteFramebuffers";return false;}
#ifdef __EMSCRIPTEN__
    DeleteProgram=&::glDeleteProgram;
#else
    DeleteProgram=reinterpret_cast<PFNGLDELETEPROGRAMPROC>(SDL_GL_GetProcAddress("glDeleteProgram"));
#endif
    if(!DeleteProgram){error="Missing GLES 3 entry point: glDeleteProgram";return false;}
#ifdef __EMSCRIPTEN__
    DeleteShader=&::glDeleteShader;
#else
    DeleteShader=reinterpret_cast<PFNGLDELETESHADERPROC>(SDL_GL_GetProcAddress("glDeleteShader"));
#endif
    if(!DeleteShader){error="Missing GLES 3 entry point: glDeleteShader";return false;}
#ifdef __EMSCRIPTEN__
    DeleteTextures=&::glDeleteTextures;
#else
    DeleteTextures=reinterpret_cast<PFNGLDELETETEXTURESPROC>(SDL_GL_GetProcAddress("glDeleteTextures"));
#endif
    if(!DeleteTextures){error="Missing GLES 3 entry point: glDeleteTextures";return false;}
#ifdef __EMSCRIPTEN__
    DeleteVertexArrays=&::glDeleteVertexArrays;
#else
    DeleteVertexArrays=reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(SDL_GL_GetProcAddress("glDeleteVertexArrays"));
#endif
    if(!DeleteVertexArrays){error="Missing GLES 3 entry point: glDeleteVertexArrays";return false;}
#ifdef __EMSCRIPTEN__
    Disable=&::glDisable;
#else
    Disable=reinterpret_cast<PFNGLDISABLEPROC>(SDL_GL_GetProcAddress("glDisable"));
#endif
    if(!Disable){error="Missing GLES 3 entry point: glDisable";return false;}
#ifdef __EMSCRIPTEN__
    DrawArrays=&::glDrawArrays;
#else
    DrawArrays=reinterpret_cast<PFNGLDRAWARRAYSPROC>(SDL_GL_GetProcAddress("glDrawArrays"));
#endif
    if(!DrawArrays){error="Missing GLES 3 entry point: glDrawArrays";return false;}
#ifdef __EMSCRIPTEN__
    Enable=&::glEnable;
#else
    Enable=reinterpret_cast<PFNGLENABLEPROC>(SDL_GL_GetProcAddress("glEnable"));
#endif
    if(!Enable){error="Missing GLES 3 entry point: glEnable";return false;}
#ifdef __EMSCRIPTEN__
    EnableVertexAttribArray=&::glEnableVertexAttribArray;
#else
    EnableVertexAttribArray=reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(SDL_GL_GetProcAddress("glEnableVertexAttribArray"));
#endif
    if(!EnableVertexAttribArray){error="Missing GLES 3 entry point: glEnableVertexAttribArray";return false;}
#ifdef __EMSCRIPTEN__
    FramebufferTexture2D=&::glFramebufferTexture2D;
#else
    FramebufferTexture2D=reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(SDL_GL_GetProcAddress("glFramebufferTexture2D"));
#endif
    if(!FramebufferTexture2D){error="Missing GLES 3 entry point: glFramebufferTexture2D";return false;}
#ifdef __EMSCRIPTEN__
    GenBuffers=&::glGenBuffers;
#else
    GenBuffers=reinterpret_cast<PFNGLGENBUFFERSPROC>(SDL_GL_GetProcAddress("glGenBuffers"));
#endif
    if(!GenBuffers){error="Missing GLES 3 entry point: glGenBuffers";return false;}
#ifdef __EMSCRIPTEN__
    GenFramebuffers=&::glGenFramebuffers;
#else
    GenFramebuffers=reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(SDL_GL_GetProcAddress("glGenFramebuffers"));
#endif
    if(!GenFramebuffers){error="Missing GLES 3 entry point: glGenFramebuffers";return false;}
#ifdef __EMSCRIPTEN__
    GenTextures=&::glGenTextures;
#else
    GenTextures=reinterpret_cast<PFNGLGENTEXTURESPROC>(SDL_GL_GetProcAddress("glGenTextures"));
#endif
    if(!GenTextures){error="Missing GLES 3 entry point: glGenTextures";return false;}
#ifdef __EMSCRIPTEN__
    GenVertexArrays=&::glGenVertexArrays;
#else
    GenVertexArrays=reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(SDL_GL_GetProcAddress("glGenVertexArrays"));
#endif
    if(!GenVertexArrays){error="Missing GLES 3 entry point: glGenVertexArrays";return false;}
#ifdef __EMSCRIPTEN__
    GetError=&::glGetError;
#else
    GetError=reinterpret_cast<PFNGLGETERRORPROC>(SDL_GL_GetProcAddress("glGetError"));
#endif
    if(!GetError){error="Missing GLES 3 entry point: glGetError";return false;}
#ifdef __EMSCRIPTEN__
    GetProgramInfoLog=&::glGetProgramInfoLog;
#else
    GetProgramInfoLog=reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(SDL_GL_GetProcAddress("glGetProgramInfoLog"));
#endif
    if(!GetProgramInfoLog){error="Missing GLES 3 entry point: glGetProgramInfoLog";return false;}
#ifdef __EMSCRIPTEN__
    GetProgramiv=&::glGetProgramiv;
#else
    GetProgramiv=reinterpret_cast<PFNGLGETPROGRAMIVPROC>(SDL_GL_GetProcAddress("glGetProgramiv"));
#endif
    if(!GetProgramiv){error="Missing GLES 3 entry point: glGetProgramiv";return false;}
#ifdef __EMSCRIPTEN__
    GetShaderInfoLog=&::glGetShaderInfoLog;
#else
    GetShaderInfoLog=reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(SDL_GL_GetProcAddress("glGetShaderInfoLog"));
#endif
    if(!GetShaderInfoLog){error="Missing GLES 3 entry point: glGetShaderInfoLog";return false;}
#ifdef __EMSCRIPTEN__
    GetShaderiv=&::glGetShaderiv;
#else
    GetShaderiv=reinterpret_cast<PFNGLGETSHADERIVPROC>(SDL_GL_GetProcAddress("glGetShaderiv"));
#endif
    if(!GetShaderiv){error="Missing GLES 3 entry point: glGetShaderiv";return false;}
#ifdef __EMSCRIPTEN__
    GetString=&::glGetString;
#else
    GetString=reinterpret_cast<PFNGLGETSTRINGPROC>(SDL_GL_GetProcAddress("glGetString"));
#endif
    if(!GetString){error="Missing GLES 3 entry point: glGetString";return false;}
#ifdef __EMSCRIPTEN__
    GetUniformLocation=&::glGetUniformLocation;
#else
    GetUniformLocation=reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(SDL_GL_GetProcAddress("glGetUniformLocation"));
#endif
    if(!GetUniformLocation){error="Missing GLES 3 entry point: glGetUniformLocation";return false;}
#ifdef __EMSCRIPTEN__
    LineWidth=&::glLineWidth;
#else
    LineWidth=reinterpret_cast<PFNGLLINEWIDTHPROC>(SDL_GL_GetProcAddress("glLineWidth"));
#endif
    if(!LineWidth){error="Missing GLES 3 entry point: glLineWidth";return false;}
#ifdef __EMSCRIPTEN__
    LinkProgram=&::glLinkProgram;
#else
    LinkProgram=reinterpret_cast<PFNGLLINKPROGRAMPROC>(SDL_GL_GetProcAddress("glLinkProgram"));
#endif
    if(!LinkProgram){error="Missing GLES 3 entry point: glLinkProgram";return false;}
#ifdef __EMSCRIPTEN__
    PixelStorei=&::glPixelStorei;
#else
    PixelStorei=reinterpret_cast<PFNGLPIXELSTOREIPROC>(SDL_GL_GetProcAddress("glPixelStorei"));
#endif
    if(!PixelStorei){error="Missing GLES 3 entry point: glPixelStorei";return false;}
#ifdef __EMSCRIPTEN__
    ReadPixels=&::glReadPixels;
#else
    ReadPixels=reinterpret_cast<PFNGLREADPIXELSPROC>(SDL_GL_GetProcAddress("glReadPixels"));
#endif
    if(!ReadPixels){error="Missing GLES 3 entry point: glReadPixels";return false;}
#ifdef __EMSCRIPTEN__
    Scissor=&::glScissor;
#else
    Scissor=reinterpret_cast<PFNGLSCISSORPROC>(SDL_GL_GetProcAddress("glScissor"));
#endif
    if(!Scissor){error="Missing GLES 3 entry point: glScissor";return false;}
#ifdef __EMSCRIPTEN__
    ShaderSource=&::glShaderSource;
#else
    ShaderSource=reinterpret_cast<PFNGLSHADERSOURCEPROC>(SDL_GL_GetProcAddress("glShaderSource"));
#endif
    if(!ShaderSource){error="Missing GLES 3 entry point: glShaderSource";return false;}
#ifdef __EMSCRIPTEN__
    TexImage2D=&::glTexImage2D;
#else
    TexImage2D=reinterpret_cast<PFNGLTEXIMAGE2DPROC>(SDL_GL_GetProcAddress("glTexImage2D"));
#endif
    if(!TexImage2D){error="Missing GLES 3 entry point: glTexImage2D";return false;}
#ifdef __EMSCRIPTEN__
    TexParameteri=&::glTexParameteri;
#else
    TexParameteri=reinterpret_cast<PFNGLTEXPARAMETERIPROC>(SDL_GL_GetProcAddress("glTexParameteri"));
#endif
    if(!TexParameteri){error="Missing GLES 3 entry point: glTexParameteri";return false;}
#ifdef __EMSCRIPTEN__
    TexStorage2D=&::glTexStorage2D;
#else
    TexStorage2D=reinterpret_cast<PFNGLTEXSTORAGE2DPROC>(SDL_GL_GetProcAddress("glTexStorage2D"));
#endif
    if(!TexStorage2D){error="Missing GLES 3 entry point: glTexStorage2D";return false;}
#ifdef __EMSCRIPTEN__
    Uniform1i=&::glUniform1i;
#else
    Uniform1i=reinterpret_cast<PFNGLUNIFORM1IPROC>(SDL_GL_GetProcAddress("glUniform1i"));
#endif
    if(!Uniform1i){error="Missing GLES 3 entry point: glUniform1i";return false;}
#ifdef __EMSCRIPTEN__
    Uniform3f=&::glUniform3f;
#else
    Uniform3f=reinterpret_cast<PFNGLUNIFORM3FPROC>(SDL_GL_GetProcAddress("glUniform3f"));
#endif
    if(!Uniform3f){error="Missing GLES 3 entry point: glUniform3f";return false;}
#ifdef __EMSCRIPTEN__
    UseProgram=&::glUseProgram;
#else
    UseProgram=reinterpret_cast<PFNGLUSEPROGRAMPROC>(SDL_GL_GetProcAddress("glUseProgram"));
#endif
    if(!UseProgram){error="Missing GLES 3 entry point: glUseProgram";return false;}
#ifdef __EMSCRIPTEN__
    VertexAttribPointer=&::glVertexAttribPointer;
#else
    VertexAttribPointer=reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(SDL_GL_GetProcAddress("glVertexAttribPointer"));
#endif
    if(!VertexAttribPointer){error="Missing GLES 3 entry point: glVertexAttribPointer";return false;}
#ifdef __EMSCRIPTEN__
    Viewport=&::glViewport;
#else
    Viewport=reinterpret_cast<PFNGLVIEWPORTPROC>(SDL_GL_GetProcAddress("glViewport"));
#endif
    if(!Viewport){error="Missing GLES 3 entry point: glViewport";return false;}
    return true;
}
}
