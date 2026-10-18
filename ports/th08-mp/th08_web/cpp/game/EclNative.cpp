#include "EclNative.hpp"
#include "AsciiManager.hpp"
#include "GameMath.hpp"
#include <cmath>
namespace th08 {
bool configure_ecl_native(EclVm& vm,const EclInstruction& ins){
    if(ins.size<16){vm.invalid=true;return false;}const i32 id=vm.operand_int(ins,0);
    if(ins.opcode==136)return execute_ecl_native(vm,id,ins);
    auto& context=vm.context();if(id<0){context.native_callback=-1;return true;}
    const i32 selected=vm.operand_int(ins,0);if(selected<0||selected>=32){vm.invalid=true;return false;}
    context.native_callback=selected;context.native_instruction=&ins;return true;
}
bool execute_ecl_native(EclVm& vm,i32 id,const EclInstruction& ins){
    if(!vm.environment||id<0||id>=32){vm.invalid=true;return false;}auto& globals=*vm.environment;auto& locals=vm.context().locals;
    if(id==2){
        bool changed=false;if(vm.position.x<=0||vm.position.x>=384){vm.velocity.x=-vm.velocity.x;changed=true;}
        if(vm.velocity.y<locals.floats[7]){vm.velocity.y=Scalar::add(vm.velocity.y,locals.floats[6]);changed=true;}
        if(vm.position.y<-64){vm.velocity.y=-vm.velocity.y;changed=true;}else if(vm.position.y>=480)vm.flags&=~0x10000000u;
        if(changed)vm.direction.z=Extended::from_double(std::atan2(double(vm.velocity.y),double(vm.velocity.x))).to_float();return true;
    }
    if(id==19){locals.integers[0]=globals.current_spell;return true;}
    if(id==23){globals.end_enemy_announcement();return true;}
    if(id==8){
        if(!vm.parent)return true;i32 count=0;EclVm* first=nullptr;u32 visited=0;const auto group=locals.counters[2];
        for(auto* enemy=vm.parent->next_familiar;enemy;enemy=enemy->next_familiar){if(++visited>480){vm.invalid=true;return false;}auto& other=enemy->context().locals;if(other.counters[2]!=group)continue;other.counters[1]=count;if(!count)first=enemy;++count;}
        locals.integers[5]=0;if(locals.integers[6]!=count){if(locals.integers[6])locals.integers[5]=1;locals.integers[6]=count;}
        const i32 index=locals.counters[1];locals.integers[7]=wrapping_add(locals.integers[7],1);if(!index)return true;if(!first||!count){vm.invalid=true;return false;}
        constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;
        float target=add_angle(first->orbit_angle,(Extended::from_int(index)*number(tau)/Extended::from_int(count)).to_float());
        if(first->context().locals.integers[7]!=locals.integers[7])target=add_angle(target,first->orbit_velocity);
        float delta=(number(target)-number(add_angle(vm.orbit_angle,vm.orbit_velocity))).to_float();if(std::fabs(delta)>pi)delta=Scalar::add(delta,delta<=0?tau:-tau);
        vm.orbit_angle=add_angle(vm.orbit_angle,Scalar::mul(delta,.02f));return true;
    }
    if(id==24){if(!globals.values){vm.invalid=true;return false;}locals.integers[0]=globals.values->captured_spells;return true;}
    auto* service=globals.native_services;if(!service){vm.invalid=true;vm.failure=EclVm::Failure::MissingNativeServices;return false;}
    if(id==0){
        // Mystia's callback writes the live AsciiManager, also cleared by
        // SpellSystem::end. These are the original globals 004e3d28/24.
        if(!globals.ascii){vm.invalid=true;vm.failure=EclVm::Failure::MissingNativeServices;return false;}
        globals.ascii->blindness_color=u32(locals.integers[0]);
        globals.ascii->blindness_radius=locals.floats[0];return true;
    }
    const auto raw=[&](u32 index){i32 value=0;if(ins.size<16+4*index){vm.invalid=true;return value;}std::memcpy(&value,reinterpret_cast<const u8*>(&ins)+12+4*index,4);return value;};
    if(id==18){if(!service->timing){vm.invalid=true;vm.failure=EclVm::Failure::MissingNativeServices;return false;}service->timing->rate=(number(1)/Extended::from_int(raw(1))).to_float();return !vm.invalid;}
    if(id==30){service->screen_effect_counter=raw(1);if(service->screen_effect_counter_value)*service->screen_effect_counter_value=service->screen_effect_counter;return !vm.invalid;}
    if(id==4||id==7||id==12||id==14||id==16||id==21||id==27||id==28||id==29)return execute_ecl_native_projectiles(vm,id,ins,*service);
    auto* actions=service->actions;if(!actions){vm.invalid=true;vm.failure=EclVm::Failure::MissingNativeServices;return false;}
    switch(id){
    case 1:case 17:actions->screen(3,id==1?60:180,1,-1,0,21);break;
    case 3:case 6:case 20:if(!actions->spell_background(id==3?0:id==20?1:2,vm.position))vm.invalid=true;break;
    case 5:if(!actions->spell_background(-1,vm.position))vm.invalid=true;break;
    case 9:case 11:case 25:{
        const Vec3 p=id==11?vm.position:vm.resolved_position;
        const Vec3 origin{Scalar::sub(p.x,locals.floats[0]),Scalar::sub(p.y,locals.floats[1]),0};
        const Vec2 center{(number(590)/number(2)+number(origin.x)).to_float(),origin.y},size{590,id==9?160.f:id==11?240.f:288.f},graze{590,id==9?128.f:id==11?192.f:224.f};
        if(vm.lifetime.changed()&&vm.lifetime.current%12==0)actions->laser(center,graze,origin,vm.animation[0].rotation.z,true);
        actions->laser(center,size,origin,vm.animation[0].rotation.z,false);break;
    }
    case 10:actions->screen(3,30,5,0x40ffffff,0,21);actions->screen(7,4,120,190,60,21);break;
    case 13:actions->tint(0xffc03030);break;
    case 15:actions->screen(7,16,20,20,20,21);break;
    case 26:globals.paused=u8(raw(1));actions->background_interrupt(globals.paused?2:1);break;
    case 22:{
        static constexpr char name[]={char(129),char(117),char(131),char(138),char(131),char(85),char(131),char(140),char(131),char(78),char(131),char(86),char(131),char(135),char(131),char(147),char(129),char(118),0};
        globals.spell_flags=(globals.spell_flags|1024)&~17u;if(!actions->spell_announcement(-1,name,1))vm.invalid=true;break;
    }
    case 31:actions->item(vm.position,actions->bomb_active()?3:5,0);break;
    default:vm.invalid=true;vm.failure=EclVm::Failure::UnsupportedNativeCallback;return false;
    }
    return !vm.invalid;
}
}
