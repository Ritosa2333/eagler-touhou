#include "EnemySceneCommands.hpp"
namespace th17 {
bool EnemySceneCommands::handles(u16 op)noexcept{
    switch(op){case 307:case 308:case 334:case 509:case 517:case 518:case 519:case 522:case 523:case 524:case 525:case 527:case 537:case 538:case 539:case 540:case 545:case 554:case 560:case 562:case 571:case 613:case 615:case 616:case 629:case 630:case 632:case 637:case 639:case 901:case 902:case 904:case 1001:return true;default:return false;}
}
int EnemySceneCommands::execute(EclContext& context,u16 op){
    const auto* instruction=context.instruction;if(!instruction){context.error="Enemy scene instruction unavailable";return -2;}bool ok=true;
    const auto integer=[&](u32 i){i32 value=0;if(u64(20)+u64(i)*4>instruction->length||!context.integer(i,instruction->argument<i32>(i),value))ok=false;return value;};
    const auto number=[&](u32 i){float value=0;if(u64(20)+u64(i)*4>instruction->length||!context.floating(i,instruction->argument<float>(i),value))ok=false;return value;};
    const auto fail=[&](const char* message){if(context.error.empty())context.error=message;return -2;};
    switch(op){
    case 307:case 308:{EnemyEffectRequest request;if(op==307){request.script=integer(1);request.resource=integer(0);request.position=enemy.motion.position;}else{request.resource=integer(0);request.script=integer(1);}if(!ok||!host||!host->effect(request))return fail("Enemy scene effect unavailable");break;}
    case 334:{const i32 script=integer(0);if(!ok||!host||!host->tracked_effect(script,enemy.motion.position))return fail("Enemy tracked effect unavailable");break;}
    case 509:case 562:if(op==509&&(world.mode_flags&48)==32)break;if(!host||!host->drop_items(enemy))return fail("Enemy immediate item drops unavailable");break;
    case 517:{const i32 last=integer(2),first=integer(1),duration=integer(0);if(!ok||!host||!host->nudge(duration,first,last))return fail("Enemy screen nudge unavailable");break;}
    case 518:{const i32 id=integer(0);if(!ok||!host||!host->scene_message(id)||!host->cancel_all_bullets(0)||!host->cancel_all_lasers(0,false)||!spawning||!spawning->clear_field(false))return fail("Enemy dialogue and field clear unavailable");break;}
    case 519:{bool complete=false;if(!host||!host->message_complete(complete))return fail("Enemy dialogue state unavailable");if(!complete)return -1;break;}
    case 522:case 537:case 538:case 539:{
        if(instruction->length<32)return fail("Truncated spell name");const u32 length=instruction->argument<u32>(3);if(!length||length>128||length>instruction->length-32)return fail("Invalid spell name length");
        SpellStartRequest request;u8 key=0x77,step=7;const auto* source=reinterpret_cast<const u8*>(instruction)+32;request.name.resize(length);
        for(u32 i=0;i<length;i++){request.name[i]=char(source[i]^key);key=u8(key+step);step=u8(step+16);}const auto end=request.name.find('\0');if(end==std::string::npos)return fail("Unterminated spell name");request.name.resize(end);
        request.id=integer(0);if(op!=522)request.id=wrapping_add(request.id,wrapping_add(world.difficulty,537-i32(op)));
        request.mode=integer(2);request.time=integer(1);if(!ok||!host||!host->begin_spell(request))return fail("Enemy spell start unavailable");
        enemy.life_flags|=1;enemy.life_budget=wrapping_mul(enemy.life,7);enemy.age_timer.set(0);break;
    }
    case 523:if(!host||!host->finish_spell())return fail("Enemy spell finish unavailable");enemy.life_flags&=~1u;break;
    case 524:{const i32 id=integer(0);if(!ok||!host||!host->chapter(id))return fail("Enemy chapter change unavailable");world.current_chapter=id;enemy.chapter=id;enemy.chapter_contribution=0;break;}
    case 525:case 571:if(!spawning||!spawning->clear_field(op==571))return fail("Enemy field clear unavailable");break;
    case 527:{const u32 color=u32(integer(2));const float fraction=float(number(1)/float(enemy.initial_life));const i32 index=integer(0);if(!ok||!host||!host->boss_segment(enemy.boss_slot,index,fraction,color))return fail("Boss health segment unavailable");break;}
    case 540:{const i32 count=integer(0);if(!ok||!host||!host->boss_segments(count))return fail("Boss health display unavailable");break;}
    case 545:if(!host||!host->clear_lasers_with_rewards())return fail("Enemy laser field clear unavailable");break;
    case 554:if(!host||!host->stage_logo())return fail("Stage logo unavailable");break;
    case 560:{const float y=number(1),x=number(0);if(!ok||!host||!host->bullet_bounds({x,y}))return fail("Bullet field bounds unavailable");break;}
    case 613:if(!host||!host->cancel_all_bullets(0)||!host->cancel_all_lasers(1,false))return fail("Enemy field cancellation unavailable");break;
    // Native 0x419cf0 clears protected bullets too; laser protection is separate.
    case 615:case 616:{const float radius=number(0);const i32 reward=op==615?1:0;if(!ok||!host||!host->cancel_bullets_circle(enemy.motion.position,radius,reward,false)||!host->cancel_lasers_circle(enemy.motion.position,radius,reward,true))return fail("Enemy circular cancellation unavailable");break;}
    case 630:{const i32 label=integer(0);if(!ok||!host||!host->background_interrupt(label))return fail("Stage background interrupt unavailable");break;}
    case 629:{auto& state=enemy.distortion;if(state.active&&(!host||!host->retire_distortion(enemy)))return fail("Enemy distortion retirement unavailable");state.active=false;state.target=number(0);state.radius=16;state.color=u32(integer(1));state.phase_x=state.phase_y=0;if(!ok)return fail("Invalid enemy distortion parameters");if(state.target>0){if(!host||!host->begin_distortion(enemy))return fail("Enemy distortion creation unavailable");state.active=true;}break;}
    case 632:case 639:case 637:{const i32 kind=integer(0);if(!ok||kind<0||kind>1)return fail("Invalid TH17 enemy update callback");const auto rule=EnemyUpdateRule(kind);if(op==637){if(!host||!host->immediate_update(enemy,rule))return fail("Immediate enemy update callback unavailable");}else{enemy.update_rule=rule;enemy.update_rule_state=op==639?1:0;}break;}
    case 1001:{const u32 value=u32(integer(0));if(!ok)return fail("Invalid Roaring spell policy");auto& flags=world.roar_flags();flags=(flags&~0x800u)|(value<<11&0x800u);break;}
    // These opcode numbers fall through the native TH17 entity dispatcher.
    case 901:case 902:case 904:break;
    default:return fail("Unsupported TH17 enemy scene opcode");
    }return 0;
}
}
