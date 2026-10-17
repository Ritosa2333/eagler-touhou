#include "EnemyDraw.hpp"
#include <algorithm>
#include <cmath>
namespace th09 {
namespace {
Vec3 add(const Vec3& a,const Vec3& b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
void rotation(AnmVm& vm,float angle){vm.rotation.z=angle;vm.updateRotation=1;}
float middle_angle(float a,float b){if(b<=a)std::swap(a,b);const float direct=b-a,wrapped=(a+6.2831854820251465f)-b;return std::min(direct,wrapped)*.5f+a;}
}
void EnemyDraw::trail(EclVm& e,EnemyDrawServices& s){
    auto& trail=e.trail;auto& vm=e.animation.layers[0];if(trail.interval<=0||trail.length<=0||u32(trail.length)>trail.history.size())return;
    const Vec2 scale=vm.scale;const auto color=vm.color1;
    if(!(trail.flags&8)){
        for(i32 n=trail.length-1;n>0;n-=trail.interval){const auto& point=trail.history[n];if(point.position.x<-990.f)continue;
            if(e.behavior_flags&0x400000)rotation(vm,point.direction);
            if(trail.flags&2)vm.scale.x=scale.x-(float(n)*scale.x)/float(trail.length);
            if(trail.flags&4)vm.color1.a=u8(u32(color.a)-u32(color.a)*u32(n)/u32(trail.length));
            vm.pos=s.geometry.to_screen(add(point.position,vm.pos2));vm.pos.z=.3f;s.draw_animation(vm);
        }
    }else if(vm.loadedSprite){
        i32 count=0;for(i32 n=0;n<trail.length;n+=trail.interval){if(trail.history[n].position.x<-990.f)break;count+=2;}
        if(count>2){
            const auto& sprite=*vm.loadedSprite;const float step=(sprite.uvEnd.x-sprite.uvStart.x)/float((count+1)/2-1);float u=vm.uvScrollPos.x+sprite.uvEnd.x,previous_angle=0;u32 written=0;
            for(i32 n=0;n<trail.length;n+=trail.interval){const auto& p=trail.history[n];if(p.position.x<-990.f)break;const float angle=n?middle_angle(trail.history[n-1].direction,p.direction):p.direction;
                bool omit=false;if((trail.flags&2)&&n>0&&n+trail.interval<trail.length&&std::fabs(previous_angle-angle)<.00001f){const float next=middle_angle(trail.history[n+trail.interval-1].direction,trail.history[trail.interval].direction);omit=std::fabs(angle-next)<.00001f;}
                if(!omit){
                    previous_angle=angle;const float sine=float(std::sin(double(angle)));const double cosine=std::cos(double(angle));float half=scale.y*sprite.heightPx*.5f,zero=0;
                    if(trail.flags&2){const float fraction=1.f-float(n)/float(trail.length);zero=0.f*fraction;half*=fraction;}
                    auto& a=trail.vertices[written++];auto& b=trail.vertices[written++];a.color=b.color=u32(color.d3dColor);
                    if(trail.flags&4){const u32 alpha=u32(color.a)-u32(color.a)*u32(n)/u32(trail.length);a.color=b.color=(a.color&0xffffff)|(alpha<<24);}
                    // The original strip uses its fixed 32/16 origin, including
                    // on the right field. Sprite trails use field coordinates.
                    const float x=float(double(zero)*cosine),y=half*sine,dy=float(double(half)*cosine),dx=zero*sine;
                    a.position={(x-y+p.position.x)+32.f,(dx+dy+p.position.y)+16.f,p.position.z};b.position={(y+x+p.position.x)+32.f,(dx-dy+p.position.y)+16.f,p.position.z};
                    a.uv={u,sprite.uvStart.y+vm.uvScrollPos.y};b.uv={u,sprite.uvEnd.y+vm.uvScrollPos.y};
                }
                u-=step;
            }
            if(written>2)s.draw_strip(vm,trail.vertices.data(),written);
        }
    }
    vm.scale=scale;vm.color1=color;
}
void EnemyDraw::enemy(EclVm& e,EnemyDrawServices& s){
    auto& body=e.animation.layers[0];auto& lower=e.animation.layers[1];auto& upper=e.animation.layers[2];const auto p=e.values.resolved_position;
    if(lower.scriptIndex>=0){if(lower.type)rotation(lower,e.values.direction);lower.pos=s.geometry.to_screen(add(p,(e.values.flags&0x20)?body.pos2:lower.pos2));lower.pos.z=.3f;s.draw_animation(lower);}
    if(e.behavior_flags&0x400000)rotation(body,e.values.direction);body.pos=s.geometry.to_screen(add(p,body.pos2));body.pos.z=.25f;
    if(e.trail.flags)trail(e,s);if(!(e.trail.flags&0x10)&&!(e.behavior_flags&0x20))s.draw_animation(body);
    if(upper.scriptIndex>=0){if(upper.type)rotation(upper,-e.values.direction);upper.pos=s.geometry.to_screen(add(p,(e.values.flags&0x20)?body.pos2:upper.pos2));upper.pos.z=.3f;s.draw_animation(upper);}
}
}
