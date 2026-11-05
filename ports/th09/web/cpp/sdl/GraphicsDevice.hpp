#pragma once
#include "../game/ZunGraphics.hpp"
#include "../game/GameResources.hpp"
#include "../game/TextureImage.hpp"
#include "../../../portable/sdl/Renderer.hpp"
#include <map>
namespace th09::multiplayer {class WorldState;}
namespace th09::sdl {
class GraphicsDevice final:public ZunGraphics,public ResourceTextures {
    struct Texture {TextureImage image;u32 revision=0;};
    std::map<u32,Texture> textures;u32 next=3;
    static touhou::sdl::Surface resolve(void*,u32);
    void state(const PipelineState&,u32,VertexLayout);
public:
    // TH09 uses the same alpha rounding profile as TH08; the API is SDL/GLES.
    touhou::sdl::Renderer backend{8,resolve,this};std::string error;
    static constexpr u32 screen=1,depth=2;
    bool skipSubmission=false;
    multiplayer::WorldState* checkpoint=nullptr;
    void before_text(u32);
    bool initialize();
    TextureAllocation create(const AnmTextureSource&,const u8*,u32)override;
    TextureAllocation image(const u8*,u32);
    void destroy(u32)override;
    TextureImage* pixels(u32);void changed(u32);
    void viewport(const Viewport&)override;
    void transform(MatrixKind,const Matrix4&)override;
    void clear(bool color,bool depth,u32,float z=1)override;
    void draw(const PipelineState&,u32,Topology,VertexLayout,const void*,u32)override;
    void triangles(const PipelineState&,u32,const SpriteVertex*,u32)override;
    void present();
};
}
