#include "Movement.hpp"
#include <cmath>
namespace th11 {
float normalize_angle(float value)noexcept{constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;u32 loops=0;while(value>pi){value=float(double(value)-tau);if(loops++>32)break;}while(value<-pi){value=float(double(value)+tau);if(loops++>32)break;}return value;}
Vec2 polar(float angle,float length)noexcept{return{float(std::cos(double(angle))*length),float(std::sin(double(angle))*length)};}
Vec2 rotate(Vec2 p,float angle)noexcept{const float c=float(std::cos(double(angle))),s=float(std::sin(double(angle)));return{float(double(c)*p.x-double(s)*p.y),float(double(s)*p.x+double(c)*p.y)};}
void Movement::update_velocity()noexcept{if(!(flags&1)){const auto p=polar(angle,speed);velocity={p.x,p.y,0};}else{radius=float(double(radial_velocity)+radius);angle=normalize_angle(float(double(speed)+angle));}}
// TH11 rounds the product to float *before* floor, then divides by 100.
// This differs from TH10's double product and float 0.01 multiplication.
void Movement::snap_position()noexcept{const auto snap=[](float p){const float scaled=float(double(p)*100),integer=float(std::floor(double(scaled)));return float(double(integer)/100);};position.x=snap(position.x);position.y=snap(position.y);}
void Movement::refresh_position()noexcept{
    if(flags&1){Vec2 p;
        if(flags&2){p=polar(normalize_angle(float(double(angle)-ellipse_angle)),radius);p.x=float(double(ellipse_ratio)*p.x);p=rotate(p,ellipse_angle);}
        else p=polar(angle,radius);
        position={float(double(velocity.x)+p.x),float(double(velocity.y)+p.y),float(double(velocity.z)+0.)};
    }
    snap_position();
}
void Movement::update()noexcept{if(!(flags&1)){position.x=float(double(velocity.x)+position.x);position.y=float(double(velocity.y)+position.y);position.z=float(double(velocity.z)+position.z);snap_position();}else refresh_position();}
}
