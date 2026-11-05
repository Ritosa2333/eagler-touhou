#include "type0.hpp"
#include "../bullet_system/shoot.hpp"
#include "../bullet_system/style.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../player_entity/player.hpp"
#include "../ecl_vm/math.hpp"
#include <cstring>
namespace th20::source::laser {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
}
void Type0Laser::execute_commands(){
    while(command_index<parameters.commands.size()){
        auto& op=parameters.commands[command_index];if(!op.words[8]||(!op.words[9]&&active_commands))return;
        const auto motion_sound=[&](){if(command_index&&parameters.field_4c>=0)program_entry::thread_registry.request_effect(parameters.field_4c,0);};
        switch(op.words[8]){
        case 2:case 21:{auto& c=commands[op.words[8]==2?1:10];active_commands|=op.words[8]==2?4u:0x200000u;c.field_10=op.words[8]==2?op.f(0):div(sub(op.f(0),field_7c),n::int_float(op.i(4)));c.field_14=bullet::resolve_angle(view_index,angle,position,op.f(1),op.f(2));n::timer_set(c.timer,0);c.field_30=op.words[4];m::polar(c.vector_24.x,c.vector_24.y,c.field_14,c.field_10);motion_sound();break;}
        case 3:{active_commands|=8;auto& c=commands[2];c.field_10=op.f(0);c.field_14=op.f(1);n::timer_set(c.timer,0);c.field_30=op.words[4];motion_sound();break;}
        case 4:{active_commands|=16;auto& c=commands[3];c.field_14=op.f(0);c.field_10=op.f(1)>-999?op.f(1):field_7c;n::timer_set(c.timer,0);c.field_30=op.words[4];c.field_34=op.words[5];c.field_38=0;c.field_3c=op.words[6];break;}
        case 6:if(op.i(4)>0){active_commands|=64;auto& c=commands[4];c.field_10=op.f(0)<0?field_7c:op.f(0);--op.words[4];c.field_34=op.words[4];c.field_30=0;c.field_38=op.words[5];}break;
        case 7:field_6cc=op.words[4];break;
        case 8:n::timer_set(timer_6bc,op.i(4));break;
        case 9:{auto& file=*static_cast<bullet::Controller*>(context->primary_owner)->file;sprite::bind_animation_script(file,animation,n::signed_bits(bullet::styles[op.i(4)].script+op.words[5]),nullptr);break;}
        case 10:state=3;break;
        case 11:program_entry::thread_registry.request_effect_at(op.i(4),position.x);break;
        case 12:active_commands|=0x1000;n::timer_set(commands[6].timer,op.i(4));break;
        case 13:{
            auto metadata=std::make_shared<bullet::ShotMetadata>();bullet::ShotParameters p;tip_position(p.position);p.position.z=0;
            metadata->type=static_cast<std::int16_t>(op.words[4]);metadata->field_44=op.words[5];p.count=static_cast<std::int16_t>(op.words[6]);p.rows=static_cast<std::int16_t>(op.words[7]);
            //Original JB after COMISS(sentinel,value) sends NaN to literal path.
            p.angle=op.f(0)<=-999990?angle:(!(op.f(0)>=999990)?op.f(0):player_entity::angle_to_player(context->objects_04[0],position));
            p.angle_step=op.f(1);p.speed=op.f(2)<=-999990?field_7c:op.f(2);p.speed_step=op.f(3);
            const auto& extra=*(&op+1);++command_index;p.fields_00[0]=extra.words[4];p.fields_00[1]=extra.words[5];const auto erase_parent=extra.words[6];
            //The original copies the newly constructed destination size (two
            //commands), not the entire source vector. Preserve this quirk.
            std::memcpy(metadata->commands.data(),parameters.commands.data(),metadata->commands.size()*sizeof(bullet::Command));
            bullet::shoot(*static_cast<bullet::Controller*>(context->primary_owner),p,std::move(metadata));++command_index;if(erase_parent)erase(0,0);continue;
        }
        case 15:handle=op.words[4];break;
        case 16:
            if(op.i(5)<1){command_index=op.words[4];continue;}
            if(!field_6a8){field_6a8=op.words[5];command_index=op.words[4];continue;}
            if(field_6a8!=1){--field_6a8;command_index=op.words[4];continue;}field_6a8=0;break;
        case 20:animation.base.flags[0]=(animation.base.flags[0]&~0xff00u)|(op.words[4]?0x100u:0);break;
        case 30:if(op.i(4)>0){active_commands|=0x40000000;n::timer_set(commands[5].timer,op.i(4));}break;
        case 33:active_commands=(active_commands&~(std::uint64_t{1}<<33))|(std::uint64_t{op.words[4]&1u}<<33);break;
        }
        ++command_index;
    }
}
}
