#include "SpellBackground.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
namespace th08 {
bool SpellBackground::begin(u32 variant,const Vec3& position){
    if(variant>=3)return false;constexpr i32 kinds[]{56,65,58},scripts[]{97,99,101};
    effects.fixed(kinds[variant],position,9,0xffffffff);auto* inner=effects.fixed(kinds[variant],position,10,0xffffffff);
    auto* file=effects.state.base_animation;if(!inner||!file||file->scriptCount<=u32(scripts[variant]))return false;
    inner->scriptIndex=i16(scripts[variant]);anm.start(*file,*inner,file->scripts[scripts[variant]]);state.callback=callback;return true;
}
void SpellBackground::end(){effects.group(9)->active=0;effects.group(10)->active=0;state.spell_vm_count=2;}
void SpellBackground::callback(BackgroundState&,AnmRenderer& renderer,void* context){static_cast<SpellBackground*>(context)->draw(renderer);}
void SpellBackground::draw(AnmRenderer& r){
    const auto first=effects.presentation_copy(*effects.group(9));const auto second=effects.presentation_copy(*effects.group(10));
    const float radii[]{Scalar::mul(first.pos.x,.7071068286895752f),Scalar::mul(second.pos.x,.7071068286895752f)};
    const auto x=number(32)+number(first.position.x),y=number(16)+number(first.position.y);
    UntexturedVertex vertices[10];
    constexpr i32 xs[]{-1,1,1,-1,-1},ys[]{-1,-1,1,1,-1};
    for(u32 i=0;i<5;++i)for(u32 side=0;side<2;++side){const auto radius=number(radii[side]);auto& v=vertices[i*2+side];v.pos={(xs[i]<0?x-radius:x+radius).to_float(),(ys[i]<0?y-radius:y+radius).to_float(),.8f};v.reciprocal_w=1;v.color=0xff000000;}
    r.draw_depth_mask(vertices,10);r.set_depth_func(DepthFunc::LessEqual);
    AnmVm presented_a,presented_b;AnmVm* ap=&state.spell_vms[0],*bp=&state.spell_vms[1];if(presentation::render_only){presented_a=view.presentation_spell_vm(0);presented_b=view.presentation_spell_vm(1);ap=&presented_a;bp=&presented_b;}
    auto& a=*ap;a.scale={-1.5f,-1.75f};a.pos={416,464,.7f};const auto color=a.color1;a.color1.d3dColor=i32(0xffe0c0c0);{TH08_AUDIT_SCOPE(Background,&state.spell_vms[0],state.spell_vms[0].currentTimeInScript.current,0);r.draw_2d(a);}a.scale={1.5f,1.75f};a.pos={32,16,.5f};a.color1=color;
    auto& b=*bp;b.rotation.z=Scalar::mul(b.rotation.z,-1);b.pos.z=.6f;const auto other=b.color1;b.color1.d3dColor=i32(0xffe0c0c0);{TH08_AUDIT_SCOPE(Background,&state.spell_vms[1],state.spell_vms[1].currentTimeInScript.current,1);r.draw_2d(b);}r.flush();b.rotation.z=Scalar::mul(b.rotation.z,-1);b.pos.z=.5f;b.color1=other;r.set_depth_func(DepthFunc::Always);
}
}
