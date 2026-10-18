#include "EnemySceneEnvironment.hpp"
namespace th10 {
// Remaining scene-control cases of 0x40e770. Rendering, dialogue and spell
// presentation are typed services until those systems have been recovered.
bool EnemyState::scene_command(EclContext& context,EclGlobals& globals,EnemySceneEnvironment& env){
    const auto opcode=context.instruction->opcode;const auto integer=[&](u32 index){return context.integer_argument(index,globals);};
    switch(opcode){
    case 0x151:{const auto third=integer(2),second=integer(1),first=integer(0);env.screen_effect(first,second,third);break;}
    case 0x152:{const auto id=integer(0);env.start_dialogue(id);env.cancel_projectiles();env.clear_enemies();break;}
    case 0x156:case 0x15c:case 0x165:case 0x166:case 0x167:{
        const auto* instruction=context.instruction;const auto length=static_cast<i32>(instruction->argument(3));
        // Names in the original spell record occupy 64 bytes. Reject malformed
        // oversized script data instead of corrupting the C++ host stack.
        if(length<0||length>64)__builtin_trap();char name[65]{};u8 key=0x77,delta=7;
        for(i32 i=0;i<length;++i){name[i]=reinterpret_cast<const u8*>(instruction)[32+i]^key;key+=delta;delta+=0x10;}
        // The ECL id selects the localized name (spells.etl); the game then
        // offsets it per difficulty to index the per-record capture table.
        const auto name_id=integer(0);auto id=name_id;const auto variant=context.instruction->opcode;
        if(variant>=0x165&&variant<=0x167)id=wrapping_add(id,wrapping_add(*env.difficulty,0x165-variant));
        (void)integer(2);const auto parameter=integer(1);env.start_spell(id,name_id,name,parameter);break;
    }
    case 0x157:env.end_spell();break;
    case 0x158:env.game->select_section(integer(0));break;
    case 0x159:env.clear_enemies();break;
    case 0x16b:*env.spell_flags|=0x10;env.registry->delete_and_clear(*env.spell_bonus_animation);break;
    case 0x16d:env.delete_lasers();break;
    default:return false;
    }
    return true;
}
}
