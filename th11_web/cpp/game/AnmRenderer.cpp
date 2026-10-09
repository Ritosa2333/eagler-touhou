#include "AnmRenderer.hpp"
#include <algorithm>
#include <cmath>
#ifdef TH11_MULTIPLAYER
#include "AnmManager.hpp"
#endif
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
u32 AnmRenderer::color(const AnmVm& vm)const noexcept {u32 original=vm.flags&0x8000?vm.secondary_color:vm.color;
#ifdef TH11_MULTIPLAYER
    original=multiplayer_color(vm,original);
#endif
    if(!tint_enabled)return original;u32 out=0;for(u32 shift=0;shift<32;shift+=8)out|=std::min(255u,(((original>>shift)&255)*((tint>>shift)&255))>>7)<<shift;return out;}
#ifdef TH11_MULTIPLAYER
u32 AnmRenderer::multiplayer_color(const AnmVm& vm,u32 color)const noexcept{if(!multiplayer_animations)return color;const auto owner=multiplayer_animations->multiplayer_tag(vm);if(!owner||owner>=multiplayer_opacity.size())return color;return (color&0xffffff)|(((color>>24)*multiplayer_opacity[owner]/255)<<24);}
u32 AnmRenderer::multiplayer_tint(u32 value,unsigned owner)const noexcept{
    if(!owner||owner>=multiplayer_opacity.size())return value;
    const unsigned opacity=multiplayer_opacity[owner];if(opacity==255)return value;
    // TH11's neutral multiplicative tint is 0x80 per channel. Interpolate
    // only the submitted multiplier; the shared Stage.tint remains native.
    u32 color=0;for(unsigned shift=0;shift<32;shift+=8){const unsigned source=(value>>shift)&255;const unsigned mixed=(source*opacity+128*(255-opacity)+127)/255;color|=mixed<<shift;}return color;
}
void AnmRenderer::multiplayer_geometry(const AnmVm& vm,Topology topology,u32 count,const AnmVertex* source){
    const auto owner=multiplayer_animations?multiplayer_animations->multiplayer_tag(vm):0;
    if(!owner||owner>=multiplayer_opacity.size()||multiplayer_opacity[owner]==255){graphics.primitives(topology,count,source,sizeof(AnmVertex));return;}
    // Draw-only copies preserve native ring/ripple/distortion vertices and RNG.
    multiplayer_geometry_vertices.assign(source,source+count+2);
    for(auto& vertex:multiplayer_geometry_vertices)vertex.color=multiplayer_color(vm,vertex.color);
    graphics.primitives(topology,count,multiplayer_geometry_vertices.data(),sizeof(AnmVertex));
}
#endif
void AnmRenderer::material(const AnmVm& vm){
    const auto handle=graphics.texture(*vm.resource,vm.sprite->texture);if(handle!=texture_handle){flush();texture_handle=handle;graphics.bind_texture(handle);}
    u32 blend=(vm.flags>>4)&7;
#ifdef TH11_MULTIPLAYER
    if(multiplayer_animations){const auto owner=multiplayer_animations->multiplayer_tag(vm);if(owner&&owner<multiplayer_opacity.size()&&multiplayer_opacity[owner]<255&&blend!=1)blend=0;}
#endif
    if(blend!=blend_mode){flush();blend_mode=blend;auto& p=graphics.pipeline();
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
int AnmRenderer::submit_quad(AnmVm& vm,Vec3 (&positions)[4],bool pixel,const u32* colors){
    for(auto& p:positions){p.x=float(double(p.x)+offset.x);p.y=float(double(p.y)+offset.y);}
    if(pixel)for(auto& p:positions){p.x=float(std::nearbyint(double(p.x))-.5);p.y=float(std::nearbyint(double(p.y))-.5);}
    float min_x=positions[0].x,max_x=min_x,min_y=positions[0].y,max_y=min_y;for(u32 i=1;i<4;++i){min_x=std::min(min_x,positions[i].x);max_x=std::max(max_x,positions[i].x);min_y=std::min(min_y,positions[i].y);max_y=std::max(max_y,positions[i].y);}
    if(max_x<double(viewport.x)||max_y<double(viewport.y)||min_x>double(viewport.x+viewport.width)||min_y>double(viewport.y+viewport.height))return 0;
    material(vm);const u32 diffuse=color(vm);AnmVertex quad[4];for(u32 i=0;i<4;++i)quad[i]={positions[i],1,colors?colors[i]:diffuse,{float(double(vm.uv[i].x)+vm.uv_offset.x),float(double(vm.uv[i].y)+vm.uv_offset.y)}};
    for(u32 i:{0,1,2,1,2,3})vertices.push_back(quad[i]);if(vertices.size()>=6144)flush();return 0;
}
int AnmRenderer::draw_ascii_sprite(AnmVm& vm){
    // 401670 submits 44fe30/44f880 directly, including zero-alpha glyphs.
    if(!vm.sprite||!vm.resource)return -2;
    Vec3 positions[4];if(!anm_quad_positions(vm,positions))return -2;
    return submit_quad(vm,positions,true);
}
int AnmRenderer::draw(AnmVm& vm){
    if((vm.flags&3)!=3||!(vm.color>>24))return -1;
#ifdef TH11_MULTIPLAYER
    if(&vm==multiplayer_hud_background){
        // The title/watermark is baked into the right frame's sprite, not a
        // separate text VM. Keep its original 16-pixel top/bottom borders and
        // fill only the interior from this same artwork's unlettered band.
        // Submitted copies leave the resource, VM and all world state intact.
        auto strip=[&](float top,float height,float source_top,float source_height){auto copy=vm;
            const float v0=vm.uv[0].y,span=vm.uv[2].y-v0;
            copy.script_position.y+=top*vm.scale.y;copy.sprite_size.y=height;
            copy.uv[0].y=copy.uv[1].y=v0+span*(source_top/480);
            copy.uv[2].y=copy.uv[3].y=v0+span*((source_top+source_height)/480);
            return draw(copy)!=-2;
        };
        return strip(0,16,0,16)&&strip(16,448,16,160)&&strip(464,16,464,16)?0:-2;
    }
#endif
    if(!vm.sprite||!vm.resource)return -2;const u32 mode=(vm.flags>>22)&15;
    if(mode<=3){
        Vec3 positions[4];const u32 saved_flags=vm.flags;
        const bool pixel=mode==0||(mode==1&&vm.rotation.z==0);
        const u32 geometry_mode=pixel?0:mode==3?(vm.rotation.z==0?2:1):mode;
        vm.flags=(vm.flags&~0x3c00000u)|(geometry_mode<<22);const bool ok=anm_quad_positions(vm,positions);vm.flags=saved_flags;if(!ok)return -2;
        return submit_quad(vm,positions,pixel);
    }
    if(mode==4||mode==5){Vec3 positions[4];if(mode==4?!anm_billboard_quad(vm,camera,positions):!anm_projected_quad(vm,camera,positions))return -1;return submit_quad(vm,positions,false);}
    if(mode==6||mode==7){
        Vec3 positions[4];u32 colors[4];
        if(mode==6){
            if(!anm_billboard_quad(vm,camera,positions))return -1;
            const Vec3 center{float(double(float(double(vm.position.x)+vm.script_position.x))+vm.child_position.x),float(double(float(double(vm.position.y)+vm.script_position.y))+vm.child_position.y),float(double(float(double(vm.position.z)+vm.script_position.z))+vm.child_position.z)};
            const float amount=fog_amount(center,camera.position);if(amount>=1)return -1;
            const u32 value=fog_color(color(vm),amount,true);for(auto& v:colors)v=value;
        }else{
            Matrix4 world;if(!anm_projected_quad(vm,camera,positions,&world))return -1;
            const Vec3 corners[]={{-128,-128,0},{128,-128,0},{-128,128,0},{128,128,0}};u32 value=vm.flags&0x8000?vm.secondary_color:vm.color;
#ifdef TH11_MULTIPLAYER
            value=multiplayer_color(vm,value);
#endif
            for(u32 i=0;i<4;++i){float p[4];GraphicsMath::transform(corners[i],world,p);colors[i]=fog_color(value,fog_amount({p[0],p[1],p[2]},fog_origin),false);}
        }
        return submit_quad(vm,positions,false,colors);
    }
    if(mode==8){
        flush();const auto world=anm_world_matrix(vm,true);material(vm);auto& p=graphics.pipeline();p.textureTransform=true;p.textureFactor=color(vm);p.color.second=p.alpha.second={ArgumentSource::Factor};
        graphics.set_matrix(MatrixKind::World,world);
        // The original tests horizontal scroll twice. A vertical-only change
        // on the same sprite retains the previous texture matrix.
        if(uv_sprite!=vm.sprite||vm.uv_offset.x!=0){uv_sprite=vm.sprite;cached_uv=vm.uv_matrix;cached_uv.m[8]=float(double(vm.uv[0].x)+vm.uv_offset.x);cached_uv.m[9]=float(double(vm.uv[0].y)+vm.uv_offset.y);}
        graphics.set_matrix(MatrixKind::Texture,cached_uv);
        struct Vertex{Vec3 position;Vec2 uv;};const Vertex quad[]={{{-128,-128,0},{0,0}},{{128,-128,0},{1,0}},{{-128,128,0},{0,1}},{{128,128,0},{1,1}}};
        graphics.set_layout(VertexLayout::WorldUv);graphics.primitives(Topology::Strip,2,quad,sizeof(Vertex));return 0;
    }
    if(mode==10)return 0; // Its registered draw callback submits the ripple.
    if(mode==9||mode==11||mode==12||mode==13){
        if(!vm.geometry)return -2;flush();material(vm);auto& p=graphics.pipeline();p.textureTransform=false;p.color.second=p.alpha.second={ArgumentSource::Diffuse};graphics.set_layout(VertexLayout::ScreenColorUv);
        const bool fan=mode==11;if(fan)p.depthWrite=false;const u32 count=u32(vm.integers[0])*2-2;
#ifdef TH11_MULTIPLAYER
        multiplayer_geometry(vm,fan?Topology::Fan:Topology::Strip,count,static_cast<const AnmVertex*>(vm.geometry));
#else
        graphics.primitives(fan?Topology::Fan:Topology::Strip,count,vm.geometry,sizeof(AnmVertex));
#endif
        return 0;
    }
    return 0; // Reserved modes have no draw operation in the original dispatcher.
}
int AnmRenderer::draw_ripple(AnmVm& vm){
    if(!vm.geometry||!vm.resource||!vm.sprite)return -2;flush();material(vm);auto& p=graphics.pipeline();p.depthWrite=false;p.textureTransform=false;p.color.second=p.alpha.second={ArgumentSource::Diffuse};graphics.set_layout(VertexLayout::ScreenColorUv);
#ifdef TH11_MULTIPLAYER
    multiplayer_geometry(vm,Topology::Fan,31,static_cast<const AnmVertex*>(vm.geometry));
#else
    graphics.primitives(Topology::Fan,31,vm.geometry,sizeof(AnmVertex));
#endif
    return 0;
}
int AnmRenderer::draw_layer(AnmVm* first){for(auto* vm=first;vm;vm=vm->draw_next){if(vm->flags&0x4000000)continue;if(vm->before_update==anm_ripple_update&&draw_ripple(*vm)==-2)return -2;if(vm->draw_callback)vm->draw_callback(*vm);if(draw(*vm)==-2)return -2;}return 1;}
}
