#include "converging_particles.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include <bit>
#include <cmath>
#include <cstring>
#include <new>
#include <stdexcept>
namespace th20::source::effects {
namespace n=recovered;namespace s=sprite;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
s::Vec3 add(s::Vec3 a,s::Vec3 b){return {n::add32(a.x,b.x),n::add32(a.y,b.y),n::add32(a.z,b.z)};}
s::Vec3 difference(s::Vec3 a,s::Vec3 b){return {sub(a.x,b.x),sub(a.y,b.y),sub(a.z,b.z)};}
s::Vec3 unit_direction(s::Vec3 v){ //45cf10, unordered length takes division
    const float length=static_cast<float>(std::sqrt(static_cast<double>(n::add32(n::add32(n::mul32(v.x,v.x),n::mul32(v.y,v.y)),n::mul32(v.z,v.z)))));
    if(!(.009999999776482582f>std::fabs(length)))return {div(v.x,length),div(v.y,length),div(v.z,length)};return v;
}
s::Vec3 set_length(s::Vec3 v,float length){v=unit_direction(v);return {n::mul32(v.x,length),n::mul32(v.y,length),n::mul32(v.z,length)};} //4532c0
s::Vec3 polar(float angle,float length){s::Vec3 result{};m::polar(result.x,result.y,angle,length);return result;}
s::Vec3 random_offset(float radius){const float length=n::mul32(state::unit(state::random_streams[1]),radius);const float angle=n::mul32(state::signed_unit(state::random_streams[1]),3.1415927410125732f);return polar(angle,length);}
void curve(s::Animation& a,int duration,s::Vec3 start,s::Vec3 tangent_start,s::Vec3 end,s::Vec3 tangent_end){ //45d150 ->45d190
    auto& p=a.base.interpolation_8c;p.duration=duration;p.mode=8;p.start=start;p.end=end;p.tangent_start=tangent_start;p.tangent_end=tangent_end;p.current=start;n::timer_set(p.timer,0);
}
}
ConvergingParticles::ConvergingParticles(s::Animation& a):AttachedCallback(a),handles{},targets{},tangents{},stages{},near_center{},far_center{},center{},field_192c(0),age{}{}
int ConvergingParticles::initialize(const Parameters&,int){n::timer_set(age,0);return 0;}
std::int32_t ConvergingParticles::update(){
    auto& a=*animation;center=far_center=near_center=a.vector_5bc;
    far_center=add(far_center,polar(a.base.vector_38.z,300));near_center=add(near_center,polar(n::add32(a.base.vector_38.z,std::bit_cast<float>(a.base.fields_444[10])),150));
    auto& sprites=environment::sprites();
    if(age.current!=age.previous&&age.current<50){
        const int index=n::signed_bits(static_cast<std::uint32_t>(age.current)*4u);if(index<0)throw std::out_of_range("ConvergingParticles negative child index");
        for(int i=0;i<4;++i)handles[index+i]=s::spawn_named_animation(sprites,*controller(0)->files[0],"effect",i<3?149:150);
        auto color=a.base.field_490;
        for(int i=0;i<4;++i){if(i==3){for(unsigned byte=0;byte<3;++byte){const unsigned shift=byte*8;const auto channel=(300u-((color>>shift)&255u))&255u;color=(color&~(255u<<shift))|(channel<<shift);}}
            if(auto* child=s::resolve_animation_handle(sprites,handles[index+i])){s::set_animation_color(*child,color);child->base.fields_444[0]=a.base.fields_444[0];}
        }
    }
    int active=0;
    for(int i=0;i<200;++i){auto* child=s::resolve_animation_handle(sprites,handles[i]);if(!child)continue;
        child->fields_550[4]=std::bit_cast<std::uint32_t>(s::animation_slowdown(a));
        if(stages[i]==0){
            const auto start=add(far_center,random_offset(150));const auto end=add(near_center,random_offset(50));
            auto tangent_end=add(unit_direction(difference(end,start)),unit_direction(difference(center,end)));
            const float end_length=n::add32(n::mul32(state::unit(state::random_streams[1]),200),200);tangent_end=set_length(tangent_end,end_length);
            const auto delta=difference(end,start);const float start_length=n::add32(n::mul32(state::unit(state::random_streams[1]),100),100);const auto tangent_start=set_length(delta,start_length);
            curve(*child,n::signed_bits(a.base.fields_444[0]),start,tangent_start,end,tangent_end);targets[i]=end;tangents[i]=tangent_end;stages[i]=1;
        }else if(child->timer_4d8.current>=n::signed_bits(a.base.fields_444[0]+1u)&&stages[i]==1){
            const auto end=add(a.vector_5bc,random_offset(20));const auto tangent_end=random_offset(20);curve(*child,n::signed_bits(a.base.fields_444[0]),targets[i],tangents[i],end,tangent_end);stages[i]=2;
        }
        ++active;
    }
    if(!active)return -1;n::timer_tick(age,state::timer_rate);return 0;
}
void ConvergingParticles::retire(){auto& sprites=environment::sprites();for(auto& handle:handles)if(auto* child=s::resolve_animation_handle(sprites,handle)){child->base.fields_10_28[6]=0xffffffffu;child->base.flags[0]&=~0x10000u;}}
void ConvergingParticles::interrupt(std::int32_t value){if(value==1)n::timer_add(age,300.f,state::timer_rate);}
namespace unrecovered {
void __cdecl initialize_1(s::Animation* a,const void* p,int view){void* memory=::operator new(sizeof(ConvergingParticles),std::nothrow);if(!memory)throw std::bad_alloc();std::memset(memory,0,sizeof(ConvergingParticles));auto* value=new(memory)ConvergingParticles(*a);value->initialize(*static_cast<const Parameters*>(p),view);}
}
}
