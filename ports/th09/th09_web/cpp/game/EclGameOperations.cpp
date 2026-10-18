#include "EclGameOperations.hpp"
#include <algorithm>
namespace th09 {
bool EclGameOperations::execute(EclVm& vm,EclInstruction& i){
    const auto iv=[&](u32 n){return vm.integer(i,n);};const auto fv=[&](u32 n){return vm.real(i,n);};
    EclExecutor executor(manager.timing,manager.frame_step,manager.difficulty_mask,manager.bindings);
    switch(i.opcode){
    case 86:{
        i32 value=signed_bits(vm.raw(i,1));if(i.variable_mask&2){auto target=boss(iv(2));if(!target)return false;value=target->values.read_int(value);}
        auto p=vm.integer_target(i,0);if(p)*p=value;break;
    }
    case 87:{
        if(!boss(iv(2)))break;float value=vm.raw_float(i,1);
        if(i.variable_mask&2){auto target=boss(iv(2));if(!target)return false;value=target->values.read_float(value);}
        auto p=vm.float_target(i,0);if(p)*p=value;break;
    }
    case 88:{auto target=boss(iv(0));if(!target||!executor.call_from_other(*target,i16(vm.raw(i,1))))return false;break;}
    case 89:{
        if(!boss(iv(0)))break;const i16 sub=i16(iv(1));auto target=boss(iv(0));if(!target)return false;target->pending_interrupt=sub;break;
    }
    case 93:case 94:{
        if(vm.values.life<=0)break;EnemySpawn request;request.script=i16(vm.raw(i,0));request.character_program=1;
        request.position={fv(1),fv(2),fv(3)};
        if(i.opcode==94){const auto& p=vm.values.position;request.position={request.position.x+p.x,request.position.y+p.y,request.position.z+p.z};}
        request.score=iv(6);request.item=i8(iv(5));request.life=iv(4);
        manager.create(request,&vm.context().locals);break;
    }
    case 95:cancel_enemies(8000);break;
    case 124:scene.play_positioned_sound(iv(0),vm.values.position.x);break;
    case 127:{
        if(iv(0)<0){
            if(vm.values.boss_id>=manager.bosses.size())return false;manager.bosses[vm.values.boss_id]=nullptr;vm.behavior_flags&=~2u;
            scene.boss_indicator(vm.values.boss_id,2);scene.release_attached_effects(vm);vm.status.attached_effect_count=0;
            scene.boss_indicator_position(vm.values.boss_id,{-999,-999,0});
        }else {
            const i32 index=iv(0);if(index<0||u32(index)>=manager.bosses.size())return false;manager.bosses[index]=&vm;vm.behavior_flags|=2;
            vm.values.boss_id=u8(iv(0));scene.boss_indicator(vm.values.boss_id,1);vm.movement.player_protect_squared=0;
        }break;
    }
    case 139:case 140:{
        EclEffectRequest r;r.position=vm.values.position;r.explicit_velocity=i.opcode==140;
        if(r.explicit_velocity)r.velocity={fv(3),fv(4),fv(5)};
        r.count=iv(1);r.type=iv(0);auto color=vm.integer_target(i,2);if(!color)return false;r.color=u32(*color);scene.effect(r);break;
    }
    case 147:scene.scene_setting(iv(0));break;
    case 148:scene.add_script_extra_time(1800);break;
    case 163:manager.timeline_control=iv(0);break;
    case 175:manager.attack_control=iv(0);break;
    case 186:scene.end_attack((vm.values.flags>>10)&3);break;
    default:return false;
    }
    return !vm.invalid;
}
i32 EclGameOperations::cancel_enemies(i32 maximum_score,i32 total){
    i32 amount=2000;
    for(u32 index=0;index<EnemyManager::capacity;++index){auto& enemy=manager.enemies[index];const u32 flags=enemy.behavior_flags;
        if(!(flags&1)||(flags&2)||(enemy.values.flags&8))continue;enemy.values.life=0;
        if(flags&0x80){
            const auto& p=enemy.values.position;const auto& d=enemy.position_offset;enemy.values.resolved_position={p.x+d.x,p.y+d.y,p.z+d.z};
            const auto award=[&](const Vec3& position){scene.score_popup(position,amount,amount<maximum_score?0xffffffffu:0xffffff00u);total=wrapping_add(total,amount);amount=wrapping_add(amount,30);if(amount>maximum_score)amount=maximum_score;};
            award(enemy.values.resolved_position);
            if(enemy.trail.flags)for(i32 n=0;n<enemy.trail.length;n+=6){if(n>=i32(enemy.trail.history.size()))break;award(enemy.trail.history[n].position);}
        }
        if(enemy.status.death_subroutine>=0){EclExecutor(manager.timing,manager.frame_step,manager.difficulty_mask,manager.bindings).enter_main(enemy,enemy.status.death_subroutine);enemy.status.death_subroutine=-1;}
    }
    return total;
}
}
