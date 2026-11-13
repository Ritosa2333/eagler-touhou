#include "AnmGeometry.hpp"
#include <new>
#include "GraphicsMath.hpp"
#include "PortTitle.hpp"
#include <cmath>
namespace th11 {
#if PORT_TITLE == 16
bool anm_th16_install_effect(AnmVm& vm,i32 type,AnmEnvironment& env){
    // Retail 4a2250 row 1 calls 405670, then stores all six callback selectors.
    // Keep this block opaque until the independent particle updater is ported.
    if(type!=1||!env.allocate_geometry(vm,0x193c))return false;
    std::memset(vm.geometry,0,0x193c);
    reinterpret_cast<Timer*>(static_cast<u8*>(vm.geometry)+0x1928)->set(0,&env.rate);
    const u32 selectors[]={2,2,2,2,1,1};std::memcpy(vm.th16_callbacks,selectors,sizeof selectors);
    return true;
}
#endif
Vec2 anm_vertex_uv(const AnmVm& vm,u32 i) noexcept {
#if PORT_TITLE == 14
    // 475880..4759a7: each subtraction, multiply and sum rounds in SSE.
    float u=float(vm.uv[i].x+vm.uv_offset.x),v=float(vm.uv[i].y+vm.uv_offset.y);
    if(i&1)u=float(float(float(vm.uv[i].x-vm.uv[i-1].x)*vm.texture_scale.x)+float(vm.uv[i-1].x+vm.uv_offset.x));
    if(i&2)v=float(float(float(vm.uv[i].y-vm.uv[i-2].y)*vm.texture_scale.y)+float(vm.uv[i-2].y+vm.uv_offset.y));
    return {u,v};
#elif PORT_TITLE == 13
    // 46750b..4675b5 scales edges against their own row/column origin.
    const double u=(i&1)?(double(vm.uv[i].x)-vm.uv[i-1].x)*vm.texture_scale.x+vm.uv[i-1].x+vm.uv_offset.x:double(vm.uv[i].x)+vm.uv_offset.x;
    const double v=(i&2)?(double(vm.uv[i].y)-vm.uv[i-2].y)*vm.texture_scale.y+vm.uv[i-2].y+vm.uv_offset.y:double(vm.uv[i].y)+vm.uv_offset.y;
    return {float(u),float(v)};
#else
    return {float(double(vm.uv[i].x)+vm.uv_offset.x),float(double(vm.uv[i].y)+vm.uv_offset.y)};
#endif
}
#if PORT_TITLE == 14
static bool anm_quad_positions_impl(const AnmVm& vm,Vec3 (&out)[4],const AnmRenderSpace& space,u32 mode) noexcept {
    const u32 anchor_x=(vm.flags>>18)&3,anchor_y=(vm.flags>>20)&3;
#else
bool anm_quad_positions(const AnmVm& vm,Vec3 (&out)[4],const AnmRenderSpace& space) noexcept {
    const u32 mode=vm.drawing_mode(),anchor_x=(vm.flags>>18)&3,anchor_y=(vm.flags>>20)&3;
#endif
    if(mode>3||anchor_x==3||anchor_y==3)return false;
#if PORT_TITLE == 16
    // Own465c40/4660b0: pivot and resolution precede inherited sprite scale.
    const float left=anchor_x==0?-.5f:anchor_x==1?0.f:-1.f,top=anchor_y==0?-.5f:anchor_y==1?0.f:-1.f;
    const float xs[]={left,left+1,left,left+1},ys[]={top,top,top+1,top+1};
    float sx=float(vm.extra_scale.x*vm.scale.x),sy=float(vm.extra_scale.y*vm.scale.y);
    if(vm.parent&&!(vm.flags2&0x10000)){
        sx=float(sx*float(vm.parent->extra_scale.x*vm.parent->scale.x));sy=float(sy*float(vm.parent->extra_scale.y*vm.parent->scale.y));
    }
    const u32 resolution=(vm.flags2>>20)&7;const float factor=resolution==1?space.scale:resolution==2?float(space.scale*.5f):1.f;
    const auto position=vm.render_position(space);
    float angle=vm.rotation.z;
    for(auto* node=&vm;node->th16_transform_parent&&!(node->flags2&0x10000);node=node->th16_transform_parent)angle=float(angle+node->th16_transform_parent->rotation.z);
    const float c=float(std::cos(double(angle))),s=float(std::sin(double(angle)));
    const float z=float(float(vm.position.z+vm.script_position.z)+vm.child_position.z);
    for(u32 i=0;i<4;++i){
        const float x=float(float(float(float(xs[i]*vm.sprite_size.x)-vm.pivot.x)*factor)*sx),y=float(float(float(float(ys[i]*vm.sprite_size.y)-vm.pivot.y)*factor)*sy);
        if(mode==1)out[i]={float(float(float(x*c)-float(y*s))+position.x),float(float(float(x*s)+float(y*c))+position.y),z};
        else out[i]={float(x+position.x),float(y+position.y),z};
    }return true;
#elif PORT_TITLE == 14
    // 476100 / 4766e0: float SSE steps are observable at non-integral scales.
    float sx=float(vm.scale.x*vm.extra_scale.x),sy=float(vm.scale.y*vm.extra_scale.y),angle=vm.rotation.z;
    if(vm.parent&&!(vm.flags2&0x8000)){
        sx=float(float(vm.parent->scale.x*vm.parent->extra_scale.x)*sx);
        sy=float(float(vm.parent->scale.y*vm.parent->extra_scale.y)*sy);
        angle=float(angle+vm.parent->rotation.z);
    }
    const float left=anchor_x==0?-.5f:anchor_x==1?0.f:-1.f,top=anchor_y==0?-.5f:anchor_y==1?0.f:-1.f;
    const float xs[]={left,left+1,left,left+1},ys[]={top,top,top+1,top+1};
    const auto position=vm.render_position(space);const u32 resolution=(vm.flags2>>19)&7;
    const float factor=resolution==1?space.scale:resolution==2?float(space.scale*.5f):1.f;
    // Both helpers overwrite all four Z values with the untransformed local Z.
    const float z=float(float(vm.position.z+vm.script_position.z)+vm.child_position.z);
    const float c=float(std::cos(double(angle))),s=float(std::sin(double(angle)));
    for(u32 i=0;i<4;++i){
        const float x=float(float(float(float(xs[i]*vm.sprite_size.x)-vm.pivot.x)*factor)*sx);
        const float y=float(float(float(float(ys[i]*vm.sprite_size.y)-vm.pivot.y)*factor)*sy);
        if(mode==1){const float rx=float(float(c*x)-float(s*y)),ry=float(float(c*y)+float(s*x));out[i]={float(rx+position.x),float(ry+position.y),z};}
        else out[i]={float(x+position.x),float(y+position.y),z};
    }
    return true;
#elif PORT_TITLE == 13
    // 467b50 / 467e80: pivot is in sprite units, before scaling/rotation.
    float sx=float(double(vm.scale.x)*vm.extra_scale.x),sy=float(double(vm.scale.y)*vm.extra_scale.y),angle=vm.rotation.z;
    if(vm.parent&&!(vm.flags2&0x10000)){
        sx=float(double(vm.parent->scale.x)*vm.parent->extra_scale.x*sx);
        sy=float(double(vm.parent->scale.y)*vm.parent->extra_scale.y*sy);
        angle=float(double(angle)+vm.parent->rotation.z);
    }
    const float left=anchor_x==0?-.5f:anchor_x==1?0.f:-1.f,top=anchor_y==0?-.5f:anchor_y==1?0.f:-1.f;
    const float xs[]={left,left+1,left,left+1},ys[]={top,top,top+1,top+1};
    const auto position=vm.render_position();const float z=float(double(vm.position.z)+vm.script_position.z+vm.child_position.z);
    const float c=float(std::cos(double(angle))),s=float(std::sin(double(angle)));
    for(u32 i=0;i<4;++i){
        const float x=float(double(float(double(float(double(xs[i])*vm.sprite_size.x))-vm.pivot.x))*sx);
        const float y=float(double(float(double(float(double(ys[i])*vm.sprite_size.y))-vm.pivot.y))*sy);
        if(mode==1){const float rx=float(double(c)*x-double(s)*y),ry=float(double(c)*y+double(s)*x);out[i]={float(double(rx)+position.x),float(double(ry)+position.y),z};}
        else out[i]={float(double(x)+position.x),float(double(y)+position.y),z};
    }
    return true;
#else
    const float w=float(double(vm.sprite_size.x)*vm.scale.x),h=float(double(vm.sprite_size.y)*vm.scale.y);
    const auto resolved=vm.render_position();
    const double x=kPortTitle==13?resolved.x:double(vm.position.x)+vm.script_position.x+vm.child_position.x;
    const double y=kPortTitle==13?resolved.y:double(vm.position.y)+vm.script_position.y+vm.child_position.y;
    const float z=kPortTitle==13?resolved.z:float(double(vm.position.z)+vm.script_position.z+vm.child_position.z);
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
            if(anchor==0){low=float(origin-float(double(size)*.5));if(mode==0)low=std::floor(low);high=float(double(low)+size);}
            else if(anchor==1){low=float(origin);high=float(origin+size);}
            else{low=float(origin-size);high=float(origin);}
        };
        float left,right,top,bottom;extent(x,w,anchor_x,left,right);extent(y,h,anchor_y,top,bottom);
        out[0]={left,top,z};out[1]={right,top,z};out[2]={left,bottom,z};out[3]={right,bottom,z};
    }
    return true;
#endif
}
#if PORT_TITLE == 14
bool anm_quad_positions(const AnmVm& vm,Vec3 (&out)[4],const AnmRenderSpace& space) noexcept {
    return anm_quad_positions_impl(vm,out,space,vm.drawing_mode());
}
bool anm_th14_draw_quad_positions(const AnmVm& vm,Vec3 (&out)[4],const AnmRenderSpace& space) noexcept {
    // 468170 selects rotated geometry for draw mode3 without a VM store.
    const u32 mode=vm.drawing_mode();
    return anm_quad_positions_impl(vm,out,space,mode==3?1:mode);
}
#endif
Vec2 anm_screen_uv(Vec3 p) noexcept {Vec2 uv{float(double(p.x)/640),float(double(p.y)/480)};if(uv.x<0)uv.x=0;if(uv.y<0)uv.y=0;return uv;}
bool anm_ring_vertices(const AnmVm& vm,AnmVertex* out,u32 capacity,const AnmRenderSpace& space) noexcept {
    const u32 mode=vm.drawing_mode();const i32 count=vm.integers[0];
    if((mode!=9&&mode!=13&&!((kPortTitle==13||kPortTitle==14)&&mode==14))||count<2||u32(count)>capacity/2||!out)return false;
    constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;
    auto normalize=[](float value){u32 loops=0;while(value>pi){value=float(double(value)-tau);if(loops++>32)break;}while(value<-pi){value=float(double(value)+tau);if(loops++>32)break;}return value;};
    float angle=mode==9?vm.rotation.z:normalize(float(double(vm.rotation.z)-(mode==14?0:double(vm.rotation.x)*.5)));
    const float angle_step=float((mode==9?tau:double(vm.rotation.x))/float(count-1));
    const float uv_step=float(double(vm.integers[1])/float(count-1));float v=0;
    const bool modern=kPortTitle==13||kPortTitle==14;
    const bool secondary=modern?(vm.th13_native_flags&0x60000)!=0:(vm.flags&0x8000)!=0;
    const u32 color=secondary?vm.secondary_color:vm.color;
    const Vec3 center=modern?vm.render_position(space):Vec3{float(double(float(double(vm.script_position.x)+vm.child_position.x))+vm.position.x),float(double(float(double(vm.script_position.y)+vm.child_position.y))+vm.position.y),float(double(float(double(vm.script_position.z)+vm.child_position.z))+vm.position.z)};
    for(i32 i=0;i<(mode==9?count-1:count);++i){
        const double cs=std::cos(double(angle)),sn=std::sin(double(angle));
        for(u32 side=0;side<2;++side){
            float radius=float(double(vm.scale.y)+(side?-1:1)*double(vm.scale.x)*.5);
            if constexpr(kPortTitle==14){
                const float half=float(vm.scale.x*.5f);radius=side?float(vm.scale.y-half):float(vm.scale.y+half);
                const u32 resolution=(vm.flags2>>19)&7;
                if(resolution==1)radius=float(radius*space.scale);else if(resolution==2)radius=float(radius*float(space.scale*.5f));
            }
            const float x=float(cs*radius),y=float(sn*radius);
            out[i*2+side]={{float(double(x)+center.x),float(double(y)+center.y),float(double(center.z)+0.)},1,modern&&mode==9&&side==0?vm.color:color,{float(double(vm.uv_offset.x)+vm.uv[side].x),float(double(v)+vm.uv_offset.y)}};
        }
        v=float(double(v)+uv_step);angle=normalize(float(double(angle)+angle_step));
    }
    if(mode==9){out[(count-1)*2]=out[0];out[(count-1)*2+1]=out[1];out[(count-1)*2].uv.y=out[(count-1)*2+1].uv.y=float(double(v)+vm.uv_offset.y);}
    return true;
}
#if PORT_TITLE == 14
bool anm_th14_install_effect(AnmVm& vm,i32 type,AnmEnvironment& env){
    if(type==0){
        if(!vm.resource||!env.allocate_geometry(vm,sizeof(TransitionType0Th14)))return false;
        auto* transition=new(vm.geometry)TransitionType0Th14();
        if(!transition->initialize(vm,*vm.resource,env))return false;
        vm.th13_effect=1;return true;
    }
    if(type==7){
        if(!env.allocate_geometry(vm,sizeof(AnmTh14Particles)))return false;
        auto& p=*static_cast<AnmTh14Particles*>(vm.geometry);std::memset(&p,0,sizeof p);p.timer.set(0,&env.rate);vm.th13_effect=7;return true;
    }
    // Effect 8 in 4d5618: the 40-segment strip used by st02wl.anm.
    if(type!=8||!env.allocate_geometry(vm,sizeof(AnmTh14Ribbon)))return false;
    auto& r=*static_cast<AnmTh14Ribbon*>(vm.geometry);std::memset(&r,0,sizeof r);
    r.phase=float(float(env.script_rng.signed_unit())*3.1415927410125732421875f);
    r.timer.set(1,&env.rate);for(auto& step:r.steps)step={0,0,4};
    r.angular_speed=.1745329201221466f;
    vm.position=vm.native_prefix_position;vm.layer=15;vm.th13_effect=8;return true;
}
int anm_th14_particles_update(AnmVm& vm,AnmEnvironment& env){
    auto& p=*static_cast<AnmTh14Particles*>(vm.geometry);
    auto add=[](Vec3 a,Vec3 b){return Vec3{float(a.x+b.x),float(a.y+b.y),float(a.z+b.z)};};
    auto subtract=[](Vec3 a,Vec3 b){return Vec3{float(a.x-b.x),float(a.y-b.y),float(a.z-b.z)};};
    auto multiply=[](Vec3 a,float b){return Vec3{float(a.x*b),float(a.y*b),float(a.z*b)};};
    auto polar=[](float a,float r){return Vec3{float(std::cos(double(a))*r),float(std::sin(double(a))*r),0};};
    // Retail effect7 calls the same D3DX normalize service as its camera.
    auto normalize=[](Vec3 a){return GraphicsMath::normalize(a);};
    auto random_offset=[&](float radius){const float angle=float(env.visual_rng.signed_unit()*3.1415927410125732f);return polar(angle,float(env.visual_rng.unit()*radius));};
    auto interpolate=[&](AnmVm& child,Vec3 from,Vec3 initial,Vec3 to,Vec3 final){
        auto& v=child.position_interpolation;std::memcpy(v.start,&from,12);std::memcpy(v.end,&to,12);
        std::memcpy(v.initial_tangent,&initial,12);std::memcpy(v.final_tangent,&final,12);
        v.duration=vm.integers[0];v.mode=InterpolationMode::Hermite;v.timer.set(0,&env.rate);
    };
    p.origin=vm.position;p.far_center=add(vm.position,polar(vm.rotation.z,300));
    p.near_center=add(vm.position,polar(float(vm.rotation.z+vm.extra_floats.z),150));
    if(p.timer.current!=p.timer.previous&&p.timer.current>=0&&p.timer.current<50){
        const u32 first=u32(p.timer.current)*4;
        for(u32 i=0;i<4;++i){
            auto* child=env.spawn_effect(vm,i==3?152:151);if(!child)return -1;
            p.children[first+i]=child->id;child->color=vm.color;child->integers[0]=vm.integers[0];
            if(i==3){u32 inverse=vm.color&0xff000000;for(u32 shift=0;shift<24;shift+=8)inverse|=u32(u8(44-i32((vm.color>>shift)&255)))<<shift;child->color=inverse;}
        }
    }
    u32 active=0;
    for(u32 i=0;i<200;++i){
        auto* child=env.find(p.children[i]);if(!child){p.children[i]=0;continue;}
        child->slowdown_factor=vm.inherited_slowdown();++active;
        if(p.states[i]==0){
            const Vec3 from=add(p.far_center,random_offset(150)),to=add(p.near_center,random_offset(50));
            const Vec3 delta=subtract(to,from);
            const Vec3 final_direction=add(normalize(delta),normalize(subtract(p.origin,to)));
            const float final_length=float(float(env.visual_rng.unit()*200.f)+200.f);
            const Vec3 final=multiply(normalize(final_direction),final_length);
            const float initial_length=float(float(env.visual_rng.unit()*100.f)+100.f);
            const Vec3 initial=multiply(normalize(delta),initial_length);
            interpolate(*child,from,initial,to,final);p.positions[i]=to;p.tangents[i]=final;p.states[i]=1;
        }else if(p.states[i]==1&&child->age.current>=wrapping_add(vm.integers[0],1)){
            const Vec3 to=add(vm.position,random_offset(20)),final=random_offset(20);
            interpolate(*child,p.positions[i],p.tangents[i],to,final);p.states[i]=2;
        }
    }
    if(!active)return 1;p.timer.tick();return 0;
}
void anm_th14_particles_destroy(AnmVm& vm,AnmEnvironment& env) noexcept{
    auto& p=*static_cast<AnmTh14Particles*>(vm.geometry);
    for(auto& id:p.children)if(auto* child=env.find(id)){child->flags&=~1u;child->instruction=nullptr;}else id=0;
}
void anm_th14_ribbon_update(AnmVm& vm,AnmEnvironment& env) noexcept{
    auto& r=*static_cast<AnmTh14Ribbon*>(vm.geometry);
    if(r.timer.current%2==0){
        for(u32 i=39;i>0;--i)r.steps[i]=r.steps[i-1];
        constexpr float amount=.01745329238474369f;
        if(r.angular_speed>=.5235987901687622f)r.angular_speed=float(r.angular_speed-float(float(env.script_rng.unit())*amount));
        else if(r.angular_speed<=.1745329201221466f)r.angular_speed=float(r.angular_speed+float(float(env.script_rng.unit())*amount));
        else r.angular_speed=float(r.angular_speed+float(float(float(env.script_rng.signed_unit())*amount)*2.f));
        // 40b253 stores this sum back to +960, not to the angle at +964.
        r.angular_speed=float(r.angular_speed+r.angle);
        r.steps[0].x=-float(std::sin(double(r.angular_speed))*4.);r.steps[0].z=float(std::cos(double(r.angular_speed))*4.);
    }
    Vec3 position{0,0,-200};
    for(u32 i=0;i<40;++i){
        const float v=float(float(i)*.02500000037252903f);
        r.vertices[i*2]={{float(position.x+8.f),position.y,position.z},0x80ffffff,{0,v}};
        r.vertices[i*2+1]={{float(position.x-8.f),position.y,position.z},0x80ffffff,{1,v}};
        const auto& step=r.steps[i];position={float(position.x+step.x),float(position.y+step.y),float(position.z+step.z)};
    }
    r.timer.tick();
}
bool anm_world_ring_vertices(const AnmVm& vm,AnmWorldVertex* out,u32 capacity) noexcept{
    const u32 mode=vm.drawing_mode();const i32 count=vm.integers[0];
    if((mode!=23&&mode!=24)||count<2||u32(count)>capacity/2||!out)return false;
    constexpr float pi=3.1415927410125732421875f,tau=6.283185482025146484375f;
    auto normalize=[](float value){u32 loops=0;while(value>pi){value=float(value-tau);if(loops++>32)break;}while(value<-pi){value=float(value+tau);if(loops++>32)break;}return value;};
    float angle=normalize(float(vm.floats[3]-float(vm.floats[0]*.5f))),v=0;
    const float step=float(vm.floats[0]/float(count-1)),uv_step=float(float(vm.integers[1])/float(count-1));
    const float half=float(vm.floats[1]*.5f),height=mode==23?half:0.f;
    const float radii[]={mode==24?float(vm.floats[2]-half):vm.floats[2],mode==24?float(vm.floats[2]+half):vm.floats[2]};
    const u32 color=vm.th13_native_flags&0x60000?vm.secondary_color:vm.color;
    for(i32 i=0;i<count;++i){
        const double c=std::cos(double(angle)),s=std::sin(double(angle));
        for(u32 side=0;side<2;++side)out[i*2+side]={{float(c*radii[side]),side?-height:height,float(s*radii[side])},color,{float(vm.uv[side].x+vm.uv_offset.x),float(v+vm.uv_offset.y)}};
        v=float(v+uv_step);angle=normalize(float(angle+step));
    }
    return true;
}
#endif
bool AnmEnvironment::screen_uv(AnmVm& vm){Vec3 p[4];if(!anm_quad_positions(vm,p,render_space))return false;for(u32 i=0;i<4;++i)vm.uv[i]=anm_screen_uv(p[i]);return true;}
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
#if PORT_TITLE == 13
bool anm_th13_install_effect(AnmVm& vm,i32 type,AnmEnvironment& env){
    // The six shipped st03wl scripts request effect 5 from 4bba80.
    if(type!=5||!env.allocate_geometry(vm,sizeof(AnmRibbon)))return false;
    auto& ribbon=*static_cast<AnmRibbon*>(vm.geometry);std::memset(&ribbon,0,sizeof ribbon);
    ribbon.phase=float(double(env.script_rng.signed_unit())*3.1415927410125732421875);
    ribbon.timer.set(1,&env.rate);for(auto& step:ribbon.steps)step={3,0,-16};
    vm.position=vm.native_prefix_position;vm.layer=13;vm.th13_effect=5;return true;
}
void anm_th13_ribbon_update(AnmVm& vm,AnmEnvironment& env) noexcept{
    auto& r=*static_cast<AnmRibbon*>(vm.geometry);
    if(r.timer.current%4==0){
        for(u32 i=15;i>0;--i)r.steps[i]=r.steps[i-1];
        const float x=r.steps[0].x;constexpr double amount=.10000000149011612;
        if(x>=3)r.steps[0].x=float(double(x)-double(env.script_rng.unit())*amount);
        else r.steps[0].x=float(double(x)+double(x<=0?env.script_rng.unit():env.script_rng.signed_unit())*amount);
    }
    Vec3 position{};float half_width=0;
    for(u32 i=0;i<16;++i){
        const float v=float(double(i)*.0625);
        r.vertices[i*2]={{float(double(position.x)+half_width),position.y,position.z},0x80ffffff,{0,v}};
        r.vertices[i*2+1]={{float(double(position.x)-half_width),position.y,position.z},0x80ffffff,{1,v}};
        if(half_width<8)half_width=float(double(half_width)+2);
        const auto& step=r.steps[i];position={float(double(position.x)+step.x),float(double(position.y)+step.y),float(double(position.z)+step.z)};
    }
    for(const auto i:{30,31,0,1,15,10,24,3,12,27})r.vertices[i].color=0xffffff;
    r.vertices[28].color=r.vertices[29].color=0x50ffffff;r.vertices[26].color=0xb0ffffff;
    r.timer.tick();
}
#endif
}
