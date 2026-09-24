#include "../platform/LegacyDeviceAbi.hpp"
#include "GraphicsHost.hpp"
#include "../platform/PlatformDevices.hpp"
#include "Renderer.hpp"
#include "AssetPixelFormat.hpp"
#ifndef TH_NATIVE_PLATFORM
#include "LegacyGraphics.hpp"
#endif
#include <memory>
#include "../game/TriangleCoefficients.hpp"
namespace th08 {
namespace {
std::unique_ptr<touhou::sdl::Renderer> gpu;BrowserRuntime* runtime=nullptr;
std::map<u32,touhou::sdl::Surface> known;bool capture_checked=false,capture_exact=false,capture_testing=false;u32 capture_mismatches=0;
touhou::sdl::Surface resolve(void*,u32 h){if(h==0xffffffff)return {h,640,480,touhou::graphics::PixelFormat::Depth16,0,nullptr,0,0};
 if(const auto* p=runtime->texture(h)){touhou::sdl::Surface s{p->handle,p->width,p->height,touhou::graphics::asset_pixel_format(p->format),p->pitch,reinterpret_cast<u8*>(p->data),p->size,p->revision};known[h]=s;return s;}auto it=known.find(h);return it==known.end()?touhou::sdl::Surface{}:it->second;
}
template<class T=u32>T* ptr(u32 p){return reinterpret_cast<T*>(uintptr_t(p));}
}
bool sdl_attach(BrowserRuntime* r){runtime=r;known.clear();capture_checked=capture_exact=capture_testing=false;capture_mismatches=0;gpu=std::make_unique<touhou::sdl::Renderer>(8,resolve,nullptr);return gpu->initialize();}
void sdl_detach(){gpu.reset();runtime=nullptr;known.clear();}
struct SDLGraphics final:ZunGraphics {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
 void invalidate_texture(u32 handle)override{frame().release(handle);known.erase(handle);}
#endif
 bool resample(u32 source,const TextureRect& from,u32 target,const TextureRect& to,bool triangle)override{
  if(!capture_testing&&(!capture_checked||!capture_exact))return false;
  if(arithmetic_precision()!=Precision::Single||arithmetic_rounding()!=Rounding::NearestEven)return false;
  const u32 sw=from.right-from.left,sh=from.bottom-from.top,w=to.right-to.left,h=to.bottom-to.top;
  if(w==sw&&h==sh)triangle=false;
  if(!triangle&&runtime->texture(source)->format!=runtime->texture(target)->format)return false;
  if(!triangle)return frame().resample(source,&from.left,target,&to.left,nullptr,0,0);
  if(std::min({sw,sh,w,h})<16||std::max({sw,sh,w,h})>1024)return false;
  struct Filter{u32 width=0;std::vector<float> values;};
  static std::map<std::array<u32,4>,Filter> filters;const std::array<u32,4> key{sw,sh,w,h};
  if(filters.size()>=8&&!filters.count(key))filters.clear();auto& filter=filters[key];
  if(filter.values.empty()){
   std::vector<std::vector<std::pair<float,float>>> rows(w+h);
   for(u32 axis=0;axis<2;++axis){const u32 size=axis?sh:sw,dest=axis?h:w;
    auto* table=TriangleCoefficients::create(size,dest,true);if(!table){filters.erase(key);return false;}
    auto* block=table+4;for(u32 sample=0;sample<size;++sample){u32 length;std::memcpy(&length,block,4);
     for(auto* p=block+4;p<block+length;p+=8){FilterWeight value;std::memcpy(&value,p,8);rows[(axis?w:0)+value.index].push_back({float(sample),value.weight});}
     block+=length;
    }std::free(table);
   }
   for(const auto& row:rows)filter.width=std::max(filter.width,u32(row.size()+1));
   filter.values.resize(filter.width*(w+h)*2);
   for(u32 row=0;row<w+h;++row){auto* p=filter.values.data()+row*filter.width*2;p[0]=float(rows[row].size());for(u32 j=0;j<rows[row].size();++j){p[2+j*2]=rows[row][j].first;p[3+j*2]=rows[row][j].second;}}
  }
  return frame().resample(source,&from.left,target,&to.left,filter.values.data(),filter.width,w+h);
 }
 void prepare_texture(u32 handle)override{frame().prepare(handle);}
 touhou::sdl::Renderer& frame(){auto& g=*gpu;g.state.target=runtime->backbuffer();g.state.depth=0xffffffff;return g;}
 void flush()override{frame().flush();}void texture(u32 h)override{frame().state.texture=h;}
#ifdef TH_NATIVE_PLATFORM
 PipelineState& pipeline()override{return frame().pipeline();}
 void transform(MatrixParameter kind,const Matrix4& matrix)override{frame().transform(kind,&matrix);}
#else
 void render_state(u32 key,u32 value)override{touhou::graphics::legacy::render(frame().pipeline(),key,value);}
 void stage_state(u32 key,u32 value)override{touhou::graphics::legacy::stage(frame().pipeline(),key,value);}
 void transform(MatrixParameter kind,const Matrix4& matrix)override{frame().transform(touhou::graphics::legacy::matrix(kind),&matrix);}
#endif
 void viewport(const Viewport& view)override{frame().viewport(*reinterpret_cast<const touhou::sdl::Viewport*>(&view));}
 void draw(Primitive primitive,VertexFormat format,const void* vertices,u32 count)override{
  auto& g=frame();
#ifdef TH_NATIVE_PLATFORM
  const auto layout=format==VertexFormat::World?VertexLayout::WorldUv:format==VertexFormat::ColoredWorld?VertexLayout::WorldColorUv:format==VertexFormat::Screen?VertexLayout::ScreenColorUv:VertexLayout::ScreenColor;
  g.state.layout=attributes(layout);const auto type=primitive;const auto bytes=touhou::graphics::stride(layout);
#else
  g.state.layout=touhou::graphics::legacy::vertices(u32(format));const auto type=touhou::graphics::legacy::topology(u32(primitive));const u32 bytes=u32(format)==0x144?28:u32(format)==0x142?24:20;
#endif
  if(g.state.texture)g.prepare(g.state.texture);g.prepare(g.state.target);if(type==Topology::Triangles)g.draw_batch(count/3,vertices,bytes);else g.draw(type,count-2,vertices,bytes);
 }
 void clear(u32 flags,u32 color,float depth,u32 stencil)override{frame().clear(flags,color,depth,stencil);}
 bool present(u32 surface)override{auto& g=frame();g.present(surface);for(auto it=known.begin();it!=known.end();){if(!runtime->texture(it->first)){g.release(it->first);it=known.erase(it);}else ++it;}return true;}
 void read(u32 surface)override{frame().read(surface);}void discard()override{frame().discard();}
 void copy(u32 source,const TextureRect& rect,u32 target,i32 x,i32 y)override{i32 point[]{x,y};frame().copy(source,&rect.left,target,point);}
};
ZunGraphics& graphics_device(){static SDLGraphics graphics;return graphics;}

// Probe actual shader arithmetic once during initialization. A mobile driver
// that contracts/rounds the filter differently retains the exact CPU filter.
// No readback or comparison occurs during ordinary drawing.
void sdl_validate_capture(){
 if(capture_checked)return;capture_testing=true;capture_exact=true;
 const auto precision=arithmetic_precision();const auto rounding=arithmetic_rounding();arithmetic_mode(Precision::Single,Rounding::NearestEven);
 for(u32 inputFormat:{21u,22u,23u})for(u32 format:{21u,22u,23u,24u,25u,26u}){
  TexturePixels input,expected,actual;input.create(96,96,inputFormat);expected.create(64,64,format);actual.create(64,64,format);
  u32 rng=0x182d83u;for(auto& b:input.pixels){rng=rng*1664525u+1013904223u;b=u8(rng>>24);}
  PixelSurface from{inputFormat,96,96,96*TexturePixels::describe(inputFormat).bytes,input.pixels.data()},to{format,64,64,64*TexturePixels::describe(format).bytes,expected.pixels.data()};
  TextureResample::triangle(to,{0,0,64,64},from,{0,0,96,96});
  const auto source=runtime->app.textures.insert(std::move(input)),destination=runtime->app.textures.insert(std::move(actual),0,true);
  bool ok=graphics_device().resample(source,{0,0,96,96},destination,{0,0,64,64},true);runtime->app.textures.changed(destination);
  if(ok){gpu->read(destination);const auto& got=runtime->app.textures.get(destination)->image;
   for(u32 i=0;i<got.pixels.size();++i){if(format==22&&i%4==3)continue;if(got.pixels[i]!=expected.pixels[i]){++capture_mismatches;ok=false;}}
  }
  capture_exact&=ok;gpu->release(source);gpu->release(destination);runtime->app.textures.release(source);runtime->app.textures.release(destination);known.erase(source);known.erase(destination);
 }
 arithmetic_mode(precision,rounding);capture_testing=false;capture_checked=true;
}
extern "C" __attribute__((export_name("sdl_capture_validation"))) u32 sdl_capture_validation(){return capture_checked?(capture_exact?1:2):0;}
extern "C" __attribute__((export_name("sdl_capture_mismatches"))) u32 sdl_capture_mismatches(){return capture_mismatches;}

#ifndef TH_NATIVE_PLATFORM
i32 sdl_graphics(u32 op,u32 a,u32 b,u32 c){
 if(!gpu)return -1;auto& g=*gpu;g.state.target=runtime->backbuffer();g.state.depth=0xffffffff;
 switch(op){
 case 1:g.flush();break;case 2:g.state.texture=a;break;case 3:touhou::graphics::legacy::render(g.pipeline(),a,b);break;case 4:touhou::graphics::legacy::stage(g.pipeline(),a,b);break;
 case 5:g.transform(touhou::graphics::legacy::matrix(a),ptr(b));break;case 6:g.viewport(*ptr<touhou::sdl::Viewport>(a));break;
 case 7:{const auto& d=*ptr<BrowserDraw>(a);const u32 stride=d.format==0x144?28:d.format==0x142?24:20;g.state.layout=touhou::graphics::legacy::vertices(d.format);if(g.state.texture)g.prepare(g.state.texture);g.prepare(g.state.target);g.draw(touhou::graphics::legacy::topology(d.primitive),d.primitive==4?d.count/3:d.count-2,ptr(d.vertices),stride);break;}
 case 8:{const auto& d=*ptr<BrowserClear>(a);g.clear(d.flags,d.color,d.depth,d.stencil);break;}
 case 9:g.present(a);for(auto it=known.begin();it!=known.end();){if(!runtime->texture(it->first)){g.release(it->first);it=known.erase(it);}else ++it;}break;
 case 10:g.read(a);break;case 11:{const auto& d=*ptr<BrowserCopy>(a);g.copy(d.source,reinterpret_cast<const i32*>(&d.rect),d.target,&d.x);break;}
 case 12:g.discard();break;default:return -1;
 }return 0;
}
#endif

}
extern "C" __attribute__((export_name("sdl_read_back"))) uint32_t sdl_read_back(){auto* g=touhou::sdl::current();if(!g)return 0;auto s=g->resolve(g->owner,g->state.target);g->read(s.handle);return reinterpret_cast<uintptr_t>(s.data);}
