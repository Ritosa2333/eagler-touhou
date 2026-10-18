#include "AnmCamera.hpp"
#include "AnmTransforms.hpp"
#include <cmath>
namespace th15 {
Vec3 project_vertex(const Vec3& p,const Matrix4& world,const AnmCamera& camera,const GraphicsViewport& viewport)noexcept{
    const Matrix4 combined=matrix_product(matrix_product(world,camera.view),camera.projection);float values[4];for(u32 c=0;c<4;c++)values[c]=float(float(float(p.x*combined.m[c])+float(p.y*combined.m[4+c]))+float(float(p.z*combined.m[8+c])+combined.m[12+c]));
    // The source refines its single-precision reciprocal once, then performs
    // viewport conversion at the wider precision of the Windows math library.
    const float estimate=float(1.f/values[3]),w=float(float(estimate+estimate)-float(float(estimate*values[3])*estimate)),x=float(values[0]*w),y=float(values[1]*w),z=float(values[2]*w);return{float((double(x)+1)*.5*double(viewport.width)+double(viewport.x)),float((1-double(y))*.5*double(viewport.height)+double(viewport.y)),float((double(viewport.far_depth)-double(viewport.near_depth))*double(z)+double(viewport.near_depth))};
}
int anm_projected_quad(AnmVm& vm,const AnmCamera& camera,const GraphicsViewport& viewport,Vec3 (&out)[4])noexcept{
    auto& v=vm.visual;const Vec3 rotation=vm.total_rotation();Matrix4 world;world.identity();world.m[12]=float(float(v.translation.x+vm.variables.position.x)+v.child_anchor.x);world.m[13]=float(float(v.translation.y+vm.variables.position.y)+v.child_anchor.y);world.m[14]=float(float(v.translation.z+vm.variables.position.z)+v.child_anchor.z);
    const Vec3 center=project_vertex({},world,camera,viewport);if(center.z<0||center.z>1)return -1;const Vec3 reference=project_vertex(camera.reference,world,camera,viewport);const float dx=float(reference.x-center.x),dy=float(reference.y-center.y),dz=float(reference.z-center.z),length=float(std::sqrt(double(float(float(float(dy*dy)+float(dx*dx))+float(dz*dz))))),factor=float(length*.5f);
    const float width=float(float(float(v.sprite_size.x*factor)*v.scale.x)*v.secondary_scale.x),height=float(float(float(v.sprite_size.y*factor)*v.scale.y)*v.secondary_scale.y),sine=float(std::sin(double(rotation.z))),cosine=float(std::cos(double(rotation.z)));const u32 ax=v.flags>>21&3,ay=v.flags>>23&3;if(ax>2||ay>2)return -2;constexpr float xs[3][4]={{-.5f,.5f,-.5f,.5f},{0,1,0,1},{-1,0,-1,0}},ys[3][4]={{-.5f,-.5f,.5f,.5f},{0,0,1,1},{-1,-1,0,0}};
    for(u32 i=0;i<4;i++){const float x=float(xs[ax][i]*width),y=float(ys[ay][i]*height);out[i]={float(float(float(x*cosine)-float(y*sine))+center.x),float(float(float(y*cosine)+float(x*sine))+center.y),center.z};}return 0;
}
}
