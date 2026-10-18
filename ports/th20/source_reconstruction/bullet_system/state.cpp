#include "bullet.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::bullet {
namespace n=recovered;
namespace {float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}}
Bullet::Bullet() noexcept {scheduler::initialize_link(link,reinterpret_cast<scheduler::Node*>(this));}
void assign_timer_float(recovered::Timer& timer,float value) noexcept {
    n::timer_initialize_if_needed(timer);timer.current=_mm_cvtt_ss2si(_mm_set_ss(value));timer.current_f=value;
    timer.previous=_mm_cvtt_ss2si(_mm_set_ss(sub(value,1)));
}
bool outside_viewport(const sprite::Vec3& p,float w,float h) noexcept {
    return n::add32(p.x,w)<=-192||sub(p.x,w)>=192||n::add32(p.y,h)<=0||sub(p.y,h)>=448;
}
bool in_circle(const sprite::Vec3& a,const sprite::Vec3& b,float radius) noexcept {
    const auto x=sub(a.x,b.x),y=sub(a.y,b.y);return n::add32(n::mul32(x,x),n::mul32(y,y))<=n::mul32(radius,radius);
}
Controller* controller(std::int32_t index) noexcept {return static_cast<Controller*>(game_session::context(index).primary_owner);}
void Controller::select_context(std::int32_t index) noexcept {view_index=index;context=&game_session::context(index);}
}
