#include "Movement.hpp"
#include <cmath>
namespace th10 {
void Movement::update_velocity() noexcept {
    if(!(flags&1)){const auto next=polar(angle,speed);velocity={next.x,next.y,0};}
    else{radius=Scalar::add(radial_velocity,radius);angle=normalize_angle(Scalar::add(speed,angle)).to_float();}
}
// 0x44c2a0. The original snaps x/y downward, including negative coordinates.
// The multiply is extended precision, then explicitly spilled to double before
// floor, then multiplied by the original *float* approximation of 0.01.
void Movement::update() noexcept {
    if(flags&1){
        const auto offset=polar(angle,radius);
        position.x=Scalar::add(offset.x,velocity.x);
        position.y=Scalar::add(offset.y,velocity.y);
        position.z=velocity.z;
    }else{
        position.x=Scalar::add(position.x,velocity.x);
        position.y=Scalar::add(velocity.y,position.y);
        position.z=Scalar::add(velocity.z,position.z);
    }
    const auto snap=[](float value){
        return (Extended::from_double(std::floor((number(value)*number(100.0f)).to_double()))*number(0.01f)).to_float();
    };
    position.x=snap(position.x);position.y=snap(position.y);
}
}
