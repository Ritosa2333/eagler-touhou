#include "AnmProjection.hpp"
#include "GameMath.hpp"
#include "PresentationAudit.hpp"
namespace th10 {
namespace {
Extended sum(float a,float b,float c){return number(a)+number(b)+number(c);}
Extended distance(Extended x,Extended y,Extended z){return (z*z+y*y+x*x).square_root();}
u32 selected_color(const AnmVm& vm){return vm.flags&0x8000?vm.secondary_color:vm.color;}
u32 tint(u32 color,const AnmManager& manager){if(!manager.tint_enabled)return color;u32 result=0;for(u32 shift=0;shift<32;shift+=8)result|=AnmRenderer::modulate_channel(color>>shift,manager.tint>>shift)<<shift;return result;}
void anchor(u32 mode,Extended size,Extended& first,Extended& last){switch(mode){case 0:first=number((size*number(-.5f)).to_float());last=size*number(.5f);break;case 1:first=number(0);last=size;break;case 2:first=number((-size).to_float());last=number(0);break;default:__builtin_trap();}}
}
// Shared matrix preparation in 0x444240 and 0x444760.
void AnmProjection::update_transform(AnmVm& vm){
    if((vm.flags&0x4000)||!(vm.flags&12))return;
    vm.transform_matrix=vm.sprite_matrix;
    vm.transform_matrix.elements[0][0]=Scalar::mul(vm.scale.x,vm.transform_matrix.elements[0][0]);
    vm.transform_matrix.elements[1][1]=Scalar::mul(vm.scale.y,vm.transform_matrix.elements[1][1]);vm.flags&=~8u;
    const float* angles[]={&vm.rotation.x,&vm.rotation.y,&vm.rotation.z};
    for(u32 axis=0;axis<3;++axis)if(*angles[axis]!=0){Matrix4 rotation;environment.rotation(rotation,axis,*angles[axis]);environment.multiply(vm.transform_matrix,vm.transform_matrix,rotation);}
    vm.flags&=~4u;
}
// 0x444240. The model is a 256-unit square; its size was placed in the VM's
// sprite matrix. X/Y translation includes matrix translation, Z replaces it.
i32 AnmProjection::project_quad(AnmVm& vm){
    update_transform(vm);auto world=vm.transform_matrix;
    world.elements[3][0]=(sum(vm.child_position.x,vm.position.x,vm.script_position.x)+number(world.elements[3][0])).to_float();
    world.elements[3][1]=(sum(vm.child_position.y,vm.position.y,vm.script_position.y)+number(world.elements[3][1])).to_float();
    world.elements[3][2]=sum(vm.child_position.z,vm.position.z,vm.script_position.z).to_float();
    constexpr float first[]={-128,0,-256},last[]={128,256,0};const auto horizontal=(vm.flags>>18)&3,vertical=(vm.flags>>20)&3;if(horizontal==3||vertical==3)__builtin_trap();
    const Vec3 points[]={{first[horizontal],first[vertical],0},{last[horizontal],first[vertical],0},{first[horizontal],last[vertical],0},{last[horizontal],last[vertical],0}};
    for(u32 i=0;i<4;++i)environment.project(renderer.environment.quad[i].position,points[i],world);renderer.manager.render_world_matrix=world;return 0;
}
// 0x443b60. Project a point and a camera reference vector to obtain pixel scale,
// then rotate a billboard in screen space. A depth outside [0,1] rejects it.
i32 AnmProjection::billboard_geometry(const AnmVm& vm){
    Matrix4 world;world.identity();world.elements[3][0]=sum(vm.child_position.x,vm.position.x,vm.script_position.x).to_float();world.elements[3][1]=sum(vm.child_position.y,vm.position.y,vm.script_position.y).to_float();world.elements[3][2]=sum(vm.child_position.z,vm.position.z,vm.script_position.z).to_float();
    Vec3 center,reference;const Vec3 origin{};environment.project(center,origin,world);if(center.z<0||center.z>1)return -1;environment.project(reference,*environment.camera_unit,world);
    const auto dx=number(Scalar::sub(reference.x,center.x)),dy=number(Scalar::sub(reference.y,center.y)),dz=number(Scalar::sub(reference.z,center.z));
    const auto ratio=distance(dx,dy,dz)*number(.5f);const auto width=number((number(vm.sprite_size.x)*number(vm.scale.x)*ratio).to_float()),height=number((number(vm.sprite_size.y)*number(vm.scale.y)*ratio).to_float());
    auto* q=renderer.environment.quad;for(u32 i=0;i<4;++i)q[i].position.z=center.z;
    const auto c=number(cosine(number(vm.rotation.z)).to_float()),s=number(sine(number(vm.rotation.z)).to_float()),x=number(center.x),y=number(center.y);
    Extended left,right,top,bottom;anchor((vm.flags>>18)&3,width,left,right);anchor((vm.flags>>20)&3,height,top,bottom);const auto r=number(right.to_float()),b=number(bottom.to_float());
    q[0].position.x=(left*c-top*s+x).to_float();q[0].position.y=(left*s+top*c+y).to_float();q[1].position.x=(right*c-top*s+x).to_float();q[1].position.y=(top*c+right*s+y).to_float();q[2].position.x=(left*c-bottom*s+x).to_float();q[2].position.y=(left*s+c*bottom+y).to_float();q[3].position.x=(r*c-b*s+x).to_float();q[3].position.y=(r*s+b*c+y).to_float();return 0;
}
// 0x443f80 / 0x443fb0. Billboard fog uses one distance, integer fog channels
// and fading alpha. Projected polygon fog below uses four distances and keeps A.
i32 AnmProjection::draw_billboard(AnmVm& vm,bool fog_enabled){
    if(billboard_geometry(vm))return -1;if(!fog_enabled)return renderer.submit(vm,0);
    const auto& fog=*environment.fog;const auto denominator=number(fog.near_distance)-number(fog.far_distance);auto color=selected_color(vm);
    const auto px=number(sum(vm.position.x,vm.script_position.x,vm.child_position.x).to_float());
    const auto py=sum(vm.position.y,vm.script_position.y,vm.child_position.y),pz=number(Scalar::add(vm.position.z,vm.script_position.z))+number(vm.child_position.z);
    const auto dx=number((px-number(environment.camera_position->x)).to_float()),dy=number((py-number(environment.camera_position->y)).to_float()),dz=number((pz-number(environment.camera_position->z)).to_float());
    const auto length=distance(dx,dy,dz);color=tint(color,renderer.manager);
    if(number(fog.near_distance)<length){const auto fraction=(number(fog.near_distance)-length)/denominator;if(!(fraction<number(1)))return -1;u32 result=0;
        for(u32 channel=0;channel<3;++channel){const auto value=(color>>(channel*8))&255;const auto delta=wrapping_add(value,static_cast<i32>(0u-static_cast<u32>(Scalar::truncate(fog.color[channel]))));const auto amount=(Extended::from_int(delta)*fraction).truncate_int();result|=static_cast<u8>(value-static_cast<u32>(amount))<<(channel*8);}
        const auto alpha=((number(1)-fraction)*Extended::from_int(color>>24)).truncate_int();color=result|(static_cast<u32>(static_cast<u8>(alpha))<<24);
    }
    for(u32 i=0;i<4;++i)renderer.environment.quad[i].color=color;return renderer.submit(vm,2);
}
// 0x444580 / 0x4445c0.
i32 AnmProjection::draw_projected(AnmVm& vm,bool fog_enabled){
    project_quad(vm);auto* q=renderer.environment.quad;
    if(fog_enabled){const auto& fog=*environment.fog;const auto denominator=number(Scalar::sub(fog.near_distance,fog.far_distance));const auto color=selected_color(vm);
        for(u32 i=0;i<4;++i){float world[4];environment.transform(world,renderer.manager.model_vertices[i].position,renderer.manager.render_world_matrix);
            const auto length=distance(number(world[0])-number(environment.camera_position->x),number(world[1])-number(environment.camera_position->y),number(world[2])-number(environment.camera_position->z));
            auto result=color;if(number(fog.near_distance)<length){const auto fraction=(number(fog.near_distance)-length)/denominator;if(!(fraction<number(1)))result=(fog.packed_color&0xffffff)|(color&0xff000000);else{result=color&0xff000000;for(u32 channel=0;channel<3;++channel){const auto value=(color>>(channel*8))&255;const auto amount=((Extended::from_int(value)-number(fog.color[channel]))*fraction).truncate_int();result|=static_cast<u32>(static_cast<u8>(value-static_cast<u32>(amount)))<<(channel*8);}}}q[i].color=result;
        }
    }
    const auto result=renderer.submit(vm,fog_enabled?2:0);for(u32 i=0;i<4;++i)q[i].reciprocal_w=1;return result;
}
// 0x444ce0. A pretransformed triangle strip bypasses the quad arena.
i32 AnmProjection::draw_strip(AnmVm& vm,const void* vertices,u32 count){
    if((vm.flags&3)!=3||!(vm.color>>24))return -1;auto& manager=renderer.manager;auto& platform=renderer.environment;
    renderer.flush();if(manager.current_texture!=vm.sprite->texture){manager.current_texture=vm.sprite->texture;platform.set_texture(manager.current_texture);}if(manager.cached_draw_state[2]!=3){platform.vertex_format(Layouts::Screen);manager.cached_draw_state[2]=3;}presentation_audit::capture(vm,static_cast<const AnmVertex*>(vertices),count,4);renderer.apply_state(vm);RenderCommands(platform).SetDiffuseArg(TextureArg::Diffuse);platform.draw_triangles(Primitives::Strip,count-2,vertices,28);return 0;
}
// 0x444760. Draw the model-space vertex buffer with world and UV transforms.
i32 AnmProjection::draw_model(AnmVm& vm){
    if((vm.flags&3)!=3||!(vm.color>>24))return -1;renderer.flush();update_transform(vm);auto world=vm.transform_matrix;
    const auto translated=[](u32 anchor,float size,float scale,float child,float position,float script,float previous){auto half=number(size)*number(scale)*number(.5f);half.exponent&=0x7fff;switch(anchor){case 0:return sum(child,position,script).to_float();case 1:return (sum(child,position,script)-half).to_float();case 2:return (half+number(child)+number(position)+number(script)).to_float();default:return previous;}};
    world.elements[3][0]=translated((vm.flags>>18)&3,vm.sprite_size.x,vm.scale.x,vm.child_position.x,vm.position.x,vm.script_position.x,world.elements[3][0]);
    world.elements[3][1]=translated((vm.flags>>20)&3,vm.sprite_size.y,vm.scale.y,vm.child_position.y,vm.position.y,vm.script_position.y,world.elements[3][1]);
    renderer.apply_material(vm);world.elements[3][2]=sum(vm.child_position.z,vm.script_position.z,vm.position.z).to_float();auto& platform=renderer.environment;auto& manager=renderer.manager;platform.set_transform(Matrices::World,world);
    if(manager.current_texture!=vm.sprite->texture){manager.current_texture=vm.sprite->texture;platform.set_texture(manager.current_texture);}
    // Both original scroll checks read U. A V-only scroll does not invalidate
    // an unchanged sprite's texture transform, which is preserved here.
    if(manager.current_uv_sprite!=vm.sprite||vm.uv_offset.x!=0){manager.current_uv_sprite=vm.sprite;auto uv=vm.uv_matrix;uv.elements[2][0]=Scalar::add(vm.sprite->u0,vm.uv_offset.x);uv.elements[2][1]=Scalar::add(vm.sprite->v0,vm.uv_offset.y);platform.set_transform(Matrices::Texture,uv);}
    if(manager.cached_draw_state[2]!=2){platform.stream_source(manager.model_vertex_buffer,20);platform.vertex_format(Layouts::World);RenderCommands(platform).SetDiffuseArg(TextureArg::Factor);manager.cached_draw_state[2]=2;}presentation_audit::capture_model(vm,manager.model_vertices,4,5);platform.draw_buffer(Primitives::Strip,0,2);return 0;
}
}
