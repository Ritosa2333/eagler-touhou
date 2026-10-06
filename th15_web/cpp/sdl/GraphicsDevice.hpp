#pragma once
#include "../game/ZunGraphics.hpp"
#include "../game/TextureImage.hpp"
#include "../../../portable/sdl/Renderer.hpp"
#include <map>
#include "PresentationFrame.hpp"
namespace th15::sdl {
class GraphicsDevice final:public ZunGraphics {
    struct Texture {TextureImage image;u32 revision=0,renderScale=1;};
    std::map<u32,Texture> textures;
    std::map<const AnmResource*,std::vector<u32>> resources;
    u32 next_handle=3;
    static touhou::sdl::Surface resolve(void*,u32);
public:
    // Profile 10 selects shared float-alpha/D16 precision and world-quad
    // instancing. It is a precision profile, not a Windows graphics API.
    touhou::sdl::Renderer backend{10,resolve,this};
    static constexpr u32 screen=1,depth=2;
    std::string error;u32 render_scale=1;
    PresentationFrame presentation;
    void presentation_offset(Vec2 value)override{presentation.offset=value;}
    void presentation_range(const AnmVm& vm,u32 first,u32 count,u32 instance=0)override{presentation.range(vm,first,count,instance);}
    void presentation_camera(bool enabled)override{presentation.camera=enabled;}
    bool initialize();
    bool preload(AnmResource&,bool low_color=false);
    void unload(const AnmResource&);
    u32 texture(const AnmResource&,u32)override;
    TextureImage* pixels(u32 handle){auto it=textures.find(handle);return it==textures.end()?nullptr:&it->second.image;}
    void changed(u32 handle,std::array<u32,4> region={}){auto it=textures.find(handle);if(it!=textures.end()){backend.flush();backend.changed(handle,it->second.revision,region);++it->second.revision;}}
    void bind_texture(u32 handle)override{backend.state.texture=handle;}
    touhou::graphics::PipelineState& pipeline()override{return backend.pipeline();}
    void set_layout(touhou::graphics::VertexLayout layout)override{backend.state.layout=touhou::graphics::attributes(layout);}
    void set_matrix(touhou::graphics::MatrixKind kind,const Matrix4& matrix)override{backend.transform(kind,matrix.m);}
    void triangles(u32 count,const void* data,u32 stride)override{presentation.draw(backend.state,touhou::graphics::Topology::Triangles,count,data,stride,true);backend.draw_batch(count,data,stride);}
    void primitives(touhou::graphics::Topology kind,u32 count,const void* data,u32 stride)override{presentation.draw(backend.state,kind,count,data,stride,false);backend.draw(kind,count,data,stride);}
    bool select_target(const AnmResource*,u32)override;
    bool clear_target(u32,const GraphicsViewport*)override;
    bool clear_depth(const GraphicsViewport*)override;
    bool copy_surface(u32 source,const i32* region,u32 target,const i32* point);
    bool resample_surface(u32 source,const i32* region,u32 target,const i32* destination);
    bool upload_png(u32 target,const u8*,u32 size);
    bool clear_image(u32 target);
    void set_viewport(const GraphicsViewport& v)override{viewport(v);}
    void viewport(const GraphicsViewport& v){backend.viewport({v.x,v.y,v.width,v.height,v.near_depth,v.far_depth});}
    void clear(u32 color,bool clear_depth=true){presentation.clear(backend.state,clear_depth?3:1,color,nullptr);backend.clear(clear_depth?3:1,color,1,0);}
    void present(){backend.present(screen);}
};
}
