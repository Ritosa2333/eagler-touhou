#include "type2.hpp"
#include "type1.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../bullet_system/style.hpp"
#include "../gameplay/enemy.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../ecl_vm/math.hpp"
#include <cstring>
#include <new>
namespace th20::source::laser {
Type2Laser::Type2Laser():field_131c(1.100000023841858f),field_1320(0),field_1324(0),samples(nullptr),geometry(nullptr){sprite::construct_animation(animation);sprite::construct_animation(origin_animation);}
Type2Laser::~Type2Laser(){finish();sprite::destroy_animation_contents(origin_animation);sprite::destroy_animation_contents(animation);}
int Type2Laser::finish(){
    auto* node=path.next;while(node){auto* next=node->next;runtime::release_bytes(node);node=next;}path.next=nullptr;
    if(geometry){runtime::release_bytes(geometry);geometry=nullptr;}if(samples){runtime::release_bytes(samples);samples=nullptr;}return 0;
}
Type2Laser* create_type2(){void* p=::operator new(sizeof(Type2Laser),std::nothrow);if(!p)return nullptr;std::memset(p,0,sizeof(Type2Laser));return new(p)Type2Laser;}
CurveNode* create_curve_node(){void* p=runtime::allocate_bytes(sizeof(CurveNode));return p?new(p)CurveNode:nullptr;}
std::int32_t __cdecl remap_curve_sprite(sprite::Animation* a,std::int32_t){return recovered::signed_bits(reinterpret_cast<const Laser*>(a->field_5c8)->field_6e8+0x20cu);}
int Type2Laser::initialize(const Type2Parameters& input){
    parameters=input;{Type2Parameters returned_copy(parameters);} //4c8d70 returns a value, immediately destroyed by4d3230.
    field_1320=parameters.field_50;state=2;field_24=2;field_6e4=parameters.type;field_6e8=parameters.color;select_context(parameters.view_index);
    sprite::reset_animation_state(animation);auto& file=*static_cast<Controller*>(context->objects_04[4])->file;
    if(field_6e4==1){bullet::restart_animation(file,animation,0x147);field_131c=1.100000023841858f;}
    else if(field_6e4==2){auto& owner=*static_cast<gameplay::EnemyController*>(context->objects_04[1]);bullet::restart_animation(*owner.services->existing_animation(owner,3),animation,0xb);field_131c=.550000011920929f;}
    else{animation.field_5e0=reinterpret_cast<std::uintptr_t>(&remap_curve_sprite);animation.field_5c8=reinterpret_cast<std::uintptr_t>(this);bullet::restart_animation(file,animation,recovered::signed_bits(field_6e4+0x91));field_131c=1.100000023841858f;}
    field_6d0=0xffd08080;const auto& style=bullet::styles[field_6e4];if(style.cancel_type==6)field_6d0=style.colors[field_6e8].words[4];
    sprite::set_animation_interrupt(animation,2);sprite::execute_animation(animation);animation.base.flags[0]=(animation.base.flags[0]&~0xffffu)|0x101u;animation.base.flags[4]=0;animation.base.flags[5]=2;animation.base.flags[2]=(animation.base.flags[2]&~0x3000000u)|0x1000000u;
    field_1324=parameters.field_54;sprite::bind_animation_script(file,origin_animation,recovered::signed_bits(parameters.color+0x3a),nullptr);sprite::set_animation_interrupt(origin_animation,2);sprite::execute_animation(origin_animation);
    origin_animation.base.flags[0]=(origin_animation.base.flags[0]&~0xffffu)|0x101u;origin_animation.base.flags[2]=(origin_animation.base.flags[2]&~0x3000000u)|0x1000000u;
    geometry=runtime::allocate_bytes(parameters.count*0x38u);if(!geometry)return -1;
    samples=static_cast<CurveSample*>(runtime::allocate_bytes(parameters.count*0x20u));if(!samples)return -1;
    std::memset(geometry,0,parameters.count*0x38u);std::memset(samples,0,parameters.count*0x20u);position=parameters.position;
    if(parameters.radial_offset!=0){sprite::Vec3 offset{};ecl::math::polar(offset.x,offset.y,parameters.angle,parameters.radial_offset);position.x=recovered::add32(position.x,offset.x);position.y=recovered::add32(position.y,offset.y);parameters.position=position;parameters.radial_offset=0;}
    speed=parameters.width;angle=parameters.angle;field_7c=parameters.speed;ecl::math::polar(velocity.x,velocity.y,angle,field_7c);velocity.z=0;
    for(std::int32_t i=0;i<recovered::signed_bits(parameters.count);++i)samples[i]={position,{},parameters.angle,parameters.speed};
    bullet::assign_timer_float(timer_48,parameters.time);
    if(!parameters.path){path.next=nullptr;path.speed=field_7c;path.angle=ecl::math::wrap_angle(angle);ecl::math::polar(path.direction.x,path.direction.y,angle,1);path.direction.z=0;path.kind=0;path.begin=0;path.position=position;path.end=999999;command_index=parameters.command_index;}
    else{path=*parameters.path;auto* destination=&path;for(auto* source=parameters.path;source;source=source->next){if(source->next){destination->next=create_curve_node();if(!destination->next)return -1;*destination->next=*source->next;destination=destination->next;}}parameters.path=nullptr;command_index=99;}
    samples[0].velocity=velocity;bool backwards=false;
    for(std::int32_t i=0;i<recovered::signed_bits(parameters.count);++i){
        const float t=_mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(timer_48.current_f),_mm_set_ss(static_cast<float>(i))));auto& sample=samples[i];
        if(t<0)sample={parameters.position,{},parameters.angle,parameters.speed};
        else{const auto& previous=i? samples[i-1]:sample;sample_curve_path(&path,sample.position,sample.speed,sample.angle,previous.position,previous.speed,previous.angle,t,backwards);backwards=true;}
    }
    recovered::timer_set(timer_6ac,30);if(parameters.sound>=0)program_entry::thread_registry.request_effect_at(parameters.sound,0);recovered::timer_set(timer_38,0);field_88=field_84=0x3f800000u;field_6dc=parameters.count-1;allocated_6d8=runtime::allocate_bytes(field_6dc*0x3cu);return allocated_6d8?0:-1;
}
int Type2Laser::compute_segments(Segment* copy){
    auto* out=static_cast<Segment*>(allocated_6d8);std::uint32_t count=0;flags|=0x40;field_84=0x3f800000u;float distance=0;
    for(std::int32_t i=0;i<recovered::signed_bits(parameters.count-1);++i){
        const auto& s=samples[i];sprite::Vec3 center{};ecl::math::polar(center.x,center.y,s.angle,recovered::mul32(s.speed,.5f));center.x=recovered::add32(center.x,s.position.x);center.y=recovered::add32(center.y,s.position.y);center.z=recovered::add32(center.z,s.position.z);distance=recovered::add32(distance,s.speed);
        if(distance>=16){sprite::Vec3 extra{};ecl::math::polar(extra.x,extra.y,angle,_mm_cvtss_f32(_mm_div_ss(_mm_set_ss(s.speed),_mm_set_ss(2))));center.x=recovered::add32(center.x,extra.x);center.y=recovered::add32(center.y,extra.y);center.z=recovered::add32(center.z,extra.z);
            out->position=center;out->size.x=recovered::mul32(s.speed,1.100000023841858f);out->size.y=recovered::mul32(recovered::mul32(speed,.5f),field_131c);out->angle=out->angle_28=s.angle;out->flags=(out->flags&~3u)|4;out->field_2c=s.speed;++out;++count;}
    }
    field_6e0=count;if(count&&copy)std::memcpy(copy,allocated_6d8,count*sizeof(Segment));return recovered::signed_bits(count);
}
}
