#include "../../cpp/game/GraphicsMath.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
using namespace th09;

// Frozen scalar algorithm from the pre-optimization authored Web renderer.
// This oracle does not call the new projection or any original executable.
static Matrix4 reference_multiply(const Matrix4& a,const Matrix4& b) {
    static constexpr u8 order[16][4]={{3,0,2,1},{1,3,2,0},{3,0,2,1},{3,2,0,1},{3,0,1,2},{2,3,1,0},{1,3,0,2},{1,3,2,0},{3,0,1,2},{3,2,1,0},{1,0,3,2},{3,2,1,0},{3,0,1,2},{3,2,1,0},{1,0,3,2},{3,2,1,0}};
    Matrix4 out;
    for(u32 r=0;r<4;++r)for(u32 c=0;c<4;++c){const auto& k=order[r*4+c];out.m[r][c]=((a.m[r][k[0]]*b.m[k[0]][c]+a.m[r][k[1]]*b.m[k[1]][c])+a.m[r][k[2]]*b.m[k[2]][c])+a.m[r][k[3]]*b.m[k[3]][c];}
    return out;
}
static Vec3 reference_project(const Vec3& p,const Viewport& v,const Matrix4& projection,const Matrix4& view,const Matrix4& world) {
    const auto m=reference_multiply(reference_multiply(world,view),projection);
    float q[4];for(u32 c=0;c<4;++c)q[c]=((p.x*m.m[0][c]+p.y*m.m[1][c])+p.z*m.m[2][c])+m.m[3][c];
    const float inv=1.f/q[3];return {(q[0]*inv+1.f)*float(v.width)*.5f+float(v.x),(1.f-q[1]*inv)*float(v.height)*.5f+float(v.y),q[2]*inv*(v.far_depth-v.near_depth)+v.near_depth};
}
template<class T>static void identical(const T& a,const T& b){assert(std::memcmp(&a,&b,sizeof(T))==0);}
int main() {
    u32 seed=0x31415926;
    const auto value=[&](){seed=seed*1664525u+1013904223u;switch(seed&31){case 0:return 0.f;case 1:return -0.f;case 2:return 0x1p-130f;default:return float(i32(seed>>8)-0x7fffff)/4096.f;}};
    Matrix4 identity;identity.identity();
    for(u32 trial=0;trial<20000;++trial){
        Matrix4 view,projection,world;
        for(auto* m:{&view,&projection,&world})for(auto& row:m->m)for(float& x:row)x=value();
        if(trial%2){view=GraphicsMath::look_at({value(),value(),value()},{value(),value(),value()},{0,1,0});projection=GraphicsMath::perspective(.3f+float(trial%100)/100.f,1.3f,1.f,20000.f);}
        Viewport viewport{};viewport.x=trial%200;viewport.y=trial%71;viewport.width=trial%1024;viewport.height=trial%768;viewport.near_depth=-.3f;viewport.far_depth=1.1f;
        const Vec3 point{value(),value(),value()},translation{value(),value(),value()};
        const auto combined=GraphicsMath::multiply(GraphicsMath::multiply(world,view),projection);
        identical(GraphicsMath::project(point,viewport,combined),reference_project(point,viewport,projection,view,world));
        auto translated=identity;translated.m[3][0]=translation.x;translated.m[3][1]=translation.y;translated.m[3][2]=translation.z;
        const auto base=GraphicsMath::multiply(GraphicsMath::multiply(identity,view),projection);
        const auto fast=GraphicsMath::translated_projection(translation,view,projection,base);
        identical(fast,reference_multiply(reference_multiply(translated,view),projection));
        identical(GraphicsMath::project(point,viewport,fast),reference_project(point,viewport,projection,view,translated));
    }
    std::puts("TH09 prepared projection: 20000 bit-exact matrix and vertex cases PASS");
}
