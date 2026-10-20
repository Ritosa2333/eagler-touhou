#pragma once
#include "BattleGeometry.hpp"
#include "../../../portable/sdl/GraphicsState.hpp"
namespace th075 {
using namespace touhou::graphics;
struct ScreenVertex {float x=0,y=0,z=0.5f,reciprocalW=1;u32 color=0xffffffff;float u=0,v=0;};
struct TextureSize {u32 width=0,height=0;};
struct GfxInterface:StateCommands {
    virtual TextureSize texture_size(u32)=0;
    virtual void draw(u32 texture,Topology,const ScreenVertex*,u32 vertices)=0;
    virtual void clear_depth()=0;
};
enum class SpriteBlend:u8 {Alpha,Opaque,AlphaAdd,InverseColor,Multiply,Add,MultiplyAdd,InverseDestination};
enum class SpriteAlpha:u8 {Disabled,Texture,Vertex};
enum class DepthMode:u8 {Disabled,Enabled,WriteEnabled,WriteDisabled,LessEqual,Always,Less,GreaterEqual,Clear};
class ZunGraphics {
public:
    GfxInterface& backend;
    float colorMultiplier=1,drawScale=1,originX=0,originY=0;
    explicit ZunGraphics(GfxInterface& b):backend(b){}
    void monochrome(bool);
    void blend(SpriteBlend);void alpha(SpriteAlpha);void depth(DepthMode);void filter(Filter);
    void filled_rectangle(const CollisionRectangle&,u32 color);
    void textured_quad(const std::array<std::array<float,4>,4>&,const CollisionRectangle&,u32 color,u32 texture);
    bool projected_quad(const std::array<std::array<float,4>,4>&,const CollisionRectangle&,float centerX,float centerY,float distance,u32 color,u32 texture);
    void projected_strip(ScreenVertex*,u32 vertices,u32 texture);
private:
    bool monochromeEnabled=false;Combiner previousColor{},previousAlpha{};
    u32 modulate(u32)const;void transform(ScreenVertex&)const;
};
}
