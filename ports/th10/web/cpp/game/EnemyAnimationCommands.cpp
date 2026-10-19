#include "EnemyAnimationEnvironment.hpp"
namespace th10 {
// Animation cases of 0x40e770. A script argument may pop the ECL stack, so the
// repeated reads in 0x103 must remain separate from the one-read 0x106 case.
bool EnemyState::animation_command(EclContext& context,EclGlobals& globals,EnemyAnimationEnvironment& env){
    const auto opcode=context.instruction->opcode;
    const auto integer=[&](u32 index){return context.integer_argument(index,globals);};
    const auto configure=[&](i32 slot){
        auto* vm=env.manager->registry.find_and_clear(animations[slot]);
        if(slot==0){visual_size.x=Scalar::mul(vm->sprite_size.y,vm->scale.y);visual_size.y=Scalar::mul(vm->sprite_size.x,vm->scale.x);}
        if(flags&0x10)env.manager->registry.set_visibility(animations[slot],false);
    };
    switch(opcode){
    case 0x102:animation_file=integer(0);break;
    case 0x103:{
        const auto slot=integer(0),initial_script=integer(1);env.manager->registry.delete_and_clear(animations[slot]);if(initial_script<0)break;
        const auto script=integer(1);animations[slot]=env.create(animation_file,script,5,AnimationPlacement::WorldFront);
        if(slot==0){animation_script=integer(1);bound_animation_file=animation_file;}configure(slot);break;
    }
    case 0x106:{
        const auto slot=integer(0),script=integer(1);env.manager->registry.delete_and_clear(animations[slot]);animations[slot]=env.create(animation_file,script,5,AnimationPlacement::WorldFront);configure(slot);
        if(slot==0){flags|=0x1000;base_animation=animation_script=script;direction=0;bound_animation_file=animation_file;}break;
    }
    case 0x10d:{
        const auto slot=integer(0);env.manager->registry.delete_and_clear(animations[slot]);animations[slot]=env.create(animation_file,wrapping_add(base_animation,5),5,AnimationPlacement::WorldFront);configure(slot);break;
    }
    case 0x107:case 0x108:case 0x110:case 0x111:{
        const auto file=integer(0),script=integer(1);auto id=env.create(file,script,6,opcode==0x110?AnimationPlacement::WorldBack:AnimationPlacement::WorldFront);if(opcode==0x108)break;
        auto* vm=env.manager->registry.find_and_clear(id);vm->position=current.position;
        if(!(flags&0x40000)){vm->position.x=Scalar::add(current.position.x,224.f);vm->position.y=Scalar::add(current.position.y,16.f);}
        if(opcode==0x111){vm->rotation.z=context.float_argument(2,globals).to_float();vm->flags|=4;}break;
    }
    default:return false;
    }
    return true;
}
}
