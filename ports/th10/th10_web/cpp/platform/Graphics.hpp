#pragma once
#include "../game/GraphicsMath.hpp"
#include "../game/TexturePlatform.hpp"
#include <initializer_list>
namespace th10::browser {
#ifndef TH_NATIVE_PLATFORM
enum class DeviceOperation:u32 {TextureStage,Sampler,RenderState,Texture,VertexFormat,Triangles,Transform,Stream,DrawBuffer,Viewport,Clear,Begin,End,Present,BackBuffer,GetTarget,SetTarget,GetDepth,SetDepth,CreateTexture,CreateSurface,CreateTarget,CreateVertices,CreateIndices,Indices,DrawIndexed,DrawIndexedUp,Stretch,UpdateSurface,Reset,Caps,Cooperative,DisplayMode,PixelShader};
enum class ResourceOperation:u32 {AddRef,Release,Description,Surface,LockSurface,UnlockSurface,LockBuffer,UnlockBuffer,Priority,Preload};
#endif
struct GraphicsPresentation {u32 width=640,height=480,format=22,back_buffers=1,multisample=0,quality=0,swap_effect=1,window=0,windowed=1,depth_enabled=1,depth_format=80,flags=0,refresh=0,interval=0;};
static_assert(sizeof(GraphicsPresentation)==56);
struct ZunGraphics
#ifdef TH_NATIVE_PLATFORM
 : StateCommands
#endif
{
    virtual u32 create_device(const GraphicsPresentation& presentation,u32 flags)=0;
#ifndef TH_NATIVE_PLATFORM
    virtual i32 device(u32 handle,DeviceOperation operation,const u32* arguments)=0;
    virtual i32 resource(u32 handle,ResourceOperation operation,const u32* arguments)=0;
#endif
    // Typed native rendering boundary. Numbered ABI adapters compile only in
    // independent original-executable comparison fixtures.
#ifdef TH_NATIVE_PLATFORM
    virtual void bind_texture(u32 h,u32 texture)=0;
    virtual void vertex_format(u32 h,LayoutParameter format)=0;
    virtual void draw(u32 h,TopologyParameter primitive,u32 count,const void* vertices,u32 stride)=0;
    virtual void transform(u32 h,MatrixParameter kind,const Matrix4& matrix)=0;
    virtual void viewport(u32 h,const CameraViewport& view)=0;
    virtual u32 retain_resource(void* value)=0;
    virtual u32 resource_revision(void* value)=0;
    virtual u32 release_resource(void* value)=0;
    virtual i32 create_texture(u32 width,u32 height,u32 format,void*& out)=0;
    virtual void* texture_surface(void* texture)=0;
    virtual TextureDescription describe_surface(void* surface)=0;
    virtual TextureLock map_surface(void* surface)=0;
    virtual void unmap_surface(void* surface)=0;
    virtual void resource_priority(void* value,u32 priority)=0;
    virtual void preload_resource(void* value)=0;
    virtual i32 create_vertices(u32 bytes,LayoutParameter format,void*& out)=0;
    virtual void* map_vertices(void* buffer)=0;
    virtual void unmap_vertices(void* buffer)=0;
    virtual void vertex_buffer(void* buffer,u32 stride)=0;
    virtual void draw_vertices(TopologyParameter primitive,u32 first,u32 count)=0;
    virtual void clear_target(u32 flags,u32 color,float depth,u32 stencil,const i32* rectangles,u32 count)=0;
    virtual i32 present_frame()=0;
    virtual void* back_surface()=0;
    virtual i32 reset_presentation(void* parameters)=0;
    virtual void clear_shader()=0;
    virtual i32 create_surface(u32 width,u32 height,u32 format,u32 pool,bool target,void*& out)=0;
    virtual i32 copy_surface(void* source,const TextureRect* region,void* destination,const TextureRect* target,u32 filter)=0;
    virtual i32 update_surface(void* source,const TextureRect& region,void* destination,i32 x,i32 y)=0;

#else
    virtual void texture_stage(u32 h,u32 stage,u32 setting,u32 value){u32 a[]{stage,setting,value};device(h,DeviceOperation::TextureStage,a);}
    virtual void sampler(u32 h,u32 stage,u32 setting,u32 value){u32 a[]{stage,setting,value};device(h,DeviceOperation::Sampler,a);}
    virtual void render_state(u32 h,u32 setting,u32 value){u32 a[]{setting,value};device(h,DeviceOperation::RenderState,a);}
    virtual void bind_texture(u32 h,u32 texture){u32 a[]{0,texture};device(h,DeviceOperation::Texture,a);}
    virtual void vertex_format(u32 h,LayoutParameter format){device(h,DeviceOperation::VertexFormat,&format);}
    virtual void draw(u32 h,TopologyParameter primitive,u32 count,const void* vertices,u32 stride){u32 a[]{primitive,count,u32(reinterpret_cast<uintptr_t>(vertices)),stride};device(h,DeviceOperation::Triangles,a);}
    virtual void transform(u32 h,MatrixParameter kind,const Matrix4& matrix){u32 a[]{kind,u32(reinterpret_cast<uintptr_t>(&matrix))};device(h,DeviceOperation::Transform,a);}
    virtual void viewport(u32 h,const CameraViewport& view){u32 a=u32(reinterpret_cast<uintptr_t>(&view));device(h,DeviceOperation::Viewport,&a);}
#endif
};
using GraphicsHost=ZunGraphics;
struct GraphicsDevice {
    GraphicsHost& host;u32 handle=0;
    explicit GraphicsDevice(GraphicsHost& host):host(host){}
    void initialize(const GraphicsPresentation& presentation,u32 flags=0x40){handle=host.create_device(presentation,flags);}
#ifndef TH_NATIVE_PLATFORM
    i32 call(DeviceOperation operation,std::initializer_list<u32> args={}){return host.device(handle,operation,args.begin());}
    i32 resource(void* value,ResourceOperation operation,std::initializer_list<u32> args={}){return host.resource(static_cast<u32>(reinterpret_cast<uintptr_t>(value)),operation,args.begin());}
#endif
    static u32 address(const void* p){return u32(reinterpret_cast<uintptr_t>(p));}
    i32 create_surface(u32 width,u32 height,u32 format,u32 pool,bool target,void*& out){
#ifdef TH_NATIVE_PLATFORM
        return host.create_surface(width,height,format,pool,target,out);
#else
        return target?call(DeviceOperation::CreateTarget,{width,height,format,0,0,0,address(&out),0}):call(DeviceOperation::CreateSurface,{width,height,format,pool,address(&out),0});
#endif
    }
    i32 copy_surface(void* source,const TextureRect* region,void* destination,const TextureRect* target,u32 filter){
#ifdef TH_NATIVE_PLATFORM
        return host.copy_surface(source,region,destination,target,filter);
#else
        return call(DeviceOperation::Stretch,{address(source),address(region),address(destination),address(target),filter});
#endif
    }
    i32 update_surface(void* source,const TextureRect& region,void* destination,i32 x,i32 y){
#ifdef TH_NATIVE_PLATFORM
        return host.update_surface(source,region,destination,x,y);
#else
        i32 point[]{x,y};return call(DeviceOperation::UpdateSurface,{address(source),address(&region),address(destination),address(point)});
#endif
    }
#ifdef TH_NATIVE_PLATFORM
    u32 resource_revision(void* value){return host.resource_revision(value);}
#endif
    u32 retain_resource(void* value){
#ifdef TH_NATIVE_PLATFORM
        return host.retain_resource(value);
#else
        return resource(value,ResourceOperation::AddRef);
#endif
    }
    u32 release_resource(void* value){
#ifdef TH_NATIVE_PLATFORM
        return host.release_resource(value);
#else
        return resource(value,ResourceOperation::Release);
#endif
    }
    i32 create_texture(u32 width,u32 height,u32 format,void*& out){
#ifdef TH_NATIVE_PLATFORM
        return host.create_texture(width,height,format,out);
#else
        return call(DeviceOperation::CreateTexture,{width,height,1,0,format,1,address(&out),0});
#endif
    }
    void* texture_surface(void* texture){
#ifdef TH_NATIVE_PLATFORM
        return host.texture_surface(texture);
#else
        void* out=nullptr;resource(texture,ResourceOperation::Surface,{0,address(&out)});return out;
#endif
    }
    TextureDescription describe_surface(void* surface){
#ifdef TH_NATIVE_PLATFORM
        return host.describe_surface(surface);
#else
        u32 out[8]{};resource(surface,ResourceOperation::Description,{address(out)});return {out[0],out[6],out[7]};
#endif
    }
    TextureLock map_surface(void* surface){
#ifdef TH_NATIVE_PLATFORM
        return host.map_surface(surface);
#else
        TextureLock out{};resource(surface,ResourceOperation::LockSurface,{address(&out),0,0});return out;
#endif
    }
    void unmap_surface(void* surface){
#ifdef TH_NATIVE_PLATFORM
        host.unmap_surface(surface);
#else
        resource(surface,ResourceOperation::UnlockSurface);
#endif
    }
    void resource_priority(void* value,u32 priority){
#ifdef TH_NATIVE_PLATFORM
        host.resource_priority(value,priority);
#else
        resource(value,ResourceOperation::Priority,{priority});
#endif
    }
    void preload_resource(void* value){
#ifdef TH_NATIVE_PLATFORM
        host.preload_resource(value);
#else
        resource(value,ResourceOperation::Preload);
#endif
    }
    i32 create_vertices(u32 bytes,LayoutParameter format,void*& out){
#ifdef TH_NATIVE_PLATFORM
        return host.create_vertices(bytes,format,out);
#else
        return call(DeviceOperation::CreateVertices,{bytes,0,format,1,address(&out),0});
#endif
    }
    void* map_vertices(void* buffer){
#ifdef TH_NATIVE_PLATFORM
        return host.map_vertices(buffer);
#else
        void* out=nullptr;resource(buffer,ResourceOperation::LockBuffer,{0,0,address(&out),0});return out;
#endif
    }
    void unmap_vertices(void* buffer){
#ifdef TH_NATIVE_PLATFORM
        host.unmap_vertices(buffer);
#else
        resource(buffer,ResourceOperation::UnlockBuffer);
#endif
    }
    void vertex_buffer(void* buffer,u32 stride){
#ifdef TH_NATIVE_PLATFORM
        host.vertex_buffer(buffer,stride);
#else
        call(DeviceOperation::Stream,{0,address(buffer),0,stride});
#endif
    }
    void draw_vertices(TopologyParameter primitive,u32 first,u32 count){
#ifdef TH_NATIVE_PLATFORM
        host.draw_vertices(primitive,first,count);
#else
        call(DeviceOperation::DrawBuffer,{primitive,first,count});
#endif
    }
    void clear_target(u32 flags,u32 color,float depth,u32 stencil,const i32* rectangles,u32 count){
#ifdef TH_NATIVE_PLATFORM
        host.clear_target(flags,color,depth,stencil,rectangles,count);
#else
        u32 bits;std::memcpy(&bits,&depth,4);call(DeviceOperation::Clear,{count,address(rectangles),flags,color,bits,stencil});
#endif
    }
    i32 present_frame(){
#ifdef TH_NATIVE_PLATFORM
        return host.present_frame();
#else
        return call(DeviceOperation::Present,{0,0,0,0});
#endif
    }
    void* back_surface(){
#ifdef TH_NATIVE_PLATFORM
        return host.back_surface();
#else
        void* out=nullptr;call(DeviceOperation::BackBuffer,{0,0,0,address(&out)});return out;
#endif
    }
    i32 reset_presentation(void* parameters){
#ifdef TH_NATIVE_PLATFORM
        return host.reset_presentation(parameters);
#else
        return call(DeviceOperation::Reset,{address(parameters)});
#endif
    }
    void clear_shader(){
#ifdef TH_NATIVE_PLATFORM
        host.clear_shader();
#else
        call(DeviceOperation::PixelShader,{0});
#endif
    }
    void release(){if(handle){release_resource(reinterpret_cast<void*>(uintptr_t(handle)));handle=0;}}
    void viewport(const CameraViewport& view){host.viewport(handle,view);}
#ifndef TH_NATIVE_PLATFORM
    i32 render_state(u32 key,u32 value){host.render_state(handle,key,value);return 0;}
    void texture_stage(u32 stage,u32 key,u32 value){host.texture_stage(handle,stage,key,value);}
    void sampler(u32 stage,u32 key,u32 value){host.sampler(handle,stage,key,value);}
#endif
    void texture(void* value){host.bind_texture(handle,address(value));}
    i32 begin_scene(){
#ifdef TH_NATIVE_PLATFORM
        return 0;
#else
        return call(DeviceOperation::Begin);
#endif
    }
    void end_scene(){
#ifndef TH_NATIVE_PLATFORM
        call(DeviceOperation::End);
#endif
    }

};
struct GraphicsRenderer final:AnmRenderEnvironment,AnmProjectionEnvironment {
    GraphicsDevice& device;Camera& camera;
    GraphicsRenderer(GraphicsDevice& device,AnmManager& manager,Camera& active,Camera& world,AnmVertex* vertices);
#ifdef TH_NATIVE_PLATFORM
    PipelineState& pipeline()override{return device.host.pipeline();}
#else
    void texture_stage(u32 stage,u32 setting,u32 value) override;
    void sampler_state(u32 stage,u32 setting,u32 value) override;
    void render_state(u32 setting,u32 value) override;
#endif
    void set_texture(void* texture) override;
    void vertex_format(LayoutParameter format) override;
    void draw_triangles(TopologyParameter primitive,u32 count,const void* vertices,u32 stride) override;
    void set_transform(MatrixParameter kind,const Matrix4& matrix) override;
    void stream_source(void* buffer,u32 stride) override;
    void draw_buffer(TopologyParameter primitive,u32 first,u32 count) override;
    void rotation(Matrix4& matrix,u32 axis,float radians) override;
    void multiply(Matrix4& output,const Matrix4& first,const Matrix4& second) override;
    void project(Vec3& output,const Vec3& input,const Matrix4& world) override;
    void transform(float* output,const Vec3& input,const Matrix4& world) override;
    i32 special_draw(AnmManager& manager,AnmVm& vm,u32 mode) override;
};
struct GraphicsCamera final:CameraEnvironment {
    GraphicsRenderer& renderer;AnmManager* manager;
    explicit GraphicsCamera(GraphicsRenderer& renderer):renderer(renderer),manager(renderer.global_manager){animation_manager=&manager;render_environment=&renderer;}
    void look_at(Matrix4& matrix,const Vec3& eye,const Vec3& target,const Vec3& up) override {GraphicsMath::look_at(matrix,eye,target,up);}
    void perspective(Matrix4& matrix,float fov,float aspect,float near_plane,float far_plane) override {GraphicsMath::perspective(matrix,fov,aspect,near_plane,far_plane);}
    void normalize(Vec3& value) override {GraphicsMath::normalize(value,value);}
    void set_viewport(const CameraViewport& viewport) override;
};
}
