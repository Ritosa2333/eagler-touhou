#include "GameMath.hpp"
#include <cmath>
namespace th10 {
// 0x44bc70. Both loops share a limit; there is no intermediate float store.
namespace {
Extended wrap_angle(Extended result) noexcept {
    const auto pi=number(3.1415927410125732421875f),tau=number(6.283185482025146484375f);
    i32 iterations=0;
    while(pi<result){result=result-tau;if(iterations++>32)break;}
    while(result<-pi){result=result+tau;if(iterations++>32)break;}
    return result;
}
}
Extended normalize_angle(float radians) noexcept {return wrap_angle(number(radians));}
Extended add_angle(float radians,float delta) noexcept {return wrap_angle(number(radians)+number(delta));}
// 0x408660 / 0x428ce0: a single wrap, including the original store boundaries.
Extended angle_difference(float target,float current) noexcept {
    const auto difference=number(target)-number(current);
    if(number(3.1415927410125732f)<difference)return number(target)-(number(current)+number(6.2831854820251465f));
    if(number(3.1415927410125732f)<number(current)-number(target))return number(target)-(number(current)-number(6.2831854820251465f));
    return difference;
}
// These use the same libm precision as the existing web backend. Exact native
// x87 transcendental rounding still requires an independent hardware oracle.
Extended sine(Extended radians) noexcept {return Extended::from_double(std::sin(radians.to_double()));}
Extended cosine(Extended radians) noexcept {return Extended::from_double(std::cos(radians.to_double()));}
Extended tangent(Extended radians) noexcept {return Extended::from_double(std::tan(radians.to_double()));}
Extended arccosine(Extended value) noexcept {return Extended::from_double(std::acos(value.to_double()));}
Extended remainder(Extended value,Extended divisor) noexcept {return Extended::from_double(std::fmod(value.to_double(),divisor.to_double()));}
Extended angle_to(Extended y,Extended x) noexcept {return Extended::from_double(std::atan2(y.to_double(),x.to_double()));}
// 0x4501b0.
Vec2 polar(float radians,float length) noexcept {
    return {(cosine(number(radians))*number(length)).to_float(),(sine(number(radians))*number(length)).to_float()};
}
}
