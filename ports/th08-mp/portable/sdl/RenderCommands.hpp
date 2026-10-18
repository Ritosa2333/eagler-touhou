#pragma once
#include <cstdint>
#include "GraphicsState.hpp"
#include <cstring>

namespace touhou::graphics {
enum class DepthFunc {LessEqual,Always};
enum class ColorOp {SelectFirst,Modulate};
enum class TextureArg {Diffuse,Texture,Factor};
enum class BlendMode {One,InverseSourceAlpha};

// Semantic operations are used directly in production. Original numeric
// calls below exist only for independent executable comparison fixtures.
template<class Backend> class RenderCommands {
    Backend& backend;
    static std::uint32_t bits(float value){std::uint32_t out;std::memcpy(&out,&value,4);return out;}
    static std::uint32_t argument(TextureArg value){return value==TextureArg::Diffuse?0:value==TextureArg::Texture?2:3;}
public:
    explicit RenderCommands(Backend& value):backend(value){}
#ifdef TH_NATIVE_PLATFORM
    void SetDepthFunc(DepthFunc v){backend.set_depth_compare(v==DepthFunc::LessEqual?Compare::LessEqual:Compare::Always);}
    void SetDepthMask(bool v){backend.set_depth_mask(v);}
    void SetFogEnabled(bool v){backend.set_fog(v);}
    void SetFogColor(std::uint32_t v){backend.set_fog_color(v);}
    void SetFogRange(float a,float b){backend.set_fog_range(a,b);}
    void SetDestinationBlend(BlendMode v){backend.set_destination_blend(v==BlendMode::One?BlendFactor::One:BlendFactor::InverseSourceAlpha);}
    void SetTextureFactor(std::uint32_t v){backend.set_texture_factor(v);}
    void SetColorOp(ColorOp v){const auto op=v==ColorOp::SelectFirst?ColorOperation::First:ColorOperation::Multiply;backend.set_color_operation(Component::Alpha,op);backend.set_color_operation(Component::Color,op);}
    void SetTextureArg(TextureArg v){set_arg(ArgumentSlot::First,v);}
    void SetDiffuseArg(TextureArg v){set_arg(ArgumentSlot::Second,v);}
private:
    void set_arg(ArgumentSlot slot,TextureArg v){Argument arg{v==TextureArg::Texture?ArgumentSource::Texture:v==TextureArg::Factor?ArgumentSource::Factor:ArgumentSource::Diffuse};backend.set_texture_argument(Component::Alpha,slot,arg);backend.set_texture_argument(Component::Color,slot,arg);}
#else
    void SetDepthFunc(DepthFunc value){backend.render_state(23,value==DepthFunc::LessEqual?4:8);}
    void SetDepthMask(bool value){backend.render_state(14,value);}
    void SetFogEnabled(bool value){backend.render_state(28,value);}
    void SetFogColor(std::uint32_t value){backend.render_state(34,value);}
    void SetFogRange(float nearPlane,float farPlane){backend.render_state(36,bits(nearPlane));backend.render_state(37,bits(farPlane));}
    void SetDestinationBlend(BlendMode value){backend.render_state(20,value==BlendMode::One?2:6);}
    void SetTextureFactor(std::uint32_t value){backend.render_state(60,value);}
    void SetColorOp(ColorOp value){const auto op=value==ColorOp::SelectFirst?2u:4u;backend.stage_state(4,op);backend.stage_state(1,op);}
    void SetTextureArg(TextureArg value){const auto arg=argument(value);backend.stage_state(5,arg);backend.stage_state(2,arg);}
    void SetDiffuseArg(TextureArg value){const auto arg=argument(value);backend.stage_state(6,arg);backend.stage_state(3,arg);}
#endif
};
}
