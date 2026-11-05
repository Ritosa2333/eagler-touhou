#include "command.hpp"
#include "movement.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../gameplay/enemy_variables.hpp"
#include "../player_entity/player.hpp"
#include <cstring>
namespace th20::source::bullet {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float player_angle(game_session::Context& context,const sprite::Vec3& p){
    return player_entity::angle_to_player(context.objects_04[0],p);
}
void polar(sprite::Vec3& out,float angle,float speed){m::polar(out.x,out.y,angle,speed);} //439330 preserves z
void command_sound(const Bullet& b){if(b.field_38!=0&&n::signed_bits(b.field_40)>=0)program_entry::thread_registry.request_effect(n::signed_bits(b.field_40),0);}
void clear(Bullet& b,std::uint64_t flag){b.field_90&=~flag;}
}
float resolve_angle(std::int32_t view,float current,const sprite::Vec3& p,float requested,float spread){
    if(spread<=-999990.0f)spread=3.1415927410125732f;
    if(requested<=-999990.0f)return current;
    if(requested>=999990.0f&&requested<1999990.0f)return n::add32(player_angle(game_session::context(view),p),spread);
    if(requested>=2999990.0f&&requested<3999990.0f){const float a=player_angle(game_session::context(view),p);return n::add32(a,n::mul32(state::signed_unit(state::random_streams[0]),spread));}
    if(requested>=3999990.0f&&requested<4999990.0f)return n::add32(n::mul32(state::signed_unit(state::random_streams[0]),spread),current);
    if(requested>=4999990.0f){const auto target=gameplay::enemy_position(gameplay::selected_enemy(&gameplay::enemy_controller(view),0));return m::arctangent(sub(target.y,p.y),sub(target.x,p.x));}
    return requested;
}
void execute_extended_commands(Bullet& b){
    for(;;){
        if(n::signed_bits(b.field_38)>23)return;
        auto& data=*static_cast<ShotMetadata*>(b.metadata.get());
        if(b.field_38>=data.commands.size())return;
        const Command& op=data.commands[b.field_38];const auto type=op.words[8];
        if(type==0)return;
        if(op.words[9]==0){ //MSVC __allshl uses CL, then returns zero for counts >=64.
            const unsigned count=type&255u;const std::uint64_t active=count<64?(std::uint64_t{1}<<count):0;
            if(b.field_90&~(active|0x200000100ull))return;
        }
        switch(type){
        case 1:
            sprite::set_animation_interrupt(*b.animation,static_cast<std::int16_t>(op.words[4])+7);b.state=2;
            b.position.x=sub(b.position.x,n::mul32(b.velocity.x,4));b.position.y=sub(b.position.y,n::mul32(b.velocity.y,4));b.position.z=sub(b.position.z,n::mul32(b.velocity.z,4));break;
        case 2:case 21:case 32:{
            b.field_90|=type==21?0x200000u:4u;auto& c=b.commands[type==21?10:1];
            c.field_10=type==2?op.f(0):div(sub(type==32?n::mul32(op.f(0),b.field_20):op.f(0),b.field_20),n::int_float(op.i(4)));
            c.field_14=resolve_angle(b.view_index,b.angle,b.position,op.f(1),op.f(2));
            n::timer_set(c.timer,0);c.field_30=op.words[4];polar(c.vector_24,c.field_14,c.field_10);command_sound(b);break;
        }
        case 3:{b.field_90|=8;auto& c=b.commands[2];c.field_10=op.f(0);c.field_14=op.f(1);n::timer_set(c.timer,0);c.field_30=op.words[4];command_sound(b);break;}
        case 4:{
            b.field_90|=16;auto& c=b.commands[3];c.field_10=op.f(1)<=-999990.0f?b.field_20:op.f(1);
            const float a=resolve_angle(b.view_index,b.angle,b.position,op.f(0),op.f(2));
            switch(op.words[6]){
            case 0:case 1:case 4:c.field_14=a;break;
            case 2:c.field_14=m::wrap_angle(n::add32(player_angle(*b.context,b.commands[12].vector_18),a));break;
            case 3:c.field_14=m::wrap_angle(n::add32(a,b.commands[12].field_14));break;
            case 5:case 6:c.field_14=n::mul32(state::signed_unit(state::random_streams[0]),op.f(0));break;
            case 7:c.field_14=op.f(0)<=-999990.0f?b.angle:(!(op.f(0)>=990.0f)?op.f(0):player_angle(*b.context,b.position));c.field_10=n::add32(n::mul32(state::signed_unit(state::random_streams[0]),op.f(1)),b.field_20);break;
            }
            n::timer_set(c.timer,0);c.field_30=op.words[4];c.field_34=op.words[5];c.field_38=0;c.field_3c=op.words[6];break;
        }
        case 6:{b.field_90|=64;auto& c=b.commands[4];c.field_10=op.f(0);c.vector_18.x=(op.words[5]&32u)?op.f(1):384.0f;c.vector_18.y=(op.words[5]&32u)?op.f(2):448.0f;c.field_34=op.words[4];c.field_30=0;c.field_3c=op.words[5];break;}
        case 7:b.field_18=op.i(4);break;
        case 8:b.field_90|=0x100;n::timer_set(b.commands[11].timer,op.i(4));b.commands[11].field_30=op.words[5];break;
        case 9:unrecovered::change_bullet_style(b,op);break;
        case 10:if(op.words[4]==1)b.cancel_script=-1;cancel(b,0);break;
        case 11:program_entry::thread_registry.request_effect_at(op.i(4),b.position.x);break;
        case 12:{b.field_90|=0x1000;auto& c=b.commands[6];c.field_34=op.words[4];c.field_30=0;c.field_38=op.words[5];break;}
        case 13:unrecovered::spawn_extended_bullets(b,op);continue;
        case 15:b.field_2c=op.words[4];break;
        case 16:
            if(op.i(5)<1){b.field_38=op.words[4];continue;}
            if(b.field_3c==0){b.field_3c=op.words[5];b.field_38=op.words[4];continue;}
            if(b.field_3c!=1){--b.field_3c;b.field_38=op.words[4];continue;}b.field_3c=0;break;
        case 17:{
            b.field_90|=0x20000;auto& c=b.commands[8];c.vector_24.x=op.f(0);c.vector_24.y=op.f(1);
            if(op.words[5]&0x100u){c.vector_24.x=n::add32(c.vector_24.x,b.position.x);c.vector_24.y=n::add32(c.vector_24.y,b.position.y);c.vector_24.z=n::add32(c.vector_24.z,b.position.z);}
            c.field_10=b.field_20;c.vector_24.z=0;c.field_30=op.words[4];c.field_34=op.words[5]&255u;n::timer_set(c.timer,0);
            auto& p=b.interpolation_420;p.start=b.position;p.end=c.vector_24;p.tangent_start=p.tangent_end={0,0,0};p.duration=op.i(4);p.mode=op.words[5]&255u;n::timer_set(p.timer,0);break;
        }
        case 18:
            if(!(op.f(0)>=990.0f)){if(op.f(0)>=-990.0f)b.angle=m::wrap_angle(op.f(0));}
            else b.angle=m::wrap_angle(m::wrap_angle(n::add32(player_angle(*b.context,b.position),sub(op.f(0),999.0f))));
            if(op.f(1)>=-990.0f)b.field_20=op.f(1);polar(b.velocity,b.angle,b.field_20);break;
        case 19:{clear(b,0x80000);auto& c=b.commands[9];polar(c.vector_24,op.f(0),op.f(1));c.vector_24.z=0;c.field_14=op.f(0);c.field_10=op.f(1);c.field_30=op.words[4];n::timer_set(c.timer,0);break;}
        case 20:b.animation->base.flags[0]=(b.animation->base.flags[0]&~0xff00u)|((op.words[4]==2?2u:op.words[4]==1?1u:0u)<<8);break;
        case 22:{clear(b,0x400000);auto& p=b.interpolation_474;p.start=op.f(0);p.end=op.f(1);p.tangent_start=p.tangent_end=0;p.duration=op.i(4);p.mode=op.i(5);n::timer_set(p.timer,0);b.flags|=64;break;}
        case 23:b.commands[12].vector_18=b.position;b.commands[12].field_14=b.angle;b.commands[12].field_10=b.field_20;break;
        case 24:unrecovered::spawn_extended_enemy(b,op);break;
        case 25:b.field_44=op.words[4];break;
        case 26:if(op.i(4)>0){b.field_90|=0x4000000;n::timer_set(b.commands[13].timer,op.i(4));}break;
        case 27:unrecovered::spawn_extended_laser(b,op);continue;
        case 29:b.size.x=b.size.y=op.f(0)>=0?op.f(0):bullet_radius(b.field_4c);break;
        case 30:if(op.i(4)>0){b.field_90|=0x40000000;n::timer_set(b.commands[5].timer,op.i(4));}break;
        case 31:{b.field_90|=0x80000000;auto& c=b.commands[7];c.field_10=op.f(0);c.field_14=op.f(1);c.vector_24.x=op.f(2);n::timer_set(c.timer,0);c.field_30=op.words[4];break;}
        default:break;
        case 33:b.field_90=(b.field_90&~0x200000000ull)|(std::uint64_t(op.words[4]&1u)<<33);break;
        }
        ++b.field_38;
    }
}
namespace unrecovered {void execute_extended_commands_0047dcf0(Bullet& b){execute_extended_commands(b);}}
}
