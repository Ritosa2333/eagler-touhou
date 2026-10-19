#include "laser.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../ecl_vm/math.hpp"
#include <cstring>
namespace th20::source::laser {
Laser::Laser() noexcept {
    //4c85b0 clears even the original vptr. Its derived constructors immediately
    //restore theirs. Independent C++ retains its own vptr throughout lifetime.
    std::memset(reinterpret_cast<std::uint8_t*>(this)+4,0,sizeof(Laser)-4);recovered::timer_set(age,0);
}
Laser::~Laser(){if(allocated_6d8){runtime::release_bytes(allocated_6d8);allocated_6d8=nullptr;}}
void Laser::select_context(std::int32_t index) noexcept {view_index=index;context=&game_session::context(index);}
void Laser::set_position(const sprite::Vec3& p) noexcept {position=p;}
void Laser::point_at_distance(float distance,sprite::Vec3& p) const {
    ecl::math::polar(p.x,p.y,angle,distance);p.x=recovered::add32(p.x,position.x);p.y=recovered::add32(p.y,position.y);p.z=recovered::add32(p.z,position.z);
}
int Laser::proximity(const sprite::Vec3& p,float radius) const {
    const auto sub=[](float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));};
    const auto div=[](float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));};
    float x=sub(p.x,position.x),y=sub(p.y,position.y);ecl::math::rotate(x,y,-angle);
    if(sub(x,radius)>field_74||sub(y,radius)>div(speed,2)||0>recovered::add32(x,radius)||div(-speed,2)>recovered::add32(y,radius))return 0;
    return 2; //All four original JA rejection branches preserve unordered inputs.
}
}
