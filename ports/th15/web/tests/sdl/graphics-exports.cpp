// Developer-only rasterization fixture. Not a playable-game entry point.
#include "../../cpp/sdl/GraphicsDevice.hpp"
#include "../../cpp/sdl/FontDevice.hpp"
#include "../../cpp/game/AnmGeometry.hpp"
#include "../../cpp/game/ScreenTargets.hpp"
#include "../../cpp/game/AsciiFrame.hpp"
#include "../../cpp/sdl/SceneCaptureDevice.hpp"
#include <memory>
#include <cmath>
#include <emscripten.h>
using namespace th15;using namespace touhou::graphics;
namespace {std::unique_ptr<sdl::GraphicsDevice> graphics;std::vector<std::unique_ptr<AnmResource>> files;}
namespace {
struct ScreenProbe {Rng random;AnmEnvironment environment;AnmManager animations{random,environment};AnmRenderer renderer{*graphics};ScreenViews views{renderer,environment};FrameScheduler scheduler;ScreenCompositor compositor{scheduler,animations,environment,renderer,*graphics,views};ScreenTargets targets{animations,environment,views,compositor,0};sdl::SceneCaptureDevice capture_device{*graphics,renderer};SceneCapture screenshots{animations,environment,capture_device,0};u32 snapshot=0;std::string error;
 bool initialize(const u8* data,u32 size){environment.screen_offsets={320,16,320,16};if(!animations.load(0,data,size)){error=animations.error;return false;}if(!graphics->preload(*animations.resource(0))){error=graphics->error;return false;}if(!targets.prepare()){error=targets.error;return false;}screenshots.pause_source=compositor.alternate;return true;}
 ~ScreenProbe(){renderer.flush();if(auto* file=animations.resource(0))graphics->unload(*file);}
};std::unique_ptr<ScreenProbe> screen_probe;
std::unique_ptr<AnmDrawSchedule> chapter_schedule;
std::unique_ptr<AsciiText> chapter_text;
std::unique_ptr<AsciiFrame> chapter_ascii_frame;
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int graphics_initialize_scaled(u32 scale){graphics=std::make_unique<sdl::GraphicsDevice>();graphics->render_scale=scale==2?2:1;return graphics->initialize();}
void graphics_depth_quad(float,float,float,float,float,u32);
EMSCRIPTEN_KEEPALIVE int graphics_chapter_initialize(const u8* data,u32 size){
 if(!screen_probe)return 0;auto& s=*screen_probe;
 if(!s.animations.load(1,data,size)||!graphics->preload(*s.animations.resource(1)))return 0;
 chapter_schedule=std::make_unique<AnmDrawSchedule>(s.scheduler,s.animations,s.renderer,*graphics,s.views);
 return s.animations.create(1,241,-1,0)!=0;
}
EMSCRIPTEN_KEEPALIVE int graphics_chapter_frame(u32 age){
 if(!screen_probe||!chapter_schedule)return 0;auto& s=*screen_probe;
 if(age==290){for(u32 i=0;i<s.animations.registry.ordered_count(false);++i){auto id=s.animations.registry.ordered_handle(false,i);auto* vm=s.animations.registry.find(id);if(vm&&vm->source_script==241&&!s.animations.interrupt(id,1))return 0;}}
 if(!s.animations.update(false)||!s.animations.update(true)){s.error=s.animations.error;return 0;}
 if(chapter_text){chapter_text->update();for(float y:{226.f,246.f,266.f,286.f}){HudTextDraw text;text.position={300,y,0};text.text="223.36%";text.style.font=2;text.style.coordinate_space=2;text.style.alignment=2;if(!chapter_text->enqueue(text))return 0;}}
 struct Marker {ScreenProbe* probe;u32 age;} moving{&s,age};FrameCallback background,marker;
 background.enabled=marker.enabled=true;background.owner=&s;marker.owner=&moving;
 background.run=[](void* p){auto& s=*static_cast<ScreenProbe*>(p);s.renderer.invalidate();graphics->clear_target(0xff123456,nullptr);return 1;};
 marker.run=[](void* p){auto& m=*static_cast<Marker*>(p);m.probe->renderer.invalidate();const auto state=graphics->pipeline();graphics_depth_quad(180+float(m.age%200),360,8,8,.1f,0xffff0000);graphics->backend.flush();graphics->pipeline()=state;m.probe->renderer.invalidate();return 1;};
 s.scheduler.add(background,FramePass::Draw,4);
 s.scheduler.add(marker,FramePass::Draw,29);
 const auto result=s.scheduler.draw();s.scheduler.remove(background);s.scheduler.remove(marker);s.renderer.flush();
 if(result==i32(FrameAction::Error)){s.error=s.renderer.error;return 0;}graphics->present();return 1;
}
EMSCRIPTEN_KEEPALIVE int graphics_chapter_ascii(const u8* data,u32 size){
 if(!screen_probe)return 0;auto& s=*screen_probe;
 if(!s.animations.load(2,data,size)||!graphics->preload(*s.animations.resource(2)))return 0;
 chapter_text=std::make_unique<AsciiText>(s.animations,s.environment);
 if(!chapter_text->initialize(2))return 0;
 chapter_ascii_frame=std::make_unique<AsciiFrame>(s.scheduler,*chapter_text,s.renderer,s.views);return 1;
}
int graphics_screen_initialize(const u8* data,u32 size){screen_probe=std::make_unique<ScreenProbe>();return screen_probe->initialize(data,size);}
int graphics_screen_pass(u32 index){if(!screen_probe)return 0;const bool ok=screen_probe->compositor.pass(index);if(!ok)screen_probe->error=screen_probe->compositor.error;return ok;}
int graphics_screen_capture(int results){if(!screen_probe)return 0;const bool ok=screen_probe->screenshots.capture(screen_probe->snapshot,results!=0);if(!ok)screen_probe->error=screen_probe->screenshots.error;return ok;}
u32 graphics_screen_capture_pending(){return screen_probe?screen_probe->screenshots.pending():0;}
int graphics_screen_capture_finish(){return screen_probe&&screen_probe->screenshots.finish_frame();}
u32 graphics_screen_capture_texture(){if(!screen_probe)return 0;auto* vm=screen_probe->animations.registry.find(screen_probe->snapshot);return vm&&vm->sprite_resource&&vm->sprite?graphics->texture(*vm->sprite_resource,vm->sprite->texture):0;}
u32 graphics_texture_format(u32 id){auto* image=graphics->pixels(id);return image?image->packed_format:0xffff;}
const u8* graphics_read_texture(u32 id){graphics->backend.read(id);auto* image=graphics->pixels(id);return image?image->pixels.data():nullptr;}
const char* graphics_screen_error(){return screen_probe?screen_probe->error.c_str():"No screen probe";}
void graphics_screen_paint(u32 color){graphics->clear_target(color,nullptr);}
void graphics_screen_close(){chapter_ascii_frame.reset();chapter_text.reset();chapter_schedule.reset();screen_probe.reset();graphics->select_target(nullptr,0);}
int graphics_initialize(){graphics=std::make_unique<sdl::GraphicsDevice>();return graphics->initialize();}
const char* graphics_error(){return graphics?(graphics->error.empty()?graphics->backend.error():graphics->error.c_str()):"No graphics fixture";}
u32 graphics_texture(i32 format,i32 w,i32 h,const u8* data,u32 size){auto file=std::make_unique<AnmResource>();AnmTexture t;t.name="fixture";t.format=t.pixel_format=format;t.width=t.pixel_width=w;t.height=t.pixel_height=h;t.pixels.assign(data,data+size);file->textures.push_back(std::move(t));if(!graphics->preload(*file))return 0;const u32 id=graphics->texture(*file,0);files.push_back(std::move(file));return id;}
void graphics_dirty(u32 id,u32 x,u32 y,u32 w,u32 h,u32 packed){auto* image=graphics->pixels(id);if(!image)return;for(u32 row=y;row<y+h;row++)for(u32 col=x;col<x+w;col++){const u16 value=u16(packed);std::memcpy(image->pixels.data()+row*image->pitch+col*2,&value,2);}graphics->changed(id,{x,y,x+w,y+h});}
void graphics_world_quad(u32 texture,float x,float y,float u,float v,u32 color,u32 individual){
 auto& g=*graphics;auto& p=g.pipeline();p.alphaTest=false;p.depthTest=false;p.depthWrite=false;p.blend=true;p.fog=false;p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::InverseSourceAlpha;p.minFilter=p.magFilter=Filter::Nearest;p.addressU=p.addressV=Address::Clamp;p.color.operation=p.alpha.operation=ColorOperation::Multiply;p.color.first=p.alpha.first={ArgumentSource::Texture};p.color.second=p.alpha.second={ArgumentSource::Factor};p.textureFactor=color;p.textureTransform=true;
 Matrix4 world,view,projection,atlas;world.identity();view.identity();projection.identity();atlas.identity();world.m[12]=x;world.m[13]=y;projection.m[0]=2.f/640;projection.m[5]=2.f/480;projection.m[12]=-1;projection.m[13]=-1;atlas.m[0]=atlas.m[5]=.5f;atlas.m[8]=u;atlas.m[9]=v;
 g.set_matrix(MatrixKind::World,world);g.set_matrix(MatrixKind::View,view);g.set_matrix(MatrixKind::Projection,projection);g.set_matrix(MatrixKind::Texture,atlas);g.bind_texture(texture);g.set_layout(VertexLayout::WorldUv);
 struct Vertex{Vec3 p;Vec2 uv;};const Vertex q[]={{{0,0,0},{0,0}},{{64,0,0},{1,0}},{{0,64,0},{0,1}},{{64,64,0},{1,1}}};if(individual){const Vertex t[]={q[0],q[1],q[2],q[2],q[1],q[3]};g.primitives(Topology::Triangles,2,t,sizeof(Vertex));}else g.primitives(Topology::Strip,2,q,sizeof(Vertex));
}
void graphics_topology(u32 kind,u32 count,u32 expanded,u32 variant){
 auto& g=*graphics;auto& s=g.pipeline();s.alphaTest=false;s.depthTest=false;s.depthWrite=false;s.blend=true;s.separateAlphaBlend=false;s.blendEquation=BlendEquation::Add;s.sourceBlend=BlendFactor::SourceAlpha;s.destinationBlend=BlendFactor::InverseSourceAlpha;s.cull=Cull::None;s.fog=false;s.textureTransform=false;s.color.operation=s.alpha.operation=ColorOperation::First;s.color.first=s.alpha.first={ArgumentSource::Diffuse};g.bind_texture(0);g.set_layout(VertexLayout::ScreenColorUv);
 std::vector<AnmGeometryVertex> points(count+2),triangles;const auto primitive=Topology(kind);const float shift=float(variant)*11;
 for(u32 i=0;i<points.size();i++){auto& v=points[i];v.reciprocal_w=1;v.position.z=.5f;v.color=0x70000000u|((i*37+variant*71)&255)<<16|((i*83)&255)<<8|((i*19)&255);v.uv={float(i&1),float(i)/float(count)};
  if(primitive==Topology::Fan){const float angle=float(i?i-1:0)*6.2831855f/float(count);v.position.x=i?float(180+shift+120*std::cos(double(angle))):180+shift;v.position.y=i?float(170+110*std::sin(double(angle))):170;}
  else {v.position.x=60+shift+float(i/2)*5;v.position.y=100+float((i&1)*50)+float(50*std::sin(double(float(i/2)*.11f)));}
 }
 if(expanded){triangles.reserve(count*3);for(u32 i=0;i<count;i++){const u32 a=primitive==Topology::Fan?0:i&1?i+1:i,b=primitive==Topology::Fan?i+1:i&1?i:i+1;for(u32 j:{a,b,i+2})triangles.push_back(points[j]);}g.triangles(count,triangles.data(),sizeof(AnmGeometryVertex));}
 else g.primitives(primitive,count,points.data(),sizeof(AnmGeometryVertex));
}

void graphics_clear(u32 color){graphics->clear(color);}
int graphics_clear_depth(i32 x,i32 y,i32 w,i32 h){const GraphicsViewport region{u32(x),u32(y),u32(w),u32(h),0,1};return graphics->clear_depth(w>0&&h>0?&region:nullptr);}
void graphics_depth_quad(float x,float y,float w,float h,float z,u32 color){
 auto& g=*graphics;auto& s=g.pipeline();s.depthTest=true;s.depthWrite=true;s.depthCompare=Compare::Less;s.alphaTest=false;s.blend=false;s.fog=false;s.textureTransform=false;s.color.operation=s.alpha.operation=ColorOperation::First;s.color.first=s.alpha.first={ArgumentSource::Diffuse};g.set_layout(VertexLayout::ScreenColor);
 struct V{Vec3 position;float reciprocal_w;u32 color;};const V v[]={{{x,y,z},1,color},{{x+w,y,z},1,color},{{x,y+h,z},1,color},{{x+w,y,z},1,color},{{x,y+h,z},1,color},{{x+w,y+h,z},1,color}};g.triangles(2,v,sizeof(V));
}
int graphics_upload_png(u32 target,const u8* data,u32 size){return graphics->upload_png(target,data,size);}
int graphics_clear_texture(u32 target){return graphics->clear_image(target);}
void graphics_alpha(u32 separate,u32 source,u32 destination,u32 equation){auto& state=graphics->pipeline();state.separateAlphaBlend=separate;state.sourceAlphaBlend=BlendFactor(source);state.destinationAlphaBlend=BlendFactor(destination);state.alphaBlendEquation=BlendEquation(equation);}
void graphics_quad(u32 texture,float x,float y,float w,float h,u32 color,i32 source_blend,i32 destination_blend){
 auto& g=*graphics;auto& state=g.pipeline();state.alphaTest=false;state.depthTest=false;state.depthWrite=false;state.blend=true;state.sourceBlend=BlendFactor(source_blend);state.destinationBlend=BlendFactor(destination_blend);state.blendEquation=BlendEquation::Add;state.minFilter=state.magFilter=Filter::Nearest;state.addressU=state.addressV=Address::Clamp;state.color.operation=state.alpha.operation=ColorOperation::Multiply;state.color.first=state.alpha.first={ArgumentSource::Texture};state.color.second=state.alpha.second={ArgumentSource::Diffuse};state.textureTransform=false;g.bind_texture(texture);g.set_layout(VertexLayout::ScreenColorUv);
 const AnmGeometryVertex quad[]={{{x,y,0},1,color,{0,0}},{{x+w,y,0},1,color,{1,0}},{{x,y+h,0},1,color,{0,1}},{{x+w,y,0},1,color,{1,0}},{{x,y+h,0},1,color,{0,1}},{{x+w,y+h,0},1,color,{1,1}}};g.triangles(2,quad,sizeof(AnmGeometryVertex));
}
const u8* graphics_texture_data(u32 id){auto* image=graphics->pixels(id);return image?image->pixels.data():nullptr;}
const u8* graphics_pixels(){graphics->backend.read(sdl::GraphicsDevice::screen);return graphics->pixels(sdl::GraphicsDevice::screen)->pixels.data();}
const touhou::sdl::Statistics* graphics_stats(){return &graphics->backend.stats;}
void graphics_frame(){graphics->present();}
u32 graphics_text(const char* value,i32 style,u32 color,u32 fallback){auto file=std::make_unique<AnmResource>();AnmTexture t;t.name="font-fixture";t.format=t.pixel_format=5;t.width=t.pixel_width=384;t.height=t.pixel_height=64;t.kind=AnmTexture::Kind::Blank;t.pixels.resize(384*64*2);file->textures.push_back(std::move(t));file->sprites.push_back({0,0,0,0,384,64,0,0,1,1});if(!graphics->preload(*file))return 0;AnmVm vm;vm.resource=file.get();if(!vm.select_sprite(0))return 0;vm.visual.render_flags|=0x1000;sdl::FontDevice fonts(*graphics);fonts.fallback=fallback!=0;DialogueText request;request.bytes=value;request.font=style;request.color=color;if(!fonts.text(vm,request)){graphics->error=fonts.error;return 0;}const u32 id=graphics->texture(*file,0);files.push_back(std::move(file));return id;}
int graphics_capture(u32 destination,i32 x,i32 y,i32 w,i32 h){const i32 rect[]={x,y,x+w,y+h},point[]={0,0};return graphics->copy_surface(sdl::GraphicsDevice::screen,rect,destination,point);}
int graphics_resize(u32 source,u32 target,i32 sw,i32 sh,i32 dw,i32 dh){const i32 from[]={0,0,sw,sh},to[]={0,0,dw,dh};return graphics->resample_surface(source,from,target,to);}
}
