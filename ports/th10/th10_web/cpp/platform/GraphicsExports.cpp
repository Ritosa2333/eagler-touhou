#include "Graphics.hpp"
#include <new>
#include <cstdlib>
using namespace th10;
extern "C" {
#ifdef TH_SDL3
u32 graphics_host_create(const browser::GraphicsPresentation*,u32);
i32 graphics_host_device(u32,u32,const u32*);
i32 graphics_host_resource(u32,u32,const u32*);

#ifdef TH_NATIVE_PLATFORM
i32 native_graphics_create_surface(u32,u32,u32,u32,bool,void*&);
i32 native_graphics_copy_surface(void*,const TextureRect*,void*,const TextureRect*,u32);
i32 native_graphics_update_surface(void*,const TextureRect&,void*,i32,i32);
u32 native_graphics_retain_resource(void* value);
u32 native_graphics_resource_revision(void* value);
u32 native_graphics_release_resource(void* value);
i32 native_graphics_create_texture(u32 width,u32 height,u32 format,void*& out);
void* native_graphics_texture_surface(void* texture);
TextureDescription native_graphics_describe_surface(void* surface);
TextureLock native_graphics_map_surface(void* surface);
void native_graphics_unmap_surface(void* surface);
void native_graphics_resource_priority(void* value,u32 priority);
void native_graphics_preload_resource(void* value);
i32 native_graphics_create_vertices(u32 bytes,LayoutParameter format,void*& out);
void* native_graphics_map_vertices(void* buffer);
void native_graphics_unmap_vertices(void* buffer);
void native_graphics_vertex_buffer(void* buffer,u32 stride);
void native_graphics_draw_vertices(TopologyParameter primitive,u32 first,u32 count);
void native_graphics_clear_target(u32 flags,u32 color,float depth,u32 stencil,const i32* rectangles,u32 count);
i32 native_graphics_present_frame();
void* native_graphics_back_surface();
i32 native_graphics_reset_presentation(void* parameters);
void native_graphics_clear_shader();
#endif
#ifdef TH_NATIVE_PLATFORM
touhou::graphics::PipelineState& sdl_pipeline();
#else
void sdl_texture_stage(u32,u32,u32);void sdl_sampler(u32,u32,u32);void sdl_render_state(u32,u32);
#endif
void sdl_bind_texture(u32);void sdl_vertex_format(LayoutParameter);void sdl_draw(TopologyParameter,u32,const void*,u32);
void sdl_transform(MatrixParameter,const Matrix4*);void sdl_viewport(const CameraViewport*);
#else
__attribute__((import_module("th10_graphics"),import_name("create"))) u32 graphics_host_create(const browser::GraphicsPresentation*,u32);
__attribute__((import_module("th10_graphics"),import_name("device"))) i32 graphics_host_device(u32,u32,const u32*);
__attribute__((import_module("th10_graphics"),import_name("resource"))) i32 graphics_host_resource(u32,u32,const u32*);
#endif
}
namespace {
struct Host final:browser::GraphicsHost {
#ifdef TH_NATIVE_PLATFORM
    i32 create_surface(u32 w,u32 h,u32 f,u32 pool,bool target,void*& out)override{return native_graphics_create_surface(w,h,f,pool,target,out);}
    i32 copy_surface(void* src,const TextureRect* from,void* dst,const TextureRect* to,u32 filter)override{return native_graphics_copy_surface(src,from,dst,to,filter);}
    i32 update_surface(void* src,const TextureRect& from,void* dst,i32 x,i32 y)override{return native_graphics_update_surface(src,from,dst,x,y);}
#endif
#ifdef TH_SDL3
#ifdef TH_NATIVE_PLATFORM
    PipelineState& pipeline()override{return sdl_pipeline();}
#else
    void texture_stage(u32,u32 stage,u32 key,u32 value)override{sdl_texture_stage(stage,key,value);}
    void sampler(u32,u32 stage,u32 key,u32 value)override{sdl_sampler(stage,key,value);}
    void render_state(u32,u32 key,u32 value)override{sdl_render_state(key,value);}
#endif
    void bind_texture(u32,u32 texture)override{sdl_bind_texture(texture);}
    void vertex_format(u32,LayoutParameter format)override{sdl_vertex_format(format);}
    void draw(u32,TopologyParameter primitive,u32 count,const void* vertices,u32 stride)override{sdl_draw(primitive,count,vertices,stride);}
    void transform(u32,MatrixParameter kind,const Matrix4& matrix)override{sdl_transform(kind,&matrix);}
    void viewport(u32,const CameraViewport& view)override{sdl_viewport(&view);}
#endif
#ifdef TH_NATIVE_PLATFORM
    u32 retain_resource(void* value) override{return native_graphics_retain_resource(value);}
    u32 resource_revision(void* value) override{return native_graphics_resource_revision(value);}
    u32 release_resource(void* value) override{return native_graphics_release_resource(value);}
    i32 create_texture(u32 width,u32 height,u32 format,void*& out) override{return native_graphics_create_texture(width,height,format,out);}
    void* texture_surface(void* texture) override{return native_graphics_texture_surface(texture);}
    TextureDescription describe_surface(void* surface) override{return native_graphics_describe_surface(surface);}
    TextureLock map_surface(void* surface) override{return native_graphics_map_surface(surface);}
    void unmap_surface(void* surface) override{native_graphics_unmap_surface(surface);}
    void resource_priority(void* value,u32 priority) override{native_graphics_resource_priority(value,priority);}
    void preload_resource(void* value) override{native_graphics_preload_resource(value);}
    i32 create_vertices(u32 bytes,LayoutParameter format,void*& out) override{return native_graphics_create_vertices(bytes,format,out);}
    void* map_vertices(void* buffer) override{return native_graphics_map_vertices(buffer);}
    void unmap_vertices(void* buffer) override{native_graphics_unmap_vertices(buffer);}
    void vertex_buffer(void* buffer,u32 stride) override{native_graphics_vertex_buffer(buffer,stride);}
    void draw_vertices(TopologyParameter primitive,u32 first,u32 count) override{native_graphics_draw_vertices(primitive,first,count);}
    void clear_target(u32 flags,u32 color,float depth,u32 stencil,const i32* rectangles,u32 count) override{native_graphics_clear_target(flags,color,depth,stencil,rectangles,count);}
    i32 present_frame() override{return native_graphics_present_frame();}
    void* back_surface() override{return native_graphics_back_surface();}
    i32 reset_presentation(void* parameters) override{return native_graphics_reset_presentation(parameters);}
    void clear_shader() override{native_graphics_clear_shader();}
#endif
    u32 create_device(const browser::GraphicsPresentation& value,u32 flags) override{return graphics_host_create(&value,flags);}
#ifndef TH_NATIVE_PLATFORM
    i32 device(u32 handle,browser::DeviceOperation operation,const u32* arguments) override{return graphics_host_device(handle,static_cast<u32>(operation),arguments);}
    i32 resource(u32 handle,browser::ResourceOperation operation,const u32* arguments) override{return graphics_host_resource(handle,static_cast<u32>(operation),arguments);}
#endif
} host;
}
#define GRAPHICS_EXPORT(name) extern "C" __attribute__((export_name(name)))
GRAPHICS_EXPORT("graphics_allocate") void* graphics_allocate(u32 bytes){return std::calloc(1,bytes);}
GRAPHICS_EXPORT("graphics_free") void graphics_free(void* bytes){std::free(bytes);}
GRAPHICS_EXPORT("graphics_configure_arithmetic") void graphics_configure_arithmetic(u32 flags){if(!(flags&2))arithmetic_mode(Precision::Single,Rounding::NearestEven);}
GRAPHICS_EXPORT("graphics_arithmetic_mode") void graphics_arithmetic_mode(u32 precision,u32 rounding){arithmetic_mode(static_cast<Precision>(precision),static_cast<Rounding>(rounding));}
GRAPHICS_EXPORT("graphics_create") browser::GraphicsDevice* graphics_create(const browser::GraphicsPresentation* params,u32 flags){
    auto* bytes=std::malloc(sizeof(browser::GraphicsDevice));if(!bytes)return nullptr;
    auto* device=new(bytes)browser::GraphicsDevice(host);device->initialize(*params,flags);return device;
}
GRAPHICS_EXPORT("graphics_destroy") void graphics_destroy(browser::GraphicsDevice* device){if(device){device->release();device->~GraphicsDevice();std::free(device);}}
#ifndef TH_NATIVE_PLATFORM
GRAPHICS_EXPORT("graphics_device") i32 graphics_device(browser::GraphicsDevice* device,u32 operation,const u32* arguments){return host.device(device->handle,static_cast<browser::DeviceOperation>(operation),arguments);}
GRAPHICS_EXPORT("graphics_resource") i32 graphics_resource(u32 handle,u32 operation,const u32* arguments){return host.resource(handle,static_cast<browser::ResourceOperation>(operation),arguments);}
#endif
GRAPHICS_EXPORT("graphics_draw") i32 graphics_draw(browser::GraphicsDevice* device,AnmManager* manager,AnmVm* vm,Camera* camera,Camera* world,AnmVertex* quad,u32 operation){
    browser::GraphicsRenderer environment(*device,*manager,*camera,*world,quad);AnmRenderer renderer{*manager,environment};
    if(operation==0){renderer.begin_frame();return 0;}if(operation==1){renderer.flush();return 0;}return renderer.draw(*vm);
}
GRAPHICS_EXPORT("graphics_camera") void graphics_camera(browser::GraphicsDevice* device,AnmManager* manager,Camera* camera,Camera* world,AnmVertex* quad,u32 flat){
    browser::GraphicsRenderer renderer(*device,*manager,*camera,*world,quad);browser::GraphicsCamera environment(renderer);
    if(flat)camera->configure_flat(environment);else camera->configure_world(environment);
}
