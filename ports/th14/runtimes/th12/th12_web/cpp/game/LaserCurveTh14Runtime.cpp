#include "LaserState.hpp"
#include "AnmRenderer.hpp"
#include "Movement.hpp"
namespace th11 {
#if PORT_TITLE == 14
namespace {
float field(const u8* p){float f;std::memcpy(&f,p,4);return f;}
i32 integer(const u8* p){i32 i;std::memcpy(&i,p,4);return i;}
i32 sprite_callback(AnmVm& vm,i32){
    const auto& curve=*reinterpret_cast<Th14LaserCurve*>(reinterpret_cast<u8*>(&vm)-offsetof(Th14LaserCurve,body));
    return curve.base.color+511;
}
}
bool laser_th14_curve_initialize(Th14LaserCurve& curve,const u8* request,AnmResource& resource,AnmEnvironment& animations,LaserWorld& world,u16 file,const std::vector<Th14CurveNode>* inherited){
    auto& b=curve.base;
    if(!curve.path.initialize(request,inherited)||curve.path.samples.empty())return false;
    // Retail440372/4403c4 preserves the adjusted request origin even when the
    // inherited path is sampled at a nonzero initial time.
    b.state=b.type=2;std::memcpy(&b.position,curve.path.request,12);
    b.angle=field(request+0xc);b.width=field(request+0x10);b.speed=field(request+0x14);b.sprite=i16(integer(request+0x18));b.color=i16(integer(request+0x1c));
    const auto velocity=polar(b.angle,b.speed);b.velocity={velocity.x,velocity.y,0};
    auto& body=curve.body;
    if(!body.bind_script(resource,b.sprite==1?294:b.sprite+114,file,&animations.rate))return false;
    if(b.sprite!=1)body.sprite_callback=sprite_callback;
    body.interrupt(2);if(body.update(animations)<0)return false;
    body.flags=(body.flags&~(0x70u|0x3c0000u))|0x10u|0x200000u;body.th13_native_flags&=~0x100u;body.set_drawing_mode(1);body.flags2=(body.flags2&~0x40000u)|0x20000u;
    auto& origin=curve.origin;
    if(!origin.copy_script(resource,b.color+56,file,&animations.rate)||origin.update(animations)<0)return false;
    origin.interrupt(2);if(origin.update(animations)<0)return false;
    origin.flags=(origin.flags&~(0x70u|0x3c0000u))|0x10u;origin.th13_native_flags&=~0x100u;origin.set_drawing_mode(1);origin.flags2=(origin.flags2&~0x40000u)|0x20000u;
    curve.curve_timer.set(0,&world.rate);curve.curve_timer.fractional=field(request+0x350);curve.curve_timer.current=truncate_int(curve.curve_timer.fractional);curve.curve_timer.previous=wrapping_sub(curve.curve_timer.current,1);
    b.offscreen_timer.set(30,&world.rate);b.graze_timer.set(0,&world.rate);
    const i32 sound=integer(request+0x344);if(sound>=0)world.sound(sound,0,true);
    curve.vertices.resize(curve.path.samples.size()*2);return true;
}
i32 laser_th14_curve_collision(Th14LaserCurve& curve,LaserWorld& world,bool probe){
    auto& b=curve.base;
    float distance=0,scratch_z=0;bool grazed=false;Vec3 graze{};
    for(u32 j=0;j+1<curve.path.samples.size();++j){
        // 443fe0 writes X/Y only; 440e06 retains and adds the prior scratch
        // Z. Use a defined zero initial scratch, preserving later additions.
        const auto& s=curve.path.samples[j];const auto v=polar(s.angle,s.speed*.5f);scratch_z+=s.position.z;const Vec3 center{s.position.x+v.x,s.position.y+v.y,scratch_z};
        distance+=s.speed;if(distance<16.f)continue;
        const i32 hit=probe?world.warning_collision(center,s.angle,b.width*.5f,s.speed):world.collision(center,s.angle,b.width*.5f,s.speed);if(hit<0){curve.diagnostic=2;return -2;}
        // Retail 440e52..440e85 calls this curve's rectangle consumer and
        // then continues through the retained samples, keeping graze state.
        if(hit==1&&world.th14_curve_cut(curve,world.player,{32,32},0,0,true)<0){curve.diagnostic=3;return -2;}
        if(hit==2&&!grazed&&b.graze_timer.current%3==0){grazed=true;graze=center;}
    }
    if(grazed&&!world.th14_curve_graze(graze)){curve.diagnostic=4;return -2;}
    b.graze_timer.tick();
    return 0;
}
i32 laser_th14_curve_update(Th14LaserCurve& curve,AnmEnvironment& animations,LaserWorld& world){
    auto& b=curve.base;curve.diagnostic=0;if(!curve.path.transforms()){curve.diagnostic=1;return -2;}curve.path.sample(curve.curve_timer.fractional);
    if(b.offscreen_timer.current<=0){
        bool visible=false;for(const auto& s:curve.path.samples)if(s.position.x+b.width>-192.f&&s.position.x-b.width<192.f&&s.position.y+b.width>0&&s.position.y-b.width<448.f){visible=true;break;}
        if(!visible)return 1;
    }else b.offscreen_timer.advance(-1);
    if(laser_th14_curve_collision(curve,world)<0)return -2;
    if(curve.body.update(animations)<0){curve.diagnostic=5;return -2;}
    if(curve.origin.update(animations)<0){curve.diagnostic=6;return -2;}
    curve.curve_timer.tick();return 0;
}
bool laser_th14_curve_draw(Th14LaserCurve& curve,AnmRenderer& renderer){
    th14_curve_mesh(curve.path.samples.data(),curve.path.samples.size(),field(curve.path.request+0x10),curve.body.uv[0].y,curve.body.uv[2].y,{224,16},curve.vertices.data());
    static_assert(sizeof(Th14CurveVertex)==sizeof(AnmVertex));
    if(renderer.draw_th14_screen_strip(curve.body,reinterpret_cast<const AnmVertex*>(curve.vertices.data()),curve.vertices.size())==-2)return false;
    if(i32(curve.path.samples.size())>=curve.curve_timer.current){curve.origin.script_position=curve.path.samples.back().position;if(renderer.draw(curve.origin)==-2)return false;}
    return true;
}
i32 laser_th14_curve_cancel(Th14LaserCurve& curve,bool,bool skip,LaserWorld& world){
    if(skip&&curve.base.protection)return 0;
    for(u32 j=0;j<curve.path.samples.size();j+=3)if(!world.cancel_effect(curve.path.samples[j].position,integer(curve.path.request+0x1c)*2+181))return -2;
    curve.base.state=1;return 0;
}
i32 laser_th14_curve_cut_circle(Th14LaserCurve& curve,Vec3 center,float radius,u32 rewards,bool skip,LaserWorld& world){
    std::vector<Vec3> deliveries;bool mark=false;const i32 count=curve.path.cut_circle(center,radius,curve.base.protection!=0,skip,deliveries,mark);
    if(count<0)return count;
    for(const auto p:deliveries){if(rewards&&!world.th14_curve_reward(p,rewards))return -2;if(!world.cancel_effect(p,integer(curve.path.request+0x1c)*2+181))return -2;}
    if(mark)curve.native_flags=(curve.native_flags&~4u)|2;
    curve.vertices.resize(curve.path.samples.size()*2);return count;
}
i32 laser_th14_curve_cut_rectangle(Th14LaserCurve& curve,Vec3 center,Vec2 size,float angle,u32 rewards,bool skip,std::vector<std::array<u8,0x354>>& fragments,LaserWorld& world){
    std::vector<Vec3> deliveries;bool mark=false;const i32 count=curve.path.cut_rectangle(center,size,angle,curve.base.protection!=0,skip,curve.curve_timer,deliveries,mark,fragments);
    if(count<0)return count;
    for(const auto p:deliveries){if(rewards&&!world.th14_curve_reward(p,rewards))return -2;if(!world.cancel_effect(p,integer(curve.path.request+0x1c)*2+181))return -2;}
    if(mark)curve.native_flags=(curve.native_flags&~4u)|2;
    curve.vertices.resize(curve.path.samples.size()*2);return count;
}
#endif
}
