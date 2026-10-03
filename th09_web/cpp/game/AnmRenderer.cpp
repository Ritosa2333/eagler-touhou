#include "AnmRenderer.hpp"
#include <algorithm>
#include <cmath>
namespace th09 {
AnmRenderer::AnmRenderer(ZunGraphics& g):backend(g){
    state.depthTest=true;state.depthCompare=Compare::Always;state.blend=true;state.sourceBlend=BlendFactor::SourceAlpha;state.destinationBlend=BlendFactor::InverseSourceAlpha;
    state.alphaTest=true;state.alphaReference=1;state.alphaCompare=Compare::GreaterEqual;
    state.color.operation=state.alpha.operation=ColorOperation::Multiply;state.color.first=state.alpha.first={ArgumentSource::Texture};state.color.second=state.alpha.second={ArgumentSource::Diffuse};
    batch.reserve(3072);
}
void AnmRenderer::flush(){if(batch.empty())return;backend.triangles(batch_state,batch_texture,batch.data(),u32(batch.size()));batch.clear();}
bool AnmRenderer::drawable(const AnmVm& vm)const{return (vm.flags&3)==3&&vm.color1.a&&vm.loadedSprite;}
u32 AnmRenderer::color(const AnmVm& vm)const{
    const u32 value=vm.flag17?u32(vm.color2.d3dColor):u32(vm.color1.d3dColor);if(!tint_enabled)return value;u32 result=0;
    for(u32 s=0;s<32;s+=8)result|=std::min((u32(u8(value>>s))*u32(u8(tint>>s)))>>7,255u)<<s;return result;
}
void AnmRenderer::prepare(const AnmVm& vm){
    const u8 blend=vm.blendMode;if(blend!=blend_mode){flush();blend_mode=blend;if(blend<2)state.destinationBlend=blend?BlendFactor::One:BlendFactor::InverseSourceAlpha;}
    const u8 disabled=vm.zWriteDisabled;if(disabled!=depth_disabled){flush();depth_disabled=disabled;state.depthWrite=!disabled;}
    const u32 handle=vm.loadedSprite?vm.loadedSprite->texture:0;if(handle!=texture){flush();texture=handle;}
}
void AnmRenderer::append(const SpriteVertex* v){
    if(!batch.empty()&&(!batch_state.compatible(state)||batch_texture!=texture))flush();
    if(batch.empty()){batch_state=state;batch_texture=texture;}
    for(u32 n:{0u,1u,2u,2u,1u,3u})batch.push_back(v[n]);if(batch.size()>=3072)flush();
}
void AnmRenderer::unrotated(const AnmVm& vm,bool write_z){
    float x=vm.spriteSize.x*vm.scale.x*.5f,y=vm.spriteSize.y*vm.scale.y*.5f;
    float left=vm.pos.x,top=vm.pos.y;if(!(vm.anchor&1))left-=x;else x+=x;if(!(vm.anchor&2))top-=y;else y+=y;
    const float right=x+vm.pos.x,bottom=y+vm.pos.y;quad[0].position.x=quad[2].position.x=left;quad[1].position.x=quad[3].position.x=right;
    quad[0].position.y=quad[1].position.y=top;quad[2].position.y=quad[3].position.y=bottom;if(write_z)for(auto& v:quad)v.position.z=vm.pos.z;
}
void AnmRenderer::rotated(const AnmVm& vm){
    const float sine=float(std::sin(double(vm.rotation.z))),cosine=float(std::cos(double(vm.rotation.z)));
    const float x=vm.spriteSize.x*vm.scale.x*.5f,y=vm.spriteSize.y*vm.scale.y*.5f;
    for(u32 n=0;n<4;++n){const float px=n&1?x:-x,py=n&2?y:-y;auto& v=quad[n].position;v.x=px*cosine-py*sine+vm.pos.x;v.y=px*sine+py*cosine+vm.pos.y;v.z=vm.pos.z;if(vm.anchor&1)v.x+=x;if(vm.anchor&2)v.y+=y;}
}
i32 AnmRenderer::finish(AnmVm& vm,bool round,bool mirror,bool keep_color){
    for(auto& v:quad){v.position.x+=shake.x;v.position.y+=shake.y;}
    if(round){const float left=std::nearbyint(quad[0].position.x)-.5f,right=std::nearbyint(quad[1].position.x)-.5f,top=std::nearbyint(quad[0].position.y)-.5f,bottom=std::nearbyint(quad[2].position.y)-.5f;
        quad[0].position.x=quad[2].position.x=left;quad[1].position.x=quad[3].position.x=right;quad[0].position.y=quad[1].position.y=top;quad[2].position.y=quad[3].position.y=bottom;}
    const auto& sprite=*vm.loadedSprite;const float u0=(mirror?sprite.uvEnd.x:sprite.uvStart.x)+vm.uvScrollPos.x,u1=(mirror?sprite.uvStart.x:sprite.uvEnd.x)+vm.uvScrollPos.x,v0=sprite.uvStart.y+vm.uvScrollPos.y,v1=sprite.uvEnd.y+vm.uvScrollPos.y;
    for(u32 n=0;n<4;++n)quad[n].uv={n&1?u1:u0,n&2?v1:v0};
    float minx=quad[0].position.x,maxx=minx,miny=quad[0].position.y,maxy=miny;for(u32 n=1;n<4;++n){const auto& v=quad[n].position;minx=std::min(minx,v.x);maxx=std::max(maxx,v.x);miny=std::min(miny,v.y);maxy=std::max(maxy,v.y);}
    if(maxx<float(view.x)||maxy<float(view.y)||minx>float(view.x+view.width)||miny>float(view.y+view.height))return 0;
    if(!keep_color){const u32 rgba=color(vm);for(auto& v:quad)v.color=rgba;}prepare(vm);append(quad.data());return 0;
}
i32 AnmRenderer::draw_no_rotation(AnmVm& vm,bool round,bool mirror){if(!drawable(vm))return -1;if(omit_sprite_geometry)return 0;unrotated(vm,true);return finish(vm,round,mirror);}
i32 AnmRenderer::draw_2d(AnmVm& vm,bool no_round){if(!vm.rotation.z&&!no_round)return draw_no_rotation(vm);if(!drawable(vm))return -1;if(omit_sprite_geometry)return 0;if(vm.rotation.z)rotated(vm);else unrotated(vm,false);return finish(vm,false);}
i32 AnmRenderer::draw_quad(AnmVm& vm,const SpriteVertex* p){if(!drawable(vm))return -1;if(omit_sprite_geometry)return 0;prepare(vm);append(p);return 0;}
i32 AnmRenderer::draw_strip(AnmVm& vm,const SpriteVertex* p,u32 n){if(!drawable(vm)||n<3)return -1;if(omit_sprite_geometry)return 0;prepare(vm);flush();backend.draw(state,texture,Topology::Strip,VertexLayout::ScreenColorUv,p,n);return 0;}
void AnmRenderer::draw_colors(AnmVm& vm,Topology type,const ColorVertex* p,u32 n){if(n<3)return;prepare(vm);flush();auto flat=state;flat.depthWrite=false;flat.color.operation=flat.alpha.operation=ColorOperation::First;flat.color.first=flat.alpha.first={ArgumentSource::Diffuse};backend.draw(flat,0,type,VertexLayout::ScreenColor,p,n);}
void AnmRenderer::draw_lines(const ColorVertex* p,u32 n){if(n<2)return;flush();auto flat=state;flat.depthWrite=false;flat.destinationBlend=BlendFactor::One;flat.color.operation=flat.alpha.operation=ColorOperation::First;flat.color.first=flat.alpha.first={ArgumentSource::Diffuse};backend.draw(flat,0,Topology::LineStrip,VertexLayout::ScreenColor,p,n);}
void AnmRenderer::rectangle(float left,float top,float right,float bottom,u32 top_color,u32 bottom_color){
    flush();ColorVertex v[4]={{{left,top,0},1,top_color},{{right,top,0},1,top_color},{{left,bottom,0},1,bottom_color},{{right,bottom,0},1,bottom_color}};
    auto flat=state;flat.depthWrite=false;flat.destinationBlend=BlendFactor::InverseSourceAlpha;flat.color.operation=flat.alpha.operation=ColorOperation::First;flat.color.first=flat.alpha.first={ArgumentSource::Diffuse};backend.draw(flat,0,Topology::Strip,VertexLayout::ScreenColor,v,4);
}
}
