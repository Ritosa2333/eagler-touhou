#include "type0.hpp"
#include "../ecl_vm/math.hpp"
#include "../damage_regions/geometry.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../program_entry/program_entry.hpp"
namespace th20::source::laser {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
void add(sprite::Vec3& a,const sprite::Vec3& b){a.x=n::add32(a.x,b.x);a.y=n::add32(a.y,b.y);a.z=n::add32(a.z,b.z);}
sprite::Vec3 initial(const Laser& l,sprite::Vec3& step){m::polar(step.x,step.y,l.angle,8);step.z=0;sprite::Vec3 p=l.position;add(p,step);add(step,step);return p;}
void effect(Type0Laser& l,const sprite::Vec3& p,int middle_limit){
    const int type=n::signed_bits(l.field_6e4);int script;if(type<18||type==34||type==38)script=n::signed_bits(l.parameters.color*2+0xd4);else if(type<middle_limit)script=n::signed_bits(l.parameters.color*2+0x104);else if(type<34)script=n::signed_bits(l.parameters.color*2+0x11c);else return;
    auto& owner=*static_cast<bullet::Controller*>(l.context->primary_owner);std::uint32_t handle=0;sprite::spawn_named_animation(*program_entry::sprite_controller,*owner.file,handle,"bullet",script,&p,0,-1,0);
}
template<class Test>int cancel(Type0Laser& l,int check,bool circle,Test test,int middle_limit=31){
    if(check&&l.field_6cc)return 0;std::uint8_t mask[512]{};int count=0,index=0;sprite::Vec3 step{},p=initial(l,step);p.z=0;
    for(float distance=8;n::add32(8,distance)<=l.field_74;distance=n::add32(distance,16),++index){if(test(p,step)){mask[index]=1;++count;++static_cast<bullet::Controller*>(l.context->primary_owner)->cancel_counter;if(!circle||(count-1)%4==0)effect(l,p,circle?32:middle_limit);}add(p,step);}
    if(count){if(count<index)l.split(mask,index);else l.flags=(l.flags&~6u)|2u;}return count;
}
}
int Type0Laser::cancel_rectangle(const sprite::Vec3& c,const sprite::Vec3& s,float rotation,int,int check){return cancel(*this,check,false,[&](const auto& p,const auto& step){return geometry::rectangle_rectangle(c.x,c.y,s.x,s.y,rotation,p.x,p.y,step.x,step.y,angle);});}
int Type0Laser::cancel_circle(const sprite::Vec3& c,float radius,int,int check){const float squared=n::mul32(radius,radius);return cancel(*this,check,true,[&](const auto& p,const auto&){const float x=sub(c.x,p.x),y=sub(c.y,p.y);return !(n::add32(n::mul32(x,x),n::mul32(y,y))>squared);});}
int Type0Laser::cancel_polygon(const sprite::Vec3& c,float radius,float rotation,int sides,int,int check){return cancel(*this,check,false,[&](const auto& p,const auto& step){return geometry::rectangle_polygon(p.x,p.y,step.x,step.y,angle,c.x,c.y,radius,rotation,sides);},32);}
int Type0Laser::cancel_ellipse(const sprite::Vec3& c,float rx,float ry,float rotation,int,int check){return cancel(*this,check,false,[&](const auto& p,const auto& step){return geometry::rectangle_ellipse(p.x,p.y,step.x,step.y,angle,c.x,c.y,rx,ry,rotation);},32);}
int Type0Laser::cancel_star(const sprite::Vec3& c,float outer,float inner,float rotation,int sides,int,int check){return cancel(*this,check,false,[&](const auto& p,const auto& step){return geometry::rectangle_star(p.x,p.y,step.x,step.y,angle,c.x,c.y,n::mul32(outer,outer),inner,rotation,sides);},32);}
int Type0Laser::erase(int,int check){if(check&&field_6cc)return 0;sprite::Vec3 step{},p=initial(*this,step);int count=0;for(float distance=8;n::add32(8,distance)<field_74;distance=n::add32(distance,16)){++count;effect(*this,p,32);add(p,step);}state=1;return count;}
void Type0Laser::split(const std::uint8_t* mask,int count){
    const auto old_position=position;int index=0;sprite::Vec3 step{};m::polar(step.x,step.y,angle,8);step.z=0;add(step,step);
    while(index<count&&mask[index])++index;
    if(index){add(position,{n::mul32(step.x,n::int_float(index)),n::mul32(step.y,n::int_float(index)),n::mul32(step.z,n::int_float(index))});field_74=sub(field_74,n::mul32(n::int_float(index),16));if(!(field_74>24)){flags=(flags&~6u)|2u;return;}parameters.length=field_74;field_80=n::mul32(n::int_float(index),16);}
    int length=0;while(index<count&&!mask[index]){++index;++length;}if(index>=count)return;
    parameters.length=sub(parameters.length,sub(field_74,n::mul32(n::int_float(length),16)));field_74=n::mul32(n::int_float(length),16);if(field_74<24)flags=(flags&~6u)|2u;
    while(index<count){while(index<count&&mask[index])++index;if(index>=count)break;const int start=index;length=0;while(index<count&&!mask[index]){++index;++length;}
        Type0Parameters p=parameters;p.length=p.field_14=n::mul32(n::int_float(length),16);if(!(p.length>24))continue;
        p.position={n::add32(old_position.x,n::mul32(step.x,n::int_float(start))),n::add32(old_position.y,n::mul32(step.y,n::int_float(start))),n::add32(old_position.z,n::mul32(step.z,n::int_float(start)))};spawn_type0(*static_cast<Controller*>(context->objects_04[4]),p);
    }
}
}
