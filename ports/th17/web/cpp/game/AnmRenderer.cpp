#include "AnmRenderer.hpp"
#include "AnmOverlay.hpp"
#include <algorithm>
#include <cmath>
#if TH17_DEVELOPMENT_HARNESS
#include <emscripten.h>
namespace {std::array<unsigned,32> render_modes{};}
extern "C" EMSCRIPTEN_KEEPALIVE const unsigned* th17_probe_render_modes(){return render_modes.data();}
#endif
namespace th17 {
using namespace touhou::graphics;
namespace {u32 multiply_color(u32 a,u32 b){u32 out=0;for(u32 shift=0;shift<32;shift+=8)out|=std::min(255u,(((a>>shift)&255)*((b>>shift)&255))>>7)<<shift;return out;}}
void AnmRenderer::flush(){if(!vertex_count)return;graphics.set_layout(VertexLayout::ScreenColorUv);if(quad_batch)graphics.quads(vertex_count/4,vertices.data(),sizeof(AnmGeometryVertex));else graphics.triangles(vertex_count/3,vertices.data(),sizeof(AnmGeometryVertex));vertex_count=0;}
void AnmRenderer::invalidate(){flush();bound_texture=~0u;}
void AnmRenderer::set_viewport(const GraphicsViewport& value){invalidate();viewport=value;graphics.set_viewport(value);}
void AnmRenderer::set_camera(const AnmCamera& value){flush();camera=value;graphics.set_matrix(MatrixKind::View,camera.view);graphics.set_matrix(MatrixKind::Projection,camera.projection);}
u32 AnmRenderer::tint_color(u32 color)const noexcept{return tint_enabled?multiply_color(color,tint):color;}
bool AnmRenderer::material(AnmVm& vm,bool textured){
    if(!textured){if(bound_texture!=0){flush();bound_texture=0;graphics.bind_texture(0);}render_state.textureFactor=0xffffffff;}
    if(textured){auto* source=vm.sprite_resource?vm.sprite_resource:vm.resource;if(!source||vm.visual.sprite<0||u32(vm.visual.sprite)>=source->sprites.size()){error="Animation texture source unavailable: script="+std::to_string(vm.source_script)+" sprite="+std::to_string(vm.visual.sprite)+" mode="+std::to_string(vm.visual.draw_mode())+" flags="+std::to_string(vm.visual.flags)+" resource="+(source&&!source->textures.empty()?source->textures[0].name:"none")+" sprites="+std::to_string(source?source->sprites.size():0);return false;}
        // The original material lookup uses the bank and sprite index even before
        // an ANM script has executed sprite selection (chapter root 241 does this).
        // Keep its zero geometry/UVs; selecting a sprite here would change them.
        const u32 texture=graphics.texture(*source,source->sprites[u32(vm.visual.sprite)].texture);if(texture!=bound_texture){flush();bound_texture=texture;graphics.bind_texture(texture);}}
    const auto& v=vm.visual;auto& state=render_state;const u32 mode=v.blend_mode();bool alpha_test=true;
    auto source=state.sourceBlend,destination=state.destinationBlend;auto equation=state.blendEquation;
    switch(mode){
    case 0:source=BlendFactor::SourceAlpha;destination=BlendFactor::InverseSourceAlpha;equation=BlendEquation::Add;break;
    case 1:source=BlendFactor::SourceAlpha;destination=BlendFactor::One;equation=BlendEquation::Add;break;
    case 2:source=BlendFactor::SourceAlpha;destination=BlendFactor::One;equation=BlendEquation::ReverseSubtract;break;
    case 3:alpha_test=false;source=BlendFactor::One;destination=BlendFactor::Zero;equation=BlendEquation::Add;break;
    case 4:source=BlendFactor::InverseDestinationColor;destination=BlendFactor::InverseSourceColor;equation=BlendEquation::Add;break;
    case 5:source=BlendFactor::DestinationColor;destination=BlendFactor::Zero;equation=BlendEquation::Add;break;
    case 6:source=BlendFactor::InverseSourceColor;destination=BlendFactor::InverseSourceAlpha;equation=BlendEquation::Add;break;
    case 7:source=BlendFactor::DestinationAlpha;destination=BlendFactor::InverseDestinationAlpha;equation=BlendEquation::Add;break;
    case 8:source=BlendFactor::SourceAlpha;destination=BlendFactor::One;equation=BlendEquation::Minimum;break;
    case 9:source=BlendFactor::SourceAlpha;destination=BlendFactor::One;equation=BlendEquation::Maximum;break;
    default:break;
    }
    const auto filter=v.render_flags&0x800?Filter::Nearest:Filter::Linear;const u32 u=v.render_flags&3,w=v.flags>>30;
    const auto address_u=u<3?(u==0?Address::Repeat:u==1?Address::Clamp:Address::Mirror):state.addressU,address_v=w<3?(w==0?Address::Repeat:w==1?Address::Clamp:Address::Mirror):state.addressV;
    const Combiner combiner{ColorOperation::Multiply,{ArgumentSource::Texture},{ArgumentSource::Diffuse}};
    // A sprite changes only this material subset. Avoid copying/comparing the
    // entire depth/fog/viewport-independent pipeline for every visible bullet.
    if(state.alphaTest!=alpha_test||state.sourceBlend!=source||state.destinationBlend!=destination||state.blendEquation!=equation||state.minFilter!=filter||state.magFilter!=filter||state.addressU!=address_u||state.addressV!=address_v||state.textureTransform||!(state.color==combiner)||!(state.alpha==combiner))flush();
    state.alphaTest=alpha_test;state.sourceBlend=source;state.destinationBlend=destination;state.blendEquation=equation;state.minFilter=state.magFilter=filter;state.addressU=address_u;state.addressV=address_v;state.textureTransform=false;state.color=state.alpha=combiner;return true;
}
int AnmRenderer::shape(AnmVm& vm){
    const auto& v=vm.visual;const u32 mode=v.draw_mode();float width=float(v.sprite_size.x*v.scale.x),height=float(v.sprite_size.y*v.scale.y),angle=vm.variables.rotation.z;Vec3 center;if(!anm_position(vm,center)){error="Primitive animation position unavailable";return -2;}
    if(vm.creation_parent&&!(v.render_flags&0x10000)){const auto& parent=*vm.creation_parent;width=float(parent.visual.scale.x*width);height=float(parent.visual.scale.y*height);angle=float(parent.variables.rotation.z+angle);}
    const u32 resolution=v.render_flags>>20&7;if(resolution==1||resolution==2){if(!vm.environment){error="Primitive animation scene scale unavailable";return -2;}const float factor=resolution==1?vm.environment->resolution_scale:float(vm.environment->resolution_scale*.5f);width=float(factor*width);height=float(factor*height);}
    flush();if(!material(vm,false))return -2;auto& state=render_state;state.textureTransform=false;state.color.operation=state.alpha.operation=ColorOperation::First;state.color.first=state.alpha.first={ArgumentSource::Diffuse};graphics.set_layout(VertexLayout::ScreenColor);
    if(mode==26||mode==27){
        const u32 ax=v.flags>>21&3,ay=v.flags>>23&3;if(ax>2||ay>2){error="Line animation anchor outside range";return -2;}
        constexpr float extents[3][2]={{-.5f,.5f},{0,1},{-1,0}};const float sine=float(std::sin(double(angle))),cosine=float(std::cos(double(angle)));
        const auto vertex=[&](u32 x,u32 y){const float px=float(extents[ax][x]*width),py=mode==26?0.f:float(extents[ay][y]*height);return ColorVertex{{float(float(float(px*cosine)-float(py*sine))+center.x),float(float(float(px*sine)+float(py*cosine))+center.y),0},1,x&&bool(v.flags&0x60000)?v.secondary_color:v.color};};
        if(mode==26){const ColorVertex data[]={vertex(0,0),vertex(1,0)};graphics.primitives(Topology::LineStrip,1,data,sizeof(ColorVertex));}
        else{const ColorVertex data[]={vertex(0,0),vertex(1,0),vertex(1,1),vertex(0,1),vertex(0,0)};graphics.primitives(Topology::LineStrip,4,data,sizeof(ColorVertex));}return 0;
    }
    if(mode==16||mode>=20){const u32 ax=v.flags>>21&3,ay=v.flags>>23&3;if(ax>2||ay>2){error="Primitive animation anchor outside range";return -2;}constexpr float xs[3][2]={{-.5f,.5f},{0,1},{-1,0}},ys[3][2]={{-.5f,.5f},{0,1},{-1,0}};const float sine=float(std::sin(double(angle))),cosine=float(std::cos(double(angle)));const u32 primary=v.color,secondary=mode==20||mode==22?v.secondary_color:v.color;
        const auto rectangle=[&](float w,float h,u32 a,u32 b){ColorVertex data[4];for(u32 i=0;i<4;i++){const float x=float(xs[ax][i&1]*w),y=float(ys[ay][i>>1]*h);data[i]={{float(float(float(x*cosine)-float(y*sine))+center.x),float(float(float(y*cosine)+float(x*sine))+center.y),0},1,i&1?b:a};}graphics.primitives(Topology::Strip,2,data,sizeof(ColorVertex));};
        if(mode==21||mode==22)rectangle(float(width+1),float(height+1),((primary>>1)&0x7f000000)|(primary&0xffffff),((secondary>>1)&0x7f000000)|(secondary&0xffffff));rectangle(width,height,primary,secondary);return 0;
    }
    const i32 count=vm.variables.integers[0];if(count<1||count>0x100000){error="Primitive animation segment count invalid";return -2;}shapes.clear();const float step=float(6.283185482025146484375f/float(count));if(mode==17)shapes.push_back({{center.x,center.y,0},1,v.color});
    for(i32 i=0;i<=count;i++){const auto point=[&](float radius){return Vec3{float(float(std::cos(double(angle))*double(radius))+center.x),float(float(std::sin(double(angle))*double(radius))+center.y),0};};if(mode==19){shapes.push_back({point(float(width-float(height*.5f))),1,v.color});shapes.push_back({point(float(float(height*.5f)+width)),1,v.color});}else shapes.push_back({point(width),1,mode==17?v.secondary_color:v.color});angle=normalize_angle(float(angle+step));}
    if(mode==17){state.cull=Cull::None;graphics.primitives(Topology::Fan,count,shapes.data(),sizeof(ColorVertex));}else if(mode==18)graphics.primitives(Topology::LineStrip,count,shapes.data(),sizeof(ColorVertex));else graphics.primitives(Topology::Strip,count*2,shapes.data(),sizeof(ColorVertex));return 0;
}
int AnmRenderer::quad(AnmVm& vm,bool pixel){
    Vec3 p[4];const u32 flags=vm.visual.flags;if(vm.visual.draw_mode()==3)vm.visual.flags=(flags&~0x3e000000u)|0x2000000;const bool success=anm_quad_positions(vm,p);vm.visual.flags=flags;if(!success){error="Animation quad geometry unavailable: flags="+std::to_string(vm.visual.flags)+" render="+std::to_string(vm.visual.render_flags)+" script="+std::to_string(vm.source_script)+" mode="+std::to_string(vm.visual.draw_mode());return -2;}
    return pack_quad(vm,p,pixel);
}
int AnmRenderer::pack_quad(AnmVm& vm,Vec3 (&p)[4],bool pixel,const u32* supplied){
    for(auto& point:p){point.x=float(point.x+offset.x);point.y=float(point.y+offset.y);}if(pixel){p[0].x=float(float(std::nearbyint(p[0].x))-.5f);p[1].x=float(float(std::nearbyint(p[1].x))-.5f);p[0].y=float(float(std::nearbyint(p[0].y))-.5f);p[2].y=float(float(std::nearbyint(p[2].y))-.5f);p[1].y=p[0].y;p[2].x=p[0].x;p[3].x=p[1].x;p[3].y=p[2].y;}
    std::copy(std::begin(p),std::end(p),vm.visual.quad.begin());float left=p[0].x,right=left,top=p[0].y,bottom=top;for(u32 i=1;i<4;i++){left=std::min(left,p[i].x);right=std::max(right,p[i].x);top=std::min(top,p[i].y);bottom=std::max(bottom,p[i].y);}
    if(right<float(viewport.x)||bottom<float(viewport.y)||left>float(viewport.x+viewport.width)||top>float(viewport.y+viewport.height))return 0;
    auto& v=vm.visual;const u32 color_mode=v.flags>>17&3;u32 colors[4];if(supplied)std::copy(supplied,supplied+4,colors);else if(color_mode<2){u32 color=color_mode?v.secondary_color:v.color;if((v.render_flags&0x2000000)&&vm.creation_parent)color=multiply_color(color,vm.creation_parent->visual.inherited_color);v.inherited_color=color;for(auto& c:colors)c=tint_color(color);}else{const u32 a=tint_color(v.color),b=tint_color(v.secondary_color);colors[0]=a;colors[3]=b;colors[1]=color_mode==2?b:a;colors[2]=color_mode==2?a:b;}
    if(!material(vm))return -2;Vec2 uv[4];uv[0]={float(v.uv_offset.x+v.uv[0].x),float(v.uv[0].y+v.uv_offset.y)};uv[1]={float(float(float(v.uv[1].x-v.uv[0].x)*v.uv_scale.x)+v.uv_offset.x)+v.uv[0].x,float(v.uv_offset.y+v.uv[1].y)};uv[2]={float(v.uv_offset.x+v.uv[2].x),float(float(float(v.uv[2].y-v.uv[0].y)*v.uv_scale.y)+v.uv_offset.y)+v.uv[0].y};uv[3]={float(float(float(v.uv[3].x-v.uv[2].x)*v.uv_scale.x)+v.uv_offset.x)+v.uv[2].x,float(float(float(v.uv[3].y-v.uv[1].y)*v.uv_scale.y)+v.uv_offset.y)+v.uv[1].y};
    // Trim only proven transparent atlas padding. Keep the original VM quad,
    // colour inheritance and material transitions above, and never crop copy,
    // transformed UV, gradient, or depth-writing draws.
    const auto* resource=vm.sprite_resource?vm.sprite_resource:vm.resource;
    if(indexed_quads&&!supplied&&v.draw_mode()<=1&&color_mode<2&&v.uv_offset.x==0&&v.uv_offset.y==0&&v.uv_scale.x==1&&v.uv_scale.y==1&&vm.sprite&&resource&&u32(v.sprite)<resource->alpha_bounds.size()&&render_state.alphaTest&&render_state.alphaCompare==Compare::GreaterEqual&&render_state.alphaReference>0&&!render_state.depthWrite){
        const auto& sprite=*vm.sprite;const auto& b=resource->alpha_bounds[u32(v.sprite)];
        if((b[0]!=0||b[1]!=0||b[2]!=1||b[3]!=1)&&v.uv[0].x==sprite.u0&&v.uv[0].y==sprite.v0&&v.uv[1].x==sprite.u1&&v.uv[1].y==sprite.v0&&v.uv[2].x==sprite.u0&&v.uv[2].y==sprite.v1&&v.uv[3].x==sprite.u1&&v.uv[3].y==sprite.v1){
            const Vec3 original[4]={p[0],p[1],p[2],p[3]};
            for(u32 i=0;i<4;i++){const float x=b[i&1?2:0],y=b[i&2?3:1];
                p[i]={float(original[0].x+float(float(original[1].x-original[0].x)*x)+float(float(original[2].x-original[0].x)*y)),float(original[0].y+float(float(original[1].y-original[0].y)*x)+float(float(original[2].y-original[0].y)*y)),original[0].z};
                uv[i]={float(sprite.u0+float(float(sprite.u1-sprite.u0)*x)),float(sprite.v0+float(float(sprite.v1-sprite.v0)*y))};
            }
        }
    }
    if(indexed_quads){
        if(!quad_batch)flush();quad_batch=true;
        // The arena owns initialized vertices for its whole lifetime. Write
        // directly into it, avoiding a temporary quad and vector insertion
        // for each particle. Batch boundaries and vertex order are unchanged.
        auto* data=vertices.data()+vertex_count;for(u32 i=0;i<4;i++)data[i]={p[i],1,colors[i],uv[i]};
        vertex_count+=4;if(vertex_count>=4096)flush();return 0;
    }
    constexpr u32 order[]={0,1,2,1,2,3};auto* data=vertices.data()+vertex_count;
    for(u32 i=0;i<6;i++){const u32 j=order[i];data[i]={p[j],1,colors[j],uv[j]};}
    vertex_count+=6;if(vertex_count>=6144)flush();return 0;
}
int AnmRenderer::projected_quad(AnmVm& vm,bool fog){
    Vec3 p[4];const int result=anm_projected_quad(vm,camera,viewport,p);if(result){if(result==-2)error="Projected animation anchor outside range";return result;}if(!fog)return pack_quad(vm,p,false);
    const auto& v=vm.visual;const auto origin=camera.separate_fog_origin?camera.fog_origin:camera.eye;Vec3 delta{float(float(float(v.translation.x+vm.variables.position.x)+v.child_anchor.x)-origin.x),float(float(float(v.translation.y+vm.variables.position.y)+v.child_anchor.y)-origin.y),float(float(float(v.translation.z+vm.variables.position.z)+v.child_anchor.z)-origin.z)};
    if((v.render_flags&0xc0000)&&!vm.creation_parent){if(!vm.environment){error="Projected animation scene dimensions unavailable";return -2;}delta.x=float(float(float(vm.environment->screen_width)*.5f)+delta.x);delta.y=float(float(float(float(vm.environment->screen_height)-448.f)*.5f)+delta.y);}
    const float distance=float(std::sqrt(double(float(float(float(delta.y*delta.y)+float(delta.x*delta.x))+float(delta.z*delta.z)))));const u32 mode=v.flags>>17&3;u32 a=tint_color(mode==1?v.secondary_color:v.color),b=tint_color(v.secondary_color);
    if(distance>camera.fog_near){const float amount=float(float(camera.fog_near-distance)/float(camera.fog_near-camera.fog_far));if(amount>=1)return -1;const float targets[]={camera.fog_rgb.z,camera.fog_rgb.y,camera.fog_rgb.x};const auto fade=[&](u32 color,bool cubic){u32 out=0;for(u32 k=0;k<3;k++){const i32 from=(color>>(k*8))&255,target=i32(targets[k]);out|=u32(u8(from-i32(float(float(from-target)*amount))))<<(k*8);}const float transparency=cubic?float(1.f-float(float(amount*amount)*amount)):float(1.f-amount);return out|(u32(u8(i32(float(transparency*float(color>>24)))))<<24);};a=fade(a,mode<2);if(mode>=2)b=fade(b,false);}
    const u32 colors[]={a,mode==2?b:a,mode>=2&&mode!=2?b:a,mode>=2?b:a};return pack_quad(vm,p,false,colors);
}
int AnmRenderer::world_quad(AnmVm& vm){
    // World quads use a fixed four-vertex unit sprite. Per-instance world and
    // texture matrices carry the actual geometry and atlas selection.
    const auto& v=vm.visual;if(!(v.color>>24))return 0;Matrix4 world;if(!anm_world_quad_transform(vm,world)){error="World animation transform unavailable";return -2;}
    const u32 ax=v.flags>>21&3,ay=v.flags>>23&3;if(ax>2||ay>2){error="World animation anchor outside range";return -2;}
    flush();if(!material(vm))return -2;auto& state=render_state;state.depthWrite=!(v.flags&0x2000);state.textureTransform=true;state.color.second=state.alpha.second={ArgumentSource::Factor};state.textureFactor=tint_color(v.flags&0x60000?v.secondary_color:v.color);
    graphics.set_matrix(MatrixKind::World,world);graphics.set_matrix(MatrixKind::Texture,anm_texture_transform(vm));graphics.set_layout(VertexLayout::WorldUv);
    struct Vertex {Vec3 position;Vec2 uv;};constexpr float xs[3][2]={{-128,128},{0,256},{-256,0}},ys[3][2]={{-128,128},{0,256},{-256,0}};
    const Vertex data[]={{{xs[ax][0],ys[ay][0],0},{0,0}},{{xs[ax][1],ys[ay][0],0},{1,0}},{{xs[ax][0],ys[ay][1],0},{0,1}},{{xs[ax][1],ys[ay][1],0},{1,1}}};
    const bool fog=state.fog;if(v.draw_mode()==15)state.fog=true;graphics.primitives(Topology::Strip,2,data,sizeof(Vertex));if(v.draw_mode()==15)state.fog=false;else state.fog=fog;return 0;
}
int AnmRenderer::mesh(AnmVm& vm){
    const i32 count=vm.variables.integers[0];if(count<1||u32(count)>vm.geometry.world_vertices.size()/2){error="World animation mesh buffer unavailable";return -2;}Matrix4 world;if(!anm_mesh_transform(vm,world)){error="World animation mesh transform unavailable";return -2;}
    flush();if(!material(vm))return -2;auto& state=render_state;state.depthWrite=!(vm.visual.flags&0x2000);state.textureTransform=true;
    graphics.set_matrix(MatrixKind::World,world);graphics.set_matrix(MatrixKind::Texture,anm_texture_transform(vm));graphics.set_layout(VertexLayout::WorldColorUv);graphics.primitives(Topology::Strip,u32(count)*2-2,vm.geometry.world_vertices.data(),sizeof(AnmWorldVertex));return 0;
}
int AnmRenderer::trail(AnmVm& vm){
    auto& data=*vm.geometry.trail;const i32 count=data.age.current;if(count<1||count>64){error="Particle trail vertex count outside range";return -2;}const auto& v=vm.visual;const auto& p=vm.variables.position;Vec3 center{float(float(v.child_anchor.x+v.translation.x)+p.x),float(float(v.child_anchor.y+p.y)+v.translation.y),float(float(v.child_anchor.z+p.z)+v.translation.z)};if(!anm_adjust_position(vm,center)){error="Particle trail position unavailable";return -2;}
    flush();if(!material(vm,false))return -2;graphics.set_depth_mask(false);auto& state=render_state;state.color.operation=state.alpha.operation=ColorOperation::First;state.color.first=state.alpha.first={ArgumentSource::Diffuse};shapes.clear();for(i32 i=0;i<count;i++)shapes.push_back({{float(center.x+data.points[u32(i)].x),float(center.y+data.points[u32(i)].y),0},1,data.colors[u32(i)]});graphics.set_layout(VertexLayout::ScreenColor);graphics.primitives(Topology::LineStrip,u32(count-1),shapes.data(),sizeof(ColorVertex));return 0;
}
int AnmRenderer::draw(AnmVm& vm){
#if TH17_DEVELOPMENT_HARNESS
 ++render_modes[vm.visual.draw_mode()&31];
#endif
    if(vm.geometry.overlay)return overlay(vm);
    if(vm.geometry.trail)return trail(vm);
    const auto& v=vm.visual;if(vm.geometry.distortion){flush();if(!material(vm))return -2;graphics.set_depth_mask(false);graphics.set_layout(VertexLayout::ScreenColorUv);graphics.primitives(Topology::Fan,31,vm.geometry.distortion->vertices.data(),sizeof(AnmGeometryVertex));}
    if((v.flags&3)!=3||(v.render_flags&0x60))return -1;const u32 mode=v.draw_mode();if(mode==8||mode==15){if(!(v.color>>24)&&!(v.secondary_color>>24))return -1;return world_quad(vm);}if(mode==24||mode==25)return mesh(vm);
    if(render_state.depthWrite){flush();graphics.set_depth_mask(false);}if(mode>=16&&mode<=22||mode==26||mode==27)return shape(vm);if(mode<=3){if(!(v.color>>24)&&!(v.secondary_color>>24))return -1;return quad(vm,mode==0);}
    if(mode==4||mode==6){if(!(v.color>>24)&&!(v.secondary_color>>24))return -1;return projected_quad(vm,mode==6);}
    if(mode==10||mode==23||mode>25)return 0;if(mode==9||mode==11||mode==12||mode==13||mode==14){if(mode!=11&&!(v.color>>24))return -1;const i32 count=vm.variables.integers[0];if(count<2||u32(count)>vm.geometry.screen_vertices.size()/2){error="Animation strip buffer unavailable";return -2;}flush();if(!material(vm))return -2;graphics.set_layout(VertexLayout::ScreenColorUv);graphics.primitives(mode==11?Topology::Fan:Topology::Strip,u32(count)*2-2,vm.geometry.screen_vertices.data(),sizeof(AnmGeometryVertex));return 0;}
    error="Animation drawing mode not yet implemented: "+std::to_string(mode);return -2;
}
bool AnmRenderer::draw_layer(const std::vector<AnmVm*>& layer){for(auto* vm:layer)if(draw(*vm)==-2)return false;return true;}
int AnmRenderer::draw_screen_strip(AnmVm& vm,const AnmGeometryVertex* data,u32 count){
    if((vm.visual.flags&3)!=3||!vm.sprite)return -1;if(count<3)return 0;if(!data||count>0x200000){error="Screen strip vertex range invalid";return -2;}if(!material(vm))return -2;
    if(count<18){if(quad_batch)flush();quad_batch=false;for(u32 i=0;i+2<count;i++){const u32 order[3]={i+(i&1),i+((i&1)^1u),i+2};for(u32 j:order)vertices[vertex_count++]=data[j];if(vertex_count>=6144)flush();}return 0;}
    // Curved lasers already own their complete strip. Submit it to the shared
    // renderer, which copies/joins it safely, instead of expanding it into
    // three vertices per triangle in the sprite arena first.
    flush();graphics.set_layout(VertexLayout::ScreenColorUv);graphics.primitives(Topology::Strip,count-2,data,sizeof(AnmGeometryVertex));return 0;
}
}
