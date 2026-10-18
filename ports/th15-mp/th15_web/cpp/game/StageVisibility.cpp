#include "StageVisibility.hpp"
#include "AnmTransforms.hpp"
namespace th15 {
bool stage_visible(const StageObject& o,const Vec3& instance,const AnmCamera& camera,const GraphicsViewport& vp,Vec2 origin,float distance_squared,std::array<Vec3,16>* projected)noexcept{
    const Vec3 d{float(float(o.position.x+instance.x)-camera.eye.x),float(float(o.position.y+instance.y)-camera.eye.y),float(float(o.position.z+instance.z)-camera.eye.z)};const float distance=float(float(float(d.y*d.y)+float(d.x*d.x))+float(d.z*d.z));if(distance>distance_squared)return false;
    const float hx=float(o.size.x*.5f),hy=float(o.size.y*.5f),hz=float(o.size.z*.5f),x0=float(o.position.x-hx),x1=float(o.position.x+hx),y0=float(o.position.y-hy),y1=float(o.position.y+hy),z0=float(o.position.z-hz),z1=float(o.position.z+hz),x=o.position.x,z=o.position.z;
    const std::array<Vec3,16> boxes={{{x1,y1,z1},{x1,y1,z0},{x1,y0,z1},{x1,y0,z0},{x0,y1,z1},{x0,y1,z0},{x0,y0,z1},{x0,y0,z0},{x,y0,z0},{x,y1,z0},{x,y0,z1},{x,y1,z1},{x,y0,z},{x,y1,z},{x,y0,float(z-float(hz*.5f))},{x,y1,float(z+float(hz*.5f))}}};
    Matrix4 world;world.identity();world.m[12]=instance.x;world.m[13]=instance.y;world.m[14]=instance.z;Matrix4 screen;screen.m[0]=float(double(vp.width)*.5);screen.m[5]=float(double(vp.height)*-.5);screen.m[10]=float(double(vp.far_depth)-vp.near_depth);screen.m[12]=float(double(vp.x)+double(vp.width)*.5);screen.m[13]=float(double(vp.y)+double(vp.height)*.5);screen.m[14]=vp.near_depth;screen.m[15]=1;const auto m=matrix_product(matrix_product(matrix_product(world,camera.view),camera.projection),screen);
    float min_x=float(float(origin.x+384)+8),max_x=float(origin.x-8),min_y=float(float(origin.y+448)+8),max_y=float(origin.y-8);
    for(u32 i=0;i<boxes.size();++i){const auto p=boxes[i];float q[4];for(u32 c=0;c<4;++c)q[c]=float(float(float(float(p.x*m.m[c])+float(p.y*m.m[4+c]))+float(p.z*m.m[8+c]))+m.m[12+c]);const float estimate=float(1.f/q[3]),w=float(float(estimate+estimate)-float(float(estimate*q[3])*estimate));const Vec3 point{float(q[0]*w),float(q[1]*w),float(q[2]*w)};if(projected)(*projected)[i]=point;
        if(!(point.z>=0&&point.z<=1))continue;if(point.x<min_x)min_x=point.x;if(point.x>max_x)max_x=point.x;if(point.y<min_y)min_y=point.y;if(point.y>max_y)max_y=point.y;
    }
    return max_x>=origin.x&&min_x<=float(origin.x+384)&&max_y>=origin.y&&min_y<=float(origin.y+448);
}
}
