#pragma once
#include <array>
#include <cstdint>
#include <tuple>

namespace touhou::graphics {
using u32 = std::uint32_t;
enum class Compare : u32 { Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always };
enum class BlendFactor : u32 { Zero, One, SourceColor, InverseSourceColor, SourceAlpha, InverseSourceAlpha, DestinationAlpha, InverseDestinationAlpha, DestinationColor, InverseDestinationColor, SourceAlphaSaturate };
enum class BlendEquation : u32 { Add, Subtract, ReverseSubtract, Minimum, Maximum };
enum class Cull : u32 { None, Back, Front };
enum class FogMode : u32 { None, Exponential, ExponentialSquared, Linear };
enum class ColorOperation : u32 { Disabled, First, Second, Multiply, Multiply2, Multiply4, Add, AddSigned, AddSigned2, Subtract, SmoothAdd, BlendDiffuseAlpha, BlendTextureAlpha, BlendFactorAlpha, BlendPremultipliedTextureAlpha, BlendCurrentAlpha };
enum class ArgumentSource : u32 { Diffuse, Texture, Factor, Specular };
enum class Component : u32 { Color, Alpha };
enum class ArgumentSlot : u32 { First, Second };
enum class Filter : u32 { Nearest, Linear };
enum class Address : u32 { Repeat, Mirror, Clamp };
enum class MatrixKind : u32 { World, View, Projection, Texture };
enum class Topology : u32 { Points, Lines, LineStrip, Triangles, Strip, Fan };
enum class VertexLayout : u32 { WorldUv, WorldColorUv, ScreenColorUv, ScreenColor, ScreenUv };
enum class IndexType : u32 { UInt16, UInt32 };
enum class PixelFormat : u32 { Bgr8, Bgra8, Bgrx8, Rgb565, Xrgb1555, Argb1555, Argb4444, Alpha8, Luminance8, LuminanceAlpha8, Depth16, Depth24, Depth24Stencil8 };

struct Argument {
    ArgumentSource source = ArgumentSource::Diffuse;
    bool complement = false, alphaOnly = false;
    bool operator==(const Argument& b) const { return source==b.source && complement==b.complement && alphaOnly==b.alphaOnly; }
};
struct Combiner {
    ColorOperation operation = ColorOperation::Multiply;
    Argument first{ArgumentSource::Texture}, second{};
    bool operator==(const Combiner& b) const { return operation==b.operation && first==b.first && second==b.second; }
};
struct PipelineState {
    bool depthTest=false, depthWrite=true, blend=false, alphaTest=false, fog=false, rangeFog=false, dither=false;
    Compare depthCompare=Compare::LessEqual, alphaCompare=Compare::Always;
    BlendFactor sourceBlend=BlendFactor::One, destinationBlend=BlendFactor::Zero;
    BlendEquation blendEquation=BlendEquation::Add;
    Cull cull=Cull::None;
    u32 colorMask=15, alphaReference=0, fogColor=0, textureFactor=0xffffffff;
    FogMode fogMode=FogMode::None;
    float fogNear=0, fogFar=1, fogDensity=1;
    Combiner color{}, alpha{ColorOperation::First};
    bool textureTransform=false;
    Filter minFilter=Filter::Linear, magFilter=Filter::Linear;
    Address addressU=Address::Repeat, addressV=Address::Repeat;
    // Texture factor and world matrix can vary per instance; all other state
    // must match before preserving draw order in an instanced batch.
    auto key() const { return std::tie(depthTest,depthWrite,blend,alphaTest,fog,rangeFog,dither,depthCompare,alphaCompare,sourceBlend,destinationBlend,blendEquation,cull,colorMask,alphaReference,fogColor,fogMode,fogNear,fogFar,fogDensity,textureTransform,minFilter,magFilter,addressU,addressV); }
    bool compatible(const PipelineState& b, bool instances=false) const { return key()==b.key() && color==b.color && alpha==b.alpha && (instances || textureFactor==b.textureFactor); }
};

// Game and platform owners share semantic operations. No numeric device-state
// dispatch or Windows graphics API is involved in this contract.
struct StateCommands {
    virtual ~StateCommands() = default;
    virtual PipelineState& pipeline() = 0;
    void set_depth_test(bool v) { pipeline().depthTest=v; }
    void set_depth_mask(bool v) { pipeline().depthWrite=v; }
    void set_depth_compare(Compare v) { pipeline().depthCompare=v; }
    void set_blend(bool v) { pipeline().blend=v; }
    void set_source_blend(BlendFactor v) { pipeline().sourceBlend=v; }
    void set_destination_blend(BlendFactor v) { pipeline().destinationBlend=v; }
    void set_fog(bool v) { pipeline().fog=v; }
    void set_fog_color(u32 v) { pipeline().fogColor=v; }
    void set_fog_range(float a,float b) { auto& p=pipeline();p.fogNear=a;p.fogFar=b; }
    void set_texture_factor(u32 v) { pipeline().textureFactor=v; }
    void set_color_operation(Component c,ColorOperation op) { auto& p=pipeline();(c==Component::Color?p.color:p.alpha).operation=op; }
    void set_texture_argument(Component c,ArgumentSlot slot,Argument value) { auto& p=pipeline();auto& combiner=c==Component::Color?p.color:p.alpha;(slot==ArgumentSlot::First?combiner.first:combiner.second)=value; }
    void set_texture_filter(Filter min,Filter mag) { auto& p=pipeline();p.minFilter=min;p.magFilter=mag; }
    void configure_game(u32 alphaRef) {
        auto& p=pipeline();p.depthTest=true;p.blend=true;p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::InverseSourceAlpha;
        p.depthCompare=Compare::Always;p.alphaTest=true;p.alphaReference=alphaRef;p.alphaCompare=Compare::GreaterEqual;
        p.cull=Cull::None;p.fog=true;p.fogMode=FogMode::Linear;p.fogDensity=1;p.fogColor=0xffa0a0a0;p.fogNear=1000;p.fogFar=5000;
        p.color.operation=p.alpha.operation=ColorOperation::Multiply;p.color.first=p.alpha.first={ArgumentSource::Texture};p.minFilter=p.magFilter=Filter::Linear;p.addressU=p.addressV=Address::Repeat;p.color.second=p.alpha.second={ArgumentSource::Factor};p.textureTransform=true;
    }
};

struct VertexAttributes {
    static constexpr u32 absent=0xffffffff;
    bool screen=false;
    u32 diffuse=absent, specular=absent, uv=12;
    bool operator==(const VertexAttributes& b) const { return screen==b.screen && diffuse==b.diffuse && specular==b.specular && uv==b.uv; }
    bool operator!=(const VertexAttributes& b) const { return !(*this==b); }
};
constexpr VertexAttributes attributes(VertexLayout layout) {
    return layout==VertexLayout::WorldUv?VertexAttributes{false,VertexAttributes::absent,VertexAttributes::absent,12}:
        layout==VertexLayout::WorldColorUv?VertexAttributes{false,12,VertexAttributes::absent,16}:
        layout==VertexLayout::ScreenColorUv?VertexAttributes{true,16,VertexAttributes::absent,20}:
        layout==VertexLayout::ScreenColor?VertexAttributes{true,16,VertexAttributes::absent,VertexAttributes::absent}:
        VertexAttributes{true,VertexAttributes::absent,VertexAttributes::absent,16};
}
constexpr u32 stride(VertexLayout v) { return v==VertexLayout::ScreenColorUv?28:v==VertexLayout::WorldColorUv?24:20; }
constexpr u32 vertex_count(Topology p,u32 n) { return p==Topology::Points?n:p==Topology::Lines?n*2:p==Topology::LineStrip?n+1:p==Topology::Triangles?n*3:n+2; }

// These aliases preserve the original harness signatures in comparison-only
// builds. Production owners use the enums above all the way to GLES.
#ifdef TH_NATIVE_PLATFORM
using BlendParameter=BlendFactor;
using MatrixParameter=MatrixKind;
using TopologyParameter=Topology;
using LayoutParameter=VertexLayout;
#else
using BlendParameter=u32;
using MatrixParameter=u32;
using TopologyParameter=u32;
using LayoutParameter=u32;
#endif
namespace Blends {
#ifdef TH_NATIVE_PLATFORM
constexpr auto One=BlendFactor::One, InverseSourceAlpha=BlendFactor::InverseSourceAlpha;
#else
constexpr u32 One=2, InverseSourceAlpha=6;
#endif
}
namespace Matrices {
#ifdef TH_NATIVE_PLATFORM
constexpr auto World=MatrixKind::World, View=MatrixKind::View, Projection=MatrixKind::Projection, Texture=MatrixKind::Texture;
#else
constexpr u32 World=256, View=2, Projection=3, Texture=16;
#endif
}
namespace Primitives {
#ifdef TH_NATIVE_PLATFORM
constexpr auto Points=Topology::Points, Lines=Topology::Lines, LineStrip=Topology::LineStrip, Triangles=Topology::Triangles, Strip=Topology::Strip, Fan=Topology::Fan;
#else
constexpr u32 Points=1, Lines=2, LineStrip=3, Triangles=4, Strip=5, Fan=6;
#endif
}
namespace Layouts {
#ifdef TH_NATIVE_PLATFORM
constexpr auto World=VertexLayout::WorldUv, ColoredWorld=VertexLayout::WorldColorUv, Screen=VertexLayout::ScreenColorUv, Untextured=VertexLayout::ScreenColor;
#else
constexpr u32 World=0x102, ColoredWorld=0x142, Screen=0x144, Untextured=0x44;
#endif
}
}
