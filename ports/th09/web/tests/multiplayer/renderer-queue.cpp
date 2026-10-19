// Synthetic GLES resource/queue cases, compared with frozen authored code.
#include "Renderer.hpp"
#include <array>
#include <cstring>
#include <memory>
#include <vector>
using namespace touhou::sdl;
namespace {
struct Image {u32 width=32,height=32,revision=1;std::vector<u8> pixels;};
std::array<Image,6> images;
std::unique_ptr<Renderer> renderer;
constexpr std::array<float,16> identity{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
struct Screen {float x,y,z,w;u32 color;float u,v;};
struct Flat {float x,y,z,w;u32 color;};
struct World {float x,y,z,u,v;};
struct Colored {float x,y,z;u32 color;float u,v;};
Surface resolve_image(void*,u32 id){auto& i=images[id];return {id,i.width,i.height,id==2?PixelFormat::Depth16:PixelFormat::Bgra8,i.width*4,i.pixels.data(),u32(i.pixels.size()),i.revision};}
void fill(u32 id,u32 rgba){auto& i=images[id];i.pixels.resize(i.width*i.height*4);for(u32 y=0;y<i.height;++y)for(u32 x=0;x<i.width;++x){const u32 pixel=rgba^(((x/4+y/4)&1)?0x00202020:0);std::memcpy(i.pixels.data()+(y*i.width+x)*4,&pixel,4);}++i.revision;}
void screen_state(u32 texture){auto& s=renderer->state;s.layout=attributes(VertexLayout::ScreenColorUv);s.texture=texture;s.pipeline=PipelineState{};s.pipeline.blend=true;s.pipeline.sourceBlend=BlendFactor::SourceAlpha;s.pipeline.destinationBlend=BlendFactor::InverseSourceAlpha;s.pipeline.depthWrite=false;s.pipeline.color.second=s.pipeline.alpha.second={ArgumentSource::Diffuse};s.pipeline.alpha.operation=ColorOperation::Multiply;}
void screen(float x,float y,u32 color,u32 texture=3){screen_state(texture);const Screen v[]={{x,y,.5f,1,color,0,0},{x+17,y,.5f,1,color,1,0},{x,y+19,.5f,1,color,0,1},{x+17,y+19,.5f,1,color,1,1}};renderer->draw(Topology::Strip,2,v,sizeof(Screen));}
void world(u32 i,u32 group=0){auto& s=renderer->state;s.layout=attributes(VertexLayout::WorldUv);s.texture=3;s.pipeline=PipelineState{};s.pipeline.depthWrite=false;s.pipeline.color.second=s.pipeline.alpha.second={ArgumentSource::Factor};s.pipeline.alpha.operation=ColorOperation::Multiply;s.pipeline.textureFactor=0xff80c0f0u^(group*0x00110d07u);s.pipeline.textureTransform=true;for(auto& m:s.matrix)m=identity;s.matrix[0][12]=-.95f+float(i%32)*.007f+float(group%5)*.38f;s.matrix[0][13]=-.9f+float((i/32)%32)*.009f+float(group/5)*.65f;s.matrix[3][8]=float(i%3)*.07f;s.matrix[3][9]=float(i%5)*.04f;const World v[]={{-.003f,-.004f,.4f,0,0},{.003f,-.004f,.4f,1,0},{-.003f,.004f,.4f,0,1},{.003f,.004f,.4f,1,1}};renderer->draw(Topology::Strip,2,v,sizeof(World));}
void flat(float x,float y){screen_state(0);renderer->state.layout=attributes(VertexLayout::ScreenColor);const Flat v[]={{x,y,.5f,1,0x8080ff20},{x+31,y+3,.5f,1,0xc0ff2060},{x+13,y+27,.5f,1,0xa02080ff}};renderer->draw(Topology::Triangles,1,v,sizeof(Flat));}
void colored(){screen_state(3);auto& s=renderer->state;s.layout=attributes(VertexLayout::WorldColorUv);for(auto& m:s.matrix)m=identity;const Colored v[]={{.1f,.1f,.4f,0x9080ff20,0,0},{.3f,.1f,.4f,0x90ff2080,1,0},{.1f,.3f,.4f,0x902080ff,0,1}};renderer->draw(Topology::Triangles,1,v,sizeof(Colored));}
void marker_triangle(std::vector<Screen>& v,float x,float y,u32 color){for(auto& p:v)p={-999,-999,.5f,1,0xff000000,0,0};v[v.size()-3]={x,y,.5f,1,color,0,0};v[v.size()-2]={x+11,y,.5f,1,color,1,0};v[v.size()-1]={x,y+13,.5f,1,color,0,1};}
}
extern "C" {
__attribute__((export_name("queue_test_open"))) u32 queue_test_open(){
 images[1].width=images[2].width=640;images[1].height=images[2].height=480;fill(1,0xff000000);fill(3,0xffe0c0a0);fill(4,0xff3060c0);fill(5,0xff80c040);
 renderer=std::make_unique<Renderer>(8,resolve_image,nullptr);if(!renderer->initialize())return 0;renderer->state.target=1;renderer->state.depth=2;renderer->prepare(3);renderer->prepare(4);return 1;
}
__attribute__((export_name("queue_test_case"))) u32 queue_test_case(u32 mode){
 auto& r=*renderer;r.state.target=1;r.state.depth=2;r.viewport({0,0,640,480,0,1});r.clear(3,0xff182030,1,0);
 if(mode==0){
  // New/reused VAOs and non-power-of-two stride alignment in one flush.
  screen(30,30,0xc0ff6020);world(0);world(1);flat(37,39);colored();screen(45,43,0x9040ff80);world(2);world(3);flat(51,49);colored();r.flush();
  // Resource preparation changes texture/FBO bindings while draws are queued.
  screen(100,40,0xffffffff,4);r.prepare(5);screen(123,40,0xffffffff,5);
  r.flush();fill(4,0xffc04020);screen(147,40,0xffffffff,4);
  // Indexed and line-strip paths drain preceding queued vertices first.
  screen_state(3);const Screen v[]={{30,110,.5f,1,0xc0ff9040,0,0},{61,110,.5f,1,0xc0ff9040,1,0},{30,141,.5f,1,0xc0ff9040,0,1},{61,141,.5f,1,0xc0ff9040,1,1},{0,0,0,1,0,0,0},{0,0,0,1,0,0,0}};const unsigned short indices[]{0,1,2,2,1,3};r.draw(Topology::Triangles,2,v,sizeof(Screen),indices);
  flat(43,121);const Flat line[]={{20,145,.5f,1,0xff80ff40},{99,100,.5f,1,0xff80ff40}};screen_state(0);r.state.layout=attributes(VertexLayout::ScreenColor);r.draw(Topology::Lines,1,line,sizeof(Flat));
  r.draw(Topology::Lines,1,line,sizeof(Flat));const Flat strip[]={{30,160,.5f,1,0xff2080ff},{90,160,.5f,1,0xff2080ff},{90,180,.5f,1,0xff2080ff}};r.draw(Topology::LineStrip,2,strip,sizeof(Flat));
  screen(177,40,0xffffffff,5);r.release(5);screen(201,40,0xffffffff,3);
  const i32 rect[]{103,46,111,54};r.clear(1,0xff502010,1,0,rect,1);screen(211,51,0xc0a0ff40);
  r.read(1);screen(240,40,0xffffffff);r.present(1);
 }else if(mode==1){
  // Nine 1024-instance runs cross the instance arena capacity. Each group
  // has a distinct screen region, so a later group cannot hide a lost run.
  for(u32 group=0;group<9;++group)for(u32 i=0;i<1024;++i)world(i,group);
  screen(580,400,0xffffffff);r.present(1);
 }else if(mode==2){
  // Command count, vertex arena, and an individual oversized draw.
  for(u32 i=0;i<1500;++i)screen(float((i%75)*8),float((i/75)*8),0xffc08040,i%2?3:4);
  std::vector<Screen> chunk(1536);for(u32 i=0;i<40;++i){marker_triangle(chunk,float((i%20)*29+5),float(230+i/20*35),0xff80ff40);screen_state(i%2?3:4);r.draw_batch(chunk.size()/3,chunk.data(),sizeof(Screen));}
  std::vector<Screen> oversized(60000);marker_triangle(oversized,570,390,0xffff8030);screen_state(3);r.draw_batch(oversized.size()/3,oversized.data(),sizeof(Screen));std::memset(oversized.data(),0,oversized.size()*sizeof(Screen));
  flat(555,381);r.present(1);
 }else if(mode==3){
  screen(20,20,0xffffffff);r.flush();screen(100,100,0xffff0000);r.discard();screen(200,200,0xff00ff00);r.present(1);
 }else if(mode==4){
  // Alternating shader branches must not mark an unused uniform as uploaded.
  const auto quads=[&](float x,u32 factor,u32 count,bool fog){auto& s=r.state;s.layout=attributes(VertexLayout::WorldUv);s.texture=3;s.pipeline=PipelineState{};s.pipeline.depthWrite=false;s.pipeline.fog=fog;s.pipeline.fogNear=.1f;s.pipeline.fogFar=1.7f;s.pipeline.fogColor=0xff40a020;s.pipeline.textureTransform=true;s.pipeline.color.second=s.pipeline.alpha.second={ArgumentSource::Factor};s.pipeline.alpha.operation=ColorOperation::Multiply;
   for(auto& m:s.matrix)m=identity;s.matrix[1][12]=.13f;s.matrix[2][0]=.83f;s.matrix[2][5]=1.2f;s.matrix[3][0]=.78f;s.matrix[3][8]=.11f;
   const World v[]={{-.07f,-.08f,.4f,0,0},{.07f,-.08f,.4f,1,0},{-.07f,.08f,.4f,0,1},{.07f,.08f,.4f,1,1}};
   for(u32 n=0;n<count;++n){s.matrix[0][12]=x+float(n)*.19f;s.matrix[0][13]=.35f;s.matrix[3][9]=float(n)*.09f;s.pipeline.textureFactor=factor^(n*0x00071713u);r.draw(Topology::Strip,2,v,sizeof(World));}r.flush();};
  quads(-.9f,0xffd09040,1,true);quads(-.55f,0xff40b0d0,2,true);quads(-.36f,0xff47a7c3,1,true);
  screen(301,90,0x90ff5020);r.state.matrix[0][0]=7;r.state.matrix[1][12]=3;r.state.matrix[2][5]=-4;r.state.matrix[3][0]=.23f;screen(309,99,0x9020ff50);r.flush();
  quads(.1f,0xffd09040,2,false);quads(.29f,0xffd78753,1,false);
  // Resource utilities borrow texture unit 1 and must return ownership of 0.
  GLint unit=0;glGetIntegerv(GL_ACTIVE_TEXTURE,&unit);if(unit!=GL_TEXTURE0)return 0;
  const i32 from[]{0,0,32,32},to[]{0,0,32,32};if(!r.resample(3,from,5,to,nullptr,32,32))return 0;
  glGetIntegerv(GL_ACTIVE_TEXTURE,&unit);if(unit!=GL_TEXTURE0)return 0;
  screen(401,90,0xffffffff,5);quads(.55f,0xff40d080,2,true);
  // Concatenated independent lines keep blend order and never join owners.
  screen_state(0);r.state.layout=attributes(VertexLayout::ScreenColor);for(u32 n=0;n<20;++n){const float x=80+float(n)*23;const Flat v[]={{x,310,.5f,1,0x8080ff20},{x+16,337,.5f,1,0x8080ff20},{x,337,.5f,1,0x8020a0ff},{x+16,310,.5f,1,0x8020a0ff}};r.draw(Topology::Lines,2,v,sizeof(Flat));}r.present(1);
 }else return 0;
 return glGetError()==GL_NO_ERROR;
}
}
