#include "render_state.hpp"
#include <cstring>
namespace th20::source::sprite {
void apply_animation_render_state(Controller& c,Animation& a,Device& device) {
    auto flush=[&]{flush_textured_quads(c,device);};
#ifdef TH_SDL3
    using namespace touhou::graphics;
    auto& pipeline=device.pipeline();
    const auto blend=static_cast<std::uint8_t>(a.base.flags[0]>>8);
    if(c.blend_mode!=blend){flush();c.blend_mode=blend;
        pipeline.alphaTest=true;pipeline.separateAlphaBlend=true;
        pipeline.sourceBlendAlpha=BlendFactor::One;pipeline.destinationBlendAlpha=BlendFactor::Zero;
        pipeline.blendEquationAlpha=BlendEquation::Add;
        BlendFactor source=BlendFactor::SourceAlpha,destination=BlendFactor::InverseSourceAlpha;
        BlendEquation operation=BlendEquation::Add;
        switch(blend){case 1:destination=BlendFactor::One;break;
        case 2:destination=BlendFactor::One;operation=BlendEquation::ReverseSubtract;break;
        case 3:pipeline.alphaTest=false;source=BlendFactor::One;destination=BlendFactor::Zero;break;
        case 4:source=BlendFactor::InverseDestinationColor;destination=BlendFactor::InverseSourceColor;break;
        case 5:source=BlendFactor::DestinationColor;destination=BlendFactor::Zero;break;
        case 6:source=BlendFactor::InverseSourceColor;break;
        case 7:source=BlendFactor::DestinationAlpha;destination=BlendFactor::InverseDestinationAlpha;break;
        case 8:destination=BlendFactor::One;operation=BlendEquation::Minimum;break;
        case 9:destination=BlendFactor::One;operation=BlendEquation::Maximum;break;default:break;}
        pipeline.sourceBlend=source;pipeline.destinationBlend=destination;pipeline.blendEquation=operation;
    }
    const auto filter=static_cast<std::uint8_t>((a.base.flags[2]>>2)&3);
    if(c.field_e12!=filter){flush();c.field_e12=filter;
        const auto mode=filter==0?Filter::Linear:Filter::Nearest;device.set_texture_filter(mode,mode);}
    const auto u=static_cast<std::uint8_t>((a.base.flags[2]>>16)&3);
    if(c.field_e13!=u){flush();c.field_e13=u;
        if(u<3){const auto mode=u==0?Address::Repeat:(u==1?Address::Clamp:Address::Mirror);device.set_texture_address(mode,pipeline.addressV);}}
    const auto v=static_cast<std::uint8_t>((a.base.flags[2]>>13)&3);
    if(c.field_e14!=v){flush();c.field_e14=v;
        if(v<3){const auto mode=v==0?Address::Repeat:(v==1?Address::Clamp:Address::Mirror);device.set_texture_address(pipeline.addressU,mode);}}
    ++c.field_c0;
#else
    auto render=[&](unsigned state,DWORD value){device.SetRenderState(static_cast<D3DRENDERSTATETYPE>(state),value);};
    auto sampler=[&](unsigned state,DWORD value){device.SetSamplerState(0,static_cast<D3DSAMPLERSTATETYPE>(state),value);};
    const auto blend=static_cast<std::uint8_t>(a.base.flags[0]>>8);
    if(c.blend_mode!=blend){flush();c.blend_mode=blend;render(15,1);render(206,1);render(207,2);render(208,1);render(209,1);
        unsigned source=5,destination=6,operation=1;
        switch(blend){case 1:destination=2;break;case 2:destination=2;operation=3;break;case 3:render(15,0);source=2;destination=1;break;case 4:source=10;destination=4;break;case 5:source=9;destination=1;break;case 6:source=4;break;case 7:source=7;destination=8;break;case 8:destination=2;operation=4;break;case 9:destination=2;operation=5;break;default:break;}
        render(19,source);render(20,destination);render(171,operation);
    }
    const auto filter=static_cast<std::uint8_t>((a.base.flags[2]>>2)&3);
    if(c.field_e12!=filter){flush();c.field_e12=filter;sampler(5,filter==0?2:1);sampler(6,filter==0?2:1);}
    const auto u=static_cast<std::uint8_t>((a.base.flags[2]>>16)&3);
    if(c.field_e13!=u){flush();c.field_e13=u;if(u<3)sampler(1,u==0?1:(u==1?3:2));}
    const auto v=static_cast<std::uint8_t>((a.base.flags[2]>>13)&3);
    if(c.field_e14!=v){flush();c.field_e14=v;if(v<3)sampler(2,v==0?1:(v==1?3:2));}
    ++c.field_c0;
#endif
}
void select_texture_combine(Controller& c,Device& device,std::uint8_t mode) {
    if(c.field_00==mode)return;flush_textured_quads(c,device);
#ifdef TH_SDL3
    using namespace touhou::graphics;
    auto& pipeline=device.pipeline();
    switch(mode){
    case 1: // ALPHAOP/COLOROP MODULATE, arg2 TFACTOR
        pipeline.color.operation=pipeline.alpha.operation=ColorOperation::Multiply;
        pipeline.color.first=pipeline.alpha.first={ArgumentSource::Texture};
        pipeline.color.second=pipeline.alpha.second={ArgumentSource::Factor};break;
    case 2: // SELECTARG1, all args DIFFUSE
        pipeline.color.operation=pipeline.alpha.operation=ColorOperation::First;
        pipeline.color.first=pipeline.alpha.first={ArgumentSource::Diffuse};break;
    case 3: // COLOROP MODULATE texture*diffuse; ALPHAOP SELECTARG2 (arg2 DIFFUSE)
        pipeline.color.operation=ColorOperation::Multiply;
        pipeline.color.first={ArgumentSource::Texture};pipeline.color.second={ArgumentSource::Diffuse};
        pipeline.alpha.operation=ColorOperation::First;pipeline.alpha.first={ArgumentSource::Diffuse};break;
    default: // MODULATE texture*diffuse
        pipeline.color.operation=pipeline.alpha.operation=ColorOperation::Multiply;
        pipeline.color.first=pipeline.alpha.first={ArgumentSource::Texture};
        pipeline.color.second=pipeline.alpha.second={ArgumentSource::Diffuse};break;
    }
#else
    const unsigned states[6]{5,6,2,3,4,1};unsigned values[6]{2,0,2,0,4,4};
    switch(mode){case 1:values[1]=values[3]=3;break;case 2:values[0]=values[1]=values[2]=values[3]=0;values[4]=values[5]=2;break;case 3:values[4]=3;break;default:break;}
    for(unsigned i=0;i<6;++i)device.SetTextureStageState(0,static_cast<D3DTEXTURESTAGESTATETYPE>(states[i]),values[i]);
#endif
    c.field_00=mode;
}
std::int32_t append_textured_quad(Controller& c,const Vertex28 (&vertices)[4]) noexcept {
    // Exact original upper bound leaves six vertices unused at the pool end.
    const auto limit=reinterpret_cast<std::uintptr_t>(&c)+0x7c00dcc;
    if(reinterpret_cast<std::uintptr_t>(c.textured_write)>=limit)return 1;
    auto* destination=c.textured_write;
    std::memcpy(destination,vertices,sizeof(Vertex28));std::memcpy(destination+3,vertices+1,sizeof(Vertex28));std::memcpy(destination+1,destination+3,sizeof(Vertex28));
    std::memcpy(destination+4,vertices+2,sizeof(Vertex28));std::memcpy(destination+2,destination+4,sizeof(Vertex28));std::memcpy(destination+5,vertices+3,sizeof(Vertex28));
    c.textured_write+=6;++c.quad_count;return 0;
}
}
