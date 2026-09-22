#include "../platform/Graphics.hpp"
#include "Renderer.hpp"
#include "AssetPixelFormat.hpp"
#ifndef TH_NATIVE_PLATFORM
#include "LegacyGraphics.hpp"
#endif
#include <memory>
#include <cstring>
#include <cstdlib>
#include <algorithm>
using namespace touhou::sdl;
namespace {
u32 addr(const void* p){return reinterpret_cast<uintptr_t>(p);}template<class T=u32>T* ptr(u32 p){return reinterpret_cast<T*>(uintptr_t(p));}
u32 pixel_bytes(u32 f){return f==20?3:(f>=23&&f<=26)||f==51?2:f==28||f==50?1:4;}
struct Resource {enum Kind{Device,Texture,Image,Buffer}kind;u32 refs=1,parent=0,child=0,usage=0,pool=0;
#ifndef TH_NATIVE_PLATFORM
u32 fvf=0;
#endif
VertexLayout layout=VertexLayout::WorldUv;Surface s;std::vector<u8> bytes;};
struct Host {
 std::map<u32,std::unique_ptr<Resource>> resources;u32 next=1,device=0,back=0,depth=0,target=0,depthTarget=0,bound=0,stream=0,streamOffset=0,streamStride=0,index=0;
 std::unique_ptr<Renderer> gpu;std::vector<u8> expanded;
 Resource& get(u32 h){auto it=resources.find(h);if(it==resources.end()){std::fprintf(stderr,"SDL TH10 invalid resource %u\n",h);std::abort();}return *it->second;}
 u32 create(Resource::Kind k){u32 id=next++;auto r=std::make_unique<Resource>();r->kind=k;r->s.handle=id;resources[id]=std::move(r);return id;}
 u32 image(u32 w,u32 h,u32 f,u32 pool=1,u32 usage=0){u32 id=create(Resource::Image);auto& r=get(id);r.pool=pool;r.usage=usage;r.s.width=w;r.s.height=h;r.s.format=asset_pixel_format(f);r.s.pitch=w*pixel_bytes(f);r.s.size=r.s.pitch*h;r.bytes.resize(r.s.size);r.s.data=r.bytes.data();return id;}
 u32 add(u32 id){if(!id)return 0;auto& r=get(id);return r.parent?add(r.parent):++r.refs;}
 u32 release(u32 id){if(!id)return 0;auto& r=get(id);if(r.parent)return release(r.parent);if(--r.refs)return r.refs;
  if(r.kind==Resource::Device){gpu->flush();release(bound);release(stream);release(index);release(target);release(depthTarget);release(back);release(depth);}
  else if(r.kind==Resource::Texture){gpu->release(r.child);resources.erase(r.child);}else if(r.kind==Resource::Image)gpu->release(id);
  resources.erase(id);return 0;
 }
 static Surface resolve(void* p,u32 id){
  // The shared ImGui overlay pass asks for a full-size depth surface with the
  // canonical 0xffffffff handle (Renderer::render_imgui). TH08's device resolve
  // synthesizes that surface so the overlay can attach a depth renderbuffer to
  // the backbuffer. TH10 owns real depth resource ids instead, so without this
  // the resource lookup fails and aborts the wasm module the first time any
  // ImGui window draws.
  if(id==0xffffffff)return {id,640,480,touhou::graphics::PixelFormat::Depth16,0,nullptr,0,0};
  auto& h=*static_cast<Host*>(p);auto& r=h.get(id);return r.kind==Resource::Texture?h.get(r.child).s:r.s;}
 u32 make_texture(u32 w,u32 h,u32 f,u32 pool,u32 usage){u32 s=image(w,h,f,pool,usage),t=create(Resource::Texture);get(t).child=s;get(s).parent=t;return t;}
 void desc(u32 id,u32* out){auto& r=get(id);u32 values[]{asset_pixel_code(r.s.format),1,r.usage,r.pool,0,0,r.s.width,r.s.height};std::memcpy(out,values,sizeof(values));}
 i32 copy(u32 a,const i32* from,u32 b,const i32* dest){auto& x=get(a).s;auto& y=get(b).s;if(a==b||x.format!=y.format)return i32(0x8876086c);
  i32 box[]{0,0,i32(x.width),i32(x.height)};if(!from)from=box;i32 point[]{from[0],from[1]};if(!dest)dest=point;
  if(from[0]<0||from[1]<0||from[2]<=from[0]||from[3]<=from[1]||from[2]>i32(x.width)||from[3]>i32(x.height)||dest[0]<0||dest[1]<0||dest[0]+from[2]-from[0]>i32(y.width)||dest[1]+from[3]-from[1]>i32(y.height))return i32(0x8876086c);
  gpu->copy(a,from,b,dest);y.version++;return 0;
 }
 void draw(TopologyParameter p,u32 n,const u8* data,u32 stride,const void* idx=nullptr,u32 fmt=0,u32 minimum=0,u32 vertexCount=0){
  gpu->state.texture=bound?get(bound).child:0;gpu->state.target=target;gpu->state.depth=depthTarget;
  if(idx){
#ifdef TH_NATIVE_PLATFORM
  u32 count=vertex_count(p,n);
#else
  u32 count=vertex_count(legacy::topology(p),n);
#endif
expanded.resize(count*stride);for(u32 i=0;i<count;i++){u32 k=fmt==101?static_cast<const uint16_t*>(idx)[i]:static_cast<const u32*>(idx)[i];if(k<minimum||k>=minimum+vertexCount)std::abort();std::memcpy(expanded.data()+i*stride,data+k*stride,stride);}data=expanded.data();}
#ifdef TH_NATIVE_PLATFORM
  if(p==Topology::Triangles)gpu->draw_batch(n,data,stride);else gpu->draw(p,n,data,stride);
#else
  gpu->draw(legacy::topology(p),n,data,stride);
#endif
 }
}host;
}
extern "C" {
#ifdef TH_NATIVE_PLATFORM
PipelineState& sdl_pipeline(){return host.gpu->pipeline();}
#else
void sdl_texture_stage(u32 stage,u32 key,u32 value){if(!stage)legacy::stage(host.gpu->pipeline(),key,value);}
void sdl_sampler(u32 stage,u32 key,u32 value){const u32 keys[]{0,13,14,25,15,16,17,18,19,20,21};if(!stage&&key<11)legacy::stage(host.gpu->pipeline(),keys[key],value);}
void sdl_render_state(u32 key,u32 value){legacy::render(host.gpu->pipeline(),key,value);}
#endif
void sdl_bind_texture(u32 texture){if(host.bound!=texture){host.add(texture);host.release(host.bound);host.bound=texture;}}
void sdl_vertex_format(LayoutParameter format){
#ifdef TH_NATIVE_PLATFORM
 host.gpu->state.layout=attributes(format);
#else
 host.gpu->state.layout=legacy::vertices(format);
#endif
}
void sdl_draw(TopologyParameter primitive,u32 count,const void* vertices,u32 stride){host.draw(primitive,count,static_cast<const u8*>(vertices),stride);}
void sdl_transform(MatrixParameter kind,const th10::Matrix4* matrix){
#ifdef TH_NATIVE_PLATFORM
 host.gpu->transform(kind,matrix);
#else
 host.gpu->transform(legacy::matrix(kind),matrix);
#endif
}
void sdl_viewport(const th10::CameraViewport* view){host.gpu->viewport(*reinterpret_cast<const Viewport*>(view));}
u32 graphics_host_create(const th10::browser::GraphicsPresentation* p,u32 flags){
 if(!(flags&2))th10::arithmetic_mode(th10::Precision::Single,th10::Rounding::NearestEven);
 host.gpu=std::make_unique<Renderer>(10,Host::resolve,&host);if(!host.gpu->initialize())return 0;
 host.back=host.image(p->width?p->width:640,p->height?p->height:480,p->format?p->format:22,0);host.depth=host.image(host.get(host.back).s.width,host.get(host.back).s.height,80,0);host.device=host.create(Resource::Device);
 host.target=host.back;host.depthTarget=host.depth;host.add(host.back);host.add(host.depth);host.gpu->state.target=host.back;host.gpu->state.depth=host.depth;return host.device;
}
#ifdef TH_NATIVE_PLATFORM
i32 native_graphics_create_surface(u32 w,u32 h,u32 f,u32 pool,bool target,void*& out){out=ptr<void>(host.image(w,h,f,target?0:pool,target?1:0));return 0;}
i32 native_graphics_copy_surface(void* source,const th10::TextureRect* region,void* destination,const th10::TextureRect* target,u32 filter){auto& s=host.get(addr(source)).s;auto& d=host.get(addr(destination)).s;
    th10::TextureRect from=region?*region:th10::TextureRect{0,0,i32(s.width),i32(s.height)},to=target?*target:th10::TextureRect{0,0,i32(d.width),i32(d.height)};
    if(filter>2||from.right-from.left!=to.right-to.left||from.bottom-from.top!=to.bottom-to.top)return i32(0x8876086c);
    return host.copy(addr(source),&from.left,addr(destination),&to.left);
}
i32 native_graphics_update_surface(void* source,const th10::TextureRect& region,void* destination,i32 x,i32 y){i32 point[]{x,y};return host.copy(addr(source),&region.left,addr(destination),point);}
u32 native_graphics_retain_resource(void* value){return host.add(addr(value));}
u32 native_graphics_resource_revision(void* value){auto& r=host.get(addr(value));return host.get(r.kind==Resource::Texture?r.child:addr(value)).s.version;}
u32 native_graphics_release_resource(void* value){return host.release(addr(value));}
i32 native_graphics_create_texture(u32 width,u32 height,u32 format,void*& out){out=ptr<void>(host.make_texture(width,height,format,1,0));return 0;}
void* native_graphics_texture_surface(void* texture){const auto child=host.get(addr(texture)).child;host.add(child);return ptr<void>(child);}
th10::TextureDescription native_graphics_describe_surface(void* surface){auto& r=host.get(addr(surface));const auto& s=host.get(r.kind==Resource::Texture?r.child:addr(surface)).s;return {asset_pixel_code(s.format),s.width,s.height};}
th10::TextureLock native_graphics_map_surface(void* surface){auto& r=host.get(addr(surface));const auto id=r.kind==Resource::Texture?r.child:addr(surface);host.gpu->read(id);auto& s=host.get(id).s;return {i32(s.pitch),s.data};}
void native_graphics_unmap_surface(void* surface){auto& r=host.get(addr(surface));host.get(r.kind==Resource::Texture?r.child:addr(surface)).s.version++;}
void native_graphics_resource_priority(void*,u32){}
void native_graphics_preload_resource(void* value){if(value){auto& r=host.get(addr(value));if(r.kind==Resource::Texture)host.gpu->prepare(r.child);else if(r.kind==Resource::Image)host.gpu->prepare(addr(value));}}
i32 native_graphics_create_vertices(u32 bytes,LayoutParameter format,void*& out){const auto id=host.create(Resource::Buffer);auto& r=host.get(id);r.layout=format;r.pool=1;r.bytes.resize(bytes);out=ptr<void>(id);return 0;}
void* native_graphics_map_vertices(void* buffer){return host.get(addr(buffer)).bytes.data();}
void native_graphics_unmap_vertices(void*){}
void native_graphics_vertex_buffer(void* buffer,u32 stride){const auto id=addr(buffer);host.add(id);host.release(host.stream);host.stream=id;host.streamOffset=0;host.streamStride=stride;}
void native_graphics_draw_vertices(TopologyParameter primitive,u32 first,u32 count){host.draw(primitive,count,host.get(host.stream).bytes.data()+first*host.streamStride,host.streamStride);}
void native_graphics_clear_target(u32 flags,u32 color,float depth,u32 stencil,const i32* rectangles,u32 count){host.gpu->state.target=host.target;host.gpu->state.depth=host.depthTarget;host.gpu->clear(flags,color,depth,stencil,rectangles,count);}
i32 native_graphics_present_frame(){host.gpu->present(host.back);return 0;}
void* native_graphics_back_surface(){host.add(host.back);return ptr<void>(host.back);}
i32 native_graphics_reset_presentation(void*){return 0;}
void native_graphics_clear_shader(){}
#else
i32 graphics_host_device(u32 handle,u32 op,const u32* a){auto& h=host;auto& g=*h.gpu;
 switch(op){
 case 0:if(a[0]==0)legacy::stage(g.pipeline(),a[1],a[2]);break;
 case 1:{const u32 keys[]{0,13,14,25,15,16,17,18,19,20,21};if(a[0]==0&&a[1]<11)legacy::stage(g.pipeline(),keys[a[1]],a[2]);break;}
 case 2:legacy::render(g.pipeline(),a[0],a[1]);break;
 case 3:if(a[0]==0&&h.bound!=a[1]){h.add(a[1]);h.release(h.bound);h.bound=a[1];}break;
 case 4:g.state.layout=legacy::vertices(a[0]);break;
 case 5:h.draw(a[0],a[1],ptr<u8>(a[2]),a[3]);break;
 case 6:g.transform(legacy::matrix(a[0]),ptr(a[1]));break;
 case 7:if(a[0]==0){h.add(a[1]);h.release(h.stream);h.stream=a[1];h.streamOffset=a[2];h.streamStride=a[3];}break;
 case 8:h.draw(a[0],a[2],h.get(h.stream).bytes.data()+h.streamOffset+a[1]*h.streamStride,h.streamStride);break;
 case 9:g.viewport(*ptr<Viewport>(a[0]));break;
 case 10:g.state.target=h.target;g.state.depth=h.depthTarget;g.clear(a[2],a[3],*ptr<float>(addr(a+4)),a[5],a[0]?ptr<i32>(a[1]):nullptr,a[0]);break;
 case 11:case 12:break;
 case 13:g.present(h.back);break;
 case 14:h.add(h.back);*ptr(a[3])=h.back;break;
 case 15:h.add(h.target);*ptr(a[1])=h.target;break;
 case 16:if(a[0])return i32(0x8876086c);g.flush();h.add(a[1]);h.release(h.target);h.target=a[1];g.state.target=h.target;{auto& s=h.get(h.target).s;g.viewport({0,0,s.width,s.height,0,1});}break;
 case 17:h.add(h.depthTarget);*ptr(a[0])=h.depthTarget;return h.depthTarget?0:i32(0x88760866);
 case 18:g.flush();h.add(a[0]);h.release(h.depthTarget);h.depthTarget=a[0];g.state.depth=a[0];break;
 case 19:*ptr(a[6])=h.make_texture(a[0],a[1],a[4],a[5],a[3]);break;
 case 20:*ptr(a[4])=h.image(a[0],a[1],a[2],a[3]);break;
 case 21:*ptr(a[6])=h.image(a[0],a[1],a[2],0,1);break;
 case 22:case 23:{u32 id=h.create(Resource::Buffer);auto& r=h.get(id);r.usage=a[1];r.fvf=a[2];r.pool=a[3];r.bytes.resize(a[0]);*ptr(a[4])=id;break;}
 case 24:h.add(a[0]);h.release(h.index);h.index=a[0];break;
 case 25:{auto& idx=h.get(h.index);h.draw(a[0],a[5],h.get(h.stream).bytes.data()+h.streamOffset+i32(a[1])*h.streamStride,h.streamStride,idx.bytes.data()+a[4]*(idx.fvf==101?2:4),idx.fvf,a[2],a[3]);break;}
 case 26:h.draw(a[0],a[3],ptr<u8>(a[6]),a[7],ptr(a[4]),a[5],a[1],a[2]);break;
 case 27:{auto& s=h.get(a[0]).s;auto& d=h.get(a[2]).s;i32 from[]{0,0,i32(s.width),i32(s.height)},to[]{0,0,i32(d.width),i32(d.height)};if(a[1])std::memcpy(from,ptr(a[1]),16);if(a[3])std::memcpy(to,ptr(a[3]),16);if(a[4]>2||from[2]-from[0]!=to[2]-to[0]||from[3]-from[1]!=to[3]-to[1])return i32(0x8876086c);return h.copy(a[0],from,a[2],to);}
 case 28:return h.copy(a[0],ptr<i32>(a[1]),a[2],ptr<i32>(a[3]));
 case 29:break;
 case 30:{auto* out=ptr(a[0]);std::fill(out,out+76,0);std::fill(out+2,out+22,0xffffffff);out[0]=1;out[15]=5;out[22]=out[23]=4096;out[24]=256;out[25]=8192;out[26]=4096;out[27]=16;*reinterpret_cast<float*>(out+28)=1e10f;out[35]=0x10008;out[36]=0x3ffffff;out[37]=out[38]=8;out[39]=0x7f;out[40]=8;out[41]=6;out[42]=4;out[43]=255;*reinterpret_cast<float*>(out+44)=64;out[45]=out[46]=0xfffff;out[47]=16;out[48]=255;out[49]=0xfffe0101;out[50]=96;out[51]=0xffff0104;*reinterpret_cast<float*>(out+52)=8;break;}
 case 31:break;case 32:{u32 mode[]{640,480,60,22};std::memcpy(ptr(a[1]),mode,16);break;}case 33:break;
 default:std::fprintf(stderr,"SDL TH10 device operation %u\n",op);std::abort();
 }return 0;
}
i32 graphics_host_resource(u32 id,u32 op,const u32* a){auto& h=host;if(op==0)return h.add(id);if(op==1)return h.release(id);auto& r=h.get(id);u32 s=r.kind==Resource::Texture?r.child:id;
 switch(op){
 case 2:h.desc(s,ptr(a[0]));break;
 case 3:h.add(r.child);*ptr(a[1])=r.child;break;
 case 4:{auto& surface=h.get(s).s;h.gpu->read(s);u32 output=r.kind==Resource::Texture?a[1]:a[0],rect=r.kind==Resource::Texture?a[2]:a[1];auto* result=ptr(output);const auto* box=ptr<i32>(rect);result[0]=surface.pitch;result[1]=addr(surface.data)+(rect?box[1]*surface.pitch+box[0]*pixel_bytes(surface.format):0);break;}
 case 5:h.get(s).s.version++;break;
 case 6:*ptr(a[2])=addr(r.bytes.data())+a[0];break;
 case 7:case 8:case 9:break;
 default:std::fprintf(stderr,"SDL TH10 resource operation %u\n",op);std::abort();
 }return 0;
}
#endif
__attribute__((export_name("sdl_read_back"))) u32 sdl_read_back(){host.gpu->read(host.back);return addr(host.get(host.back).s.data);}
__attribute__((export_name("sdl_shutdown"))) void sdl_shutdown(){host.gpu.reset();host.resources.clear();host.next=1;host.device=host.back=host.depth=host.target=host.depthTarget=host.bound=host.stream=host.index=0;}
}
