#include "type2.hpp"
#include "../bullet_system/style.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../ecl_vm/math.hpp"
namespace th20::source::laser {
namespace n=recovered;namespace m=ecl::math;
CurveNode* Type2Laser::append_path(float time){
    auto* tail=&path;while(tail->next)tail=tail->next;auto* next=create_curve_node();if(next){tail->end=time;next->begin=time;next->next=nullptr;next->field_04=reinterpret_cast<std::uintptr_t>(tail);}tail->next=next;return next;
}
void Type2Laser::execute_commands(){
    while(command_index<parameters.commands.size()&&n::signed_bits(command_index)<24){
        const auto& op=parameters.commands[command_index];const auto code=op.words[8];const std::uint64_t bit=code<64?(std::uint64_t{1}<<code):0;
        if(!code||(!op.words[9]&&active_commands)||(active_commands&bit))return;
        switch(code){
        case 2:case 3:{
            auto initialize_node=[](CurveNode& node){const auto& previous=*reinterpret_cast<const CurveNode*>(node.field_04);sample_curve_absolute(previous,node.position,node.speed,node.angle,previous.end);node.position.z=0;m::polar(node.direction.x,node.direction.y,node.angle,1);node.direction.z=0;};
            auto* node=append_path(n::int_float(op.i(5)));node->kind=code==2?1:2;initialize_node(*node);node->acceleration=op.f(0);node->angular_acceleration=op.f(1);
            if(op.i(4)<0)node->end=999999;else{node->end=n::add32(n::int_float(op.i(5)),n::int_float(op.i(4)));node=append_path(node->end);node->kind=0;initialize_node(*node);node->end=999999;}break;
        }
        case 4:{active_commands|=16;auto& c=commands[3];c.field_14=op.f(0);c.field_10=op.f(1)>-999?op.f(1):field_7c;n::timer_set(c.timer,0);c.field_30=op.words[4];c.field_34=op.words[5];c.field_38=0;c.field_3c=op.words[6];break;}
        case 6:if(op.i(4)>0){active_commands|=64;auto& c=commands[4];c.field_10=op.f(0)<0?field_7c:op.f(0);auto& mutable_op=parameters.commands[command_index];--mutable_op.words[4];c.field_34=mutable_op.words[4];c.field_30=0;c.field_38=op.words[5];}break;
        case 7:field_6cc=op.words[4];break;
        case 8:active_commands|=0x100;n::timer_set(commands[11].timer,op.i(4));commands[11].field_30=op.words[5];break;
        case 9:{auto& file=*static_cast<bullet::Controller*>(context->primary_owner)->file;sprite::bind_animation_script(file,animation,n::signed_bits(bullet::styles[op.i(4)].script+op.words[5]),nullptr);break;}
        case 10:state=3;break;
        case 11:program_entry::thread_registry.request_effect_at(op.i(4),position.x);break;
        case 12:active_commands|=0x1000;n::timer_set(commands[6].timer,op.i(4));break;
        case 13:reemit(op);continue;
        case 15:handle=op.words[4];break;
        case 16:command_index=op.words[4];continue;
        case 20:animation.base.flags[0]=(animation.base.flags[0]&~0xff00u)|(op.words[4]?0x100u:0);break;
        case 28:flags=(flags&~16u)|((op.words[4]&1)<<4);break;
        case 30:if(op.i(4)>0){active_commands|=0x40000000;n::timer_set(commands[5].timer,op.i(4));}break;
        case 31:{field_1324=1;active_commands|=0x80000000;auto& c=commands[7];c.field_10=op.f(0);c.field_14=op.f(1);c.vector_24.x=op.f(2);n::timer_set(c.timer,0);c.field_30=op.words[4];break;}
        case 33:active_commands=(active_commands&~(std::uint64_t{1}<<33))|(std::uint64_t{op.words[4]&1}<<33);break;
        }
        ++command_index;
    }
}
}
