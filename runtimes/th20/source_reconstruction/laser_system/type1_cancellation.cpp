#include "type1.hpp"
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
sprite::Vec3 initial_sample(const Laser& l,sprite::Vec3& step){m::polar(step.x,step.y,l.angle,8);step.z=0;sprite::Vec3 p=l.position;add(p,step);add(step,step);return p;}
void cancel_effect(Type1Laser& l,const sprite::Vec3& p){
    const auto type=n::signed_bits(l.field_6e4);int script;
    if(type<18||type==34||type==38)script=n::signed_bits(l.parameters.color*2+0xd4);
    else if(type<32||type==27)script=n::signed_bits(l.parameters.color*2+0x104);
    else if(type<34)script=n::signed_bits(l.parameters.color*2+0x11c);else return;
    auto& owner=*static_cast<bullet::Controller*>(l.context->primary_owner);std::uint32_t animation_handle=0;
    sprite::spawn_named_animation(*program_entry::sprite_controller,*owner.file,animation_handle,"bullet",script,&p,0,-1,0);
}
template<class Test> int cancel_samples(Type1Laser& l,int check,bool inclusive,bool cull,Test test){
    if(check&&l.field_6cc)return 0;std::uint8_t mask[512]{};int count=0,index=0;sprite::Vec3 step{},p=initial_sample(l,step);p.z=0;
    for(float distance=8;inclusive?n::add32(8,distance)<=l.field_74:n::add32(8,distance)<l.field_74;distance=n::add32(distance,16),++index){
        if(test(p,step)){mask[index]=1;++count;++static_cast<bullet::Controller*>(l.context->primary_owner)->cancel_counter;if(!cull||!bullet::outside_viewport(p,32,32))cancel_effect(l,p);}add(p,step);
    }
    if(count)l.split(mask,index);return count;
}
}
int Type1Laser::cancel_rectangle(const sprite::Vec3& center,const sprite::Vec3& size,float rotation,int,int check){
    return cancel_samples(*this,check,true,false,[&](const auto& p,const auto& step){return geometry::rectangle_rectangle(center.x,center.y,size.x,size.y,rotation,p.x,p.y,step.x,step.y,angle);});
}
int Type1Laser::cancel_circle(const sprite::Vec3& center,float radius,int,int check){
    const float squared=n::mul32(radius,radius);
    return cancel_samples(*this,check,false,true,[&](const auto& p,const auto&){const float x=sub(center.x,p.x),y=sub(center.y,p.y);return !(n::add32(n::mul32(x,x),n::mul32(y,y))>squared);}); //JBE accepts unordered
}
int Type1Laser::cancel_polygon(const sprite::Vec3& c,float radius,float rotation,int sides,int,int check){
    return cancel_samples(*this,check,false,true,[&](const auto& p,const auto& step){return geometry::rectangle_polygon(p.x,p.y,step.x,step.y,angle,c.x,c.y,radius,rotation,sides);});
}
int Type1Laser::cancel_ellipse(const sprite::Vec3& c,float rx,float ry,float rotation,int,int check){
    return cancel_samples(*this,check,false,true,[&](const auto& p,const auto& step){return geometry::rectangle_ellipse(p.x,p.y,step.x,step.y,angle,c.x,c.y,rx,ry,rotation);});
}
int Type1Laser::cancel_star(const sprite::Vec3& c,float outer,float inner,float rotation,int sides,int,int check){
    return cancel_samples(*this,check,false,true,[&](const auto& p,const auto& step){return geometry::rectangle_star(p.x,p.y,step.x,step.y,angle,c.x,c.y,n::mul32(outer,outer),inner,rotation,sides);}); //original squares only outer
}
int Type1Laser::erase(int,int check){
    if(check&&field_6cc)return 0;sprite::Vec3 step{},p=initial_sample(*this,step);std::uint32_t count=0;
    for(float distance=8;n::add32(8,distance)<field_74;distance=n::add32(distance,16)){++count;if(!bullet::outside_viewport(p,16,16))cancel_effect(*this,p);add(p,step);}state=1;return n::signed_bits(count);
}
void Type1Laser::split(const std::uint8_t* mask,int count){
    int index=0,length=0;sprite::Vec3 step{};m::polar(step.x,step.y,angle,8);step.z=0;add(step,step);
    while(index<count&&mask[index])++index;
    if(!index){while(index<count&&!mask[index]){++index;++length;}if(index>=count)return;field_74=n::mul32(n::int_float(length),16);}else field_74=0;
    while(index<count){
        while(index<count&&mask[index])++index;if(index>=count)break;const int start=index;length=0;while(index<count&&!mask[index]){++index;++length;}
        Type0Parameters p;p.length=n::mul32(n::int_float(length),16);p.field_14=p.length;
        p.position={n::add32(position.x,n::mul32(step.x,n::int_float(start))),n::add32(position.y,n::mul32(step.y,n::int_float(start))),n::add32(position.z,n::mul32(step.z,n::int_float(start)))};
        p.speed=8;p.angle=angle;p.width=speed;p.type=parameters.type;p.color=parameters.color;p.length_limit=sub(parameters.length_limit,n::mul32(n::int_float(start),16));p.flags=(p.flags&~1u)|((parameters.flags>>1)&1u);p.radial_offset=0;p.field_48=p.field_4c=-1;p.view_index=view_index;
        spawn_type0(*static_cast<Controller*>(context->objects_04[4]),p);
    }
}
}
