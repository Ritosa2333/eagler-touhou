#include "AnmRenderer.hpp"
#include "PresentationAudit.hpp"
#include "GameMath.hpp"
#include <cmath>
#include "../../../portable/numeric/SpriteNumber.hpp"
namespace th10 {
namespace {
Extended sum(float a,float b,float c){return number(a)+number(b)+number(c);}
Extended unsigned_number(u32 bits){auto result=Extended::from_int(static_cast<i32>(bits));if(bits&0x80000000)result=result+number(4294967296.0f);return result;}
void depth(AnmVertex* quad,Extended value){const auto z=value.to_float();for(u32 i=0;i<4;++i)quad[i].position.z=z;}
}
// 0x443080 / 0x443290. Keep their different store points and coordinate order.
u32 AnmRenderer::axis_geometry(const AnmVm& vm,AnmVertex* q,bool pixel) noexcept {
    auto evaluate=[&](auto number)->u32{
        using N=decltype(number(0));
        auto sum_values=[&](float a,float b,float c){return number(a)+number(b)+number(c);};
        auto write_depth=[](auto* q,N z){for(unsigned i=0;i<4;++i)q[i].position.z=z.to_float();};

    auto width=number(vm.sprite_size.x)*number(vm.scale.x);if(pixel)width=number(width.to_float());
    const auto height_product=number(vm.sprite_size.y)*number(vm.scale.y);
    const auto height=number(height_product.to_float()),half_height=number((height_product*number(0.5f)).to_float());
    switch((vm.flags>>18)&3){
    case 0:{auto left=sum_values(vm.child_position.x,vm.position.x,vm.script_position.x)-width*number(0.5f);
        q[0].position.x=q[2].position.x=left.to_float();q[1].position.x=q[3].position.x=(number(q[0].position.x)+width).to_float();break;}
    case 1:q[0].position.x=q[2].position.x=sum_values(vm.script_position.x,vm.position.x,vm.child_position.x).to_float();q[1].position.x=q[3].position.x=(number(vm.script_position.x)+number(vm.position.x)+width+number(vm.child_position.x)).to_float();break;
    case 2:q[0].position.x=q[2].position.x=(sum_values(vm.script_position.x,vm.position.x,vm.child_position.x)-width).to_float();q[1].position.x=q[3].position.x=sum_values(vm.script_position.x,vm.position.x,vm.child_position.x).to_float();break;
    }
    switch((vm.flags>>20)&3){
    case 0:{auto top=sum_values(vm.child_position.y,vm.position.y,vm.script_position.y)-half_height;q[0].position.y=q[1].position.y=top.to_float();q[2].position.y=q[3].position.y=(number(q[0].position.y)+height).to_float();break;}
    case 1:if(pixel){const auto top=sum_values(vm.position.y,vm.child_position.y,vm.script_position.y);q[0].position.y=q[1].position.y=top.to_float();q[2].position.y=q[3].position.y=(top+height).to_float();}
        else{q[0].position.y=q[1].position.y=sum_values(vm.child_position.y,vm.script_position.y,vm.position.y).to_float();q[2].position.y=q[3].position.y=(number(vm.child_position.y)+number(vm.script_position.y)+height+number(vm.position.y)).to_float();}break;
    case 2:{const auto bottom=pixel?sum_values(vm.position.y,vm.child_position.y,vm.script_position.y):sum_values(vm.child_position.y,vm.script_position.y,vm.position.y);q[0].position.y=q[1].position.y=(bottom-height).to_float();q[2].position.y=q[3].position.y=bottom.to_float();break;}
    }
    // The non-pixel original reads child Y here, including when rotation is zero.
    write_depth(q,sum_values(pixel?vm.child_position.z:vm.child_position.y,vm.position.z,vm.script_position.z));return pixel?1:0;

    };
    if(single_precision_nearest()&&touhou::numeric::sprite_range({vm.sprite_size.x,vm.sprite_size.y,vm.scale.x,vm.scale.y,vm.position.x,vm.position.y,vm.position.z,vm.child_position.x,vm.child_position.y,vm.child_position.z,vm.script_position.x,vm.script_position.y,vm.script_position.z}))
        return evaluate([](float v){return touhou::numeric::SpriteNumber(v);});
    return evaluate([](float v){return th10::number(v);});
}
// 0x4436c0 / 0x443910 are identical. The upper-right X offset remains extended
// while its lower-right copy is stored to float before multiplication.
u32 AnmRenderer::rotated_geometry(const AnmVm& vm,AnmVertex* q) noexcept {
    if(vm.rotation.z==0)return axis_geometry(vm,q,false);
    const float rotation_cos=cosine(number(vm.rotation.z)).to_float(),rotation_sin=sine(number(vm.rotation.z)).to_float();

    auto evaluate=[&](auto number)->u32{
        using N=decltype(number(0));
        auto sum_values=[&](float a,float b,float c){return number(a)+number(b)+number(c);};
        auto write_depth=[](auto* q,N z){for(unsigned i=0;i<4;++i)q[i].position.z=z.to_float();};

    if(vm.rotation.z==0)return axis_geometry(vm,q,false);
    const auto c=number(rotation_cos),s=number(rotation_sin);
    const auto x=sum_values(vm.child_position.x,vm.position.x,vm.script_position.x),y=sum_values(vm.child_position.y,vm.position.y,vm.script_position.y);
    const auto width=number(vm.sprite_size.x)*number(vm.scale.x),height=number(Scalar::mul(vm.sprite_size.y,vm.scale.y));
    N left,right,top,bottom;
    switch((vm.flags>>18)&3){case 0:left=number((width*number(-0.5f)).to_float());right=width*number(0.5f);break;case 1:left=number(0);right=width;break;case 2:left=number((-width).to_float());right=number(0);break;default:__builtin_trap();}
    switch((vm.flags>>20)&3){case 0:top=number((height*number(-0.5f)).to_float());bottom=height*number(0.5f);break;case 1:top=number(0);bottom=height;break;case 2:top=-height;bottom=number(0);break;default:__builtin_trap();}
    const auto right_copy=number(right.to_float()),bottom_copy=number(bottom.to_float());
    q[0].position.x=(left*c-top*s+x).to_float();q[0].position.y=(left*s+top*c+y).to_float();
    q[1].position.x=(right*c-top*s+x).to_float();q[1].position.y=(top*c+right*s+y).to_float();
    q[2].position.x=(left*c-bottom*s+x).to_float();q[2].position.y=(left*s+c*bottom+y).to_float();
    q[3].position.x=(right_copy*c-bottom_copy*s+x).to_float();q[3].position.y=(right_copy*s+bottom_copy*c+y).to_float();
    write_depth(q,sum_values(vm.child_position.z,vm.position.z,vm.script_position.z));return 0;

    };
    if(single_precision_nearest()&&touhou::numeric::sprite_range({vm.sprite_size.x,vm.sprite_size.y,vm.scale.x,vm.scale.y,vm.position.x,vm.position.y,vm.position.z,vm.child_position.x,vm.child_position.y,vm.child_position.z,vm.script_position.x,vm.script_position.y,vm.script_position.z})&&touhou::numeric::sprite_range({rotation_cos,rotation_sin}))
        return evaluate([](float v){return touhou::numeric::SpriteNumber(v);});
    return evaluate([](float v){return th10::number(v);});
}
u32 AnmRenderer::modulate_channel(u32 color,u32 tint) noexcept {const auto result=((color&255)*(tint&255))>>7;return result>255?255:result;}
// 0x442f50. Flushing advances the start of the next batch; the frame owns the
// much larger vertex arena and resets its write pointer separately.
void AnmRenderer::flush(){if(!manager.batch_quads)return;RenderCommands(environment).SetDiffuseArg(TextureArg::Diffuse);environment.vertex_format(Layouts::Screen);environment.draw_triangles(Primitives::Triangles,manager.batch_quads*2,manager.batch_start,28);manager.batch_start=manager.vertex_write;manager.batch_quads=0;++manager.flushed_batches;}
// 0x4425a0. Read VM flags again after a flush, preserving callback side effects.
void AnmRenderer::apply_state(const AnmVm& vm){
    if(manager.cached_draw_state[0]!=((vm.flags>>4)&3)){flush();const auto blend=(vm.flags>>4)&3;manager.cached_draw_state[0]=blend;if(blend<2)RenderCommands(environment).SetDestinationBlend(blend?BlendMode::One:BlendMode::InverseSourceAlpha);}
    if(manager.cached_draw_state[6]!=(vm.flags>>31)){flush();const auto point=vm.flags>>31;manager.cached_draw_state[6]=point;environment.texture_filter(point);}++manager.submitted_draws;
}
// 0x4423e0. Matrix-transformed sprites use a texture factor and support the
// additional blend selector 2 that the pretransformed path leaves untouched.
void AnmRenderer::apply_material(const AnmVm& vm){
    if(manager.cached_draw_state[0]!=((vm.flags>>4)&3)){flush();const auto blend=(vm.flags>>4)&3;manager.cached_draw_state[0]=blend;if(blend<3)RenderCommands(environment).SetDestinationBlend(blend?BlendMode::One:BlendMode::InverseSourceAlpha);}
    u32 color=vm.flags&0x8000?vm.secondary_color:vm.color;if(manager.tint_enabled){u32 result=0;for(u32 shift=0;shift<32;shift+=8)result|=modulate_channel(color>>shift,manager.tint>>shift)<<shift;color=result;}
    if(manager.current_material_color!=color){flush();manager.current_material_color=color;RenderCommands(environment).SetTextureFactor(color);}
    if(manager.cached_draw_state[6]!=(vm.flags>>31)){flush();const auto point=vm.flags>>31;manager.cached_draw_state[6]=point;environment.texture_filter(point);}++manager.submitted_draws;
}
// 0x442fe0. Two triangles retain the original 0,1,2 / 1,2,3 winding.
i32 AnmRenderer::append(const AnmVertex* q) noexcept {constexpr u32 indices[]={0,1,2,1,2,3};for(auto index:indices){*manager.vertex_write=q[index];++manager.vertex_write;}++manager.batch_quads;return 0;}
// 0x442670. Offset, pixel alignment, UVs, bounds, material state, tint, batching.
i32 AnmRenderer::submit(const AnmVm& vm,u32 flags,bool flip_u){
    auto* q=environment.quad;for(u32 i=0;i<4;++i){q[i].position.x=Scalar::add(q[i].position.x,manager.draw_offset.x);q[i].position.y=Scalar::add(q[i].position.y,manager.draw_offset.y);}
    // Keep fractional motion for every screen-space sprite. The half-pixel
    // raster convention remains; integer-aligned stationary sprites are unchanged.
    if(flags&1){const auto aligned=[](float x){return (number(x)-number(0.5f)).to_float();};q[0].position.x=q[2].position.x=aligned(q[0].position.x);q[1].position.x=q[3].position.x=aligned(q[1].position.x);q[0].position.y=q[1].position.y=aligned(q[0].position.y);q[2].position.y=q[3].position.y=aligned(q[2].position.y);}
    q[0].uv.x=q[2].uv.x=Scalar::add(flip_u?vm.sprite->u1:vm.sprite->u0,vm.uv_offset.x);q[1].uv.x=q[3].uv.x=Scalar::add(flip_u?vm.sprite->u0:vm.sprite->u1,vm.uv_offset.x);q[0].uv.y=q[1].uv.y=Scalar::add(vm.sprite->v0,vm.uv_offset.y);q[2].uv.y=q[3].uv.y=Scalar::add(vm.sprite->v1,vm.uv_offset.y);
    float max_x=q[0].position.x>q[1].position.x?q[0].position.x:q[1].position.x,max_y=q[0].position.y>q[1].position.y?q[0].position.y:q[1].position.y;
    float min_x=q[0].position.x<q[1].position.x?q[0].position.x:q[1].position.x,min_y=q[0].position.y<q[1].position.y?q[0].position.y:q[1].position.y;
    for(u32 i=2;i<4;++i){if(max_x<q[i].position.x)max_x=q[i].position.x;if(max_y<q[i].position.y)max_y=q[i].position.y;if(q[i].position.x<min_x)min_x=q[i].position.x;if(q[i].position.y<min_y)min_y=q[i].position.y;}
    const auto& viewport=*environment.viewport;if(number(max_x)<unsigned_number(viewport.x)||number(max_y)<unsigned_number(viewport.y)||unsigned_number(viewport.x+viewport.width)<number(min_x)||unsigned_number(viewport.y+viewport.height)<number(min_y))return 0;
    if(manager.current_texture!=vm.sprite->texture){manager.current_texture=vm.sprite->texture;flush();environment.set_texture(manager.current_texture);}
    if(manager.cached_draw_state[2]!=1){flush();manager.cached_draw_state[2]=1;}
    if(!(flags&2)){u32 color=vm.flags&0x8000?vm.secondary_color:vm.color;if(manager.tint_enabled){u32 result=0;for(u32 shift=0;shift<32;shift+=8)result|=modulate_channel(color>>shift,manager.tint>>shift)<<shift;color=result;}for(u32 i=0;i<4;++i)q[i].color=color;}
    presentation_audit::capture(vm,q,4);apply_state(vm);return append(q);
}
// 0x4451c0. The custom mesh and matrix modes have their own render paths.
i32 AnmRenderer::draw(AnmVm& vm){if((vm.flags&3)!=3||!(vm.color>>24))return -1;const auto mode=(vm.flags>>22)&15;if(mode==0)return submit(vm,axis_geometry(vm,environment.quad,true));if(mode==2)return submit(vm,axis_geometry(vm,environment.quad,false));if(mode==1||mode==3)return submit(vm,rotated_geometry(vm,environment.quad));if(mode<=9)return environment.special_draw(manager,vm,mode);return 0;}
// 0x442f30. Called after the previous frame's remaining batch has been flushed.
void AnmRenderer::begin_frame() noexcept {manager.batch_quads=0;manager.vertex_write=manager.batch_start=manager.vertex_buffer;}
// 0x443480 uses half extents with distinct addition and storage order. Its
// submission path, 0x442ad0, reverses U while retaining the vertex winding.
void AnmRenderer::flipped_geometry(const AnmVm& vm,AnmVertex* q) noexcept {
    const auto half_width=number(vm.sprite_size.x)*number(vm.scale.x)*number(.5f);
    const auto half_height=number((number(vm.sprite_size.y)*number(vm.scale.y)*number(.5f)).to_float());
    const auto x=sum(vm.position.x,vm.child_position.x,vm.script_position.x);
    switch((vm.flags>>18)&3){
    case 0:q[0].position.x=q[2].position.x=(x-half_width).to_float();q[1].position.x=q[3].position.x=(x+half_width).to_float();break;
    case 1:q[0].position.x=q[2].position.x=x.to_float();q[1].position.x=q[3].position.x=(half_width+half_width+number(vm.position.x)+number(vm.child_position.x)+number(vm.script_position.x)).to_float();break;
    case 2:q[0].position.x=q[2].position.x=(x-half_width-half_width).to_float();q[1].position.x=q[3].position.x=x.to_float();break;
    }
    const auto y=sum(vm.position.y,vm.child_position.y,vm.script_position.y);
    switch((vm.flags>>20)&3){
    case 0:q[0].position.y=q[1].position.y=(y-half_height).to_float();q[2].position.y=q[3].position.y=(number(vm.position.y)+number(vm.child_position.y)+half_height+number(vm.script_position.y)).to_float();break;
    case 1:q[0].position.y=q[1].position.y=y.to_float();q[2].position.y=q[3].position.y=(half_height+half_height+number(vm.position.y)+number(vm.child_position.y)+number(vm.script_position.y)).to_float();break;
    case 2:q[0].position.y=q[1].position.y=(y-half_height-half_height).to_float();q[2].position.y=q[3].position.y=y.to_float();break;
    }
    depth(q,sum(vm.child_position.z,vm.position.z,vm.script_position.z));
}
i32 AnmRenderer::draw_flipped(const AnmVm& vm){if((vm.flags&3)!=3||!(vm.color>>24))return -1;flipped_geometry(vm,environment.quad);return submit(vm,1,true);}
}
