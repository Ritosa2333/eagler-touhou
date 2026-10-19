#include "AnmProjection.hpp"
#include <cmath>
namespace th11 {
Matrix4 anm_world_matrix(AnmVm& vm,bool anchor_translation)noexcept{
    if(!(vm.flags&0x4000)&&(vm.flags&12)){
        vm.transform_matrix=vm.sprite_matrix;
        vm.transform_matrix.m[0]=float(double(vm.transform_matrix.m[0])*vm.scale.x);
        vm.transform_matrix.m[5]=float(double(vm.transform_matrix.m[5])*vm.scale.y);vm.flags&=~8u;
        const float rotations[]={vm.rotation.x,vm.rotation.y,vm.rotation.z};
        for(u32 axis=0;axis<3;++axis)if(rotations[axis]!=0)vm.transform_matrix=GraphicsMath::multiply(vm.transform_matrix,GraphicsMath::rotation(axis,rotations[axis]));vm.flags&=~4u;
    }
    Matrix4 world=vm.transform_matrix;
    const double x=double(vm.position.x)+vm.script_position.x+vm.child_position.x,y=double(vm.position.y)+vm.script_position.y+vm.child_position.y;
    world.m[14]=float(double(vm.position.z)+vm.script_position.z+vm.child_position.z);
    if(anchor_translation){
        const u32 ax=(vm.flags>>18)&3,ay=(vm.flags>>20)&3;
        const float half_x=std::abs(float(double(vm.sprite_size.x)*vm.scale.x*.5)),half_y=std::abs(float(double(vm.sprite_size.y)*vm.scale.y*.5));
        world.m[12]=float(x+(ax==1?-double(half_x):ax==2?double(half_x):0));
        world.m[13]=float(y+(ay==1?-double(half_y):ay==2?double(half_y):0));
    }else{world.m[12]=float(x+world.m[12]);world.m[13]=float(y+world.m[13]);}
    return world;
}
bool anm_projected_quad(AnmVm& vm,const AnmCamera& camera,Vec3 (&out)[4],Matrix4* result)noexcept{
    const u32 ax=(vm.flags>>18)&3,ay=(vm.flags>>20)&3;if(ax==3||ay==3)return false;
    const Matrix4 world=anm_world_matrix(vm,false);if(result)*result=world;
    const float left=ax==0?-128:ax==1?0:-256,right=ax==0?128:ax==1?256:0,top=ay==0?-128:ay==1?0:-256,bottom=ay==0?128:ay==1?256:0;
    const Vec3 quad[]={{left,top,0},{right,top,0},{left,bottom,0},{right,bottom,0}};
    for(u32 i=0;i<4;++i)out[i]=GraphicsMath::project(quad[i],camera.viewport,camera.projection,camera.view,world);return true;
}
bool anm_billboard_quad(const AnmVm& vm,const AnmCamera& camera,Vec3 (&out)[4])noexcept{
    Matrix4 world;world.identity();world.m[12]=float(double(vm.position.x)+vm.script_position.x+vm.child_position.x);world.m[13]=float(double(vm.position.y)+vm.script_position.y+vm.child_position.y);world.m[14]=float(double(vm.position.z)+vm.script_position.z+vm.child_position.z);
    const Vec3 center=GraphicsMath::project({},camera.viewport,camera.projection,camera.view,world);if(center.z<0||center.z>1)return false;
    const Vec3 right=GraphicsMath::project(camera.right,camera.viewport,camera.projection,camera.view,world);
    const float dx=float(double(right.x)-center.x),dy=float(double(right.y)-center.y),dz=float(double(right.z)-center.z);
    const float squared=float((double(dx)*dx+double(dy)*dy)+double(dz)*dz),half=float(double(float(std::sqrt(double(squared))))*.5);
    const float width=float(double(vm.sprite_size.x)*half*vm.scale.x),height=float(double(half)*vm.sprite_size.y*vm.scale.y);
    const u32 ax=(vm.flags>>18)&3,ay=(vm.flags>>20)&3;if(ax==3||ay==3)return false;
    const float left=ax==0?float(-double(width)*.5):ax==1?0:-width,right_x=ax==0?float(double(width)*.5):ax==1?width:0;
    const float top=ay==0?float(-double(height)*.5):ay==1?0:-height,bottom=ay==0?float(double(height)*.5):ay==1?height:0;
    const float xs[]={left,right_x,left,right_x},ys[]={top,top,bottom,bottom},c=float(std::cos(double(vm.rotation.z))),s=float(std::sin(double(vm.rotation.z)));
    for(u32 i=0;i<4;++i)out[i]={float(double(c)*xs[i]-double(s)*ys[i]+center.x),float(double(c)*ys[i]+double(s)*xs[i]+center.y),center.z};return true;
}
}
