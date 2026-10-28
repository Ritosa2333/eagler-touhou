#include "AnmRenderer.hpp"
#include "AnmOverlay.hpp"
#include "Probe.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#if TH13_DRAW_CAPTURE
#include "DrawCapture.hpp"  // 语义绘制清单钩子（draw-contract.md；仅 harness 构建）
#endif
namespace th13 {
using namespace touhou::graphics;
AnmDrawProbe anm_draw_probe;
#if TH13_DRAW_CAPTURE
// 提交语境钩子：每个精灵的顶点进入渲染批次时登记层号与九宫格锚点组（原版
// 0x46f380 层 watch + startVertex 公式的移植侧同构物）。captures 为空（web 构建）
// 时零开销。
// [层号错记修正] 钩子必须在 material() 之后、顶点入批之前调用：material 内的
// flush 会提交「上一个 VM」的遗留批次——若钩子先于它落笔，该批 op 的 layer
// 会被当前 VM 覆盖（串扰=下一绘制调用的层，f349 title00a 右半 22→23 实证）。
// 各绘制路径（quad 族/shape/world/mesh/trail/distortion/strip）就地挂钩；
// overlay 掩蔽矩形在 draw() 入口挂（其掩蔽 flush 保持历史行为，见 draw()）。
#define TH13_DRAW_SPRITE(vm) \
    do { \
        if (draw_capture) draw_capture->sprite((vm).visual.layer, (vm).visual.flags); \
    } while (false)
#else
#define TH13_DRAW_SPRITE(vm) do { } while (false)  // web 构建：钩子编译出局
#endif
namespace {u32 multiply_color(u32 a,u32 b){u32 out=0;for(u32 shift=0;shift<32;shift+=8)out|=std::min(255u,(((a>>shift)&255)*((b>>shift)&255))>>7)<<shift;return out;}}
void AnmRenderer::flush(){if(vertices.empty())return;graphics.set_layout(VertexLayout::ScreenColorUv);graphics.triangles(vertices.size()/3,vertices.data(),sizeof(AnmGeometryVertex));vertices.clear();}
void AnmRenderer::invalidate(){flush();bound_texture=~0u;}
// 视口切换只冲顶点批，不丢纹理缓存：原版 SetViewport（0x45d2f0:30 每帧）
// 不清精灵侧纹理缓存 mgr+0xb78188（oracle f1-8 tex=None 的缓存语义）。
void AnmRenderer::set_viewport(const GraphicsViewport& value){flush();viewport=value;graphics.set_viewport(value);}
void AnmRenderer::set_camera(const AnmCamera& value){flush();graphics.presentation_camera(false);camera=value;graphics.set_matrix(MatrixKind::View,camera.view);graphics.set_matrix(MatrixKind::Projection,camera.projection);}
u32 AnmRenderer::tint_color(u32 color)const noexcept{return tint_enabled?multiply_color(color,tint):color;}
bool AnmRenderer::material(AnmVm& vm,bool textured){
    if(textured){auto* source=vm.sprite_resource?vm.sprite_resource:vm.resource;const auto* sprite=vm.texture_sprite();if(!source||!sprite){error="Animation texture source unavailable: script="+std::to_string(vm.source_script)+" sprite="+std::to_string(vm.visual.sprite)+" mode="+std::to_string(vm.visual.draw_mode())+" flags="+std::to_string(vm.visual.flags)+" resource="+(source&&!source->textures.empty()?source->textures[0].name:"none")+" sprites="+std::to_string(source?source->sprites.size():0);return false;}
        // The original material lookup uses the bank and sprite index even before
        // an ANM script has executed sprite selection (chapter root 241 does this).
        // Keep its zero geometry/UVs; selecting a sprite here would change them.
        const u32 texture=graphics.texture(*source,sprite->texture);if(texture!=bound_texture){flush();bound_texture=texture;graphics.bind_texture(texture);}}
    const auto& v=vm.visual;auto next=graphics.pipeline();const u32 mode=v.blend_mode();next.alphaTest=true;
    // [原版机制·黑块根因] 原版 ANM quad 全族以 ALPHABLENDENABLE=TRUE 绘制
    // （oracle title-draws 全窗 blend 三元组实证：mode0=[5,6,1]、mode1=[5,2,1]、
    // mode2=[5,2,3]、mode3=[2,1,1]——D3D SRCALPHA=5/INVSRCALPHA=6/ONE=2/ZERO=1，
    // 第三元=eq ADD=1/REVSUB=3；mode3 的 (One,Zero) 也在混合启用下提交）。
    // th15 共享基座靠 configure_game 预置 blend=true；th13 不经 configure_game
    // （GraphicsDevice::initialize 仅写真机 TFACTOR=0），缺此位导致 alpha=0
    // 纹素以不透明黑写入——菜单字条/LOGO/立绘全族黑块的用户可见根因。
    // 按模式落混合因子+恒开混合位（原版 SetRenderState 语义）。
    next.blend=true;
    switch(mode){
    case 0:next.sourceBlend=BlendFactor::SourceAlpha;next.destinationBlend=BlendFactor::InverseSourceAlpha;next.blendEquation=BlendEquation::Add;break;
    case 1:next.sourceBlend=BlendFactor::SourceAlpha;next.destinationBlend=BlendFactor::One;next.blendEquation=BlendEquation::Add;break;
    case 2:next.sourceBlend=BlendFactor::SourceAlpha;next.destinationBlend=BlendFactor::One;next.blendEquation=BlendEquation::ReverseSubtract;break;
    case 3:next.alphaTest=false;next.sourceBlend=BlendFactor::One;next.destinationBlend=BlendFactor::Zero;next.blendEquation=BlendEquation::Add;break;
    case 4:next.sourceBlend=BlendFactor::InverseDestinationColor;next.destinationBlend=BlendFactor::InverseSourceColor;next.blendEquation=BlendEquation::Add;break;
    case 5:next.sourceBlend=BlendFactor::DestinationColor;next.destinationBlend=BlendFactor::Zero;next.blendEquation=BlendEquation::Add;break;
    case 6:next.sourceBlend=BlendFactor::InverseSourceColor;next.destinationBlend=BlendFactor::InverseSourceAlpha;next.blendEquation=BlendEquation::Add;break;
    case 7:next.sourceBlend=BlendFactor::DestinationAlpha;next.destinationBlend=BlendFactor::InverseDestinationAlpha;next.blendEquation=BlendEquation::Add;break;
    case 8:next.sourceBlend=BlendFactor::SourceAlpha;next.destinationBlend=BlendFactor::One;next.blendEquation=BlendEquation::Minimum;break;
    case 9:next.sourceBlend=BlendFactor::SourceAlpha;next.destinationBlend=BlendFactor::One;next.blendEquation=BlendEquation::Maximum;break;
    default:break;
    }
    next.minFilter=next.magFilter=v.render_flags&0x800?Filter::Nearest:Filter::Linear;const u32 u=v.render_flags&3,w=v.flags>>30;
    if(u<3)next.addressU=u==0?Address::Repeat:u==1?Address::Clamp:Address::Mirror;if(w<3)next.addressV=w==0?Address::Repeat:w==1?Address::Clamp:Address::Mirror;
    next.textureTransform=false;next.color.operation=next.alpha.operation=ColorOperation::Multiply;next.color.first=next.alpha.first={ArgumentSource::Texture};next.color.second=next.alpha.second={ArgumentSource::Diffuse};
    if(!graphics.pipeline().compatible(next))flush();graphics.pipeline()=next;return true;
}
void AnmRenderer::submit(AnmVm& vm,Topology kind,u32 count,const void* data,u32 stride){graphics.presentation_offset({});graphics.presentation_range(vm,0,vertex_count(kind,count),presentation_instance);graphics.primitives(kind,count,data,stride);}
int AnmRenderer::shape(AnmVm& vm){
    const auto& v=vm.visual;const u32 mode=v.draw_mode();float width=float(v.sprite_size.x*v.scale.x),height=float(v.sprite_size.y*v.scale.y),angle=vm.variables.rotation.z;Vec3 center;if(!anm_position(vm,center)){error="Primitive animation position unavailable";return -2;}
    if(vm.creation_parent&&!(v.render_flags&0x10000)){const auto& parent=*vm.creation_parent;width=float(parent.visual.scale.x*width);height=float(parent.visual.scale.y*height);angle=float(parent.variables.rotation.z+angle);}
    const u32 resolution=v.render_flags>>20&7;if(resolution==1||resolution==2){if(!vm.environment){error="Primitive animation scene scale unavailable";return -2;}const float factor=resolution==1?vm.environment->resolution_scale:float(vm.environment->resolution_scale*.5f);width=float(factor*width);height=float(factor*height);}
    flush();if(!material(vm,false))return -2;TH13_DRAW_SPRITE(vm);auto& state=graphics.pipeline();state.textureTransform=false;state.color.operation=state.alpha.operation=ColorOperation::First;state.color.first=state.alpha.first={ArgumentSource::Diffuse};graphics.set_layout(VertexLayout::ScreenColor);
    // 0046a700/0046aa2d pass primary and secondary to opposite rectangle
    // edges; 604 is a filled fan, while 605 is an outline, not a strip.
    if(mode==15||mode==19||mode>=20){const u32 ax=v.flags>>21&3,ay=v.flags>>23&3;if(ax>2||ay>2){error="Primitive animation anchor outside range";return -2;}constexpr float xs[3][2]={{-.5f,.5f},{0,1},{-1,0}},ys[3][2]={{-.5f,.5f},{0,1},{-1,0}};const float sine=float(std::sin(double(angle))),cosine=float(std::cos(double(angle)));const u32 primary=v.color,secondary=mode==19||mode==21?v.secondary_color:primary;
        const auto rectangle=[&](float w,float h,u32 a,u32 b){ColorVertex data[4];for(u32 i=0;i<4;i++){const float x=float(xs[ax][i&1]*w),y=float(ys[ay][i>>1]*h);data[i]={{float(float(float(x*cosine)-float(y*sine))+center.x),float(float(float(y*cosine)+float(x*sine))+center.y),0},1,i&1?b:a};}submit(vm,Topology::Strip,2,data,sizeof(ColorVertex));};
        if(mode==20||mode==21)rectangle(float(width+1),float(height+1),((primary>>1)&0x7f000000)|(primary&0xffffff),((secondary>>1)&0x7f000000)|(secondary&0xffffff));rectangle(width,height,primary,secondary);return 0;
    }
    const i32 count=vm.variables.integers[0];if(count<1||count>0x100000){error="Primitive animation segment count invalid";return -2;}shapes.clear();const float step=float(6.283185482025146484375f/float(count));if(mode==16)shapes.push_back({{center.x,center.y,0},1,v.color});
    for(i32 i=0;i<=count;i++){const Vec3 point{float(float(std::cos(double(angle))*double(width))+center.x),float(float(std::sin(double(angle))*double(width))+center.y),0};shapes.push_back({point,1,mode==16?v.secondary_color:v.color});angle=normalize_angle(float(angle+step));}
    if(mode==16){state.cull=Cull::None;submit(vm,Topology::Fan,count,shapes.data(),sizeof(ColorVertex));}else submit(vm,Topology::LineStrip,count,shapes.data(),sizeof(ColorVertex));return 0;
}
int AnmRenderer::circle(AnmVm& vm){
    auto& data=*vm.geometry.circle;flush();if(!material(vm,false))return -2;TH13_DRAW_SPRITE(vm);
    auto& state=graphics.pipeline();state.color.operation=state.alpha.operation=ColorOperation::First;state.color.first=state.alpha.first={ArgumentSource::Diffuse};graphics.set_layout(VertexLayout::ScreenColor);
    shapes.clear();for(const auto& v:data.vertices)shapes.push_back({v.position,1,tint_color(v.color)});
    state.cull=Cull::None;submit(vm,Topology::Fan,32,shapes.data(),sizeof(ColorVertex));return 0;
}
int AnmRenderer::draw_glyph(AnmVm& vm){
    if(anm_draw_probe.enabled)++anm_draw_probe.glyphs;
    // 捕获钩子在 pack_quad 内（material 后）；此处不再入口登记（次序修正见文件头注）。
    // Preserve original atlas glyphs. At integer high-resolution scale, sample
    // their texels directly rather than blurring them a second time.
    const u32 flags=vm.visual.render_flags;
    if(vm.environment&&vm.environment->raster_scale>1&&vm.visual.scale.x==1&&vm.visual.scale.y==1)vm.visual.render_flags|=0x800;
    const int result=quad(vm,true);vm.visual.render_flags=flags;return result;
}
int AnmRenderer::quad(AnmVm& vm,bool pixel){
    Vec3 p[4];const u32 flags=vm.visual.flags;if(vm.visual.draw_mode()==3)vm.visual.flags=(flags&~0x3e000000u)|0x2000000;const bool success=anm_quad_positions(vm,p);vm.visual.flags=flags;if(!success){error="Animation quad geometry unavailable: flags="+std::to_string(vm.visual.flags)+" render="+std::to_string(vm.visual.render_flags)+" script="+std::to_string(vm.source_script)+" mode="+std::to_string(vm.visual.draw_mode());return -2;}
    return pack_quad(vm,p,pixel);
}
int AnmRenderer::pack_quad(AnmVm& vm,Vec3 (&p)[4],bool pixel,const u32* supplied){
    // 临时探针：四边形顶点取证（-96px 排查；按 layer+script 去重）
    if(probe::enabled){static u32 seen_count=0;const u64 key=(u64(vm.visual.layer)<<32)|u32(vm.source_script)*2654435761u+u32(vm.visual.draw_mode());static u64 seen[256]={};bool dup=false;for(u32 i=0;i<seen_count&&i<256;i++)if(seen[i]==key){dup=true;break;}if(!dup&&seen_count<256){seen[seen_count++]=key;std::printf("[probe-quad] layer=%u script=%d mode=%u rflags=%06x p0=(%.3f,%.3f) p2=(%.3f,%.3f) envoff2=%d\n",vm.visual.layer,vm.source_script,vm.visual.draw_mode(),vm.visual.render_flags,double(p[0].x),double(p[0].y),double(p[2].x),double(p[2].y),vm.environment?vm.environment->screen_offsets[2]:-1);}}
    for(auto& point:p){point.x=float(point.x+offset.x);point.y=float(point.y+offset.y);}if(pixel&&!vm.presentation_motion&&vm.interpolators.position.duration<=0){const float scale=vm.environment?vm.environment->raster_scale:1;const auto snap=[scale](float value){return float(std::nearbyint(value*scale)/scale-.5f);};p[0].x=snap(p[0].x);p[1].x=snap(p[1].x);p[0].y=snap(p[0].y);p[2].y=snap(p[2].y);p[1].y=p[0].y;p[2].x=p[0].x;p[3].x=p[1].x;p[3].y=p[2].y;}
    if(pixel&&(vm.presentation_motion||vm.interpolators.position.duration>0))for(auto& point:p){point.x-=.5f;point.y-=.5f;}
    std::copy(std::begin(p),std::end(p),vm.visual.quad.begin());float left=p[0].x,right=left,top=p[0].y,bottom=top;for(u32 i=1;i<4;i++){left=std::min(left,p[i].x);right=std::max(right,p[i].x);top=std::min(top,p[i].y);bottom=std::max(bottom,p[i].y);}
    if(right<float(viewport.x)||bottom<float(viewport.y)||left>float(viewport.x+viewport.width)||top>float(viewport.y+viewport.height))return 0;
    auto& v=vm.visual;const u32 color_mode=v.flags>>17&3;u32 colors[4];if(supplied)std::copy(supplied,supplied+4,colors);else if(color_mode<2){u32 color=color_mode?v.secondary_color:v.color;if((v.render_flags&0x2000000)&&vm.creation_parent)color=multiply_color(color,vm.creation_parent->visual.inherited_color);v.inherited_color=color;for(auto& c:colors)c=tint_color(color);}else{const u32 a=tint_color(v.color),b=tint_color(v.secondary_color);colors[0]=a;colors[3]=b;colors[1]=color_mode==2?b:a;colors[2]=color_mode==2?a:b;}
    if(!material(vm))return -2;TH13_DRAW_SPRITE(vm);Vec2 uv[4];uv[0]={float(v.uv_offset.x+v.uv[0].x),float(v.uv[0].y+v.uv_offset.y)};uv[1]={float(float(float(v.uv[1].x-v.uv[0].x)*v.uv_scale.x)+v.uv_offset.x)+v.uv[0].x,float(v.uv_offset.y+v.uv[1].y)};uv[2]={float(v.uv_offset.x+v.uv[2].x),float(float(float(v.uv[2].y-v.uv[0].y)*v.uv_scale.y)+v.uv_offset.y)+v.uv[0].y};uv[3]={float(float(float(v.uv[3].x-v.uv[2].x)*v.uv_scale.x)+v.uv_offset.x)+v.uv[2].x,float(float(float(v.uv[3].y-v.uv[1].y)*v.uv_scale.y)+v.uv_offset.y)+v.uv[1].y};
    graphics.presentation_offset(offset);graphics.presentation_range(vm,vertices.size(),6,presentation_instance);
    for(u32 index:{0u,1u,2u,1u,2u,3u})vertices.push_back({p[index],1,colors[index],uv[index]});if(vertices.size()>=6144)flush();return 0;
}
int AnmRenderer::projected_quad(AnmVm& vm,bool fog){
    Vec3 p[4];const int result=anm_projected_quad(vm,camera,viewport,p);if(result){if(result==-2)error="Projected animation anchor outside range";return result;}if(!fog)return pack_quad(vm,p,false);
    const auto& v=vm.visual;Vec3 delta{float(float(float(v.child_anchor.x+v.translation.x)+vm.variables.position.x)-camera.eye.x),float(float(float(v.child_anchor.y+v.translation.y)+vm.variables.position.y)-camera.eye.y),float(float(float(v.child_anchor.z+v.translation.z)+vm.variables.position.z)-camera.eye.z)};
    if((v.render_flags&0xc0000)&&!vm.creation_parent){if(!vm.environment){error="Projected animation scene dimensions unavailable";return -2;}delta.x=float(float(float(vm.environment->screen_width)*.5f)+delta.x);delta.y=float(float(float(float(vm.environment->screen_height)-448.f)*.5f)+delta.y);}
    const float distance=float(std::sqrt(double(float(float(float(delta.y*delta.y)+float(delta.x*delta.x))+float(delta.z*delta.z)))));const u32 mode=v.flags>>17&3;u32 a=tint_color(mode==1?v.secondary_color:v.color),b=tint_color(v.secondary_color);
    if(distance>camera.fog_near){const float amount=float(float(camera.fog_near-distance)/float(camera.fog_near-camera.fog_far));if(amount>=1)return -1;const float targets[]={camera.fog_rgb.z,camera.fog_rgb.y,camera.fog_rgb.x};const auto fade=[&](u32 color,bool cubic){u32 out=0;for(u32 k=0;k<3;k++){const i32 from=(color>>(k*8))&255,target=i32(targets[k]);out|=u32(u8(from-i32(float(float(from-target)*amount))))<<(k*8);}const float transparency=cubic?float(1.f-float(float(amount*amount)*amount)):float(1.f-amount);return out|(u32(u8(i32(float(transparency*float(color>>24)))))<<24);};a=fade(a,mode<2);if(mode>=2)b=fade(b,false);}
    const u32 colors[]={a,mode==2?b:a,mode>=2&&mode!=2?b:a,mode>=2?b:a};return pack_quad(vm,p,false,colors);
}
int AnmRenderer::world_quad(AnmVm& vm){
    // World quads use a fixed four-vertex unit sprite. Per-instance world and
    // texture matrices carry the actual geometry and atlas selection.
    const auto& v=vm.visual;if(!(v.color>>24))return 0;Matrix4 world;if(!anm_world_quad_transform(vm,world)){error="World animation transform unavailable";return -2;}
    // 临时探针：world 矩阵平移取证（-96px 排查；按 layer+script 去重）
    if(probe::enabled){static u32 wseen_count=0;const u64 key=(u64(v.layer)<<32)|u32(vm.source_script);static u64 wseen[256]={};bool dup=false;for(u32 i=0;i<wseen_count&&i<256;i++)if(wseen[i]==key){dup=true;break;}if(!dup&&wseen_count<256){wseen[wseen_count++]=key;std::printf("[probe-world] layer=%u script=%d rflags=%06x world12=%.3f world13=%.3f envoff2=%d\n",v.layer,vm.source_script,v.render_flags,double(world.m[12]),double(world.m[13]),vm.environment?vm.environment->screen_offsets[2]:-1);}}

    const u32 ax=v.flags>>21&3,ay=v.flags>>23&3;if(ax>2||ay>2){error="World animation anchor outside range";return -2;}
    flush();if(!material(vm))return -2;TH13_DRAW_SPRITE(vm);auto& state=graphics.pipeline();state.depthWrite=!(v.flags&0x2000);state.textureTransform=true;state.color.second=state.alpha.second={ArgumentSource::Factor};state.textureFactor=tint_color(v.flags&0x60000?v.secondary_color:v.color);
    graphics.set_matrix(MatrixKind::World,world);graphics.set_matrix(MatrixKind::Texture,anm_texture_transform(vm));graphics.set_layout(VertexLayout::WorldUv);
    struct Vertex {Vec3 position;Vec2 uv;};constexpr float xs[3][2]={{-128,128},{0,256},{-256,0}},ys[3][2]={{-128,128},{0,256},{-256,0}};
    const Vertex data[]={{{xs[ax][0],ys[ay][0],0},{0,0}},{{xs[ax][1],ys[ay][0],0},{1,0}},{{xs[ax][0],ys[ay][1],0},{0,1}},{{xs[ax][1],ys[ay][1],0},{1,1}}};
    const bool fog=state.fog;if(v.draw_mode()==15)state.fog=true;submit(vm,Topology::Strip,2,data,sizeof(Vertex));if(v.draw_mode()==15)state.fog=false;else state.fog=fog;return 0;
}
int AnmRenderer::mesh(AnmVm& vm){
    const i32 count=vm.geometry.wind?32:vm.geometry.ribbon?16:vm.variables.integers[0];if(count<1||(!vm.geometry.wind&&!vm.geometry.ribbon&&u32(count)>vm.geometry.world_vertices.size()/2)){error="World animation mesh buffer unavailable";return -2;}Matrix4 world;if(!anm_mesh_transform(vm,world)){error="World animation mesh transform unavailable";return -2;}
    flush();if(!material(vm))return -2;TH13_DRAW_SPRITE(vm);auto& state=graphics.pipeline();state.depthWrite=!(vm.visual.flags&0x2000);state.textureTransform=true;
    graphics.set_matrix(MatrixKind::World,world);graphics.set_matrix(MatrixKind::Texture,anm_texture_transform(vm));graphics.set_layout(VertexLayout::WorldColorUv);submit(vm,Topology::Strip,u32(count)*2-2,(vm.geometry.wind?vm.geometry.wind->vertices.data():vm.geometry.ribbon?vm.geometry.ribbon->vertices.data():vm.geometry.world_vertices.data()),sizeof(AnmWorldVertex));return 0;
}
int AnmRenderer::trail(AnmVm& vm){
    auto& data=*vm.geometry.trail;const i32 count=data.age.current;if(count<1||count>64){error="Particle trail vertex count outside range";return -2;}const auto& v=vm.visual;const auto& p=vm.variables.position;Vec3 center{float(float(v.child_anchor.x+v.translation.x)+p.x),float(float(v.child_anchor.y+p.y)+v.translation.y),float(float(v.child_anchor.z+p.z)+v.translation.z)};if(!anm_adjust_position(vm,center)){error="Particle trail position unavailable";return -2;}
    flush();if(!material(vm,false))return -2;TH13_DRAW_SPRITE(vm);graphics.set_depth_mask(false);auto& state=graphics.pipeline();state.color.operation=state.alpha.operation=ColorOperation::First;state.color.first=state.alpha.first={ArgumentSource::Diffuse};shapes.clear();for(i32 i=0;i<count;i++)shapes.push_back({{float(center.x+data.points[u32(i)].x),float(center.y+data.points[u32(i)].y),0},1,data.colors[u32(i)]});graphics.set_layout(VertexLayout::ScreenColor);submit(vm,Topology::LineStrip,u32(count-1),shapes.data(),sizeof(ColorVertex));return 0;
}
int AnmRenderer::draw(AnmVm& vm){
    if(anm_draw_probe.enabled){
        ++anm_draw_probe.calls;
        const auto& v0=vm.visual;
        if(v0.draw_mode()<=3)++anm_draw_probe.quads;else if(v0.draw_mode()>=15&&v0.draw_mode()<=22)++anm_draw_probe.shapes_drawn;else ++anm_draw_probe.other;
        for(u32 i=0;i<AnmDrawProbe::kSlots;i++){
            auto& probe=anm_draw_probe;
            if(probe.sprite_ids[i]==v0.sprite||probe.sprite_counts[i]==0){
                probe.sprite_ids[i]=v0.sprite;++probe.sprite_counts[i];
                probe.sprite_sizes[i][0]=u32(v0.sprite_size.x);probe.sprite_sizes[i][1]=u32(v0.sprite_size.y);
                break;
            }
        }
    }
    // 捕获钩子次序（见文件头注）：overlay 掩蔽矩形在 overlay() 内部最先提交，
    // 其入口就地登记（掩蔽 flush 的遗留批次语义维持历史行为，非 overlay 路径
    // 已在各绘制路径 material 后挂钩修正）；overlay 面板经 draw() 递归自挂。
    if(vm.geometry.overlay){TH13_DRAW_SPRITE(vm);return overlay(vm);}
    if(vm.geometry.circle)return circle(vm);
    if(vm.geometry.trail)return trail(vm);
    if(vm.geometry.wind){
        // 402340 draws in the STD camera inherited from406d90/407310. The
        // web scheduler restores a2D camera at the end of its STD pass, so
        // select that same saved STD state explicitly for this world effect.
        const auto saved_camera=camera;const auto saved_viewport=viewport;
        if(scenery_camera_ready){set_viewport(scenery_viewport);set_camera(scenery_camera);}
        flush();graphics.set_fog_color(camera.fog_color);graphics.set_fog_range(camera.fog_near,camera.fog_far);
        graphics.set_fog(true);graphics.set_depth_compare(Compare::LessEqual);
        const int result=mesh(vm);flush();graphics.set_depth_compare(Compare::Always);graphics.set_fog(false);
        if(scenery_camera_ready){set_viewport(saved_viewport);set_camera(saved_camera);}
        return result;
    }
    if(vm.geometry.ribbon){flush();graphics.set_fog(true);const int result=mesh(vm);flush();graphics.set_fog(false);return result;}
    const auto& v=vm.visual;if(vm.geometry.distortion){flush();if(!material(vm))return -2;TH13_DRAW_SPRITE(vm);graphics.set_depth_mask(false);graphics.set_layout(VertexLayout::ScreenColorUv);submit(vm,Topology::Fan,31,vm.geometry.distortion->vertices.data(),sizeof(AnmGeometryVertex));}
    if((v.flags&3)!=3||(v.render_flags&0x20))return -1;const u32 mode=v.draw_mode();if(mode==8||mode==18){if(!(v.color>>24)&&!(v.secondary_color>>24))return -1;if(mode==18){flush();graphics.set_fog(true);}const int result=world_quad(vm);if(mode==18){flush();graphics.set_fog(false);}return result;}if(mode==24||mode==25)return mesh(vm);
    if(graphics.pipeline().depthWrite){flush();graphics.set_depth_mask(false);}if(mode>=15&&mode<=22)return shape(vm);if(mode<=3){if(!(v.color>>24)&&!(v.secondary_color>>24))return -1;return quad(vm,mode==0);}
    if(mode==4||mode==6){if(!(v.color>>24)&&!(v.secondary_color>>24))return -1;return projected_quad(vm,mode==6);}
    if(mode==10||mode==23||mode>25)return 0;if(mode==9||mode==11||mode==12||mode==13||mode==14){if(mode!=11&&!(v.color>>24))return -1;const i32 count=vm.variables.integers[0];if(count<2||u32(count)>vm.geometry.screen_vertices.size()/2){error="Animation strip buffer unavailable";return -2;}flush();if(!material(vm))return -2;TH13_DRAW_SPRITE(vm);graphics.set_layout(VertexLayout::ScreenColorUv);submit(vm,mode==11?Topology::Fan:Topology::Strip,u32(count)*2-2,vm.geometry.screen_vertices.data(),sizeof(AnmGeometryVertex));return 0;}
    error="Animation drawing mode not yet implemented: "+std::to_string(mode);return -2;
}
// UP 批冲批纪律（f349/f61 双窗实证，蓝图 §8.9 诊断清单裁定）：原版 UP 批仅按
// 材质键（纹理+管线态）冲批，层不是冲批触发——f61 敌机 UP 批跨上下文并入
// L56 终批即铁证；f349 的三段 title_logo op 全部由键差驱动（97/98 blend=1 vs
// 96/100 blend=0）。故此处不做层边界 flush（加了会破坏对局窗 L56 终批口径，
// 实测首分歧 511→61）；op 的 layer 字段精度由捕获钩子次序保证（顶点入批时
// 登记 VM 真值层，见文件头注）。
bool AnmRenderer::draw_layer(const std::vector<AnmVm*>& layer){for(auto* vm:layer)if(draw(*vm)==-2)return false;return true;}
int AnmRenderer::draw_screen_strip(AnmVm& vm,const AnmGeometryVertex* data,u32 count){
    if(anm_draw_probe.enabled)++anm_draw_probe.strips;
    if((vm.visual.flags&3)!=3||!vm.sprite)return -1;if(count<3)return 0;if(!data||count>0x200000){error="Screen strip vertex range invalid";return -2;}if(!material(vm))return -2;
    if(count<18){if(vertices.size()+(count-2)*3>6144)flush();TH13_DRAW_SPRITE(vm);graphics.presentation_offset({});graphics.presentation_range(vm,vertices.size(),(count-2)*3,presentation_instance);for(u32 i=0;i+2<count;i++){const u32 order[3]={i+(i&1),i+((i&1)^1u),i+2};for(u32 j:order)vertices.push_back(data[j]);if(vertices.size()>=6144)flush();}return 0;}
    // Curved lasers already own their complete strip. Submit it to the shared
    // renderer, which copies/joins it safely, instead of expanding it into
    // three vertices per triangle in the sprite arena first.
    TH13_DRAW_SPRITE(vm);flush();graphics.set_layout(VertexLayout::ScreenColorUv);submit(vm,Topology::Strip,count-2,data,sizeof(AnmGeometryVertex));return 0;
}
}
