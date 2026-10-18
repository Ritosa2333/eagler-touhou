#include "shoot.hpp"
#include "style.hpp"
#include "movement.hpp"
#include "../player_entity/player.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
namespace th20::source::bullet {
namespace n=recovered;namespace m=ecl::math;
namespace {float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}}
int shoot_one(Controller& owner,const ShotParameters& p,std::shared_ptr<ShotMetadata> metadata,std::uint32_t column,std::uint32_t row,float player_angle){
    auto* first=owner.free.sentinel.next;if(!first)return 1;auto& b=*reinterpret_cast<Bullet*>(first->value);
    scheduler::unlink(b.link);scheduler::insert_after(owner.active.sentinel,b.link);b.link.owner=&owner.active;if(owner.active.tail==&owner.active.sentinel)owner.active.tail=&b.link;
    auto& data=*metadata;b.metadata=std::move(metadata);b.view_index=owner.view_index;b.context=&game_session::context(b.view_index);
    assign_timer_float(b.timer_4d8,1);assign_timer_float(b.timer_4e8,1);
    const auto trajectory=shot_trajectory(p,static_cast<std::uint16_t>(data.type),column,row,player_angle);
    b.field_24=trajectory.initial_speed;b.field_20=trajectory.speed;b.angle=m::wrap_angle(m::wrap_angle(n::add32(trajectory.angle,0)));
    b.position=p.position;if(data.field_00!=0){sprite::Vec2 delta{};m::polar(delta.x,delta.y,b.angle,data.field_00);b.position.x=n::add32(b.position.x,delta.x);b.position.y=n::add32(b.position.y,delta.y);}
    b.position.z=0.10000000149011612f;b.state=1;b.flags|=1;n::timer_set(b.timer_4f8,0);n::timer_set(b.timer_508,0);
    b.field_18=0;b.scale=1;b.interpolation_474.duration=0;m::polar(b.velocity.x,b.velocity.y,trajectory.angle,trajectory.speed);
    b.field_4e=static_cast<std::int16_t>(p.fields_00[1]);b.field_4c=static_cast<std::int16_t>(p.fields_00[0]);b.field_54=0;b.flags=(b.flags&~12u)|2u;b.field_4d0=60;
    n::timer_set(b.timer_4a0,0);n::timer_set(b.timer_4b0,0);n::timer_set(b.timer_4c0,0);b.field_4d4=3;
    auto& sprites=*program_entry::sprite_controller;b.animation=sprite::allocate_animation(sprites);auto& a=*b.animation;
    sprite::reset_animation_state(a);a.field_5c8=reinterpret_cast<std::uintptr_t>(&b);a.field_5e0=reinterpret_cast<std::uintptr_t>(&remap_sprite);
    const auto& style=styles[p.fields_00[0]];restart_animation(*owner.file,a,n::signed_bits(style.script));
    b.flags|=16;a.base.flags[2]=(a.base.flags[2]&~0x3000000u)|0x1000000u;
    if(style.child_script){auto& own=*static_cast<Controller*>(owner.context->primary_owner);sprite::spawn_named_animation(sprites,*own.file,b.animation_handle,"bullet",n::signed_bits(style.child_script),&b.position,0,-1,0);}
    b.field_58=reinterpret_cast<std::uintptr_t>(&style);b.handle=0xffd08080u;
    switch(style.cancel_type){
    case 0:b.cancel_script=n::signed_bits(p.fields_00[1]*2u+6u);break;
    case 1:{constexpr int values[8]={6,10,14,18,22,26,30,36};b.cancel_script=values[p.fields_00[1]];break;}
    case 2:b.cancel_script=-1;b.flags|=16;break;
    case 3:b.cancel_script=18;break;
    case 4:b.cancel_script=8;break;
    //No case5 in4818e0; its previous cancel_script is intentionally preserved.
    case 6:b.cancel_script=n::signed_bits(styles[b.field_4c].colors[p.fields_00[1]].words[3]);b.handle=styles[b.field_4c].colors[p.fields_00[1]].words[4];b.flags|=16;break;
    case 7:b.cancel_script=0x107;b.flags|=16;break;
    case 8:b.cancel_script=0x10a;b.flags|=16;break;
    case 9:b.cancel_script=0x10d;b.flags|=16;break;
    case 10:b.cancel_script=0x116;b.flags|=16;break;
    }
    b.flags&=~0x1800u;b.field_44=style.draw_group;b.field_40=data.field_40;b.field_30=5;b.size.x=b.size.y=style.radius;b.field_90=0;b.field_3c=0;b.field_38=data.field_44;
    if(b.field_38<data.commands.size()&&data.commands[b.field_38].words[8]==1){
        const auto kind=static_cast<std::int16_t>(data.commands[b.field_38].words[4]);if(kind!=1)sprite::set_animation_interrupt(a,kind+7);
        b.state=2;b.position.x=sub(b.position.x,n::mul32(b.velocity.x,4));b.position.y=sub(b.position.y,n::mul32(b.velocity.y,4));b.position.z=sub(b.position.z,n::mul32(b.velocity.z,4));++b.field_38;
    }else sprite::set_animation_interrupt(a,2);
    execute_extended_commands(b);sprite::execute_animation(*b.animation);
    if(owner.field_48>0){const auto pos=player_entity::position(owner.context->objects_04[0]);const auto dx=sub(b.position.x,pos.x),dy=sub(b.position.y,pos.y);if(n::add32(n::mul32(dx,dx),n::mul32(dy,dy))<owner.field_48){retire(b);return -1;}}
    return 0;
}
int shoot(Controller& owner,const ShotParameters& p,std::shared_ptr<ShotMetadata> metadata){
    const float angle=player_entity::angle_to_player(owner.context->objects_04[0],p.position);
    for(int row=0;row<p.rows;++row)for(int column=0;column<p.count;++column)if(shoot_one(owner,p,metadata,column,row,angle)==1)goto done;
done:
    if(metadata->field_3c>=0)program_entry::thread_registry.request_effect_at(metadata->field_3c,p.position.x);return 0;
}
namespace unrecovered {
void spawn_extended_bullets(Bullet& b,const Command& op){
    auto metadata=std::make_shared<ShotMetadata>();ShotParameters p;p.position=b.position;metadata->type=static_cast<std::int16_t>(op.words[4]);metadata->field_44=op.words[5];p.count=static_cast<std::int16_t>(op.words[6]);p.rows=static_cast<std::int16_t>(op.words[7]);
    const Command& extra=*(&op+1);p.angle=resolve_angle(b.view_index,b.angle,b.position,op.f(0),extra.f(3));p.angle_step=op.f(1);p.speed=op.f(2)<=-999990.0f?b.field_20:op.f(2);p.speed_step=op.f(3);
    ++b.field_38;p.fields_00[0]=extra.words[4];p.fields_00[1]=extra.words[5];const bool cancel_parent=extra.words[6]!=0;
    metadata->commands=static_cast<ShotMetadata*>(b.metadata.get())->commands;shoot(*static_cast<Controller*>(b.context->primary_owner),p,std::move(metadata));++b.field_38;if(cancel_parent)cancel(b,0);
}
}
}
