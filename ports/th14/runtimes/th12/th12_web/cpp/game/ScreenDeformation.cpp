#include "ScreenDeformation.hpp"
#include "EnemyAnimations.hpp"
#include "GraphicsMath.hpp"
#include <cmath>
namespace th11 {
ScreenDeformation::~ScreenDeformation(){EnemyAnimations helper(manager);for(auto id:animations)helper.erase(id);if(control_animation)helper.erase(control_animation);}
bool ScreenDeformation::initialize(AnmResource& text,u32 width,u32 height,bool second_target){
    if(width<2||height<3||width>128||height>128||!animations.empty())return false;
    if constexpr(kPortTitle==16)if(width!=17)return false;
    columns=width;rows=height;vertices.resize(columns*rows);positions.resize(columns*rows);animations.resize(columns-1);
#if PORT_TITLE == 16
    // 418c70 creates a separate control VM before the sixteen column strips.
    auto* control=manager.create(text,59,0,34);if(!control)return false;control_animation=control->id;
    if(!manager.allocate_geometry(*control,0x70))return false;
    control->set_drawing_mode(0);control->th16_callbacks[1]=4;
    control->reserved_418=u32(reinterpret_cast<uintptr_t>(this));
    control->draw_callback=draw_callback;
#endif
    for(auto& id:animations){
        auto* vm=manager.create(text,kPortTitle==14||kPortTitle==16?59:second_target?82:81,0,kPortTitle==16?34:kPortTitle==14?32:second_target?28:27,false,false);if(!vm)return false;id=vm->id;
        if(!manager.allocate_geometry(*vm,rows*2*sizeof(AnmVertex)))return false;
        vm->flags=(vm->flags&0xff3fffffu)|0x3000000;vm->integers[0]=rows;
        auto* strip=static_cast<AnmVertex*>(vm->geometry);
        for(u32 i=0;i<rows*2;++i){strip[i].position.z=0;strip[i].reciprocal_w=1;strip[i].color=0xffffffff;}
        if constexpr(kPortTitle!=16){vm->draw_callback=draw_callback;vm->reserved_418=u32(reinterpret_cast<uintptr_t>(this));}vm->flags&=0xffffff8f;
#if PORT_TITLE == 14 || PORT_TITLE == 16
        vm->th13_native_flags=(vm->th13_native_flags&0xd9ffffffu)|0x18000000;
        vm->flags2&=~(kPortTitle==16?0xc0000u:0x60000u);
#endif
    }return true;
}
void ScreenDeformation::copy_strips(){
    for(u32 x=0;x<columns-1;++x){auto* vm=manager.find(animations[x]);if(!vm||!vm->geometry)continue;
        auto* out=static_cast<AnmVertex*>(vm->geometry);
        for(u32 y=0;y<rows;++y){out[y*2]=vertices[x*rows+y];out[y*2+1]=vertices[(x+1)*rows+y];}
    }
}
void ScreenDeformation::draw_callback(AnmVm& vm){reinterpret_cast<ScreenDeformation*>(uintptr_t(vm.reserved_418))->copy_strips();}
void ScreenDeformation::rectangle(float x,float y,float width,float height){
#if PORT_TITLE == 16
    float px=x+manager.render_space.play_origin.x;
    const float top=y+manager.render_space.play_origin.y,dx=width/float(columns-1),dy=height/float(rows-1);
    for(u32 col=0;col<columns;++col){float py=top;for(u32 row=0;row<rows;++row){const u32 n=col*rows+row;
        positions[n]={px,py,0};const float u=px/640.f,v=py/480.f;
        vertices[n]={{px,py,0},1,0xffffffff,{u<0?0:u,v<0?0:v}};py=py+dy;
    }px=px+dx;}copy_strips();
#else
    float px=kPortTitle==14?float(x+manager.render_space.play_origin.x):float(double(x)+192+32);
    const float top=kPortTitle==14?float(y+manager.render_space.play_origin.y):float(double(y)+16),dx=float(double(width)/(columns-1)),dy=float(double(height)/(rows-1));
    for(u32 col=0;col<columns;++col){float py=top;for(u32 row=0;row<rows;++row){const u32 n=col*rows+row;
        positions[n]={px,py,0};const float u=kPortTitle==14?px/(manager.render_space.scale*640.f):float(double(px)/640),v=kPortTitle==14?py/(manager.render_space.scale*480.f):float(double(py)/480);
        vertices[n]={{px,py,0},1,0xffffffff,{u<0?0:u,v<0?0:v}};py=float(double(py)+dy);
    }px=float(double(px)+dx);}copy_strips();
#endif
}
void ScreenDeformation::update(EnemyState& e,i32 spell_id){
#if PORT_TITLE == 14 || PORT_TITLE == 16
    (void)spell_id;
    // TH14 424100 / TH16 41cbd0: SSE before sin and D3DX normalization.
    const float radius=e.deformation_radius;float phase_x=e.deformation_phase_x,phase_y=e.deformation_phase_y;
    if(e.deformation_target>radius)e.deformation_radius=radius+manager.rate*2.f;
    const float extent=radius*2.f+40.f;
    rectangle((e.current.position.x-radius)-20.f,(e.current.position.y-radius)-20.f,extent,extent);
    const Vec3 center{e.current.position.x+(kPortTitle==16?manager.render_space.play_origin.x:(manager.render_space.scale*640.f)*.5f),e.current.position.y+manager.render_space.play_origin.y,e.current.position.z};
    const float squared=radius*radius,left=manager.render_space.play_origin.x-192.f,top=manager.render_space.play_origin.y;
    for(u32 n=0;n<vertices.size();++n){auto& v=vertices[n];auto& p=positions[n];
        const Vec3 delta{p.x-center.x,p.y-center.y,p.z-center.z};
        const float xx=delta.x*delta.x,yy=delta.y*delta.y,distance=yy+xx,remaining=squared-distance;
        if(kPortTitle==16?remaining>=0:!(remaining<0)){
            const float amount=remaining/squared;u32 color=0xff000000;
            for(u32 shift:{0u,8u,16u}){const float loss=float(255-((e.deformation_color>>shift)&255))*amount;const float channel=255.f-loss;color|=u32(u8(truncate_int(channel)))<<shift;}
            v.color=color;const float strength=amount*32.f;auto direction=GraphicsMath::normalize(delta);
            const float sx=float(std::sin(double(phase_x))),sy=float(std::sin(double(phase_y)));
            const float wave_x=(sx*amount)*8.f,wave_y=(sy*amount)*8.f;
            v.position.x+=(direction.x*strength)+wave_x;v.position.y+=(direction.y*strength)+wave_y;v.position.z=p.z=0;
        }else v.color&=0xffffff;
        phase_x=normalize_angle(phase_x+0.09817477315664291f);phase_y=normalize_angle(phase_y-0.04908738657832146f);
        if(p.x<=left)v.position.x=p.x=left+1.f;else if(p.x>=left+384.f)v.position.x=p.x=(left+384.f)-1.f;
        if(p.y<=top)v.position.y=p.y=top+1.f;else if(p.y>=top+448.f)v.position.y=p.y=(top+448.f)-1.f;
        // Retail4245d5/4245ed divide by the current display width/height.
        v.uv={p.x/(kPortTitle==14?manager.render_space.scale*640.f:640.f),p.y/(kPortTitle==14?manager.render_space.scale*480.f:480.f)};if(v.uv.x<0)v.uv.x=0;if(v.uv.y<0)v.uv.y=0;
    }
    e.deformation_phase_x=normalize_angle(e.deformation_phase_x+manager.rate*0.19634954631328583f);
    e.deformation_phase_y=normalize_angle(e.deformation_phase_y+manager.rate*0.09817477315664291f);
    return;
#else
    const float radius=e.deformation_radius;float phase_x=e.deformation_phase_x,phase_y=e.deformation_phase_y;
    const bool faint=spell_id==0xad;const float wave=faint?1:8;
    if(e.deformation_radius<e.deformation_target)e.deformation_radius=float(double(e.deformation_radius)+2);
    const u8 alpha=e.deformation_radius<32?0:e.deformation_radius>=64?255:u8(i32((double(e.deformation_radius)-32)*255*.03125));
    const float extent=float(double(radius)*2+40);const Vec3 center{float(double(e.current.position.x)+32+192),float(double(e.current.position.y)+16),e.current.position.z};
    rectangle(float(double(e.current.position.x)-radius-20),float(double(e.current.position.y)-radius-20),extent,extent);
    const double squared=double(radius)*radius;
    for(u32 n=0;n<vertices.size();++n){auto& v=vertices[n];auto& p=positions[n];
        const Vec3 delta{float(double(p.x)-center.x),float(double(p.y)-center.y),float(double(p.z)-center.z)};
        const float distance=float(double(delta.x)*delta.x+double(delta.y)*delta.y),remaining=float(squared-distance);
        if(remaining<0)v.color&=0xffffff;
        else{
            const float amount=float(double(remaining)/squared);u32 color=0;
            for(u32 shift:{0u,8u,16u})color|=u32(u8(i32(255.-double(255-((e.deformation_color>>shift)&255))*amount)))<<shift;
            color|=u32(faint?u8(i32(double(alpha)*amount)):255)<<24;v.color=color;
            const float strength=float(double(amount)*(faint?4:32));auto direction=GraphicsMath::normalize(delta);
            direction.x=float(double(direction.x)*strength);direction.y=float(double(direction.y)*strength);
            direction.x=float(double(float(std::sin(double(phase_x))))*amount*wave+direction.x);
            direction.y=float(double(float(std::sin(double(phase_y))))*amount*wave+direction.y);
            v.position.x=float(double(v.position.x)+direction.x);v.position.y=float(double(v.position.y)+direction.y);v.position.z=p.z=0;
        }
        phase_x=normalize_angle(float(double(phase_x)+0.09817477315664291));
        phase_y=normalize_angle(float(double(phase_y)-0.04908738657832146));
    }
    e.deformation_phase_x=normalize_angle(float(double(e.deformation_phase_x)+0.19634954631328583));
    e.deformation_phase_y=normalize_angle(float(double(e.deformation_phase_y)+0.09817477315664291));
#endif
}
}
