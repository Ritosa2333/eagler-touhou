#include "AnmGeometry.hpp"
#include <cmath>
namespace th11 {
bool anm_quad_positions(const AnmVm& vm,Vec3 (&out)[4],bool preserve_subpixel) noexcept {
    return anm_quad_positions(vm,out,(vm.flags>>22)&15,preserve_subpixel);
}
bool anm_quad_positions(const AnmVm& vm,Vec3 (&out)[4],u32 mode,bool preserve_subpixel) noexcept {
    const u32 anchor_x=(vm.flags>>18)&3,anchor_y=(vm.flags>>20)&3;
    if(mode>3||anchor_x==3||anchor_y==3)return false;
    const float w=float(double(vm.sprite_size.x)*vm.scale.x),h=float(double(vm.sprite_size.y)*vm.scale.y);
    const double x=double(vm.position.x)+vm.script_position.x+vm.child_position.x;
    const double y=double(vm.position.y)+vm.script_position.y+vm.child_position.y;
    const float z=float(double(vm.position.z)+vm.script_position.z+vm.child_position.z);
    if(mode==1){
        const float cx=float(x),cy=float(y),c=float(std::cos(double(vm.rotation.z))),s=float(std::sin(double(vm.rotation.z)));
        const float left=anchor_x==0?float(-double(w)*.5):anchor_x==1?0:-w;
        const float right=anchor_x==0?float(double(w)*.5):anchor_x==1?w:0;
        const float top=anchor_y==0?float(-double(h)*.5):anchor_y==1?0:-h;
        const float bottom=anchor_y==0?float(double(h)*.5):anchor_y==1?h:0;
        const float xs[]={left,right,left,right},ys[]={top,top,bottom,bottom};
        for(u32 i=0;i<4;++i)out[i]={float(double(c)*xs[i]-double(s)*ys[i]+cx),float(double(c)*ys[i]+double(s)*xs[i]+cy),z};
    }else{
        auto extent=[&](double origin,float size,u32 anchor,float& low,float& high){
            if(anchor==0){low=float(origin-float(double(size)*.5));if(mode==0&&!preserve_subpixel)low=std::floor(low);high=float(double(low)+size);}
            else if(anchor==1){low=float(origin);high=float(origin+size);}
            else{low=float(origin-size);high=float(origin);}
        };
        float left,right,top,bottom;extent(x,w,anchor_x,left,right);extent(y,h,anchor_y,top,bottom);
        out[0]={left,top,z};out[1]={right,top,z};out[2]={left,bottom,z};out[3]={right,bottom,z};
    }
    return true;
}
Vec2 anm_screen_uv(Vec3 p) noexcept {Vec2 uv{float(double(p.x)/640),float(double(p.y)/480)};if(uv.x<0)uv.x=0;if(uv.y<0)uv.y=0;return uv;}
bool anm_ring_vertices(const AnmVm& vm,AnmVertex* out,u32 capacity) noexcept {
    const u32 mode=(vm.flags>>22)&15;const i32 count=vm.integers[0];
    if((mode!=9&&mode!=13)||count<2||u32(count)>capacity/2||!out)return false;
    constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;
    auto normalize=[](float value){u32 loops=0;while(value>pi){value=float(double(value)-tau);if(loops++>32)break;}while(value<-pi){value=float(double(value)+tau);if(loops++>32)break;}return value;};
    float angle=mode==9?vm.rotation.z:normalize(float(double(vm.rotation.z)-double(vm.rotation.x)*.5));
    const float angle_step=float((mode==9?tau:double(vm.rotation.x))/float(count-1));
    const float uv_step=float(double(vm.integers[1])/float(count-1));float v=0;
    const u32 color=vm.flags&0x8000?vm.secondary_color:vm.color;
    const Vec3 center{float(double(float(double(vm.script_position.x)+vm.child_position.x))+vm.position.x),float(double(float(double(vm.script_position.y)+vm.child_position.y))+vm.position.y),float(double(float(double(vm.script_position.z)+vm.child_position.z))+vm.position.z)};
    for(i32 i=0;i<(mode==9?count-1:count);++i){
        const double cs=std::cos(double(angle)),sn=std::sin(double(angle));
        for(u32 side=0;side<2;++side){
            const float radius=float(double(vm.scale.y)+(side?-1:1)*double(vm.scale.x)*.5);
            const float x=float(cs*radius),y=float(sn*radius);
            out[i*2+side]={{float(double(x)+center.x),float(double(y)+center.y),float(double(center.z)+0.)},1,color,{float(double(vm.uv_offset.x)+vm.uv[side].x),float(double(v)+vm.uv_offset.y)}};
        }
        v=float(double(v)+uv_step);angle=normalize(float(double(angle)+angle_step));
    }
    if(mode==9){out[(count-1)*2]=out[0];out[(count-1)*2+1]=out[1];out[(count-1)*2].uv.y=out[(count-1)*2+1].uv.y=float(double(v)+vm.uv_offset.y);}
    return true;
}
bool AnmEnvironment::screen_uv(AnmVm& vm){Vec3 p[4];if(!anm_quad_positions(vm,p))return false;for(u32 i=0;i<4;++i)vm.uv[i]=anm_screen_uv(p[i]);return true;}
namespace {
constexpr double ripple_step=.20268340408802032,pi=3.1415927410125732421875,tau=6.283185482025146484375;
Vec3 ripple_center(const AnmVm& vm){return {float(double(vm.position.x)+vm.script_position.x),float(double(vm.position.y)+vm.script_position.y),float(double(vm.position.z)+vm.script_position.z)};}
void ripple_position(AnmVertex& v,float angle,float radius,Vec3 center){const float x=float(std::cos(double(angle))*radius),y=float(std::sin(double(angle))*radius);v.position.x=float(double(x)+center.x);v.position.y=float(double(y)+center.y);v.position.z=float(double(v.position.z)+center.z);}
}
void anm_ripple_initialize(AnmVm& vm,AnmRipple& r,Rng& rng) noexcept {
    r.scroll_x=float(double(rng.signed_unit())*double(.008333333767950535f));
    r.scroll_y=float(double(rng.signed_unit())*double(.008333333767950535f));
    const auto center=ripple_center(vm);r.vertices[0].position=center;r.vertices[0].reciprocal_w=1;r.vertices[0].uv={.5,.5};
    float velocity=float(double(rng.signed_unit())*double(.06666667014360428f)),angle=float(-pi);
    for(u32 i=0;i<31;++i){
        if(angle>=pi)angle=float(double(angle)-tau);auto& v=r.vertices[i+1];v.reciprocal_w=1;
        v.uv={float(double(float(std::cos(double(angle))*.5))+.5),float(double(float(std::sin(double(angle))*.5))+.5)};v.position.z=0;
        r.radii[i]=float(double(float(double(rng.signed_unit())*8))+80);r.velocities[i]=velocity;
        velocity=float(double(float(double(rng.signed_unit())*double(.03333333507180214f)))+velocity);
        if(velocity<-.06666667014360428f)velocity=-.06666667014360428f;else if(velocity>.06666667014360428f)velocity=.06666667014360428f;
        ripple_position(v,angle,r.radii[i],center);angle=float(double(angle)+ripple_step);
    }
    vm.before_update=anm_ripple_update;
}
void anm_ripple_update(AnmVm& vm) noexcept {
    auto& r=*static_cast<AnmRipple*>(vm.geometry);const auto center=ripple_center(vm);r.vertices[0].position=center;
    auto scroll=[&](AnmVertex& v){
        // The original uses scroll_x for both axes, including the wrap pass.
        v.uv.x=float(double(v.uv.x)+r.scroll_x);if(v.uv.x<0)for(auto& p:r.vertices)p.uv.x=float(double(p.uv.x)+1);
        v.uv.y=float(double(v.uv.y)+r.scroll_x);if(v.uv.y<0)for(auto& p:r.vertices)p.uv.y=float(double(p.uv.y)+1);
    };
    scroll(r.vertices[0]);r.vertices[0].color=vm.color;float angle=float(-pi);
    for(u32 i=0;i<31;++i){auto& v=r.vertices[i+1];scroll(v);v.color=vm.color&0xffffff;r.radii[i]=float(double(r.radii[i])+r.velocities[i]);ripple_position(v,angle,r.radii[i],center);angle=float(double(angle)+ripple_step);}
    r.vertices[32]=r.vertices[1];
}
}
