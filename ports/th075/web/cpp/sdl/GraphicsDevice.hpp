#pragma once
#include "../game/ZunGraphics.hpp"
#include "../game/BitmapData.hpp"
#include "../../../portable/sdl/Renderer.hpp"
#include <map>
namespace th075::sdl {
class GraphicsDevice final:public GfxInterface {
    struct Image {TextureSize size;PixelFormat format=PixelFormat::Bgra8;u32 pitch=0,revision=0,scale=1;std::vector<u8> bytes;};
    std::map<u32,Image> images;u32 nextHandle=3;
    std::vector<ScreenVertex> vertices; touhou::sdl::State batchState;
    void flush_vertices();
    static touhou::sdl::Surface resolve(void*,u32);
public:
    // 8 is the original alpha/raster precision profile. The implementation is
    // exactly the shared SDL3/GLES renderer; no Direct3D device exists here.
    touhou::sdl::Renderer renderer{8,resolve,this};
    static constexpr u32 screen=1,depthBuffer=2;
    std::string error;
    bool initialize(u32 renderScale=2);
    u32 render_scale()const{return images.at(screen).scale;}
    bool preload(const TexturePack&,std::vector<u32>& handles);
    u32 upload(const BitmapData&);
    u32 create_surface(u32 width,u32 height,u32 scale=1);
    bool update_pixels(u32 handle,const void*,u32 bytes);
    u32 snapshot();
    void destroy(u32 handle){unload({handle});}
    void unload(const std::vector<u32>&);
    PipelineState& pipeline()override{return renderer.pipeline();}
    TextureSize texture_size(u32)override;
    void draw(u32,Topology,const ScreenVertex*,u32)override;
    void clear_depth()override{flush_vertices();renderer.clear(2,0,1,0);}
    void clear(u32 color){flush_vertices();renderer.clear(3,color,1,0);}
    void present(){flush_vertices();renderer.present(screen);}
    const u8* capture_pixels(){flush_vertices();renderer.read(screen);return images.at(screen).bytes.data();}
};
}


