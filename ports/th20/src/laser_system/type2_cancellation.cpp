#include "type2.hpp"
#include "../ecl_vm/math.hpp"
#include "../damage_regions/geometry.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../program_entry/program_entry.hpp"
#include "../runtime_state/state.hpp"
#include <cmath>
#include <cstring>
namespace th20::source::laser {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
void effect(Type2Laser& l,const sprite::Vec3& p){auto& owner=*static_cast<bullet::Controller*>(l.context->primary_owner);std::uint32_t handle=0;sprite::spawn_named_animation(*program_entry::sprite_controller,*owner.file,handle,"bullet",n::signed_bits(l.parameters.color*2+0xd4),&p,0,-1,0);}
template<class Test>int cancel(Type2Laser& l,int check,bool heap_mask,Test test){
    if(check&&l.field_6cc)return 0;std::uint8_t fixed[512]{};auto* mask=heap_mask?static_cast<std::uint8_t*>(runtime::allocate_bytes(l.parameters.count)):fixed;if(!mask)return 0;if(heap_mask)std::memset(mask,0,l.parameters.count);int count=0,index=0;
    for(;index<n::signed_bits(l.parameters.count);++index){const auto& p=l.samples[index].position;if(test(p)){mask[index]=1;++count;++static_cast<bullet::Controller*>(l.context->primary_owner)->cancel_counter;if(index%20==0)effect(l,p);}}
    if(count<n::signed_bits(l.parameters.count)){if(count>0)l.split(mask,index);}else l.flags=(l.flags&~6u)|2u;
    if(heap_mask)runtime::release_bytes(mask);return count;
}
}
bool curve_rectangle_point(const sprite::Vec3& c,const sprite::Vec3& s,float rotation,const sprite::Vec3& p){float x=sub(p.x,c.x),y=sub(p.y,c.y);if(rotation!=0)m::rotate(x,y,-rotation);return div(s.x,2)>=std::fabs(x)&&div(s.y,2)>=std::fabs(y);} //458e90
int Type2Laser::cancel_rectangle(const sprite::Vec3& c,const sprite::Vec3& s,float rotation,int,int check){return cancel(*this,check,true,[&](const auto& p){return curve_rectangle_point(c,s,rotation,p);});}
int Type2Laser::cancel_circle(const sprite::Vec3& c,float radius,int,int check){const float squared=n::mul32(radius,radius);return cancel(*this,check,false,[&](const auto& p){const float x=sub(c.x,p.x),y=sub(c.y,p.y);return !(n::add32(n::mul32(x,x),n::mul32(y,y))>squared);});}
int Type2Laser::cancel_polygon(const sprite::Vec3& c,float radius,float rotation,int sides,int,int check){return cancel(*this,check,false,[&](const auto& p){return geometry::polygon_point(p.x,p.y,c.x,c.y,radius,rotation,sides);});}
int Type2Laser::cancel_ellipse(const sprite::Vec3& c,float rx,float ry,float rotation,int,int check){return cancel(*this,check,false,[&](const auto& p){return geometry::ellipse_point(p.x,p.y,c.x,c.y,rx,ry,rotation);});}
int Type2Laser::cancel_star(const sprite::Vec3& c,float outer,float inner,float rotation,int sides,int,int check){return cancel(*this,check,false,[&](const auto& p){return geometry::star_point(p.x,p.y,c.x,c.y,outer,inner,rotation,sides);});}
int Type2Laser::erase(int,int check){if(check&&field_6cc)return 0;for(std::int32_t i=0;i<n::signed_bits(parameters.count);++i)if(i%3==0)effect(*this,samples[i].position);state=1;return 0;}
void Type2Laser::split(std::uint8_t* mask,int count){
    int index=0;while(index<n::signed_bits(parameters.count)&&mask[index])++index;
    if(index){for(int i=index,k=0;i<n::signed_bits(parameters.count);++i,++k)mask[k]=mask[i];if(field_1324)for(int i=index,k=0;i<n::signed_bits(parameters.count);++i,++k)samples[k]=samples[i];
        n::timer_add(timer_48,n::int_float(-index),state::timer_rate);parameters.count-=index;if(n::signed_bits(parameters.count)<4){flags=(flags&~6u)|2u;return;}index=0;}
    while(index<n::signed_bits(parameters.count)&&!mask[index])++index;const int retained=index;
    while(index<n::signed_bits(parameters.count)){
        while(index<n::signed_bits(parameters.count)&&mask[index])++index;const int start=index;if(index>=count)break;int length=0;while(index<n::signed_bits(parameters.count)&&!mask[index]){++index;++length;}
        if(!field_1324&&length>3){Type2Parameters p=parameters;p.path=&path;p.count=length;p.time=sub(timer_48.current_f,n::int_float(start));p.sound=-1;p.field_50=999;spawn_type2(*static_cast<Controller*>(context->objects_04[4]),p);}
    }
    if(retained<4)flags=(flags&~6u)|2u;else{parameters.count=retained;field_1320=9999;}
}
std::uint32_t spawn_type2(Controller& owner,const Type2Parameters& p){if(n::signed_bits(owner.count)>511)return 0;++owner.next_handle;if(n::signed_bits(owner.next_handle)<0x10000)owner.next_handle=0x10000;auto* l=create_type2();if(!l)return 0;l->handle=owner.next_handle;if(l->initialize(p)<0)return 0;owner.attach(*l);return owner.next_handle;}
}
