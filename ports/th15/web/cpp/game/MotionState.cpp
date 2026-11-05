#include "MotionState.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
namespace {
Vec2 polar(float angle,float radius)noexcept{return {float(std::cos(double(angle))*double(radius)),float(std::sin(double(angle))*double(radius))};}
float shortest(float angle,float reference)noexcept{float difference=float(angle-reference);if(difference>3.1415927410125732421875f)difference=float(angle-float(reference+6.283185482025146484375f));else if(float(reference-angle)>3.1415927410125732421875f)difference=float(angle-float(reference-6.283185482025146484375f));return normalize_angle(difference);}
float quantize(float position)noexcept{return float(float(std::floor(double(float(position*100.f))))/100.f);}
}
void MotionState::set_angle(float value)noexcept{angle=normalize_angle(value);}
void MotionState::integrate(float rate)noexcept{
    switch(flags&15){
        case 0:{const auto delta=polar(angle,float(speed*rate));velocity.x=delta.x;velocity.y=delta.y;break;}
        case 2:case 3:radius=float(float(angular_velocity*rate)+radius);angle=normalize_angle(float(float(speed*rate)+angle));break;
        case 4:{phase=normalize_angle(float(phase+float(angular_velocity*rate)));const auto delta=polar(axis_angle,float(speed*rate));velocity={delta.x,delta.y,0};break;}
        default:break;
    }
}
void MotionState::advance(float rate)noexcept{
    switch(flags&15){
        case 0:position={float(position.x+velocity.x),float(position.y+velocity.y),float(position.z+velocity.z)};break;
        case 2:{const auto delta=polar(angle,radius);position={float(delta.x+velocity.x),float(velocity.y+delta.y),float(velocity.z+0.f)};break;}
        case 3:{const auto delta=polar(normalize_angle(shortest(angle,axis_angle)),radius);const float x=float(axis_scale*delta.x),sine=float(std::sin(double(axis_angle))),cosine=float(std::cos(double(axis_angle)));const float rx=float(float(cosine*x)-float(sine*delta.y)),ry=float(float(cosine*delta.y)+float(sine*x));position={float(velocity.x+rx),float(velocity.y+ry),float(velocity.z+0.f)};break;}
        case 4:{const Vec3 old=position;origin={float(origin.x+velocity.x),float(origin.y+velocity.y),float(origin.z+velocity.z)};const float bearing=normalize_angle(float(axis_angle+1.57079637050628662109375f)),distance=float(float(float(std::sin(double(phase)))*radius)*rate);const auto delta=polar(bearing,distance);position={float(origin.x+delta.x),float(origin.y+delta.y),float(origin.z+0.f)};set_angle(float(std::atan2(double(float(position.y-old.y)),double(float(position.x-old.x)))));break;}
        default:break;
    }
    position.x=quantize(position.x);position.y=quantize(position.y);
}
}
