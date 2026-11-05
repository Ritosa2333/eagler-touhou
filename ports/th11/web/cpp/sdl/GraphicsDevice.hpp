#pragma once
#include "../game/ZunGraphics.hpp"
#include "../game/TextureImage.hpp"
#include "../game/GraphicsMath.hpp"
#include "../../../portable/sdl/Renderer.hpp"
#include <map>
namespace th11::sdl {
class GraphicsDevice final:public ZunGraphics {
    struct Texture {TextureImage image;u32 revision=0;};
    std::map<u32,Texture> textures;
    std::map<const AnmResource*,std::vector<u32>> resources;
    u32 next_handle=3,startup_credit=0;
    static touhou::sdl::Surface resolve(void*,u32);
public:
    // Profile 10 selects shared float-alpha/D16 precision and world-quad
    // instancing. It is a precision profile, not a Windows graphics API.
    touhou::sdl::Renderer backend{10,resolve,this};
    static constexpr u32 screen=1,depth=2;
    std::string error;
    bool initialize();
    void draw_startup_branding(u32 tint);
    void release_startup_branding();
    bool preload(AnmResource&,bool low_color=false);
    void unload(const AnmResource&);
    u32 texture(const AnmResource&,u32)override;
    TextureImage* pixels(u32 handle){auto it=textures.find(handle);return it==textures.end()?nullptr:&it->second.image;}
    void changed(u32 handle){auto it=textures.find(handle);if(it!=textures.end()){backend.flush();++it->second.revision;}}
    void bind_texture(u32 handle)override{backend.state.texture=handle;}
    touhou::graphics::PipelineState& pipeline()override{return backend.pipeline();}
    void set_layout(touhou::graphics::VertexLayout layout)override{backend.state.layout=touhou::graphics::attributes(layout);}
    void set_matrix(touhou::graphics::MatrixKind kind,const Matrix4& matrix)override{backend.transform(kind,matrix.m);}
    void triangles(u32 count,const void* data,u32 stride)override{backend.draw_batch(count,data,stride);}
    void primitives(touhou::graphics::Topology kind,u32 count,const void* data,u32 stride)override{backend.draw(kind,count,data,stride);}
    bool select_target(const AnmResource*,u32)override;
    bool clear_target(u32,const GraphicsViewport*)override;
    void set_viewport(const GraphicsViewport& v)override{viewport(v);}
    void viewport(const GraphicsViewport& v){backend.viewport({v.x,v.y,v.width,v.height,v.near_depth,v.far_depth});}
    void clear(u32 color,bool clear_depth=true){backend.clear(clear_depth?3:1,color,1,0);}
    void present(){backend.present(screen);}
};
}
