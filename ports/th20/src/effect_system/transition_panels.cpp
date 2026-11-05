#include "transition_panels.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/draw.hpp"
#include "../program_entry/program_entry.hpp"
#include <bit>
#include <cstring>
#include <new>
#include <utility>
namespace th20::source::effects {
namespace s=sprite;namespace n=recovered;
TransitionPanels::TransitionPanels(s::Animation& a):AttachedCallback(a),mode(0),frames(0),closing(0){for(auto& p:panels)s::construct_animation(p);s::construct_animation(mask);}
TransitionPanels::~TransitionPanels(){s::destroy_animation_contents(mask);for(int i=3;i>=0;--i)s::destroy_animation_contents(panels[i]);}
int TransitionPanels::initialize(const Parameters&,int){
    s::set_animation_layer(panels[0],0);for(int i=0;i<4;++i){s::bind_animation_script(*controller(0)->files[2],panels[i],i+3,nullptr);panels[i].vector_5bc={320,240,0};}
    mode=0;s::set_animation_layer(panels[0],52);for(auto& p:panels)p.vector_5bc={320,240,0};s::bind_animation_script(*controller(0)->files[2],mask,11,nullptr);return 0;
}
std::int32_t TransitionPanels::update(){int finished=0;for(auto& p:panels)if(s::execute_animation(p))++finished;if(finished>=4)return 1;s::execute_animation(mask);frames=n::signed_bits(static_cast<std::uint32_t>(frames)+1u);return closing&&frames>=60?1:0;}
void TransitionPanels::interrupt(std::int32_t value){
    if(value==1){for(int i=0;i<4;++i)s::bind_animation_script(*controller(0)->files[2],panels[i],i+7,nullptr);closing=1;frames=0;return;}
    int layer=0;switch(value){case 7:mode=0;layer=35;break;case 8:mode=1;layer=23;break;case 9:mode=0;layer=45;break;case 10:mode=3;layer=35;break;default:return;}
    s::set_animation_layer(panels[0],layer);for(auto& p:panels)p.vector_5bc={320,240,0};
}
void TransitionPanels::draw(){
    auto& c=environment::sprites();auto& d=s::draw_environment::device();
    const bool masked=mode==2||(mode==0&&program_entry::graphics_state.presentation.BackBufferFormat==D3DFMT_A8R8G8B8);
    if(masked){
        s::flush_textured_quads(c,d);
#ifdef TH_SDL3
        {using namespace touhou::graphics;auto& p=d.pipeline();
        p.alphaTest=false;p.separateAlphaBlend=true;
        p.sourceBlend=BlendFactor::Zero;p.destinationBlend=BlendFactor::One;p.blendEquation=BlendEquation::Add;
        p.sourceBlendAlpha=BlendFactor::One;p.destinationBlendAlpha=BlendFactor::Zero;p.blendEquationAlpha=BlendEquation::Add;}
#else
        for(auto state:{std::pair<unsigned,unsigned>{15,0},{206,1},{19,1},{20,2},{171,1},{207,2},{208,1},{209,1}})d.SetRenderState(static_cast<D3DRENDERSTATETYPE>(state.first),state.second);
#endif
        float left=128,top=16,right=512,bottom=464;
        if(mode!=2){left=std::bit_cast<float>(c.fields_c8[2]);top=std::bit_cast<float>(c.fields_c8[3]);right=n::add32(n::int_float(program_entry::window_state.scaled_width),left);bottom=n::add32(n::int_float(program_entry::window_state.scaled_height),top);}
        s::Vertex20 vertices[4]={{left,top,0,1,0},{right,top,0,1,0},{left,bottom,0,1,0},{right,bottom,0,1,0}};
#ifdef TH_SDL3
        {using namespace touhou::graphics;auto& p=d.pipeline();
        p.color.operation=p.alpha.operation=ColorOperation::First;
        p.color.first=p.alpha.first={ArgumentSource::Diffuse};
        d.vertex_format(VertexLayout::ScreenColor);d.draw(Topology::Strip,2,vertices,sizeof(s::Vertex20));
        p.alphaTest=true;
        p.color.operation=p.alpha.operation=ColorOperation::Multiply;
        p.color.first=p.alpha.first={ArgumentSource::Texture};
        p.sourceBlendAlpha=BlendFactor::SourceAlpha;p.destinationBlendAlpha=BlendFactor::One;p.blendEquationAlpha=BlendEquation::Add;}
#else
        for(auto state:{std::pair<unsigned,unsigned>{4,2},{1,2},{5,0},{2,0}})d.SetTextureStageState(0,static_cast<D3DTEXTURESTAGESTATETYPE>(state.first),state.second);
        d.SetFVF(0x44);d.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,vertices,sizeof(s::Vertex20));
        d.SetRenderState(D3DRS_ALPHATESTENABLE,1);
        for(auto state:{std::pair<unsigned,unsigned>{4,4},{1,4},{5,2},{2,2}})d.SetTextureStageState(0,static_cast<D3DTEXTURESTAGESTATETYPE>(state.first),state.second);
        for(auto state:{std::pair<unsigned,unsigned>{207,5},{208,2},{209,1}})d.SetRenderState(static_cast<D3DRENDERSTATETYPE>(state.first),state.second);
#endif
        c.blend_mode=11;
    }
    for(auto& p:panels)s::draw_animation(c,p);
    if(mode==2||(mode==0&&program_entry::graphics_state.presentation.BackBufferFormat==D3DFMT_A8R8G8B8)){
#ifdef TH_SDL3
        d.set_separate_alpha_blend(true);
#else
        d.SetRenderState(static_cast<D3DRENDERSTATETYPE>(206),1);
#endif
        s::draw_animation(c,mask);s::flush_textured_quads(c,d);}
}
namespace unrecovered {
void __cdecl initialize_0(s::Animation* a,const void* p,int view){void* memory=::operator new(sizeof(TransitionPanels),std::nothrow);if(!memory)throw std::bad_alloc();std::memset(memory,0,sizeof(TransitionPanels));auto* value=new(memory)TransitionPanels(*a);s::set_animation_layer(*a,52);value->initialize(*static_cast<const Parameters*>(p),view);}
}
}
