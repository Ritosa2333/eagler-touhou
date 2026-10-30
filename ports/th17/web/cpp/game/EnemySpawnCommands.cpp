#include "EnemySpawnCommands.hpp"
namespace th17 {
int EnemySpawnCommands::execute(EclContext& context,u16 opcode){
    const auto* instruction=context.instruction;if(!instruction){context.error="Enemy instruction unavailable";return -2;}bool ok=true;
    auto arg=[&](u32 index){float value=0;if(20+index*4>instruction->length){context.error="Truncated enemy movement arguments";ok=false;}else if(!context.floating(index,instruction->argument<float>(index),value))ok=false;return value;};
    auto integer_arg=[&](u32 index){i32 value=0;if(20+index*4>instruction->length){context.error="Truncated enemy movement arguments";ok=false;}else if(!context.integer(index,instruction->argument<i32>(index),value))ok=false;return value;};
    switch(opcode){
        case 300:case 301:case 304:case 305:case 309:case 310:case 311:case 312:case 321:{
            if(opcode>=309&&opcode<=312){if(!spawning){context.error="Enemy spawn host unavailable";return -2;}if(spawning->boss_exists(0))break;}
            if(world.enemy_count>=world.enemy_control)break;
            if(instruction->length<20){context.error="Truncated enemy spawn name";return -2;}
            const u32 name_size=instruction->argument<u32>(0);const auto* bytes=reinterpret_cast<const u8*>(instruction);
            if(!name_size||name_size%4||name_size>instruction->length-20||!std::memchr(bytes+20,0,name_size)||u64(20)+name_size+20>instruction->length){context.error="Invalid enemy spawn arguments";return -2;}
            EnemySpawnRequest request;request.routine=reinterpret_cast<const char*>(bytes+20);const u32 offset=20+name_size;
            auto number=[&](u32 index){float literal,value=0;std::memcpy(&literal,bytes+offset+(index-1)*4,4);if(!context.floating(index,literal,value))ok=false;return value;};
            auto value=[&](u32 index){i32 literal,result=0;std::memcpy(&literal,bytes+offset+(index-1)*4,4);if(!context.integer(index,literal,result))ok=false;return result;};
            request.position={number(1),number(2),0};
            if(opcode==300||opcode==304||opcode==309||opcode==311||opcode==321){request.position.x=float(request.position.x+enemy.motion.position.x);request.position.y=float(request.position.y+enemy.motion.position.y);}
            request.mirrored=opcode==304||opcode==305||opcode==311||opcode==312;
            if(enemy.flags&0x80000){request.position.x=-request.position.x;request.mirrored=!request.mirrored;}
            request.life=value(3);request.score=value(4);request.item=value(5);request.integers=enemy.integer_registers;request.floats=enemy.float_registers;request.temporary=enemy.temporary_registers;request.parent=enemy.id;
            if(!ok)return -2;if(!spawning||!spawning->spawn_at_rate(request,context.current_interpolation_rate())){const auto* detail=spawning?spawning->failure():nullptr;context.error=detail?*detail:"Enemy creation failed";return -2;}break;
        }
        default:context.error="Unimplemented TH17 enemy creation opcode "+std::to_string(opcode);return -2;
    }return ok?0:-2;
}
}
