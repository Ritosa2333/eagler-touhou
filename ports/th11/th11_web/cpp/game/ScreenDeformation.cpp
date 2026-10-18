#include "ScreenDeformation.hpp"
#include "EnemyAnimations.hpp"
#include "GraphicsMath.hpp"
#include <cmath>
namespace th11 {
ScreenDeformation::~ScreenDeformation(){EnemyAnimations helper(manager);for(auto id:animations)helper.erase(id);}
bool ScreenDeformation::initialize(AnmResource& text,u32 width,u32 height,bool second_target){
    if(width<2||height<3||width>128||height>128||!animations.empty())return false;
    columns=width;rows=height;vertices.resize(columns*rows);positions.resize(columns*rows);animations.resize(columns-1);
    for(auto& id:animations){
        auto* vm=manager.create(text,second_target?82:81,0,second_target?28:27,false,false);if(!vm)return false;id=vm->id;
        if(!manager.allocate_geometry(*vm,rows*2*sizeof(AnmVertex)))return false;
        vm->flags=(vm->flags&0xff3fffffu)|0x3000000;vm->integers[0]=rows;
        auto* strip=static_cast<AnmVertex*>(vm->geometry);
        for(u32 i=0;i<rows*2;++i){strip[i].position.z=0;strip[i].reciprocal_w=1;strip[i].color=0xffffffff;}
        vm->draw_callback=draw_callback;vm->reserved_418=u32(reinterpret_cast<uintptr_t>(this));vm->flags&=0xffffff8f;
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
    float px=float(double(x)+192+32);const float top=float(double(y)+16),dx=float(double(width)/(columns-1)),dy=float(double(height)/(rows-1));
    for(u32 col=0;col<columns;++col){float py=top;for(u32 row=0;row<rows;++row){const u32 n=col*rows+row;
        positions[n]={px,py,0};const float u=float(double(px)/640),v=float(double(py)/480);
        vertices[n]={{px,py,0},1,0xffffffff,{u<0?0:u,v<0?0:v}};py=float(double(py)+dy);
    }px=float(double(px)+dx);}copy_strips();
}
void ScreenDeformation::update(EnemyState& e,i32 spell_id){
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
}
}
