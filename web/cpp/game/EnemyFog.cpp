#include "EnemyFog.hpp"
#include "StageMath.hpp"
#include <algorithm>
namespace th13 {
void EnemyFog::update(const Vec3& center,float rate) {
    const float r=radius;
    if(radius<target)radius=float(double(rate)*2+radius);
    const float span=float(double(r)*2+40);
    float x=float(double(float(double(float(center.x-r))-20))+320);
    const float top=float(double(float(double(float(center.y-r))-20))+16);
    const float cx=float(center.x+320),cy=float(center.y+16);
    const double r2=float(r*r); // qword storage after the reference's f32 product
    float ax=angle_x,ay=angle_y;
    for(size_t col=0;col<side;++col) {
        float y=top;
        for(size_t row=0;row<side;++row) {
            auto& v=vertices[col*side+row];v={{x,y,0},1,0xffffffff,{}};
            const float dx=x-cx,dy=y-cy,dz=-center.z;
            const float distance2=float(double(dx)*dx+double(dy)*dy);
            const float difference=float(r2-distance2);
            if(difference<0)v.color&=0xffffffu;
            else {
                const double quotient=double(difference)/r2;
                const float weight=float(quotient);
                v.color=0xff000000u;
                for(u32 shift:{0u,8u,16u})
                    v.color|=u32(u8(i32(255.0-quotient*float(255-((color>>shift)&255)))))<<shift;
                const double length=std::sqrt(double(dx)*dx+double(dy)*dy+double(dz)*dz);
                const float nx=length?float(dx/length):0,ny=length?float(dy/length):0;
                const float force=float(quotient*32);
                const float ox=float(double(float(float(std::sin(double(ax)))*weight))*8+float(force*nx));
                const float oy=float(double(float(float(std::sin(double(ay)))*weight))*8+float(force*ny));
                v.position.x+=ox;v.position.y+=oy;
            }
            ax=wrap_angle(float(double(ax)+.09817477f));
            ay=wrap_angle(float(double(ay)-.049087387f));
            float source_x=x,source_y=y;
            // 0x41a3d0: x<=128→129, x>512→511 (x==512 stays sampled);
            // y<=16→17, y>=464→463. Destination shares the same clamp.
            if(source_x<=128)source_x=v.position.x=129;
            else if(source_x>512)source_x=v.position.x=511;
            if(source_y<=16)source_y=v.position.y=17;
            else if(source_y>=464)source_y=v.position.y=463;
            v.uv={std::max(0.f,float(double(source_x)/640)),std::max(0.f,float(double(source_y)/480))};
            y=float(double(y)+double(span)/16);
        }
        x=float(double(x)+double(span)/16);
    }
    angle_x=wrap_angle(float(float(double(rate)*.19634955f)+angle_x));
    angle_y=wrap_angle(float(float(double(rate)*.09817477f)+angle_y));
}
}
