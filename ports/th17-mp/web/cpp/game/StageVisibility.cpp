#include "StageVisibility.hpp"
#include "AnmTransforms.hpp"
namespace th17 {
namespace {
using MatrixRow=std::array<float,4>;
MatrixRow multiply_translation(const MatrixRow& row,const Matrix4& matrix)noexcept{
    MatrixRow result;
    for(u32 c=0;c<4;++c){
        const float x=float(row[0]*matrix.m[c]),y=float(row[1]*matrix.m[4+c]);
        const float z=float(row[2]*matrix.m[8+c]),w=float(row[3]*matrix.m[12+c]);
        result[c]=float(float(x+y)+float(z+w));
    }
    return result;
}
}
StageVisibility::StageVisibility(const AnmCamera& camera,const GraphicsViewport& vp,Vec2 origin,float distance_squared)noexcept
    :camera(camera),origin(origin),distance_squared(distance_squared){
    screen.m[0]=float(double(vp.width)*.5);screen.m[5]=float(double(vp.height)*-.5);
    screen.m[10]=float(double(vp.far_depth)-vp.near_depth);
    screen.m[12]=float(double(vp.x)+double(vp.width)*.5);screen.m[13]=float(double(vp.y)+double(vp.height)*.5);
    screen.m[14]=vp.near_depth;screen.m[15]=1;
    Matrix4 world;world.identity();
    fixed=matrix_product(matrix_product(matrix_product(world,camera.view),camera.projection),screen);
}
bool StageVisibility::visible(const StageObject& o,const Vec3& instance,std::array<Vec3,16>* projected)const noexcept{
    const Vec3 d{float(float(o.position.x+instance.x)-camera.eye.x),float(float(o.position.y+instance.y)-camera.eye.y),float(float(o.position.z+instance.z)-camera.eye.z)};const float distance=float(float(float(d.y*d.y)+float(d.x*d.x))+float(d.z*d.z));if(distance>distance_squared)return false;
    const float hx=float(o.size.x*.5f),hy=float(o.size.y*.5f),hz=float(o.size.z*.5f),x0=float(o.position.x-hx),x1=float(o.position.x+hx),y0=float(o.position.y-hy),y1=float(o.position.y+hy),z0=float(o.position.z-hz),z1=float(o.position.z+hz),x=o.position.x,z=o.position.z;
    const std::array<Vec3,16> boxes={{{x1,y1,z1},{x1,y1,z0},{x1,y0,z1},{x1,y0,z0},{x0,y1,z1},{x0,y1,z0},{x0,y0,z1},{x0,y0,z0},{x,y0,z0},{x,y1,z0},{x,y0,z1},{x,y1,z1},{x,y0,z},{x,y1,z},{x,y0,float(z-float(hz*.5f))},{x,y1,float(z+float(hz*.5f))}}};
    // STD instances differ only in the last world matrix row. Keep the
    // original multiplication order for that row and share the other three.
    const auto translation=multiply_translation(multiply_translation(multiply_translation({instance.x,instance.y,instance.z,1},camera.view),camera.projection),screen);
    const float right=float(origin.x+384),bottom=float(origin.y+448);
    float min_x=float(right+8),max_x=float(origin.x-8),min_y=float(bottom+8),max_y=float(origin.y-8);
    const auto intersects=[&]{return max_x>=origin.x&&min_x<=right&&max_y>=origin.y&&min_y<=bottom;};
    for(u32 i=0;i<boxes.size();++i){const auto p=boxes[i];float q[4];for(u32 c=0;c<4;++c)q[c]=float(float(float(float(p.x*fixed.m[c])+float(p.y*fixed.m[4+c]))+float(p.z*fixed.m[8+c]))+translation[c]);const float estimate=float(1.f/q[3]),w=float(float(estimate+estimate)-float(float(estimate*q[3])*estimate));const Vec3 point{float(q[0]*w),float(q[1]*w),float(q[2]*w)};if(projected)(*projected)[i]=point;
        if(!(point.z>=0&&point.z<=1))continue;if(point.x<min_x)min_x=point.x;if(point.x>max_x)max_x=point.x;if(point.y<min_y)min_y=point.y;if(point.y>max_y)max_y=point.y;
        // Further points can only widen the accumulated bounds.
        if(!projected&&intersects())return true;
    }
    return intersects();
}
bool stage_visible(const StageObject& object,const Vec3& instance,const AnmCamera& camera,const GraphicsViewport& viewport,Vec2 origin,float distance_squared,std::array<Vec3,16>* projected)noexcept{
    return StageVisibility(camera,viewport,origin,distance_squared).visible(object,instance,projected);
}
}
