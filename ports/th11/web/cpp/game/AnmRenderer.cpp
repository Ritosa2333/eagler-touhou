#include "AnmRenderer.hpp"
#include "AnmPresentation.hpp"
#include "PresentationAudit.hpp"
#include <algorithm>
#include <cmath>
namespace th11 {
using namespace touhou::graphics;
void AnmRenderer::flush(){if(vertices.empty())return;auto& p=graphics.pipeline();p.textureTransform=false;p.color.second=p.alpha.second={ArgumentSource::Diffuse};graphics.set_layout(VertexLayout::ScreenColorUv);graphics.triangles(vertices.size()/3,vertices.data(),sizeof(AnmVertex));vertices.clear();}
void AnmRenderer::invalidate(){flush();texture_handle=blend_mode=filter=~0u;uv_sprite=nullptr;}
void AnmRenderer::solid_rectangle(float left,float top,float right,float bottom,u32 color){
    // 447f70 draws directly, then invalidates the ANM material cache.
    flush();auto& p=graphics.pipeline();p.alpha.operation=p.color.operation=ColorOperation::First;
    p.alpha.first=p.color.first={ArgumentSource::Diffuse};p.destinationBlend=BlendFactor::InverseSourceAlpha;
    struct Vertex{Vec3 position;float rhw;u32 color;};
    const Vertex quad[]={{{left,top,0},1,color},{{right,top,0},1,color},{{left,bottom,0},1,color},{{right,bottom,0},1,color}};
    graphics.set_layout(VertexLayout::ScreenColor);graphics.primitives(Topology::Strip,2,quad,sizeof(Vertex));
    texture_handle=blend_mode=filter=~0u;uv_sprite=nullptr;
    p.alpha.operation=p.color.operation=ColorOperation::Multiply;p.alpha.first=p.color.first={ArgumentSource::Texture};
}
void AnmRenderer::set_camera(SceneCamera& value,bool screen){
    flush();if(screen)value.screen();else value.perspective();
    camera.position=value.position;camera.right=value.right;camera.view=value.view;camera.projection=value.projection;camera.viewport=value.viewport;
    camera.fog_near=value.fog.near_distance;camera.fog_far=value.fog.far_distance;camera.fog_color=value.fog.color;
    offset=value.offset;graphics.set_matrix(MatrixKind::View,value.view);graphics.set_matrix(MatrixKind::Projection,value.projection);set_viewport(value.viewport);
}
u32 AnmRenderer::color(const AnmVm& vm)const noexcept {const u32 original=vm.flags&0x8000?vm.secondary_color:vm.color;if(!tint_enabled)return original;u32 out=0;for(u32 shift=0;shift<32;shift+=8)out|=std::min(255u,(((original>>shift)&255)*((tint>>shift)&255))>>7)<<shift;return out;}
void AnmRenderer::material(const AnmVm& vm){
    const auto handle=graphics.texture(*vm.resource,vm.sprite->texture);if(handle!=texture_handle){flush();texture_handle=handle;graphics.bind_texture(handle);}
    const u32 blend=(vm.flags>>4)&7;if(blend!=blend_mode){flush();blend_mode=blend;auto& p=graphics.pipeline();
        switch(blend){
        case 0:p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::InverseSourceAlpha;break;
        case 1:p.sourceBlend=BlendFactor::SourceAlpha;p.destinationBlend=BlendFactor::One;break;
        case 2:p.sourceBlend=BlendFactor::Zero;p.destinationBlend=BlendFactor::InverseSourceColor;break;
        case 3:p.sourceBlend=BlendFactor::One;p.destinationBlend=BlendFactor::Zero;break;
        case 4:p.sourceBlend=BlendFactor::InverseDestinationColor;p.destinationBlend=BlendFactor::InverseSourceAlpha;break;
        case 5:p.sourceBlend=BlendFactor::DestinationColor;p.destinationBlend=BlendFactor::Zero;break;
        case 6:p.sourceBlend=BlendFactor::InverseSourceColor;p.destinationBlend=BlendFactor::InverseSourceAlpha;break;
        }
    }
    const u32 point=vm.flags>>31;if(point!=filter){flush();filter=point;graphics.pipeline().minFilter=graphics.pipeline().magFilter=point?Filter::Nearest:Filter::Linear;}
}
float AnmRenderer::fog_amount(Vec3 position,Vec3 origin)const noexcept{
    const float x=float(double(position.x)-origin.x),y=float(double(position.y)-origin.y),z=float(double(position.z)-origin.z);
    const float squared=float((double(x)*x+double(y)*y)+double(z)*z),distance=float(std::sqrt(double(squared)));
    return distance<=fog_start?0:float((double(distance)-fog_start)/float(double(camera.fog_far)-camera.fog_near));
}
u32 AnmRenderer::fog_color(u32 value,float amount,bool fade_alpha)const noexcept{
    if(amount==0)return value;if(amount>=1)return (value&0xff000000)|(camera.fog_color&0xffffff);
    const float channels[]={fog_channels.x,fog_channels.y,fog_channels.z};u32 out=0;
    for(u32 i=0;i<3;++i){const i32 original=(value>>(i*8))&255;const double target=fade_alpha?double(i32(channels[i])):double(channels[i]);out|=u32(u8(original-i32((double(original)-target)*amount)))<<(i*8);}
    return out|(fade_alpha?u32(u8(i32(double(value>>24)*(1.-amount))))<<24:value&0xff000000);
}
int AnmRenderer::submit_quad(AnmVm& vm,Vec3 (&positions)[4],bool pixel,const u32* colors,bool preserve_subpixel){
    for(auto& p:positions){p.x=float(double(p.x)+offset.x);p.y=float(double(p.y)+offset.y);}
    // Keep the D3D half-pixel convention, but do not quantize continuous bullet
    // motion at final submission. This is draw-only; authored VM state is unchanged.
    if(pixel)for(auto& p:positions){p.x=float((preserve_subpixel?double(p.x):std::nearbyint(double(p.x)))-.5);p.y=float((preserve_subpixel?double(p.y):std::nearbyint(double(p.y)))-.5);}
    float min_x=positions[0].x,max_x=min_x,min_y=positions[0].y,max_y=min_y;for(u32 i=1;i<4;++i){min_x=std::min(min_x,positions[i].x);max_x=std::max(max_x,positions[i].x);min_y=std::min(min_y,positions[i].y);max_y=std::max(max_y,positions[i].y);}
    if(max_x<double(viewport.x)||max_y<double(viewport.y)||min_x>double(viewport.x+viewport.width)||min_y>double(viewport.y+viewport.height))return 0;
    material(vm);const u32 diffuse=color(vm);AnmVertex quad[4];for(u32 i=0;i<4;++i)quad[i]={positions[i],1,colors?colors[i]:diffuse,{float(double(vm.uv[i].x)+vm.uv_offset.x),float(double(vm.uv[i].y)+vm.uv_offset.y)}};
    presentation_audit::capture(vm,quad,4);
    for(u32 i:{0,1,2,1,2,3})vertices.push_back(quad[i]);if(vertices.size()>=6144)flush();return 0;
}
int AnmRenderer::draw_ascii_sprite(AnmVm& vm){
    // 401670 submits 44fe30/44f880 directly, including zero-alpha glyphs.
    if(!vm.sprite||!vm.resource)return -2;
    Vec3 positions[4];if(!anm_quad_positions(vm,positions))return -2;
    return submit_quad(vm,positions,true);
}
int AnmRenderer::draw(AnmVm& vm,bool preserve_subpixel){
    AnmVm smoothed;
    AnmVm* target=&vm;
    if(high_refresh::interpolating()){smoothed=vm;if(presentation::present(smoothed,vm))target=&smoothed;}
    // Only an actually interpolated position may bypass retail pixel alignment.
    const bool continuous_position=continuous&&(target!=&vm||high_refresh::smooth_submission);
    const bool subpixel=preserve_subpixel||continuous_position;
    AnmVm& v=*target;
    if((v.flags&3)!=3||!(v.color>>24))return -1;
    if(!v.sprite||!v.resource)return -2;const u32 mode=(v.flags>>22)&15;
    if(mode<=3){
        Vec3 positions[4];const u32 saved_flags=v.flags;
        const bool pixel=mode==0||(mode==1&&v.rotation.z==0);
        const u32 geometry_mode=pixel?0:mode==3?(v.rotation.z==0?2:1):mode;
        v.flags=(v.flags&~0x3c00000u)|(geometry_mode<<22);const bool ok=anm_quad_positions(v,positions,subpixel);v.flags=saved_flags;if(!ok)return -2;
        return submit_quad(v,positions,pixel,nullptr,subpixel);
    }
    if(mode==4||mode==5){Vec3 positions[4];if(mode==4?!anm_billboard_quad(v,camera,positions):!anm_projected_quad(v,camera,positions))return -1;return submit_quad(v,positions,false);}
    if(mode==6||mode==7){
        Vec3 positions[4];u32 colors[4];
        if(mode==6){
            if(!anm_billboard_quad(v,camera,positions))return -1;
            const Vec3 center{float(double(float(double(v.position.x)+v.script_position.x))+v.child_position.x),float(double(float(double(v.position.y)+v.script_position.y))+v.child_position.y),float(double(float(double(v.position.z)+v.script_position.z))+v.child_position.z)};
            const float amount=fog_amount(center,camera.position);if(amount>=1)return -1;
            const u32 value=fog_color(color(v),amount,true);for(auto& c:colors)c=value;
        }else{
            Matrix4 world;if(!anm_projected_quad(v,camera,positions,&world))return -1;
            const Vec3 corners[]={{-128,-128,0},{128,-128,0},{-128,128,0},{128,128,0}};const u32 value=v.flags&0x8000?v.secondary_color:v.color;
            for(u32 i=0;i<4;++i){float projected[4];GraphicsMath::transform(corners[i],world,projected);colors[i]=fog_color(value,fog_amount({projected[0],projected[1],projected[2]},fog_origin),false);}
        }
        return submit_quad(v,positions,false,colors);
    }
    if(mode==8){
        flush();const auto world=anm_world_matrix(v,true);material(v);auto& p=graphics.pipeline();p.textureTransform=true;p.textureFactor=color(v);p.color.second=p.alpha.second={ArgumentSource::Factor};
        graphics.set_matrix(MatrixKind::World,world);
        // The original tests horizontal scroll twice. A vertical-only change
        // on the same sprite retains the previous texture matrix.
        if(uv_sprite!=v.sprite||v.uv_offset.x!=0){uv_sprite=v.sprite;cached_uv=v.uv_matrix;cached_uv.m[8]=float(double(v.uv[0].x)+v.uv_offset.x);cached_uv.m[9]=float(double(v.uv[0].y)+v.uv_offset.y);}
        graphics.set_matrix(MatrixKind::Texture,cached_uv);
        struct Vertex{Vec3 position;Vec2 uv;};const Vertex quad[]={{{-128,-128,0},{0,0}},{{128,-128,0},{1,0}},{{-128,128,0},{0,1}},{{128,128,0},{1,1}}};
        AnmVertex audit_quad[4];for(u32 i=0;i<4;++i)audit_quad[i]={quad[i].position,1,color(v),quad[i].uv};
        presentation_audit::capture(v,audit_quad,4);
        graphics.set_layout(VertexLayout::WorldUv);graphics.primitives(Topology::Strip,2,quad,sizeof(Vertex));return 0;
    }
    if(mode==10)return 0; // Its registered draw callback submits the ripple.
    if(mode==9||mode==11||mode==12||mode==13){
        if(!v.geometry)return -2;flush();material(v);auto& p=graphics.pipeline();p.textureTransform=false;p.color.second=p.alpha.second={ArgumentSource::Diffuse};graphics.set_layout(VertexLayout::ScreenColorUv);
        const bool fan=mode==11;if(fan)p.depthWrite=false;const u32 count=u32(v.integers[0])*2-2;
        presentation_audit::capture(v,static_cast<const AnmVertex*>(v.geometry),std::min(count,16u),2);
        graphics.primitives(fan?Topology::Fan:Topology::Strip,count,v.geometry,sizeof(AnmVertex));return 0;
    }
    return 0; // Reserved modes have no draw operation in the original dispatcher.
}
int AnmRenderer::draw_ripple(AnmVm& vm){
    if(!vm.geometry||!vm.resource||!vm.sprite)return -2;flush();material(vm);presentation_audit::capture(vm,static_cast<const AnmVertex*>(vm.geometry),16,3);auto& p=graphics.pipeline();p.depthWrite=false;p.textureTransform=false;p.color.second=p.alpha.second={ArgumentSource::Diffuse};graphics.set_layout(VertexLayout::ScreenColorUv);graphics.primitives(Topology::Fan,31,vm.geometry,sizeof(AnmVertex));return 0;
}
int AnmRenderer::draw_layer(AnmVm* first){for(auto* vm=first;vm;vm=vm->draw_next){if(vm->flags&0x4000000)continue;if(vm->before_update==anm_ripple_update&&draw_ripple(*vm)==-2)return -2;if(vm->draw_callback)vm->draw_callback(*vm);if(draw(*vm)==-2)return -2;}return 1;}
}
