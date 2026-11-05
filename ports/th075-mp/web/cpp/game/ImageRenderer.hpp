#pragma once
#include "SpriteRenderer.hpp"
namespace th075 {
// Screen-space images used by backgrounds, menus and the HUD. Image dimensions
// remain separate from the padded GPU texture dimensions.
class ImageRenderer {
public:
    ZunGraphics& graphics;
    SpriteResources& resources;
    ImageRenderer(ZunGraphics& g,SpriteResources& r):graphics(g),resources(r){}
    void blend(SpriteBlend mode){graphics.depth(mode==SpriteBlend::AlphaAdd||mode==SpriteBlend::InverseColor||mode==SpriteBlend::MultiplyAdd?DepthMode::WriteDisabled:DepthMode::WriteEnabled);graphics.blend(mode);}
    void image(i16 index,float x,float y,u32 color,float anchorX,float anchorY,float scale=1,float rotation=0,u8 flags=0,bool centeredScale=false);
    void quad(i16 index,const QuadPoints&,const CollisionRectangle&,u32 color);
    void projected(i16 index,const QuadPoints&,const CollisionRectangle&,float x,float y,float distance,u32 color);
    void strip(i16 index,ScreenVertex*,u32 count);
};
}
