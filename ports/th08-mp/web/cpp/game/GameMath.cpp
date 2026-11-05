#include "GameMath.hpp"
#include <cmath>
namespace th08 {
float add_angle(float angle,float delta) noexcept {
    // TH08 0x43edb0 stores a float after every addition and each wrap. Both
    // directions share the original loop counter, limited after 18 iterations.
    angle=Scalar::add(angle,delta);i32 iterations=0;
    while(angle>3.1415927410125732f){angle=Scalar::sub(angle,6.283185482025147f);if(iterations++>16)break;}
    while(angle<-3.1415927410125732f){angle=Scalar::add(angle,6.283185482025147f);if(iterations++>16)break;}
    return angle;
}
Extended sine(float value) noexcept {return Extended::from_double(std::sin(double(value)));}
Extended cosine(float value) noexcept {return Extended::from_double(std::cos(double(value)));}
Extended tangent(float value) noexcept {return Extended::from_double(std::tan(double(value)));}
Extended arccosine(float value) noexcept {
    // The original CRT uses atan2(sqrt((1+x)*(1-x)),x). It retains the
    // caller's significand precision for the products and square root.
    if(value<-1||value>1)return Extended::from_double(std::acos(double(value)));
    const auto x=number(value),one=number(1);
    const auto height=((one+x)*(one-x)).square_root();
    return Extended::from_double(std::atan2(height.to_double(),double(value)));
}
Extended arctangent(float value) noexcept {return Extended::from_double(std::atan(double(value)));}
Extended float_remainder(float value,float divisor) noexcept {return Extended::from_double(std::fmod(double(value),double(divisor)));}
}
