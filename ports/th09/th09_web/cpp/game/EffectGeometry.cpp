#include "EffectManager.hpp"
#include "GameMath.hpp"
#include <algorithm>
#include <cmath>
namespace th09 {
namespace {
constexpr float pi=3.1415927410125732f,half_pi=1.5707963705062866f,tau=6.2831854820251465f;
Vec3 polar(float angle,float radius){return {float(std::cos(double(angle))*double(radius)),float(std::sin(double(angle))*double(radius)),0};}
Vec3 plus(Vec3 a,Vec3 b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
Vec3 screen(const PlayfieldGeometry& g,const Vec3& p){auto result=g.to_screen(p);result.z=p.z;return result;}
AttackColorVertex vertex(const PlayfieldGeometry& g,Vec3 p,u32 color){return {screen(g,p),1,color};}
void circle(AttackColorVertex* vertices,const PlayfieldGeometry& g,const Vec3& center,float radius,i32 count,float angle,u32 middle=0x40600040,u32 edge=0x70600040){
    vertices[0]=vertex(g,center,middle);const float step=tau/float(count-1);
    for(i32 i=0;i<count;++i){if(angle>=pi)angle-=tau;vertices[i+1]=vertex(g,plus(polar(angle,radius),center),edge);angle+=step;}
}
void stripe(AttackColorVertex* v,const PlayfieldGeometry& g,const Vec3& p,float radius,bool horizontal){
    for(u32 j=0;j<3;++j)for(u32 k=0;k<2;++k){Vec3 pos=p;
        if(horizontal){pos.x=k?144.f:-144.f;pos.y=j==0?p.y-radius:j==1?p.y:radius+p.y;}
        else{pos.x=j==0?p.x-radius:j==1?p.x:radius+p.x;pos.y=k?448.f:0.f;}
        v[j*2+k]=vertex(g,pos,j==1?0x30600040:0x70600040);
    }
}
void ring_uv(EffectActor& a){
    if(!a.animation->loadedSprite||a.segments<1)return;const auto& vm=*a.animation;const auto& sprite=*vm.loadedSprite;
    const float start=vm.uvScrollPos.y+sprite.uvEnd.y,delta=(sprite.uvEnd.y-sprite.uvStart.y)/float(a.segments);float value=start;
    for(i32 i=0;i<=a.segments;++i){auto& v=a.texture[i*2];v.uv={sprite.uvStart.x+vm.uvScrollPos.x,value};v.color=u32(vm.color1.d3dColor);v.reciprocal_w=1;value-=delta;}
    value=start;for(i32 i=0;i<=a.segments;++i){auto& v=a.texture[i*2+1];v.uv={sprite.uvEnd.x+vm.uvScrollPos.x,value};v.color=u32(vm.color1.d3dColor);v.reciprocal_w=1;value-=delta;}
}
void ring(EffectActor& a,EffectServices& s){
    if(a.segments<1||a.segments>128)return;const u32 count=u32(a.segments*2+2);a.texture.resize(std::max<u32>(count,a.texture.size()));
    if(a.dirty){
        const float step=tau/float(a.segments),angle=(pi-step)*.5f;const float thickness=float(double(a.thickness)/std::sin(double(angle))),outer=a.radius+thickness,inner=a.radius-thickness;
        ring_uv(a);float heading=a.distortion!=0&&a.cycle==0?0:a.angle,wave=a.rotation;const float wave_step=(a.cycle*tau)/float(a.segments);
        for(i32 i=0;i<=a.segments;++i){
            if(a.distortion==0){if(heading>=pi)heading-=tau;a.texture[i*2].position=screen(s.geometry[a.side],plus(polar(heading,outer),a.anchor));a.texture[i*2+1].position=screen(s.geometry[a.side],plus(polar(heading,inner),a.anchor));}
            else if(a.cycle==0){
                const float radii[2]={outer,inner},vertical[2]={a.distortion+thickness,a.distortion-thickness};for(u32 j=0;j<2;++j){Vec2 point{float(std::cos(double(heading))*double(radii[j])),float(std::sin(double(heading))*double(vertical[j]))},rotated;rotate(rotated,point,a.rotation);auto p=screen(s.geometry[a.side],plus({rotated.x,rotated.y,0},a.anchor));p.z=0;a.texture[i*2+j].position=p;}
            }else{
                if(heading>=pi)heading-=tau;if(wave>=pi)wave-=tau;const float ripple=float(std::cos(double(wave))*double(a.distortion));for(u32 j=0;j<2;++j){auto p=screen(s.geometry[a.side],plus(polar(heading,ripple+(j?inner:outer)),a.anchor));p.z=0;a.texture[i*2+j].position=p;}wave+=wave_step;
            }
            heading+=step;
        }a.dirty=0;
    }s.draw_texture_strip(*a.animation,a.texture.data(),count);
}
void burst(EffectActor& a,EffectServices& s){
    auto& b=*a.burst;const auto& g=s.geometry[a.side];const u8 alpha=b.phase?u8(255-a.time.current*255/50):255;const u32 color=u32(alpha)<<24;
    b.fan[0]=vertex(g,a.position,(u32(alpha)<<22)|0xff0000);float angle=-pi;constexpr float step=.20268340408802032f;
    for(u32 i=0;i<32;++i){if(angle>=pi)angle-=tau;b.fan[i+1]=vertex(g,plus(polar(angle,b.radius),a.position),color|0xff0000);angle+=step;}
    s.draw_color_fan(*a.animation,b.fan.data(),33);
    for(u32 r=0;r<(b.phase?4u:1u);++r){for(u32 i=0;i<32;++i){if(angle>=pi)angle-=tau;b.rings[r][i]=vertex(g,plus(polar(angle,b.jitter[r][i]+b.radius),a.position),color|0xffffff);angle+=step;}s.draw_additive_lines(b.rings[r].data(),32);angle+=.05067085102200508f;}
}
}
bool draw_effect(EffectActor& a,EffectServices& s){
    const auto k=a.kind;auto& v=a.colors;const auto& g=s.geometry[a.side];
    if(k==EffectKind::ring||k==EffectKind::ring_alternate||(k>=EffectKind::protection_one&&k<=EffectKind::protection_three)){ring(a,s);return true;}
    if(k==EffectKind::reisen_burst){burst(a,s);return true;}
    if(k==EffectKind::lyrica_shots)return false;
    if(k!=EffectKind::reimu_field&&!(k>=EffectKind::marisa_field&&k<=EffectKind::youmu_field)&&k<EffectKind::mystia_field)return false;
    const i32 n=a.segments;
    if(k==EffectKind::medicine_field){if(!a.dirty)return true;v.resize(std::max<u32>(v.size(),u32(n+1+(n/2+1)*6)));circle(v.data(),g,a.position,a.radius,n,-pi);s.draw_color_fan(*a.animation,v.data(),n+1);
        constexpr float offsets[]={0,1.0471975803375244f,2.094395160675049f,pi,4.188790321350098f,5.235987663269043f};u32 at=n+1;
        for(u32 i=0;i<6;++i){const float angle=i==0?a.angle:float(add_angle(a.angle,offsets[i]));const auto center=plus(polar(angle,a.radius+24),a.position);circle(v.data()+at,g,center,24,n/2,angle);s.draw_color_fan(*a.animation,v.data()+at,n/2+1);at+=n/2+1;}return true;
    }
    if(k==EffectKind::lyrica_field||k==EffectKind::merlin_field||k==EffectKind::lunasa_field){v.resize(k==EffectKind::lunasa_field?12:6);
        if(a.dirty){stripe(v.data(),g,a.position,a.radius,k==EffectKind::merlin_field);if(k==EffectKind::lunasa_field){s.draw_color_strip(*a.animation,v.data(),6);stripe(v.data()+6,g,a.position,a.radius,true);s.draw_color_strip(*a.animation,v.data()+6,6);return true;}}
        if(k!=EffectKind::lunasa_field)s.draw_color_strip(*a.animation,v.data(),6);return true;
    }
    if(a.dirty){v.resize(std::max<u32>(u32(n+1),v.size()));
        if(k==EffectKind::marisa_field){auto center=a.position;center.y-=432;v[0]=vertex(g,center,0x30600040);float angle=0;const float step=pi/float(n-1);
            for(i32 i=0;i<n;++i){auto p=polar(angle,448);p.x*=a.spread;p=plus(p,a.position);p.y-=432;v[i+1]=vertex(g,p,0x50600040);angle+=step;}
        }else if(k==EffectKind::sakuya_field||k==EffectKind::cirno_field){v[0]=vertex(g,a.position,0x30600040);float angle=a.angle-a.spread*.5f;const float step=a.spread/float(n-1);
            for(i32 i=0;i<n;++i){angle=float(add_angle(angle,0));v[i+1]=vertex(g,plus(polar(angle,a.radius),a.position),0x50600040);angle+=step;}
        }else if(k==EffectKind::reisen_field||k==EffectKind::aya_field){
            const bool aya=k==EffectKind::aya_field;const float sector=aya?1.0471975803375244f:2.094395160675049f,offset=aya?1.0471975803375244f:.5235987901687622f,step=sector/float(n/2-1);
            float angle=float(add_angle(a.angle,offset));const auto delta=polar(float(add_angle(a.angle,-half_pi)),a.radius*(aya?.8660253882408142f:.5f));v[0]=vertex(g,a.position,0x40600040);u32 at=1;
            for(u32 half=0;half<2;++half){const Vec3 center={half?a.position.x-delta.x:a.position.x+delta.x,half?a.position.y-delta.y:a.position.y+delta.y,0};for(i32 i=0;i<(n+1)/2;++i){if(angle>=pi)angle-=tau;auto p=plus(polar(angle,a.radius),center);p.z=0;v[at++]=vertex(g,p,0x70600040);angle+=step;}if(half==0)angle=float(add_angle(angle,(aya?2.094395160675049f:1.0471975803375244f)-step));}
        }else if(k==EffectKind::komachi_field){
            Vec3 points[10];float angle=a.angle;for(u32 i=0;i<5;++i){points[i*2]=polar(angle,a.radius);angle=float(add_angle(angle,1.2566370964050293f));}
            constexpr float inverse=1.f/1.618f;for(u32 i=0;i<5;++i){const auto& from=points[i*2];const auto& to=points[((i+2)%5)*2];points[i*2+1]={(from.x-to.x)*inverse+to.x,(from.y-to.y)*inverse+to.y,0};}
            v[0]=vertex(g,a.position,0x20600040);for(u32 i=0;i<10;++i)v[i+1]=vertex(g,plus(points[i],a.position),0x70600040);v[11]=vertex(g,plus(points[0],a.position),0x70600040);
        }else circle(v.data(),g,a.position,a.radius,n,-pi);
    }
    if(k!=EffectKind::yuuka_field||a.dirty)s.draw_color_fan(*a.animation,v.data(),k==EffectKind::komachi_field?12:n+1);return true;
}
}
