#pragma once
#include <GLES3/gl3.h>
#include <array>
#include <cstdint>
#include <cstring>

namespace touhou::sdl {
// Measure the monotonic float-to-A8 conversion used by this device. GL permits
// conversion precision which is not reproduced by float32 *255 +.5. The D3D9
// alpha test operates on byte alpha; use the same boundaries as RGBA8 output.
// This runs once per context, before the renderer establishes its state cache.
inline bool measure_alpha_boundaries(std::array<float,256>& boundaries){
    constexpr const char* vs=R"GLSL(#version 300 es
precision highp float;
void main(){vec2 p=vec2((gl_VertexID<<1)&2,gl_VertexID&2);gl_Position=vec4(p*2.0-1.0,0.0,1.0);}
)GLSL";
    constexpr const char* fs=R"GLSL(#version 300 es
precision highp float;
uniform highp sampler2D values;
out vec4 color;
void main(){color=vec4(0.0,0.0,0.0,texelFetch(values,ivec2(gl_FragCoord.xy),0).r);}
)GLSL";
    GLuint vertex=0,fragment=0,program=0,vao=0,framebuffer=0,output=0,input=0;
    bool passed=false;
    auto compile=[](GLenum kind,const char* source){const auto s=glCreateShader(kind);glShaderSource(s,1,&source,nullptr);glCompileShader(s);GLint okay=0;glGetShaderiv(s,GL_COMPILE_STATUS,&okay);if(!okay){glDeleteShader(s);return GLuint(0);}return s;};
    auto bits=[](float f){std::uint32_t n;std::memcpy(&n,&f,4);return n;};
    auto value=[](std::uint32_t n){float f;std::memcpy(&f,&n,4);return f;};
    do {
        vertex=compile(GL_VERTEX_SHADER,vs);fragment=compile(GL_FRAGMENT_SHADER,fs);if(!vertex||!fragment)break;
        program=glCreateProgram();glAttachShader(program,vertex);glAttachShader(program,fragment);glLinkProgram(program);
        GLint okay=0;glGetProgramiv(program,GL_LINK_STATUS,&okay);if(!okay)break;
        glGenVertexArrays(1,&vao);glBindVertexArray(vao);glUseProgram(program);
        glGenFramebuffers(1,&framebuffer);glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
        glGenTextures(1,&output);glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,output);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,256,1,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,output,0);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)break;
        glGenTextures(1,&input);glBindTexture(GL_TEXTURE_2D,input);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,256,1,0,GL_RED,GL_FLOAT,nullptr);
        glUniform1i(glGetUniformLocation(program,"values"),0);
        glViewport(0,0,256,1);glDisable(GL_BLEND);glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);glDisable(GL_SCISSOR_TEST);glDisable(GL_DITHER);glColorMask(true,true,true,true);
        glPixelStorei(GL_PACK_ALIGNMENT,1);glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        std::array<std::uint32_t,256> low{},high{},middle{};std::array<float,256> samples{};std::array<unsigned char,1024> pixels{};
        for(unsigned i=1;i<256;++i){low[i]=bits(float(i-1)/255);high[i]=bits(float(i)/255);}
        for(unsigned step=0;step<32;++step){
            bool pending=false;for(unsigned i=1;i<256;++i){middle[i]=low[i]+(high[i]-low[i])/2;samples[i]=value(middle[i]);pending|=high[i]-low[i]>1;}
            if(!pending)break;
            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,256,1,GL_RED,GL_FLOAT,samples.data());
            glDrawArrays(GL_TRIANGLES,0,3);glReadPixels(0,0,256,1,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
            for(unsigned i=1;i<256;++i)if(high[i]-low[i]>1){if(pixels[i*4+3]>=i)high[i]=middle[i];else low[i]=middle[i];}
        }
        // The lower and upper representable floats must straddle each byte.
        for(unsigned endpoint=0;endpoint<2;++endpoint){
            for(unsigned i=1;i<256;++i)samples[i]=value(endpoint?high[i]:low[i]);
            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,256,1,GL_RED,GL_FLOAT,samples.data());
            glDrawArrays(GL_TRIANGLES,0,3);glReadPixels(0,0,256,1,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());
            okay=1;for(unsigned i=1;i<256;++i)if(high[i]-low[i]!=1||(pixels[i*4+3]>=i)!=bool(endpoint))okay=0;
            if(!okay)break;
        }
        if(!okay)break;boundaries[0]=0;for(unsigned i=1;i<256;++i)boundaries[i]=value(high[i]);passed=true;
    } while(false);
    glBindFramebuffer(GL_FRAMEBUFFER,0);glBindTexture(GL_TEXTURE_2D,0);glBindVertexArray(0);glUseProgram(0);
    if(input)glDeleteTextures(1,&input);if(output)glDeleteTextures(1,&output);if(framebuffer)glDeleteFramebuffers(1,&framebuffer);if(vao)glDeleteVertexArrays(1,&vao);if(program)glDeleteProgram(program);if(vertex)glDeleteShader(vertex);if(fragment)glDeleteShader(fragment);
    return passed;
}
}
