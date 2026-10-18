#include "firing.hpp"
#include "initialize.hpp"
#include "../damage_regions/regions.hpp"
#include "../ecl_vm/math.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstring>
namespace th20::source::player_entity {
namespace {
constexpr float pi=0x1.921fb6p+1f;
template<class T>T load(const void* bytes,unsigned offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(bytes)+offset,sizeof(value));return value;}
void prepend(scheduler::List& list,scheduler::Link& link){scheduler::insert_after(list.sentinel,link);link.owner=&list;if(list.tail==&list.sentinel)list.tail=&link;}
void angle(sprite::Animation& animation,float value){animation.base.vector_38.z=value;animation.base.flags[1]|=2;}
void scale(sprite::Animation& animation,float value){animation.base.vector_58={value,value};animation.base.flags[1]|=4;}
int option_damage_bonus(game_session::Player& record,bool focused){
    auto& value=record.fields_30[(focused?0x9c-0x30:0xa0-0x30)/4];
    const int result=std::clamp(std::bit_cast<std::int32_t>(value),0,100);value=std::uint32_t(result);return result;
}
}
Option& shot_option(Player& player,int index) noexcept{return index<100?player.options[index]:player.secondary_options[index-100];}
Fixed2& shot_option_position(Player& player,int index) noexcept{return shot_option(player,index).vector_78;}
float shot_option_angle(Player& player,int index) noexcept{return std::bit_cast<float>(shot_option(player,index).field_d4);}
const ShotRecord& shot_record(const Player& player,std::uint32_t packed) noexcept{
    const auto pattern=std::bit_cast<std::int32_t>(packed)>>8;
    const auto rows=load<const ShotRecord*>(player.shot_data,0x5d4+std::uint32_t(pattern)*4u);return rows[packed&255u];
}
void advance_shot_handle(ShotController& owner) noexcept{
    auto low=(owner.field_12460+1u)&0xffffu;if(!low)low=1;owner.field_12460=(std::uint32_t(owner.view_index)<<16)|low;
}
Shot* allocate_shot(ShotController& owner,FiringServices& host){
    auto* link=owner.free.sentinel.next;Shot* shot;
    if(!link){
        shot=host.create_heap_shot();if(!shot)return nullptr;
        scheduler::initialize_link(shot->link,reinterpret_cast<scheduler::Node*>(shot));prepend(owner.active,shot->link);
        shot->flags=owner.field_12460|0x1000000u;select_context(*shot,0,host.session());advance_shot_handle(owner);
    }else{
        scheduler::unlink(*link);prepend(owner.active,*link);shot=reinterpret_cast<Shot*>(link->value);shot->flags=owner.field_12460;advance_shot_handle(owner);
    }
    return shot;
}
int initialize_shot(Shot& shot,std::uint32_t packed,int frame,const sprite::Vec3& position,Option* attachment,int view,FiringServices& host){
    select_context(shot,view,host.session());auto& player=*static_cast<Player*>(shot.context->objects_04[0]);const auto& record=shot_record(player,packed);
    shot.fields_98[1]=1;shot.fields_b8[(0xd8-0xb8)/4]=packed;recovered::timer_set(shot.timer_1c,0);
    shot.fields_98[5]=std::uint32_t(std::int32_t(record.damage));shot.vector_b0=record.size;shot.field_110=reinterpret_cast<std::uint32_t>(attachment);shot.byte_114=0;
    const bool focus=player.focused_204c!=0;shot.fields_3c[0]=(shot.fields_3c[0]&~2u)|(std::uint32_t(focus)<<1);
    if((record.source&15)!=0){const auto bonus=option_damage_bonus(*host.session().contexts[0].current_player,focus);
        const auto product=std::bit_cast<std::int32_t>(std::uint32_t(bonus)*shot.fields_98[5]);shot.fields_98[5]+=std::uint32_t(product/100);}
    const auto callbacks=host.callbacks(record);shot.fields_3c[1]=callbacks.initialize;shot.fields_3c[2]=callbacks.update;shot.fields_3c[3]=callbacks.extra;shot.fields_3c[4]=callbacks.hit;
    std::memset(&shot.motion,0,sizeof(shot.motion));const int source=record.source&15;
    if(!source)shot.motion.position=position;
    else if(source==6)shot.motion.position={};
    else{const auto& fixed=shot_option_position(player,source-1);shot.motion.position={float(fixed.x)/128.0f,float(fixed.y)/128.0f,0};}
    if(record.type==2){shot.fields_98[0]=std::bit_cast<std::uint32_t>(ecl::math::wrap_angle(record.angle));shot.owner->counters_12468[record.group]=1;shot.owner->counters_124e0[record.group]=reinterpret_cast<std::uint32_t>(&shot);}
    shot.motion.field_18=record.speed;
    // COMISS/JB makes unordered values follow the same branch as angle<1000.
    if(record.angle>=1000.0f&&source){
        const auto base=shot_option_angle(player,source-1);const auto random=host.signed_random();
        shot.motion.angle_1c=ecl::math::wrap_angle(base+(random*pi)/12.0f);
        const auto speed_random=host.signed_random();shot.motion.field_18=speed_random*2.0f+record.speed;
    }else if(record.angle>=995.0f&&source)shot.motion.angle_1c=ecl::math::wrap_angle(shot_option_angle(player,source-1));
    else shot.motion.angle_1c=ecl::math::wrap_angle(record.angle);
    state::update_motion_velocity(shot.motion,host.clock_rate());
    if(record.type==4||record.type==5||record.type==6){shot.motion.position.x+=record.offset.x;shot.motion.position.y+=record.offset.y;}
    else{shot.motion.position.x+=record.offset.x-shot.motion.vector_38.x;shot.motion.position.y+=record.offset.y-shot.motion.vector_38.y;}
    shot.field_10c=player.field_24;shot.handle_18=host.spawn_animation(*reinterpret_cast<sprite::AnimationFile*>(player.field_24),record.animation);auto& animation=host.animation(shot.handle_18);
    const auto orientation=(animation.base.flags[2]>>21)&7u;
    switch(orientation){
    case 1:angle(animation,record.angle);break;
    case 2:{
        const auto wrapped=ecl::math::wrap_angle(record.angle);
        if(wrapped>=-pi/2.0f&&wrapped<=pi/2.0f){angle(animation,wrapped);animation.base.vector_50.x=std::fabs(animation.base.vector_50.x);}
        else{angle(animation,ecl::math::wrap_angle(wrapped+pi));animation.base.vector_50.x=-std::fabs(animation.base.vector_50.x);}
        animation.base.flags[1]|=4;break;
    }
    case 3:angle(animation,ecl::math::wrap_angle(record.angle+pi));break;
    case 4:angle(animation,ecl::math::wrap_angle(pi/2.0f+record.angle));break;
    case 5:angle(animation,ecl::math::wrap_angle(record.angle-pi/2.0f));break;
    default:break;
    }
    if(shot.owner->field_1255c&8u){shot.vector_b0.x*=0.8f;shot.vector_b0.y*=0.8f;scale(animation,record.type==2?1.0f:0.8f);}
    else scale(animation,record.type==2?1.0f:1.1f);
    auto& damage_handle=shot.fields_b8[(0xdc-0xb8)/4];damage_handle=host.create_damage(*shot.context,shot);
    if(auto* region=host.damage(damage_handle)){region->hit_callback=1;region->field_90=std::bit_cast<std::int32_t>(shot.flags);region->flags=(region->flags&~0x40u)|0x40u;}
    shot.fields_3c[0]|=1;
#ifdef TH_SDL3
    // Stored initializer is __fastcall (Shot*,void* dummy,int) on Windows;
    // wasm lowers conventions away, so pass the dummy as a real argument.
    if(callbacks.initialize&&reinterpret_cast<int(*)(Shot*,void*,int)>(callbacks.initialize)(&shot,nullptr,frame)!=0){host.retire(shot);return -1;}
#else
    if(callbacks.initialize&&reinterpret_cast<ShotInitializeCallback>(callbacks.initialize)(&shot,frame)!=0){host.retire(shot);return -1;}
#endif
    if(record.sound>=0)host.sound_at(record.sound,shot.motion.position.x);
    animation.vector_5bc=shot.motion.position;return 0;
}
int create_shot(ShotController& owner,std::uint32_t packed,int frame,const sprite::Vec3& position,Option* attachment,FiringServices& host){
    const auto& row=shot_record(*static_cast<Player*>(owner.context->objects_04[0]),packed);
    if(row.type==2&&owner.counters_12468[row.group]){auto* existing=reinterpret_cast<Shot*>(owner.counters_124e0[row.group]);existing->fields_b8[(0xd8-0xb8)/4]=packed;return 0;}
    auto* shot=allocate_shot(owner,host);if(!shot)return 0;return initialize_shot(*shot,packed,frame,position,attachment,owner.view_index,host)==0?0:-1;
}
int fire_shots(ShotController& owner,int frame,int secondary,int pattern,FiringServices& host){
    auto* row=load<const ShotRecord*>(reinterpret_cast<const void*>(owner.field_12558),0x5d4+std::uint32_t(pattern)*4u);
    for(std::uint32_t index=0;;++index,++row){
        if(row->period<0)return 0;
        const bool fire=!row->period||(row->secondary_period?secondary%row->secondary_period==row->secondary_phase:frame%row->period==row->phase);
        if(fire){auto& player=*static_cast<Player*>(owner.context->objects_04[0]);auto* option=row->source?&shot_option(player,(row->source&15)-1):nullptr;
            create_shot(owner,(std::uint32_t(pattern)<<8)|index,frame,player.position_614,option,host);}
    }
}
}
