#include "AnmTransforms.hpp"
#include <cmath>
namespace th15 {
Matrix4 matrix_product(const Matrix4& a,const Matrix4& b)noexcept{
    Matrix4 out;for(u32 r=0;r<4;r++)for(u32 c=0;c<4;c++){const float x=float(a.m[r*4]*b.m[c]),y=float(a.m[r*4+1]*b.m[4+c]),z=float(a.m[r*4+2]*b.m[8+c]),w=float(a.m[r*4+3]*b.m[12+c]);out.m[r*4+c]=r==0||r==3?float(float(x+y)+float(z+w)):float(float(float(x+y)+z)+w);}return out;
}
Matrix4 axis_rotation(u32 axis,float angle)noexcept{
    Matrix4 out;out.identity();const float s=float(std::sin(double(angle))),c=float(std::cos(double(angle)));const u32 a=(axis+1)%3,b=(axis+2)%3;out.m[a*4+a]=out.m[b*4+b]=c;out.m[a*4+b]=s;out.m[b*4+a]=-s;return out;
}
namespace {
void scale_sprite(AnmVm& vm){auto& v=vm.visual;v.transform_matrix=v.sprite_matrix;v.transform_matrix.m[0]=float(float(v.secondary_scale.x*v.scale.x)*v.transform_matrix.m[0]);v.transform_matrix.m[5]=float(float(v.secondary_scale.y*v.scale.y)*v.transform_matrix.m[5]);v.flags&=~8u;}
void rotate(AnmVm& vm,const Vec3& angles,u32 order){constexpr u8 orders[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};const float values[]={angles.x,angles.y,angles.z};if(order<6)for(u32 axis:orders[order])if(values[axis]!=0)vm.visual.transform_matrix=matrix_product(vm.visual.transform_matrix,axis_rotation(axis,values[axis]));vm.visual.flags&=~4u;}
bool resolution(AnmVm& vm){const u32 mode=vm.visual.render_flags>>20&7;if(mode!=1&&mode!=2)return true;if(!vm.environment)return false;const float factor=mode==1?vm.environment->resolution_scale:float(vm.environment->resolution_scale*.5f);vm.visual.transform_matrix.m[0]=float(factor*vm.visual.transform_matrix.m[0]);vm.visual.transform_matrix.m[5]=float(factor*vm.visual.transform_matrix.m[5]);return true;}
}
bool anm_billboard_transform(AnmVm& vm,Matrix4& out)noexcept{
    auto& v=vm.visual;if(!(v.flags&0x10000)){scale_sprite(vm);rotate(vm,vm.total_rotation(),0);}out=v.transform_matrix;
    out.m[12]=float(float(float(v.translation.x+vm.variables.position.x)+v.child_anchor.x)+v.transform_matrix.m[12]);float y=v.transform_matrix.m[13];
    if((v.render_flags&0xc0000)&&!vm.creation_parent){if(!vm.environment)return false;out.m[12]=float(float(float(vm.environment->screen_width)*.5f)+out.m[12]);y=float(float(float(float(vm.environment->screen_height)-448.f)*.5f)+y);}
    out.m[13]=float(float(float(v.translation.y+vm.variables.position.y)+v.child_anchor.y)+y);out.m[14]=float(float(v.translation.z+vm.variables.position.z)+v.child_anchor.z);
    if(vm.creation_parent&&!(v.render_flags&0x10000)){const auto& p=*vm.creation_parent;out.m[12]=float(float(float(p.visual.translation.x+p.variables.position.x)+p.visual.child_anchor.x)+out.m[12]);out.m[13]=float(float(float(p.visual.translation.y+p.variables.position.y)+p.visual.child_anchor.y)+out.m[13]);out.m[14]=float(float(float(p.visual.translation.z+p.variables.position.z)+p.visual.child_anchor.z)+out.m[14]);}return true;
}
bool anm_world_quad_transform(AnmVm& vm,Matrix4& out)noexcept{
    auto& v=vm.visual;if(!(v.flags&0x10000)){scale_sprite(vm);if(!resolution(vm))return false;rotate(vm,vm.total_rotation(),v.render_flags>>2&7);}out=v.transform_matrix;
    Vec3 p{float(float(float(v.translation.x+vm.variables.position.x)+v.child_anchor.x)-float(float(v.size.x*v.scale.x)*v.secondary_scale.x)),float(float(float(v.translation.y+vm.variables.position.y)+v.child_anchor.y)-float(float(v.size.y*v.scale.y)*v.secondary_scale.y)),out.m[14]};if(!anm_adjust_position(vm,p))return false;out.m[12]=p.x;out.m[13]=p.y;out.m[14]=float(float(v.translation.z+vm.variables.position.z)+v.child_anchor.z);return true;
}
bool anm_mesh_transform(AnmVm& vm,Matrix4& out)noexcept{
    auto& v=vm.visual;v.sprite_matrix.identity();scale_sprite(vm);rotate(vm,vm.variables.rotation,0);out=v.transform_matrix;out.m[12]=float(float(v.translation.x+vm.variables.position.x)+v.child_anchor.x);
    if((v.render_flags&0xc0000)&&!vm.creation_parent){if(!vm.environment)return false;out.m[12]=float(float(vm.environment->screen_offsets[2])+out.m[12]);}out.m[13]=float(float(v.translation.y+vm.variables.position.y)+v.child_anchor.y);out.m[14]=float(float(v.translation.z+vm.variables.position.z)+v.child_anchor.z);return true;
}
Matrix4 anm_texture_transform(const AnmVm& vm)noexcept{const auto& v=vm.visual;Matrix4 out=v.uv_matrix;out.m[0]=float(v.uv_scale.x*v.uv_matrix.m[0]);out.m[5]=float(v.uv_scale.y*v.uv_matrix.m[5]);out.m[8]=float(v.uv[0].x+v.uv_offset.x);out.m[9]=float(v.uv[0].y+v.uv_offset.y);return out;}
}
