#include "stone_selection.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include <bit>
#include <cstring>
#include <new>
namespace th20::source::effects {
namespace n=recovered;namespace s=sprite;namespace e=stone_selection_environment;namespace m=ecl::math;
namespace {
std::uint32_t spawn(int script){std::uint32_t handle;const s::Vec3 zero{};s::spawn_named_animation(environment::sprites(),e::file(),handle,"stone",script,&zero,0,-1,4);return handle;}
void signal(std::uint32_t handle,int event){s::interrupt_animation_children(environment::sprites(),handle,event);}
void execute(std::uint32_t handle,int event){s::execute_animation_interrupt(environment::sprites(),handle,event);}
void fade(s::Animation& animation,std::int32_t target){auto& p=animation.base.interpolation_134;p.duration=20;p.mode=0;p.start=animation.base.field_490>>24;p.end=target;p.current=p.start;n::timer_set(p.timer,0);}
}
StoneSelection::StoneSelection(s::Animation& a):AttachedCallback(a),basis{},points{},colors{},draw_colors{},handles{},center_handle(0),background_handle(0),field_140(0),radius(0),color(0),age{},remaining{},selection(0),expanded(0),pulses{},buttons{}{}
int StoneSelection::initialize(const Parameters& p,int){
    float angle=m::wrap_angle(std::bit_cast<float>(p.value_18));radius=std::bit_cast<float>(p.value_24);
    for(unsigned i=0;i<4;++i){basis[i*3]={};m::polar(basis[i*3+1].x,basis[i*3+1].y,angle,radius);angle=m::wrap_angle(n::add32(angle,1.5707963705062866f));m::polar(basis[i*3+2].x,basis[i*3+2].y,angle,radius);}
    constexpr std::uint32_t base_colors[]={0xff202020u,0x80202020u,0x80202020u,0xff808080u,0x80808080u,0x80808080u,0xfff0f0f0u,0x80f0f0f0u,0x80f0f0f0u,0xff808080u,0x80808080u,0x80808080u};std::memcpy(colors,base_colors,sizeof(colors));
    color=p.value_20;s::set_animation_color(*animation,color);animation->vector_5bc=p.vector_00;s::set_animation_layer(*animation,30);animation->base.flags[6]=(animation->base.flags[6]&~2u)|2u;
    for(unsigned i=0;i<4;++i){handles[i]=spawn(1);execute(handles[i],e::stone(i)+7);}center_handle=spawn(15);background_handle=spawn(0);
    n::timer_set(age,0);n::timer_set(remaining,-1);selection=0;return 0;
}
std::int32_t StoneSelection::update(){
    if(!e::updates_enabled())return 0;
    n::timer_tick(age,state::timer_rate);if(remaining.current>0){n::timer_add(remaining,-1.f,state::timer_rate);if(remaining.current<=0)return 1;}
    for(unsigned i=0;i<4;++i)buttons[i].pressed=e::button(i);
    for(unsigned i=0;i<4;++i){auto& b=buttons[i];if(!b.pressed){if(b.remaining>=0){if(b.remaining==0)execute(handles[i],28);b.remaining=n::signed_bits(static_cast<std::uint32_t>(b.remaining)-1u);}}else if(b.remaining<0){execute(handles[i],27);b.remaining=10;}}
    for(auto& pulse:pulses)if(pulse.enabled){pulse.radius=n::add32(pulse.radius,.019999999552965164f);if(pulse.radius>=1.2999999523162842f)pulse.radius=0;}
    return 0;
}
void StoneSelection::retire(){auto& c=environment::sprites();for(auto& h:handles)s::request_animation_deletion(c,h);s::request_animation_deletion(c,center_handle);s::request_animation_deletion(c,background_handle);}
void StoneSelection::interrupt(std::int32_t value){
    signal(background_handle,value);
    switch(value){
    case 1:fade(*animation,0);n::timer_set(remaining,20);for(auto h:handles)signal(h,value);signal(center_handle,value);break;
    case 2:for(unsigned i=0;i<4;++i)execute(handles[i],e::selected_profile(i)+7);fade(*animation,255);for(auto h:handles)signal(h,value);break;
    case 3:fade(*animation,96);for(auto h:handles)signal(h,value);break;
    case 6:for(auto h:handles)signal(h,value);break;
    case 7:case 12:for(auto h:handles)signal(h,3);signal(center_handle,2);selection=0;break;
    case 8:case 9:case 10:case 11:case 13:case 14:case 15:case 16:{const unsigned index=value>=13?value-13:value-8;if(value>=13)execute(handles[index],e::selected_profile(index)+7);for(unsigned i=0;i<4;++i)signal(handles[i],i==index?2:3);signal(center_handle,3);selection=index+1;break;}
    case 17:{expanded=1;auto& c=environment::sprites();s::request_animation_deletion(c,center_handle);s::request_animation_deletion(c,background_handle);background_handle=spawn(4);for(auto& h:handles){s::request_animation_deletion(c,h);h=spawn(5);}for(unsigned i=0;i<4;++i){const bool alt=e::alternate(i);execute(handles[i],e::stone(i)+(alt?17:7));}break;}
    case 27:signal(center_handle,6);break;
    }
}
namespace unrecovered {
void __cdecl initialize_12(s::Animation* a,const void* p,int view){void* memory=::operator new(sizeof(StoneSelection),std::nothrow);if(!memory)throw std::bad_alloc();std::memset(memory,0,sizeof(StoneSelection));auto* value=new(memory)StoneSelection(*a);value->initialize(*static_cast<const Parameters*>(p),view);}
}
}
