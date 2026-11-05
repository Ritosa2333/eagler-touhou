#include "EnemyAttributeCommands.hpp"
namespace th17 {
int EnemyAttributeCommands::execute(EclContext& context,u16 opcode){
    const auto* instruction=context.instruction;if(!instruction){context.error="Enemy instruction unavailable";return -2;}bool ok=true;
    auto arg=[&](u32 index){float value=0;if(20+index*4>instruction->length){context.error="Truncated enemy movement arguments";ok=false;}else if(!context.floating(index,instruction->argument<float>(index),value))ok=false;return value;};
    auto integer_arg=[&](u32 index){i32 value=0;if(20+index*4>instruction->length){context.error="Truncated enemy movement arguments";ok=false;}else if(!context.integer(index,instruction->argument<i32>(index),value))ok=false;return value;};
    switch(opcode){
        case 500:enemy.hitbox={arg(0),arg(1)};if(!enemy.chapter_contribution){enemy.chapter_contribution=1;world.chapter_total=wrapping_add(world.chapter_total,1);}break;
        case 501:enemy.hurtbox={arg(0),arg(1)};break;
        case 502:case 503:{const u32 flags=u32(integer_arg(0));if(!ok)return -2;if(opcode==502)enemy.flags|=flags;else enemy.flags&=~flags;const bool paused=(enemy.flags&32)!=0;if(paused==(opcode==502)){for(u32 handle:enemy.animation_handles)if(handle&&(!host||!host->pause_animation(handle,paused))){context.error="Enemy animation pause host unavailable";return -2;}}break;}
        case 506:enemy.item_drops={};break;
        case 507:{const i32 index=integer_arg(0),value=integer_arg(1);if(!ok)return -2;if(index<0||index>34){context.error="Enemy drop index outside resource range";return -2;}if(index==0)enemy.primary_drop=value;else enemy.item_drops[index-1]=value;break;}
        case 508:enemy.drop_radius={arg(0),arg(1)};break;
        case 510:enemy.primary_drop=integer_arg(0);break;
        case 511:{const i32 life=integer_arg(0);if(!ok)return -2;enemy.life=enemy.initial_life=enemy.phase_life=life;enemy.life_budget=wrapping_sub(wrapping_mul(life,8),life);if(enemy.flags&0x800000)enemy.flags|=0x40000000;break;}
        case 512:{const i32 slot=integer_arg(0);if(!ok)return -2;world.manager_flags&=~1u;if(slot<0){if(enemy.flags&0x800000){if(enemy.boss_slot<0||enemy.boss_slot>=3){context.error="Enemy Boss slot outside range";return -2;}world.boss_ids[enemy.boss_slot]=0;}enemy.flags&=~0x800000u;}else{if(slot>=3){context.error="Enemy Boss slot outside range";return -2;}enemy.flags|=0x800000;world.boss_ids[slot]=enemy.id;enemy.boss_slot=slot;}world.refresh_boss();break;}
        case 513:enemy.age_timer.set(0);break;
        case 514:case 521:case 556:{
            const auto read_name=[&](u32 offset,std::string& out){if(offset>=instruction->length){context.error="Truncated enemy script name";return false;}const char* data=reinterpret_cast<const char*>(instruction)+offset;u32 length=0;while(offset+length<instruction->length&&data[length])length++;if(offset+length==instruction->length||length>=64){context.error="Invalid enemy script name";return false;}out.assign(data,length);return true;};
            if(opcode==556){if(!read_name(20,enemy.death_script))return -2;break;}
            if(opcode==514){
                const i32 time=integer_arg(2),life=((world.mode_flags&48)==32&&(enemy.flags&0x800000))?0:integer_arg(1),slot=integer_arg(0);if(!ok)return -2;if(slot<0||slot>=8){context.error="Enemy interrupt index outside resource range";return -2;}auto& interrupt=enemy.interrupts[slot];interrupt.life=life;
                if(life>=0){interrupt.time=time;if((world.mode_flags&48)==32&&(enemy.flags&0x800000)){interrupt.script=world.stage_number==7&&world.current_chapter<41?(enemy.boss_slot==0?"MBossDead":"MBossBDead"):"BossDead";interrupt.timeout_script="BossEscape";}else{if(!read_name(32,interrupt.script))return -2;interrupt.timeout_script=interrupt.script;}}
            }else{const i32 slot=integer_arg(0);if(!ok)return -2;if(slot<0||slot>=8){context.error="Enemy interrupt index outside resource range";return -2;}if(!read_name(24,enemy.interrupts[slot].timeout_script))return -2;}break;
        }
        case 515:enemy.collision_timer.set(integer_arg(0));break;
        case 516:{const i32 id=integer_arg(0);if(!ok)return -2;if(!host||!host->sound(id,enemy.motion.position)){context.error="Enemy sound host unavailable";return -2;}break;}
        case 520:if(world.boss_at(0)||world.boss_at(1)||world.boss_at(2))return -1;break;
        case 541:enemy.invulnerability_timer.set(integer_arg(0));break;
        case 542:world.spell_flags()|=8;break;
        case 544:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.flags=(enemy.flags&~0x8000000u)|(value<<27&0x8000000u);break;}
        case 546:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.flags=(enemy.flags&~0x10000000u)|(value<<28&0x10000000u);const i32 script=integer_arg(1);if(!ok)return -2;enemy.flags&=~0x20000001u;enemy.inactive_animation=script;enemy.normal_animation=enemy.animation_base;break;}
        case 547:{const float value=arg(0);if(!ok)return -2;if(!world.frame_rate){context.error="Enemy shared frame clock unavailable";return -2;}*world.frame_rate=value;break;}
        case 548:{std::array<i32,4> delays;for(u32 i=0;i<4;i++)delays[i]=integer_arg(i);if(!ok)return -2;const i32 index=world.difficulty>=0&&world.difficulty<=2?world.difficulty:3;context.time=float(context.time-float(delays[index]));break;}
        case 549:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.flags=(enemy.flags&0x7fffffffu)|(value<<31);break;}
        case 553:enemy.hit_sound=integer_arg(0);break;
        case 555:{const u32 id=u32(integer_arg(1));if(!ok)return -2;auto* destination=context.integer_destination(0);if(!destination){context.error="Enemy lookup destination unavailable";return -2;}*destination=world.find(id)?1:0;break;}
        case 558:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.flags=(enemy.flags&~0x80000u)|(value<<19&0x80000u);break;}
        case 559:world.enemy_control=integer_arg(0);break;
        case 563:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.flags=(enemy.flags&~0x1000u)|(value<<12&0x1000u);break;}
        case 564:enemy.rotation_angle=arg(0);break;
        case 565:enemy.damage_multiplier=arg(0);break;
        case 568:{const u32 value=u32(integer_arg(0));if(!ok)return -2;enemy.life_flags=(enemy.life_flags&~1u)|(value&1u);break;}
        case 569:{const i32 previous=enemy.chapter_contribution;if(previous==0||previous==1){const i32 value=integer_arg(0);if(!ok)return -2;enemy.chapter_contribution=value;if(previous==1)world.chapter_total=wrapping_add(wrapping_add(world.chapter_total,-1),value);else if(value>0)world.chapter_total=wrapping_add(world.chapter_total,value);}break;}
        case 570:if(enemy.chapter_contribution&&enemy.chapter==world.current_chapter){world.chapter_defeated=wrapping_add(world.chapter_defeated,enemy.chapter_contribution);enemy.chapter_contribution=0;}break;
        case 535:{std::array<i32,4> values;for(u32 i=0;i<4;i++)values[i]=integer_arg(i+1);if(!ok)return -2;auto* destination=context.integer_destination(0);if(!destination){context.error="Enemy difficulty integer destination unavailable";return -2;}*destination=values[world.difficulty>=0&&world.difficulty<3?world.difficulty:3];break;}
        case 536:{std::array<float,4> values;for(u32 i=0;i<4;i++)values[i]=arg(i+1);if(!ok)return -2;auto* destination=context.float_destination(0);if(!destination){context.error="Enemy difficulty float destination unavailable";return -2;}*destination=values[world.difficulty>=0&&world.difficulty<3?world.difficulty:3];break;}
        case 572:{const i32 life=integer_arg(0);if(!ok)return -2;enemy.life=life;enemy.life_budget=wrapping_mul(life,7);break;}
        default:context.error="Unimplemented TH17 enemy attribute opcode "+std::to_string(opcode);return -2;
    }return ok?0:-2;
}
}
