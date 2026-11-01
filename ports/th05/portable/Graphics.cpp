#define TH05_GRAPHICS_IMPLEMENTATION
#include "Graphics.hpp"
#include <GLES2/gl2.h>
#include <array>
#include <vector>
#include <map>
#include <cstring>
#include <stdexcept>
namespace th5w::web::graphics {
using Matrix=std::array<float,16>;
Matrix Unit(){Matrix m{};m[0]=m[5]=m[10]=m[15]=1;return m;}
Matrix Multiply(const Matrix& a,const Matrix& b){Matrix r{};for(int c=0;c<4;c++)for(int row=0;row<4;row++)for(int k=0;k<4;k++)r[c*4+row]+=a[k*4+row]*b[c*4+k];return r;}
struct VertexData{float x,y,z,u,v,r,g,b,a;};
struct Texture{int width=0,height=0;std::vector<unsigned char> rgba;};
struct State{bool textured=false,stipple=false,blend=false,depth=false,scissor=false;GLenum src=GL_ONE,dst=GL_ZERO,env=GL_MODULATE,polygon=GL_FILL;GLuint texture=0;std::array<GLboolean,4> mask{1,1,1,1};};
static State state;static std::vector<State> states;
static Matrix projection=Unit(),model=Unit();static std::vector<Matrix> ps,ms;
static GLenum matrixMode=GL_MODELVIEW,primitive;static std::array<float,4> color{1,1,1,1};static float u=0,v=0;
static GLuint program=0,buffer=0,white=0,maskTexture=0;
static GLuint gameFramebuffer=0,gameTexture=0,gameDepth=0;
static GLint transformUniform,textureUniform,stippleUniform,envUniform;
static std::map<GLuint,Texture> textures;static std::vector<VertexData> vertices,triangles;
static Matrix& Current(){return matrixMode==GL_PROJECTION?projection:model;}
GLuint Shader(GLenum kind,const char* source){GLuint id=glCreateShader(kind);glShaderSource(id,1,&source,nullptr);glCompileShader(id);GLint ok=0;glGetShaderiv(id,GL_COMPILE_STATUS,&ok);if(!ok){char error[2048];glGetShaderInfoLog(id,sizeof(error),nullptr,error);throw std::runtime_error(error);}return id;}
bool Initialize(){
 const char* vs="attribute vec3 position;attribute vec2 texcoord;attribute vec4 color;uniform mat4 transform;varying vec2 uv;varying vec4 tint;void main(){gl_Position=transform*vec4(position,1.);uv=texcoord;tint=color;}";
 const char* fs="precision mediump float;uniform sampler2D image;uniform sampler2D maskImage;uniform bool useStipple;uniform bool replaceTexture;varying vec2 uv;varying vec4 tint;void main(){if(useStipple&&texture2D(maskImage,mod(floor(gl_FragCoord.xy),32.)/32.).a<.5)discard;vec4 pixel=texture2D(image,uv);gl_FragColor=replaceTexture?pixel:pixel*tint;}";
 GLuint a=Shader(GL_VERTEX_SHADER,vs),b=Shader(GL_FRAGMENT_SHADER,fs);program=glCreateProgram();glAttachShader(program,a);glAttachShader(program,b);
 glBindAttribLocation(program,0,"position");glBindAttribLocation(program,1,"texcoord");glBindAttribLocation(program,2,"color");glLinkProgram(program);glDeleteShader(a);glDeleteShader(b);GLint ok;glGetProgramiv(program,GL_LINK_STATUS,&ok);if(!ok)return false;
 transformUniform=glGetUniformLocation(program,"transform");textureUniform=glGetUniformLocation(program,"image");stippleUniform=glGetUniformLocation(program,"useStipple");envUniform=glGetUniformLocation(program,"replaceTexture");
 glGenBuffers(1,&buffer);glGenTextures(1,&white);glBindTexture(GL_TEXTURE_2D,white);const unsigned char pixel[]={255,255,255,255};glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,pixel);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
 glGenTextures(1,&maskTexture);glBindTexture(GL_TEXTURE_2D,maskTexture);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);glPixelStorei(GL_UNPACK_ALIGNMENT,1);
 glGenTextures(1,&gameTexture);glBindTexture(GL_TEXTURE_2D,gameTexture);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,640,480,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
 glGenFramebuffers(1,&gameFramebuffer);glBindFramebuffer(GL_FRAMEBUFFER,gameFramebuffer);glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,gameTexture,0);glGenRenderbuffers(1,&gameDepth);glBindRenderbuffer(GL_RENDERBUFFER,gameDepth);glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,640,480);glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,gameDepth);return glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE;
}
void BeginFrame(){glBindFramebuffer(GL_FRAMEBUFFER,gameFramebuffer);glViewport(0,0,640,480);}
void Present(){const auto oldProjection=projection,oldModel=model;const auto oldMode=matrixMode;PushAttrib(GL_ALL_ATTRIB_BITS);glBindFramebuffer(GL_FRAMEBUFFER,0);glViewport(0,0,640,480);Disable(GL_BLEND);Disable(GL_SCISSOR_TEST);Disable(GL_DEPTH_TEST);Disable(GL_POLYGON_STIPPLE);Enable(GL_TEXTURE_2D);BindTexture(GL_TEXTURE_2D,gameTexture);TexEnv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);PolygonMode(GL_FRONT_AND_BACK,GL_FILL);ColorMask(1,1,1,1);MatrixMode(GL_PROJECTION);Identity();Ortho(0,640,0,480,-1,1);MatrixMode(GL_MODELVIEW);Identity();Color(1,1,1,1);Begin(GL_QUADS);TexCoord(0,0);Vertex(0,0);TexCoord(1,0);Vertex(640,0);TexCoord(1,1);Vertex(640,480);TexCoord(0,1);Vertex(0,480);End();projection=oldProjection;model=oldModel;matrixMode=oldMode;PopAttrib();}
void Begin(GLenum value){primitive=value;vertices.clear();}
void Vertex(float x,float y,float z){vertices.push_back({x,y,z,u,v,color[0],color[1],color[2],color[3]});}
void TexCoord(float x,float y){u=x;v=y;}
void Color(float r,float g,float b,float a){color={r,g,b,a};}
void End(){
 if(vertices.empty())return;triangles.clear();GLenum mode=primitive;
 auto triangle=[&](int a,int b,int c){triangles.push_back(vertices[a]);triangles.push_back(vertices[b]);triangles.push_back(vertices[c]);};
 if(primitive==GL_QUADS){mode=GL_TRIANGLES;for(size_t i=0;i+3<vertices.size();i+=4){if(state.polygon==GL_LINE){for(int j=0;j<4;j++){triangles.push_back(vertices[i+j]);triangles.push_back(vertices[i+(j+1)%4]);}mode=GL_LINES;}else{triangle(i,i+1,i+2);triangle(i,i+2,i+3);}}}
 else if(primitive==GL_POLYGON){mode=state.polygon==GL_LINE?GL_LINE_LOOP:GL_TRIANGLES;if(mode==GL_TRIANGLES)for(size_t i=1;i+1<vertices.size();i++)triangle(0,i,i+1);else triangles=vertices;}
 else triangles=vertices;
 glUseProgram(program);glBindBuffer(GL_ARRAY_BUFFER,buffer);glBufferData(GL_ARRAY_BUFFER,triangles.size()*sizeof(VertexData),triangles.data(),GL_STREAM_DRAW);
 for(int n=0;n<3;n++)glEnableVertexAttribArray(n);glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexData),nullptr);glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(VertexData),(void*)(3*sizeof(float)));glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,sizeof(VertexData),(void*)(5*sizeof(float)));
 Matrix transform=Multiply(projection,model);glUniformMatrix4fv(transformUniform,1,GL_FALSE,transform.data());glUniform1i(textureUniform,0);glUniform1i(glGetUniformLocation(program,"maskImage"),1);glUniform1i(stippleUniform,state.stipple);glUniform1i(envUniform,state.textured&&state.env==GL_REPLACE);
 glActiveTexture(GL_TEXTURE1);glBindTexture(GL_TEXTURE_2D,maskTexture);glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,state.textured&&state.texture?state.texture:white);
 glDrawArrays(mode,0,triangles.size());
 // Restore the texture binding used by uploads between draws.
 glBindTexture(GL_TEXTURE_2D,state.texture);
}
void MatrixMode(GLenum value){matrixMode=value;}
void PushMatrix(){(matrixMode==GL_PROJECTION?ps:ms).push_back(Current());}
void PopMatrix(){auto& s=matrixMode==GL_PROJECTION?ps:ms;if(s.empty())throw std::runtime_error("Graphics matrix stack underflow");Current()=s.back();s.pop_back();}
void Identity(){Current()=Unit();}
void Ortho(double l,double r,double b,double t,double n,double f){Matrix m=Unit();m[0]=2/(r-l);m[5]=2/(t-b);m[10]=-2/(f-n);m[12]=-(r+l)/(r-l);m[13]=-(t+b)/(t-b);m[14]=-(f+n)/(f-n);Current()=Multiply(Current(),m);}
void Translate(float x,float y,float z){Matrix m=Unit();m[12]=x;m[13]=y;m[14]=z;Current()=Multiply(Current(),m);}
void Scale(float x,float y,float z){Matrix m=Unit();m[0]=x;m[5]=y;m[10]=z;Current()=Multiply(Current(),m);}
void Enable(GLenum value){if(value==GL_TEXTURE_2D)state.textured=true;else if(value==GL_POLYGON_STIPPLE)state.stipple=true;else{if(value==GL_BLEND)state.blend=true;if(value==GL_DEPTH_TEST)state.depth=true;if(value==GL_SCISSOR_TEST)state.scissor=true;glEnable(value);}}
void Disable(GLenum value){if(value==GL_TEXTURE_2D)state.textured=false;else if(value==GL_POLYGON_STIPPLE)state.stipple=false;else{if(value==GL_BLEND)state.blend=false;if(value==GL_DEPTH_TEST)state.depth=false;if(value==GL_SCISSOR_TEST)state.scissor=false;glDisable(value);}}
void BindTexture(GLenum target,GLuint id){state.texture=id;glBindTexture(target,id);}
void TexImage(GLenum target,GLint level,GLint,GLsizei w,GLsizei h,GLint border,GLenum format,GLenum type,const void* data){
 Texture& t=textures[state.texture];t.width=w;t.height=h;t.rgba.assign(w*h*4,0);const auto* p=static_cast<const unsigned char*>(data);if(data&&type!=GL_UNSIGNED_BYTE)throw std::runtime_error("Unsupported texture pixel type");
 if(p)for(int i=0;i<w*h;i++){auto* d=t.rgba.data()+i*4;if(format==GL_RGBA)memcpy(d,p+i*4,4);else if(format==GL_RGB){memcpy(d,p+i*3,3);d[3]=255;}else if(format==GL_LUMINANCE_ALPHA){d[0]=d[1]=d[2]=p[i*2];d[3]=p[i*2+1];}else if(format==GL_ALPHA){d[0]=d[1]=d[2]=255;d[3]=p[i];}else d[0]=d[1]=d[2]=d[3]=p[i];}
 glTexImage2D(target,level,GL_RGBA,w,h,border,GL_RGBA,GL_UNSIGNED_BYTE,t.rgba.data());
}
void TexSubImage(GLenum target,GLint level,GLint x,GLint y,GLsizei w,GLsizei h,GLenum format,GLenum type,const void* data){auto& t=textures[state.texture];if(format!=GL_RGBA||type!=GL_UNSIGNED_BYTE)throw std::runtime_error("Unsupported texture update");if(x<0||y<0||x+w>t.width||y+h>t.height)throw std::runtime_error("Texture update bounds");for(int row=0;row<h;row++)memcpy(t.rgba.data()+((y+row)*t.width+x)*4,static_cast<const unsigned char*>(data)+row*w*4,w*4);glTexSubImage2D(target,level,x,y,w,h,format,type,data);}
void TexParameter(GLenum t,GLenum p,GLint v){glTexParameteri(t,p,v==GL_CLAMP?GL_CLAMP_TO_EDGE:v);}
void TexLevel(GLenum,GLint,GLenum p,GLint* out){const auto& t=textures[state.texture];*out=p==GL_TEXTURE_WIDTH?t.width:t.height;}
void GetTexImage(GLenum,GLint,GLenum format,GLenum type,void* out){if(format!=GL_RGBA||type!=GL_UNSIGNED_BYTE)throw std::runtime_error("Unsupported texture read");const auto& t=textures[state.texture];memcpy(out,t.rgba.data(),t.rgba.size());}
void DeleteTextures(GLsizei n,const GLuint* ids){for(int i=0;i<n;i++)textures.erase(ids[i]);glDeleteTextures(n,ids);}
void PolygonMode(GLenum,GLenum mode){state.polygon=mode;}
void Stipple(const GLubyte* bits){std::array<unsigned char,32*32*4> data{};for(int y=0;y<32;y++)for(int x=0;x<32;x++)data[(y*32+x)*4+3]=(bits[y*4+x/8]&(128>>(x%8)))?255:0;glActiveTexture(GL_TEXTURE1);glBindTexture(GL_TEXTURE_2D,maskTexture);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,data.data());glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,state.texture);}
void PushAttrib(GLbitfield){states.push_back(state);}
void PopAttrib(){if(states.empty())throw std::runtime_error("Graphics state stack underflow");state=states.back();states.pop_back();BindTexture(GL_TEXTURE_2D,state.texture);state.blend?glEnable(GL_BLEND):glDisable(GL_BLEND);state.depth?glEnable(GL_DEPTH_TEST):glDisable(GL_DEPTH_TEST);state.scissor?glEnable(GL_SCISSOR_TEST):glDisable(GL_SCISSOR_TEST);glBlendFunc(state.src,state.dst);glColorMask(state.mask[0],state.mask[1],state.mask[2],state.mask[3]);}
void TexEnv(GLenum,GLenum,GLfloat value){state.env=GLenum(value);}
void Blend(GLenum s,GLenum d){state.src=s;state.dst=d;glBlendFunc(s,d);}
void ColorMask(GLboolean a,GLboolean b,GLboolean c,GLboolean d){state.mask={a,b,c,d};glColorMask(a,b,c,d);}
}
