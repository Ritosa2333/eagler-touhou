#include "LaserCommands.hpp"
namespace th15 {
int LaserCommands::execute(EclContext& context,u16 opcode){
    const auto* instruction=context.instruction;if(!instruction){context.error="Laser instruction unavailable";return -2;}
    i32 slot=0;if(instruction->length<20||!context.integer(0,instruction->argument<i32>(0),slot))return -2;
    if((opcode>=704&&opcode<=710)||opcode==714){
        if(!scene){context.error="Laser scene unavailable for attribute command";return -2;}const u32 id=u32(slot);const bool found=scene->manager.find(id)!=nullptr;
        const auto scalar=[&](u32 index,float& value){return instruction->length>=20+index*4&&context.floating(index,instruction->argument<float>(index),value);};
        bool ok=true;float first=0,second=0;
        if(opcode==704||opcode==705){if(!scalar(1,first)||!scalar(2,second)){context.error="Invalid laser position/vector arguments";return -2;}ok=opcode==704?scene->set_position(id,{first,second,0}):scene->set_script_vector(id,{first,second,0});}
        else if(opcode==710)ok=scene->cancel_identifier(id);
        else if(found){if(opcode==714){i32 ignored=0;if(instruction->length<24||!context.integer(1,instruction->argument<i32>(1),ignored))return -2;/* The original moving/stationary callback is a no-op. */}
            else{if(!scalar(1,first)){context.error="Invalid laser scalar argument";return -2;}ok=opcode==706?scene->set_speed(id,first):opcode==707?scene->set_width(id,first):opcode==708?scene->set_angle(id,first):scene->set_script_angular_velocity(id,first);}}
        if(!ok){context.error=scene->error;return -2;}return 0;
    }
    if(slot<0||slot>=16){context.error="Laser shooter index outside resource range";return -2;}
    const auto& shooters=enemy.shooters;const auto& shot=shooters.shooters[u32(slot)];const auto& offset=shooters.offset[u32(slot)];const auto& origin=shooters.origin[u32(slot)];
    const Vec3 position=origin.z>.9f?Vec3{float(offset.x+origin.x),float(offset.y+origin.y),0}:Vec3{float(enemy.motion.position.x+offset.x),float(enemy.motion.position.y+offset.y),float(enemy.motion.position.z+offset.z)};
    if(opcode==702){MovingLaserRequest request;request.position=position;request.angle=normalize_angle(shot.angle);request.maximum_length=shot.position.y;request.initial_length=shot.position.x;request.end_distance=shot.position.z;request.width=float_from_bits(shot.extra[0]);request.speed=shot.speed;request.type=shot.sprite;request.color=shot.color;request.start_offset=shot.radius;request.transform_parameter=signed_bits(shot.extra[8]|1u);request.transforms=shot.transforms;request.sound=shot.shoot_sound;request.reflection_sound=shot.transform_sound;
        if(host?host->emit(request):scene?(scene->create_moving(request)!=0||scene->error.empty()):false)return 0;
    }else if(opcode==703){i32 id=0;if(instruction->length<24||!context.integer(1,instruction->argument<i32>(1),id))return -2;StationaryLaserRequest request;request.position=position;request.angle=normalize_angle(shot.angle);request.maximum_length=shot.position.y;request.initial_length=shot.position.x;request.width=float_from_bits(shot.extra[0]);request.speed=shot.speed;request.delay=signed_bits(shot.extra[4]);request.warmup=signed_bits(shot.extra[5]);request.active=signed_bits(shot.extra[6]);request.fade=signed_bits(shot.extra[7]);request.flags=shot.extra[8]|2;request.id=u32(id);request.type=shot.sprite;request.color=shot.color;request.start_offset=shot.radius;request.transforms=shot.transforms;request.sound=shot.shoot_sound;request.transform_sound=shot.transform_sound;
        if(host?host->emit(request):scene?(scene->create_stationary(request)!=0||scene->error.empty()):false)return 0;
    }else if(opcode==711){CurveLaserRequest request;request.position=position;request.angle=normalize_angle(shot.angle);request.width=float_from_bits(shot.extra[0]);request.speed=shot.speed;request.type=shot.sprite;request.color=shot.color;request.count=shot.extra[4];request.start_offset=shot.radius;request.flags=1;request.transforms=shot.transforms;request.sound=shot.shoot_sound;request.transform_sound=shot.transform_sound;
        if(host?host->emit(request):scene?(scene->create_curved(request)!=0||scene->error.empty()):false)return 0;
    }else if(opcode==713){i32 id=0;if(instruction->length<24||!context.integer(1,instruction->argument<i32>(1),id))return -2;SegmentedLaserRequest request;request.position=position;request.angle=normalize_angle(shot.angle);request.length=shot.position.z;request.width=float_from_bits(shot.extra[0]);request.id=u32(id);request.color=shot.color;request.start_offset=shot.radius;request.flags=shot.extra[4];request.transforms=shot.transforms;
        if(host?host->emit(request):scene?(scene->create_segmented(request)!=0||scene->error.empty()):false)return 0;
    }else{context.error="Unimplemented laser ECL opcode "+std::to_string(opcode);return -2;}
    context.error=scene&&!scene->error.empty()?scene->error:"Laser emission host unavailable";return -2;
}
}
