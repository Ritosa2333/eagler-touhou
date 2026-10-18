#include "GraphicsMath.hpp"
#include <cmath>
namespace th09 {
float GraphicsMath::length(const Vec3& v){return std::sqrt((v.x*v.x+v.y*v.y)+v.z*v.z);}
Vec3 GraphicsMath::normalize(const Vec3& v){
    const float squared=(v.x*v.x+v.y*v.y)+v.z*v.z;
    if(std::fabs(squared-1.f)<=0x1p-23f)return v;if(squared<=0x1p-126f)return {};
    const float inverse=1.f/std::sqrt(squared);return {v.x*inverse,v.y*inverse,v.z*inverse};
}
Vec3 GraphicsMath::cross(const Vec3& a,const Vec3& b){return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
Matrix4 GraphicsMath::multiply(const Matrix4& a,const Matrix4& b){
    // Retain the scalar library's accumulation order to avoid changing the
    // original camera projection at adjacent pixel boundaries.
    static constexpr u8 order[16][4]={{3,0,2,1},{1,3,2,0},{3,0,2,1},{3,2,0,1},{3,0,1,2},{2,3,1,0},{1,3,0,2},{1,3,2,0},{3,0,1,2},{3,2,1,0},{1,0,3,2},{3,2,1,0},{3,0,1,2},{3,2,1,0},{1,0,3,2},{3,2,1,0}};
    Matrix4 out;for(u32 r=0;r<4;++r)for(u32 c=0;c<4;++c){const auto& k=order[r*4+c];out.m[r][c]=((a.m[r][k[0]]*b.m[k[0]][c]+a.m[r][k[1]]*b.m[k[1]][c])+a.m[r][k[2]]*b.m[k[2]][c])+a.m[r][k[3]]*b.m[k[3]][c];}return out;
}
Matrix4 GraphicsMath::rotation(u32 axis,float angle){
    Matrix4 m;m.identity();const float c=float(std::cos(double(angle))),s=float(std::sin(double(angle)));
    if(axis==0){m.m[1][1]=m.m[2][2]=c;m.m[1][2]=s;m.m[2][1]=-s;}
    else if(axis==1){m.m[0][0]=m.m[2][2]=c;m.m[0][2]=-s;m.m[2][0]=s;}
    else{m.m[0][0]=m.m[1][1]=c;m.m[0][1]=s;m.m[1][0]=-s;}return m;
}
Matrix4 GraphicsMath::look_at(const Vec3& eye,const Vec3& target,const Vec3& up){
    const Vec3 z=normalize({target.x-eye.x,target.y-eye.y,target.z-eye.z});
    const Vec3 x=normalize(cross(up,z)),y=cross(z,x);Matrix4 m{};
    m.m[0][0]=x.x;m.m[1][0]=x.y;m.m[2][0]=x.z;m.m[3][0]=-((x.y*eye.y+x.x*eye.x)+x.z*eye.z);
    m.m[0][1]=y.x;m.m[1][1]=y.y;m.m[2][1]=y.z;m.m[3][1]=-((y.y*eye.y+y.x*eye.x)+y.z*eye.z);
    m.m[0][2]=z.x;m.m[1][2]=z.y;m.m[2][2]=z.z;m.m[3][2]=-((z.y*eye.y+z.x*eye.x)+z.z*eye.z);m.m[3][3]=1;return m;
}
Matrix4 GraphicsMath::perspective(float fov,float aspect,float near_plane,float far_plane){
    const float half=fov*.5f,c=float(std::cos(double(half))),s=float(std::sin(double(half))),cot=c/s;Matrix4 m{};
    m.m[0][0]=cot/aspect;m.m[1][1]=cot;m.m[2][2]=far_plane/(far_plane-near_plane);m.m[2][3]=1;m.m[3][2]=-(m.m[2][2]*near_plane);return m;
}
Vec3 GraphicsMath::project(const Vec3& p,const Viewport& v,const Matrix4& projection,const Matrix4& view,const Matrix4& world){
    return project(p,v,multiply(multiply(world,view),projection));
}
Vec3 GraphicsMath::project(const Vec3& p,const Viewport& v,const Matrix4& m){
    float q[4];for(u32 c=0;c<4;++c)q[c]=((p.x*m.m[0][c]+p.y*m.m[1][c])+p.z*m.m[2][c])+m.m[3][c];
    const float inv=1.f/q[3];return {(q[0]*inv+1.f)*float(v.width)*.5f+float(v.x),(1.f-q[1]*inv)*float(v.height)*.5f+float(v.y),q[2]*inv*(v.far_depth-v.near_depth)+v.near_depth};
}
Matrix4 GraphicsMath::translated_projection(const Vec3& p,const Matrix4& view,const Matrix4& projection,const Matrix4& untranslated){
    const auto row=[](float* out,const float* a,const Matrix4& b){
        out[0]=((a[3]*b.m[3][0]+a[0]*b.m[0][0])+a[1]*b.m[1][0])+a[2]*b.m[2][0];
        out[1]=((a[3]*b.m[3][1]+a[2]*b.m[2][1])+a[1]*b.m[1][1])+a[0]*b.m[0][1];
        out[2]=((a[1]*b.m[1][2]+a[0]*b.m[0][2])+a[3]*b.m[3][2])+a[2]*b.m[2][2];
        out[3]=((a[3]*b.m[3][3]+a[2]*b.m[2][3])+a[1]*b.m[1][3])+a[0]*b.m[0][3];
    };
    const float translation[4]={p.x,p.y,p.z,1.f};float translated_view[4];row(translated_view,translation,view);
    auto result=untranslated;row(result.m[3],translated_view,projection);return result;
}
}
