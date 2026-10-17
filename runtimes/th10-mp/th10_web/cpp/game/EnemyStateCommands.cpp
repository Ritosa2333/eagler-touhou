#include "EnemyFrame.hpp"
namespace th10 {
namespace {
void seek(Timer& timer,u32& flags,i32 frame,const float* rate){
    if(!(flags&1)){timer.rate=rate;flags|=1;}
    timer.previous=wrapping_add(frame,-1);timer.current=frame;timer.fractional=Extended::from_int(frame).to_float();
}
i32 rank_five(i32 rank){return rank>=600?4:rank>=200?3:rank>=-200?2:rank>=-400?1:0;}
}
// State and parameter-selection cases of the original enemy command handler.
// Unrecovered cases return false so that the caller keeps an explicit boundary.
bool EnemyState::state_command(EclContext& context,EclGlobals& globals,EnemyFrameEnvironment& env,i32& result){
    const auto opcode=context.instruction->opcode;result=0;
    const auto integer=[&](u32 index){return context.integer_argument(index,globals);};
    const auto floating=[&](u32 index){return context.float_argument(index,globals);};
    switch(opcode){
    case 0x140:hitbox.x=floating(0).to_float();hitbox.y=floating(1).to_float();return true;
    case 0x141:collision_box.x=floating(0).to_float();collision_box.y=floating(1).to_float();return true;
    case 0x142:{flags|=static_cast<u32>(integer(0));if(flags&0x10)for(const auto id:animations)env.registry->set_visibility(id,false);return true;}
    case 0x143:{flags&=~static_cast<u32>(integer(0));if(!(flags&0x10))for(const auto id:animations)env.registry->set_visibility(id,true);return true;}
    case 0x144:
        flags|=0x200;clamp_center.x=floating(0).to_float();clamp_center.y=floating(1).to_float();
        clamp_size.x=floating(2).to_float();clamp_size.y=floating(3).to_float();return true;
    case 0x145:flags&=~0x200u;return true;
    case 0x146:std::memset(drops.counts,0,sizeof(drops.counts));return true;
    case 0x147:{
        auto kind=integer(0);if(flags&0x8000){if(kind==1)kind=10;else if(kind==4)kind=11;}
        const auto count=integer(1);
        if(kind==0)drops.kind=count;else drops.counts[kind-1]=count;return true;
    }
    case 0x148:{const auto y=floating(1).to_float();drops.spread.x=floating(0).to_float();drops.spread.y=y;return true;}
    case 0x149:drops.release(current.position,*env.items);return true;
    case 0x14a:{auto kind=integer(0);if(flags&0x8000){if(kind==1)kind=10;else if(kind==4)kind=11;}drops.kind=kind;return true;}
    case 0x14b:
        health=maximum_health=integer(0);std::memset(env.health_bars,0,4*sizeof(EnemyHealthBar));health_to_interrupt=health;return true;
    case 0x14c:{
        const auto slot=integer(0);
        if(slot<0){if(flags&0x8000)env.boss_slots[boss_slot]=nullptr;flags&=~0x8000u;}
        else{flags|=0x8000;env.boss_slots[slot]=reinterpret_cast<Enemy*>(script_owner);boss_slot=slot;}
        return true;
    }
    case 0x14d:seek(lifetime,lifetime_flags,0,env.default_rate);return true;
    case 0x14e:{
        const auto time=integer(2);const auto health=integer(1);const auto index=integer(0);
        auto& interrupt=interrupts[index];interrupt.health=health;
        if(health>=0){interrupt.time=time;interrupt.health_subroutine=interrupt.time_subroutine=reinterpret_cast<const char*>(context.instruction)+32;}
        return true;
    }
    case 0x14f:{const auto frames=integer(0);seek(damage_immunity,damage_immunity_flags,frames,env.default_rate);return true;}
    case 0x150:{const auto sound=integer(0);env.play_sound(sound,current.position.x);return true;}
    case 0x153:result=env.message_status&&*env.message_status==0?-1:0;return true;
    case 0x154:result=env.boss_slots[0]?-1:0;return true;
    case 0x155:{const auto index=integer(0);interrupts[index].time_subroutine=reinterpret_cast<const char*>(context.instruction)+24;return true;}
    case 0x15a:{const auto distance=floating(0);shot_exclusion_distance_squared=(distance*distance).to_float();return true;}
    case 0x15b:{
        const auto style=integer(2);const auto amount=floating(1).to_float();const auto maximum=maximum_health;
        const auto index=integer(0);env.health_bars[index].style=style;
        env.health_bars[index].amount=(number(amount)/Extended::from_int(maximum)).to_float();return true;
    }
    case 0x15d:case 0x15e:{
        const auto index=opcode==0x15d?(*env.rank>=512?2:0):rank_five(*env.rank);
        const auto value=floating(index).to_float();*context.float_reference(0,globals)=value;return true;
    }
    case 0x15f:{
        const auto start=floating(1).to_float(),end=floating(2).to_float();auto* output=context.float_reference(0,globals);
        *output=((Extended::from_int(*env.rank)+number(1024.0f))*(number(end)-number(start))*number(0.00048828125f)+number(start)).to_float();return true;
    }
    case 0x160:case 0x161:{
        const auto index=opcode==0x160?(*env.rank>=512?2:0):rank_five(*env.rank);
        auto* output=context.integer_reference(0,globals);*output=integer(index);return true;
    }
    case 0x162:{
        const auto start=integer(1),end=integer(2);auto* output=context.integer_reference(0,globals);
        const auto product=static_cast<i32>((static_cast<u32>(end)-static_cast<u32>(start))*(static_cast<u32>(*env.rank)+1024));
        *output=wrapping_add(product/2048,start);return true;
    }
    case 0x163:case 0x164:{
        const auto difficulty=*env.difficulty;if(difficulty<0||difficulty>4)return true;
        const auto index=difficulty<3?difficulty+1:4;
        if(opcode==0x163){const auto value=integer(index);*context.integer_reference(0,globals)=value;}
        else{const auto value=floating(index).to_float();*context.float_reference(0,globals)=value;}
        return true;
    }
    case 0x168:*env.boss_lives=integer(0);return true;
    case 0x169:{const auto frames=integer(0);seek(collision_immunity,collision_immunity_flags,frames,env.default_rate);return true;}
    case 0x16a:*env.phase.spell_flags|=8;return true;
    case 0x16c:flags=(flags&~0x80000u)|((static_cast<u32>(integer(0))<<19)&0x80000);return true;
    case 0x16e:
        flags=(flags&~0x100000u)|((static_cast<u32>(integer(0))<<20)&0x100000);
        alternate_animation=integer(1);flags&=~0x200000u;saved_animation=base_animation;return true;
    case 0x16f:*env.default_rate=floating(0).to_float();return true;
    case 0x170:{
        const auto difficulty=*env.difficulty;const auto frames=Extended::from_int(integer(difficulty>=0&&difficulty<3?difficulty:3)).to_float();
        context.time=Scalar::sub(context.time,frames);return true;
    }
    default:return false;
    }
}
}
