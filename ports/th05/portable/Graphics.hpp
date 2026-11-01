#pragma once
#include <GL/gl.h>
#include <GL/glu.h>
// The game uses a small immediate graphics vocabulary. Translate it to GLES2
// and keep CPU texture copies for its palette and ending operations.
namespace th5w::web::graphics {
bool Initialize();
void BeginFrame();void Present();void Flush();
void Clear(GLbitfield);void Scissor(GLint,GLint,GLsizei,GLsizei);
void Begin(GLenum);void End();void Vertex(float,float,float=0);void TexCoord(float,float);
void Color(float,float,float,float=1);void MatrixMode(GLenum);void PushMatrix();void PopMatrix();
void Identity();void Ortho(double,double,double,double,double,double);
void Translate(float,float,float);void Scale(float,float,float);
void Enable(GLenum);void Disable(GLenum);void BindTexture(GLenum,GLuint);
void TexImage(GLenum,GLint,GLint,GLsizei,GLsizei,GLint,GLenum,GLenum,const void*);
void TexSubImage(GLenum,GLint,GLint,GLint,GLsizei,GLsizei,GLenum,GLenum,const void*);
void TexParameter(GLenum,GLenum,GLint);void TexLevel(GLenum,GLint,GLenum,GLint*);
void GetTexImage(GLenum,GLint,GLenum,GLenum,void*);void DeleteTextures(GLsizei,const GLuint*);
void PolygonMode(GLenum,GLenum);void Stipple(const GLubyte*);void PushAttrib(GLbitfield);void PopAttrib();
void TexEnv(GLenum,GLenum,GLfloat);void Blend(GLenum,GLenum);void ColorMask(GLboolean,GLboolean,GLboolean,GLboolean);
}
#ifndef TH05_GRAPHICS_IMPLEMENTATION
#define glClear th5w::web::graphics::Clear
#define glScissor th5w::web::graphics::Scissor
#define glBegin th5w::web::graphics::Begin
#define glEnd th5w::web::graphics::End
#define glVertex2f(x,y) th5w::web::graphics::Vertex(x,y)
#define glVertex3f th5w::web::graphics::Vertex
#define glTexCoord2f th5w::web::graphics::TexCoord
#define glTexCoord2d th5w::web::graphics::TexCoord
#define glColor3f(r,g,b) th5w::web::graphics::Color(r,g,b)
#define glColor4f th5w::web::graphics::Color
#define glMatrixMode th5w::web::graphics::MatrixMode
#define glPushMatrix th5w::web::graphics::PushMatrix
#define glPopMatrix th5w::web::graphics::PopMatrix
#define glLoadIdentity th5w::web::graphics::Identity
#define glOrtho th5w::web::graphics::Ortho
#define gluOrtho2D(l,r,b,t) th5w::web::graphics::Ortho(l,r,b,t,-1,1)
#define glTranslatef th5w::web::graphics::Translate
#define glScalef th5w::web::graphics::Scale
#define glEnable th5w::web::graphics::Enable
#define glDisable th5w::web::graphics::Disable
#define glBindTexture th5w::web::graphics::BindTexture
#define glTexImage2D th5w::web::graphics::TexImage
#define glTexSubImage2D th5w::web::graphics::TexSubImage
#define glTexParameteri th5w::web::graphics::TexParameter
#define glGetTexLevelParameteriv th5w::web::graphics::TexLevel
#define glGetTexImage th5w::web::graphics::GetTexImage
#define glDeleteTextures th5w::web::graphics::DeleteTextures
#define glPolygonMode th5w::web::graphics::PolygonMode
#define glPolygonStipple th5w::web::graphics::Stipple
#define glPushAttrib th5w::web::graphics::PushAttrib
#define glPopAttrib th5w::web::graphics::PopAttrib
#define glTexEnvf th5w::web::graphics::TexEnv
#define glBlendFunc th5w::web::graphics::Blend
#define glColorMask th5w::web::graphics::ColorMask
#endif
