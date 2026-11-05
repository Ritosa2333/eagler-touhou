#include "TitleMenu.hpp"
namespace th10 {
void TitleMenu::initialize(TitleMenu** current) noexcept {std::memset(this,0,sizeof(*this));flags=2;*current=this;}
void TitleMenu::reset_timer(const float* rate) noexcept {if(!(timer_flags&1)){elapsed.rate=rate;timer_flags|=1;}elapsed.initialize(-1);}
void TitleMenu::set_screen(i32 value,const float* rate) noexcept {screen=value;phase=0;reset_timer(rate);}
void TitleMenu::set_phase(i32 value,const float* rate) noexcept {phase=value;reset_timer(rate);}
void TitleMenu::create_script(i32 script,TitleAnimationEnvironment& env){animation_ids[script]=env.create(*animations,script);}
void TitleMenu::dismiss_script(i32 script,TitleAnimationEnvironment& env){auto& id=animation_ids[script];auto* vm=env.registry->find_and_clear(id);if(!vm)return;env.registry->interrupt(id,1);id=0;}
void TitleMenu::signal_script(i32 script,i32 signal,TitleAnimationEnvironment& env){env.registry->interrupt(animation_ids[script],static_cast<std::int16_t>(signal));}
// 0x434a80. Menu motion uses the VM's script-position interpolation and does not
// assign the position itself until the animation is updated.
void interpolate_menu_position(AnmVm& vm,const Vec3& end,const Vec3& start,i32 duration,u8 mode,const Vec3& tangent,const float* rate) noexcept {
    auto& interpolation=vm.position_interpolation;interpolation.duration=duration;std::memcpy(interpolation.initial_tangent,&tangent,12);std::memcpy(interpolation.final_tangent,&tangent,12);interpolation.mode=static_cast<InterpolationMode>(mode);std::memcpy(interpolation.start,&start,12);std::memcpy(interpolation.end,&end,12);if(!(interpolation.flags&1)){interpolation.timer.rate=rate;interpolation.flags|=1;}interpolation.timer.initialize(-1);
}
}
