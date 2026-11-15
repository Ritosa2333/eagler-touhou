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
    u32 next_handle=3;
    static touhou::sdl::Surface resolve(void*,u32);
    bool prepare_texture(AnmResource&,u32,u32&,bool);
public:
    // Profile 10 selects shared float-alpha/D16 precision and world-quad
    // instancing. It is a precision profile, not a Windows graphics API.
    touhou::sdl::Renderer backend{10,resolve,this};
    static constexpr u32 screen=1,depth=2;
    std::string error;
#if PORT_TITLE == 14
    bool initialize(u32 width=640,u32 height=480);
#else
    bool initialize();
#endif
    bool preload(AnmResource&,bool low_color=false);
#if PORT_TITLE == 14
    const bool* texture_high_precision=nullptr;
    bool preload_chunk(AnmResource&,u32,bool low_color=false);
    bool manual_image(AnmResource&,u32,const u8*,u32);
#endif
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
    bool clear_target(u32,const GraphicsViewport*,u32 flags=3)override;
    void set_viewport(const GraphicsViewport& v)override{viewport(v);}
    void viewport(const GraphicsViewport& v){backend.viewport({v.x,v.y,v.width,v.height,v.near_depth,v.far_depth});}
    void clear(u32 color,bool clear_depth=true){backend.clear(clear_depth?3:1,color,1,0);}
    bool backbuffer_has_alpha()const noexcept override{auto i=textures.find(screen);return i!=textures.end()&&i->second.image.format==touhou::graphics::PixelFormat::Bgra8;}
    void present(){backend.present(screen);}
};
}
