#include "CharacterCapture.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th09 {
namespace {
Vec2 polar(float angle,float radius){return {float(std::cos(double(angle))*double(radius)),float(std::sin(double(angle))*double(radius))};}
float square_distance(const Vec2& a,const Vec2& b){const float x=a.x-b.x,y=a.y-b.y;return x*x+y*y;}
bool oriented_edge(const Vec2& a,const Vec2& b,const Vec2& p){
    // Preserve the original three-product addition order at star boundaries.
    const float value=((a.y-b.y)*p.x+(b.y-p.y)*a.x)+(p.y-a.y)*b.x;return !(value<0);
}
bool triangle(const Vec2& a,const Vec2& b,const Vec2& c,const Vec2& p){return oriented_edge(a,b,p)&&oriented_edge(b,c,p)&&oriented_edge(c,a,p);}
}
bool CharacterCapture::contains(i32 character,const Vec3& player,const Vec3& target,const CaptureArea& area)noexcept{
    if(character<0||character>=16)return false;
    const float dx=player.x-target.x,dy=player.y-target.y,r=area.radius,rr=r*r;
    switch(character){
    case 1:{
        const float bottom=player.y+32.f;if(!(bottom>target.y))return false;
        const float y=target.y-(bottom-448.f);float width=((200704.f-y*y)*area.spread)*area.spread;if(width<196.f)width=196.f;return dx*dx<=width;
    }
    case 2:case 10:{
        if(!(dx*dx+dy*dy<rr))return false;const float x=target.x-player.x,y=target.y-player.y;
        const double direction=x==0&&y==0?double(1.5707963705062866f):std::atan2(double(y),double(x));
        float difference=float(direction-double(area.angle));if(std::fabs(difference)>3.1415927410125732f)difference+=difference<=0?6.2831854820251465f:-6.2831854820251465f;
        return std::fabs(difference)<=area.spread*.5f;
    }
    case 4:case 8:{
        const float angle=float(add_angle(area.angle,1.5707963705062866f));const Vec2 offset=polar(angle,r*(character==4?.5f:.8660253882408142f));
        const float dot=offset.x*(target.x-player.x)+offset.y*(target.y-player.y);
        const Vec2 center=dot<=0?Vec2{player.x+offset.x,player.y+offset.y}:Vec2{player.x-offset.x,player.y-offset.y};return square_distance(center,{target.x,target.y})<rr;
    }
    case 6:return dx*dx<rr;
    case 7:{const float y=dy-r*1.5f;return y*y+dx*dx<rr;}
    case 9:{
        const float distance=dx*dx+dy*dy;if(distance<rr)return true;const float outer=r+24.f;if(distance>outer*outer)return false;
        static constexpr float offsets[]={0,1.0471975803375244f,2.094395160675049f,3.1415927410125732f,4.188790321350098f,5.235987663269043f};
        for(u32 n=0;n<6;++n){const auto p=polar(n?float(add_angle(area.angle,offsets[n])):area.angle,outer);const Vec2 center{p.x+player.x,p.y+player.y};if(square_distance(center,{target.x,target.y})<576.f)return true;}return false;
    }
    case 11:{
        Vec2 vertices[5],inner[5];float angle=area.angle;
        for(u32 n=0;n<5;++n){vertices[n]=polar(angle,r);if(n<4)angle=float(add_angle(angle,1.2566370964050293f));}
        for(u32 n=0;n<5;++n){const auto& a=vertices[n];const auto& b=vertices[(n+2)%5];inner[n]={(a.x-b.x)/1.618f+b.x,(a.y-b.y)/1.618f+b.y};}
        const Vec2 p{target.x-player.x,target.y-player.y};return triangle(vertices[0],vertices[2],inner[3],p)||triangle(vertices[4],vertices[1],inner[2],p)||triangle(vertices[3],vertices[0],inner[1],p);
    }
    case 14:return dy*dy<rr;
    case 15:return dx*dx<rr||dy*dy<rr;
    default:return dx*dx+dy*dy<rr;
    }
}
bool CharacterCapture::update_motion(EclVm& enemy,i32 character,const FrameTiming& timing)noexcept{
    if(character<0||character>=16||!enemy.values.world)return false;auto& m=enemy.movement;
    if(character==2)m.capture_velocity={};
    else {
        m.capture_velocity.x=float(enemy.values.world->random.signed_unit())*.1f;
        m.capture_velocity.y=m.capture_time.current>120?(character==4?-1.5f:-1.f):m.capture_time.time*(character==4?-.0125f:-.008333334f);m.capture_velocity.z=0;
    }
    m.capture_time.tick(timing);return true;
}
}
