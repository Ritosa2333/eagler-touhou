#include "GraphicsMath.hpp"
#include <cmath>
namespace th11 {
Matrix4 GraphicsMath::multiply(const Matrix4& a,const Matrix4& b)noexcept{
    Matrix4 out;for(u32 r=0;r<4;++r)for(u32 c=0;c<4;++c)out.m[r*4+c]=float(((double(a.m[r*4])*b.m[c]+double(a.m[r*4+1])*b.m[c+4])+double(a.m[r*4+2])*b.m[c+8])+double(a.m[r*4+3])*b.m[c+12]);return out;
}
Matrix4 GraphicsMath::rotation(u32 axis,float angle)noexcept{
    Matrix4 m;m.identity();const float c=float(std::cos(double(angle))),s=float(std::sin(double(angle)));
    if(axis==0){m.m[5]=m.m[10]=c;m.m[6]=s;m.m[9]=-s;}
    else if(axis==1){m.m[0]=m.m[10]=c;m.m[2]=-s;m.m[8]=s;}
    else{m.m[0]=m.m[5]=c;m.m[1]=s;m.m[4]=-s;}return m;
}
Vec3 GraphicsMath::normalize(Vec3 v)noexcept{
    const float squared=float((double(v.x)*v.x+double(v.y)*v.y)+double(v.z)*v.z);
    if(std::abs(float(double(squared)-1))<=0x1p-23f)return v;
    if(squared<=0x1p-126f)return {};
    const float length=float(std::sqrt(double(squared))),inverse=float(1./length);
    return {float(double(v.x)*inverse),float(double(v.y)*inverse),float(double(v.z)*inverse)};
}
Vec3 GraphicsMath::cross(Vec3 a,Vec3 b)noexcept{return {float(double(a.y)*b.z-double(a.z)*b.y),float(double(a.z)*b.x-double(a.x)*b.z),float(double(a.x)*b.y-double(a.y)*b.x)};}
Matrix4 GraphicsMath::look_at(Vec3 eye,Vec3 target,Vec3 up)noexcept{
    const Vec3 z=normalize({float(double(target.x)-eye.x),float(double(target.y)-eye.y),float(double(target.z)-eye.z)}),x=normalize(cross(up,z)),y=cross(z,x);
    const Vec3 axes[]={x,y,z};Matrix4 m{};for(u32 c=0;c<3;++c){const auto a=axes[c];m.m[c]=a.x;m.m[4+c]=a.y;m.m[8+c]=a.z;m.m[12+c]=-float((double(a.x)*eye.x+double(a.y)*eye.y)+double(a.z)*eye.z);}m.m[15]=1;return m;
}
Matrix4 GraphicsMath::perspective(float fov,float aspect,float near_plane,float far_plane)noexcept{
    const float half=float(double(fov)*.5),c=float(std::cos(double(half))),s=float(std::sin(double(half))),cot=float(double(c)/s);Matrix4 m{};
    m.m[0]=float(double(cot)/aspect);m.m[5]=cot;m.m[10]=float(double(far_plane)/(double(far_plane)-near_plane));m.m[11]=1;m.m[14]=float(-double(m.m[10])*near_plane);return m;
}
void GraphicsMath::transform(Vec3 p,const Matrix4& m,float (&out)[4])noexcept{for(u32 c=0;c<4;++c)out[c]=float(((double(p.x)*m.m[c]+double(p.y)*m.m[4+c])+double(p.z)*m.m[8+c])+m.m[12+c]);}
Vec3 GraphicsMath::project(Vec3 p,const GraphicsViewport& v,const Matrix4& projection,const Matrix4& view,const Matrix4& world)noexcept{
    const auto m=multiply(multiply(world,view),projection);float q[4];transform(p,m,q);
    if(std::abs(float(double(q[3])-1))>0x1p-23f){const float inverse=float(1./q[3]);for(u32 c=0;c<3;++c)q[c]=float(double(q[c])*inverse);}
    return {float((double(q[0])+1)*.5*v.width+v.x),float((1.-q[1])*.5*v.height+v.y),float((double(v.far_depth)-v.near_depth)*q[2]+v.near_depth)};
}
}
