#include "EclScene.hpp"
#include "GuiState.hpp"
#include "EffectState.hpp"
namespace th08 {
bool execute_enemy_scene(EclVm& vm,const EclInstruction& ins,const FrameTiming& timing){
    if(!vm.environment){vm.invalid=true;return false;}auto& globals=*vm.environment;
    const auto n=[&](u32 j){return vm.operand_int(ins,j);};const auto f=[&](u32 j){return vm.operand_float(ins,j);};
    u32 arguments=1;switch(ins.opcode){case 95:case 162:case 176:case 179:case 180:case 181:arguments=0;break;case 128:arguments=5;break;case 139:arguments=3;break;case 140:arguments=6;break;case 158:case 164:arguments=4;break;}
    if(ins.size<12+4*arguments){vm.invalid=true;return false;}
    if(ins.opcode==146){vm.context().timer.increment(n(0),timing);return true;}
    if(ins.opcode==147){globals.stage_interrupt=n(0);return true;}
    if(ins.opcode==148){const i32 lives=n(0);globals.boss_lives=lives;if(globals.gui)globals.gui->ecl_lives=lives;auto& frames=globals.frame_count_value?*globals.frame_count_value:globals.frame_count;frames=wrapping_add(frames,1800);return true;}
    if(ins.opcode==158){
        const i32 index=n(0),end=n(2),start=n(1);if(index<0||index>=8){vm.invalid=true;return false;}
        const float upper=(Extended::from_int(end)/Extended::from_int(vm.initial_life)).to_float(),lower=(Extended::from_int(start)/Extended::from_int(vm.initial_life)).to_float();
        if(globals.gui){globals.gui->segment_start[index]=lower;globals.gui->segment_end[index]=upper;}
        const i32 color=n(3);if(globals.gui)globals.gui->segment_colors[index]=color;return true;
    }
    if(ins.opcode==163){globals.timeline_parameter=n(0);return true;}
    if(ins.opcode==164){const i32 mode=n(0);globals.spell_flags=(globals.spell_flags&~64u)|((u32(mode)&1)<<6);if(!mode){const float z=f(3),y=f(2),x=f(1);if(!globals.spell_effect){vm.invalid=true;vm.failure=EclVm::Failure::MissingEffect;return false;}globals.spell_effect->position={x,y,z};}return true;}
    if(ins.opcode==184){globals.spell_flags=(globals.spell_flags&~2048u)|((u32(n(0))&1)<<11);return true;}
    if(ins.opcode==176){const auto spell=globals.current_spell;globals.game_flags=(globals.game_flags&~0x2180u)|0x80;if(globals.game_flags&0x4000){if((spell>=143&&spell<=146)||(spell>=171&&spell<=190))globals.game_flags|=0x2000;}else if(globals.stage==6||globals.stage==7)globals.game_flags|=0x2000;vm.flags|=0x40000000;return true;}
    auto* actions=globals.scene_actions;if(!actions){vm.invalid=true;vm.failure=EclVm::Failure::MissingSceneActions;return false;}
    switch(ins.opcode){
    case 128:{
        if(vm.effect_count<0||vm.effect_count>=24){vm.invalid=true;return false;}
        auto* effect=actions->attached_effect(13,vm.position,1,0xff6060d0,false);if(!effect){vm.invalid=true;vm.failure=EclVm::Failure::MissingEffect;return false;}
        vm.effects[vm.effect_count]=effect;const auto* args=reinterpret_cast<const u8*>(&ins)+12;
        std::memcpy(&effect->direction,args+4,sizeof(Vec3));std::memcpy(&vm.effect_radius,args+16,4);++vm.effect_count;break;
    }
    case 95:{i32 score=0;if(!actions->cancel_enemies(8000,score))vm.invalid=true;break;}
    case 124:actions->panned_sound(n(0),vm.position.x);break;
    case 139:case 140:{
        Vec3 parameters;if(ins.opcode==140)parameters={f(3),f(4),f(5)};
        const i32 count=n(1),kind=n(0);const auto* color=vm.integer_target(ins,2);if(!color)return false;
        if(ins.opcode==139)actions->effect(kind,vm.position,count,u32(*color));else actions->parameter_effect(kind,vm.position,parameters,count,u32(*color));break;
    }
    case 141:actions->item(vm.position,n(0),0);break;
    case 142:case 168:{
        if(!vm.random){vm.invalid=true;return false;}const i32 count=n(0);
        for(i32 j=0;j<count;++j){Vec3 p=vm.position;p.x=(vm.random->unit()*number(128)-number(64)+number(p.x)).to_float();p.y=(vm.random->unit()*number(128)-number(64)+number(p.y)).to_float();actions->item(p,ins.opcode==168||actions->power()>=128?1:j?0:2,0);}break;
    }
    case 161:actions->clear_projectiles_near(vm.resolved_position,f(0));break;
    case 162:if(!actions->clear_projectiles(4))vm.invalid=true;break;
    case 174:{
        if(vm.familiar_effect)static_cast<EffectState*>(vm.familiar_effect)->active=0;
        auto* effect=actions->attached_effect(wrapping_add(n(0),32),vm.resolved_position,1,0xffffffff,true);vm.familiar_effect=effect;
        if(!effect){vm.invalid=true;vm.failure=EclVm::Failure::MissingEffect;return false;}effect->pendingInterrupt=globals.youkai?2:1;if(vm.pool_index&1)effect->angleVel.z=-effect->angleVel.z;break;
    }
    case 179:if(!actions->clock(0))vm.invalid=true;break;
    case 180:if(!actions->clock(3))vm.invalid=true;break;
    case 181:if(!globals.values){vm.invalid=true;return false;}if(globals.values->clock_time<12){actions->sound(45,0);globals.values->clock_time=i8(u8(globals.values->clock_time)+1);if(!actions->clock(globals.values->clock_time==12?2:1))vm.invalid=true;}break;
    default:return false;
    }
    return !vm.invalid;
}
}
