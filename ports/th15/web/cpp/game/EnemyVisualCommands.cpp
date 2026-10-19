#include "EnemyVisualCommands.hpp"
namespace th15 {
namespace {
std::array<i32,3> rgb(u32 value){return {i32(value&255),i32(value>>8&255),i32(value>>16&255)};}
}
int EnemyVisualCommands::execute(EclContext& context,u16 opcode){
    const auto* instruction=context.instruction;bool ok=instruction!=nullptr;
    auto integer=[&](u32 index){i32 value=0;if(!instruction||20+index*4>instruction->length||!context.integer(index,instruction->argument<i32>(index),value))ok=false;return value;};
    auto number=[&](u32 index){float value=0;if(!instruction||20+index*4>instruction->length||!context.floating(index,instruction->argument<float>(index),value))ok=false;return value;};
    auto failure=[&](){if(context.error.empty()){context.error="Enemy animation command failed (opcode "+std::to_string(opcode)+")";if(host)if(const auto* detail=host->failure())context.error+=": "+*detail;}return -2;};
    if(opcode==302){enemy.selected_animation_resource=integer(0);return ok?0:failure();}
    if(opcode==552){enemy.animation_layer=integer(0);return ok?0:failure();}
    if(opcode==550){enemy.animation_rotation_mode=integer(0);return ok?0:failure();}
    if(opcode==320){const i32 slot=integer(0);const float x=number(1),y=number(2);if(!ok)return failure();if(slot<0||slot>=16){context.error="Enemy animation offset slot outside range";return -2;}enemy.animation_offsets[slot]={x,y,0};return 0;}
    if(opcode==322){const i32 parent=integer(1),slot=integer(0);if(!ok)return failure();if(slot<0||slot>=16){context.error="Enemy animation parent slot outside range";return -2;}enemy.animation_parents[slot]=parent;return 0;}
    const i32 slot=opcode==318?0:integer(0);if(!ok)return failure();
    const bool property=opcode==319||(opcode>=325&&opcode<=333)||opcode==335||opcode==336;
    if(slot<0||slot>=16){if(property)return 0;context.error="Enemy animation slot outside range";return -2;}
    if(!host){context.error="Enemy animation host unavailable";return -2;}
    auto& handle=enemy.animation_handles[slot];
    if(opcode==317){const i32 label=integer(1);if(!ok)return failure();return host->interrupt(handle,i32(i16(label)))?0:failure();}
    if(opcode==303||opcode==306||opcode==313||opcode==316||opcode==318){
        const i32 parameter=opcode==303||opcode==306||opcode==316?integer(1):0;if(!ok)return failure();
        if(!host->retire(handle))return failure();
        if(opcode==303&&parameter<0)return 0;
        i32 script=opcode==303?integer(1):opcode==306?parameter:opcode==313?wrapping_add(enemy.animation_base,5):opcode==316&&parameter>=0?wrapping_add(wrapping_add(enemy.animation_base,parameter),5):enemy.animation_base;
        if(!ok||!host->create(handle,enemy.selected_animation_resource,script,wrapping_add(enemy.animation_layer,7)))return failure();
        if(opcode==303&&slot==0){enemy.animation_script=integer(1);enemy.animation_resource=enemy.selected_animation_resource;}
        if(!ok)return failure();
        if(slot==0&&opcode!=318){Vec2 size;if(!host->size(handle,size)){context.error="Created enemy animation unavailable";return -2;}enemy.sprite_size=size;}
        if((enemy.flags&32)&&opcode!=318&&!host->pause(handle))return failure();
        if(opcode==306&&slot==0){enemy.flags|=0x100000;enemy.animation_base=enemy.animation_script=parameter;enemy.animation_resource=enemy.selected_animation_resource;enemy.movement_direction=0;}
        if(opcode==318){enemy.flags&=~0x100000u;enemy.animation_script=enemy.animation_base;enemy.movement_direction=0;enemy.animation_resource=enemy.selected_animation_resource;}
        return 0;
    }
    auto* vm=host->find(handle);if(!vm){handle=0;return 0;}auto& visual=vm->visual;auto& curves=vm->interpolators;
    switch(opcode){
        case 319:vm->variables.rotation.z=number(1);visual.flags|=4;break;
        case 325:{const u32 b=u32(integer(3))&255,g=u32(integer(2))&255,r=u32(integer(1))&255;visual.color=(visual.color&0xff000000)|(r<<16)|(g<<8)|b;break;}
        case 326:{const i32 r=integer(3)&255,g=integer(4)&255,b=integer(5)&255,mode=integer(2),duration=integer(1);auto& curve=curves.color;curve.control1=curve.control2={};curve.begin(duration,mode,rgb(visual.color),{b,g,r});break;}
        case 327:visual.color=(visual.color&0xffffff)|(u32(integer(1))<<24);break;
        case 328:{const i32 value=integer(3)&255,mode=integer(2),duration=integer(1);auto& curve=curves.alpha;curve.control1=curve.control2={};curve.begin(duration,mode,{i32(visual.color>>24)},{value});break;}
        case 329:{const float y=number(2),x=number(1);visual.scale={x,y};visual.flags|=8;break;}
        case 330:{const float y=number(4),x=number(3);const i32 mode=integer(2),duration=integer(1);curves.scale.begin(duration,mode,{visual.scale.x,visual.scale.y},{x,y});break;}
        case 331:visual.secondary_color=(visual.secondary_color&0xffffff)|(u32(integer(1))<<24);break;
        case 332:{const i32 value=integer(3)&255,mode=integer(2),duration=integer(1);curves.secondary_alpha.begin(duration,mode,{i32(visual.secondary_color>>24)},{value});visual.flags=(visual.flags&~0x40000u)|0x20000;break;}
        case 333:{const float y=number(4),x=number(3);const i32 repeated_slot=integer(0);if(!ok)return failure();if(repeated_slot<0||repeated_slot>=16){context.error="Enemy position animation slot outside range";return -2;}auto* target=host->find(enemy.animation_handles[repeated_slot]);if(!target){context.error="Enemy position animation unavailable";return -2;}const i32 mode=integer(2),duration=integer(1);auto& curve=curves.position;curve.control1=curve.control2={};curve.begin(duration,mode,{target->visual.translation.x,target->visual.translation.y,target->visual.translation.z},{x,y,0});break;}
        case 335:{const float y=number(2),x=number(1);visual.secondary_scale={x,y};visual.flags|=8;break;}
        case 336:visual.set_layer(integer(1));break;
        default:context.error="Unimplemented enemy animation command";return -2;
    }return ok?0:failure();
}
}
