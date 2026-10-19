#include "shot_hit.hpp"
#include "shot_geometry.hpp"
#include "../damage_regions/regions.hpp"
#include "../ecl_vm/math.hpp"
#include "../effect_system/effect.hpp"
#include <bit>
#include <cmath>
#include <cstring>
#include <stdexcept>
namespace th20::source::player_entity {
namespace {
constexpr float pi=0x1.921fb6p+1f;
const ShotRecord& record(Shot& shot){return shot_record(*static_cast<Player*>(shot.context->objects_04[0]),shot.fields_b8[8]);}
bool tick(const Shot& shot,int period){return shot.timer_1c.previous!=shot.timer_1c.current&&shot.timer_1c.current%period==0;}
void begin_contact(Shot& shot,ShotCallbackEnvironment& environment){shot.fields_98[3]=1;if(!shot.fields_98[4]){environment.interrupt(shot.handle_18,2);shot.fields_98[4]=1;}}
template<class T>void interpolate(sprite::Interpolation<T>& value,int frames,int mode,T start,T end){value.duration=frames;value.mode=mode;value.start=start;value.end=end;value.current=start;recovered::timer_set(value.timer,0);}
void color_byte(sprite::Animation& animation,unsigned index,unsigned value){auto* bytes=reinterpret_cast<std::uint8_t*>(&animation.base.field_490);bytes[index]=static_cast<std::uint8_t>(value);}
float distance(float x,float y){return ecl::math::square_root(recovered::add32(recovered::mul32(x,x),recovered::mul32(y,y)));}
int laser_hit(Shot& shot,const sprite::Vec3& position,const sprite::Vec2* size,float angle,float radius,ShotHitServices& service){ //503450
    auto& env=service.callbacks();auto& host=env.firing();const auto& row=record(shot);begin_contact(shot,env);
    auto* region=host.damage(shot.fields_b8[9]);if(!region)return 0;
    if(!size){float near_hit,far_hit;shot_geometry::ray_circle(near_hit,far_hit,shot.motion.position,shot.motion.angle_1c,position,region->size.y/2.0f+radius);shot.vector_b0.x=near_hit+8.0f;if(shot.vector_b0.x<0)shot.vector_b0.x=0;region->size.x=shot.vector_b0.x;}
    else{sprite::Vec3 near_hit{},far_hit{};
        if(!shot_geometry::ray_rectangle(near_hit,far_hit,shot.motion.position,shot.motion.angle_1c,position.x,position.y,region->size.y+size->x,region->size.y+size->y,angle)){
            shot.vector_b0.x=distance(position.x-shot.motion.position.x,position.y-shot.motion.position.y)-24.0f;if(shot.vector_b0.x<0)shot.vector_b0.x=0;region->size.x=shot.vector_b0.x+16.0f;
        }else{const float direction=ecl::math::arctangent(near_hit.y-shot.motion.position.y,near_hit.x-shot.motion.position.x);
            if(std::fabs(ecl::math::angle_difference(direction,shot.motion.angle_1c))>=pi/2.0f)shot.vector_b0.x=8;
            else shot.vector_b0.x=distance(near_hit.x-shot.motion.position.x,near_hit.y-shot.motion.position.y)+8.0f;
            if(shot.vector_b0.x<0)shot.vector_b0.x=0;region->size.x=shot.vector_b0.x;
        }
    }
    if(tick(shot,2)){
        sprite::Vec3 where{},offset{};ecl::math::polar(where.x,where.y,shot.motion.angle_1c,shot.vector_b0.x);where.x+=shot.motion.position.x;where.y+=shot.motion.position.y;where.z=0;ecl::math::polar(offset.x,offset.y,shot.motion.angle_1c,64.0f);
        auto handle=service.spawn(*reinterpret_cast<sprite::AnimationFile*>(shot.field_10c),nullptr,static_cast<std::int16_t>(row.field_2e),where,0);
        service.remember_effect(*shot.context,handle);
        if(auto* visual=env.find_animation(handle)){visual->base.vector_38.z=shot.motion.angle_1c;visual->base.flags[1]|=2;} //450520/4505c0
        auto& animation=host.animation(handle);interpolate(animation.base.interpolation_8c,20,4,sprite::Vec3{},offset);
    }
    if(tick(shot,4)){if(auto* damage=host.damage(shot.fields_b8[9]))return damage->damage;}return 0;
}
}
std::uint32_t reserve_effect_slot(effects::Controller& owner,ShotCallbackEnvironment& env){
    //498e70 uses +20 as its cursor, aliasing files[4] in EffectInf. Preserve
    //the original field and its old-index return/new-index test discrepancy.
    std::uint32_t cursor;std::memcpy(&cursor,&owner.files[4],4);std::uint32_t selected=0xffffffffu;
    for(unsigned attempt=0;attempt<1024;++attempt){const auto previous=cursor;cursor=std::bit_cast<std::uint32_t>(recovered::signed_bits(cursor+1u)%1024);std::memcpy(&owner.files[4],&cursor,4);auto& candidate=owner.handles[recovered::signed_bits(cursor)];
        if(candidate==0){selected=previous;break;}if(env.find_animation(candidate)){selected=previous;break;}candidate=0;
    }
    return selected;
}
std::uint32_t remember_shot_effect(effects::Controller& owner,std::uint32_t handle,ShotCallbackEnvironment& env){
    const auto selected=reserve_effect_slot(owner,env);
    if(selected==0xffffffffu||recovered::signed_bits(selected)>1023)return 0;
    owner.handles[recovered::signed_bits(selected)]=handle;return selected|0x80000000u;
}
int hit_shot_callback(Shot& shot,unsigned index,const sprite::Vec3& position,const sprite::Vec2* size,float angle,float radius,ShotHitServices& service){
    auto& env=service.callbacks();auto& host=env.firing();
    switch(index){
    case 1:case 7:case 8:{ //5032d0/503ee0/503fc0
        auto& file=service.effect_file(*shot.context);const unsigned stream=index==1?0:1;
        const float random=service.random_signed(stream),offset=random*((pi/180.0f)*20.0f);float rotation=ecl::math::wrap_angle(shot.motion.angle_1c+offset);if(index==1)rotation=ecl::math::wrap_angle(rotation+pi);
        auto handle=service.spawn(file,"effect",148,shot.motion.position,rotation);
        if(index!=7){auto& animation=host.animation(handle);if(index==8)interpolate(animation.base.interpolation_1e0,20,0,sprite::Vec2{1,1},sprite::Vec2{3,3});
            color_byte(animation,2,service.random_bounded(stream,128)+127);color_byte(animation,1,service.random_bounded(stream,index==8?128:64)+64);color_byte(animation,0,service.random_bounded(stream,64)+64);if(index==1)color_byte(animation,3,service.random_bounded(stream,64)+96);}
        return service.default_hit(shot);
    }
    case 2:return laser_hit(shot,position,size,angle,radius,service);
    case 3:begin_contact(shot,env);if(tick(shot,1)){if(auto* region=host.damage(shot.fields_b8[9]))return region->damage;}return 0; //503ad0
    case 4:case 6:case 10:{ //503b70/503d40
        const auto& row=record(shot);auto* created=service.create_circle(*shot.context,shot.motion.position,24.0f,index==6?std::bit_cast<float>(row.fields_58[4]):1.0f,index==6?20:16,row.damage);if(!created)return 0;
        created->period=index==6?4:2;if(index!=6){created->motion.field_18=0.3f;created->motion.angle_1c=ecl::math::wrap_angle(-pi/2.0f);created->angle=ecl::math::wrap_angle(-pi/2.0f);}created->flags|=64;
        env.interrupt(shot.handle_18,1);shot.fields_98[1]=2;
        if(index==6){shot.motion.field_18=2;created->motion=shot.motion;}
        auto* previous=host.damage(shot.fields_b8[9]);if(!previous)return 0;
        if(index!=6){shot.motion.field_18=0.3f;previous->motion=shot.motion;}
        // The original writes this motion after retirement. A heap backed
        // damage region may be freed by retire_damage, so finish the write first.
        env.retire_damage(*previous);shot.fields_b8[9]=0;
        host.sound_at(65,shot.motion.position.x);return std::bit_cast<int>(shot.fields_98[5]);
    }
    case 5:host.sound_at(40,position.x);return service.default_hit(shot); //504180
    case 9:return service.default_hit(shot); //504160
    default:throw std::out_of_range("Invalid nonnull SHT hit callback index");
    }
}
}
