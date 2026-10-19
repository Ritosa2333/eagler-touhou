#include "Stage.hpp"
#include "GameMath.hpp"
namespace th10 {
namespace {
void set_timer(Timer& timer,u32& flags,float* rate,i32 frame){if(!(flags&1)){timer.rate=rate;flags|=1;}timer.current=frame;timer.previous=wrapping_add(frame,-1);timer.fractional=Extended::from_int(frame).to_float();}
Vec3 vector(const StageInstruction& instruction,u32 index){return {instruction.argument<float>(index),instruction.argument<float>(index+1),instruction.argument<float>(index+2)};}
void vector_interpolation(Vec3Interpolator& value,const Vec3& current,const StageInstruction& instruction,float* rate,bool hermite){
    value.duration=instruction.argument<i32>(0);value.mode=hermite?InterpolationMode::Hermite:instruction.argument<InterpolationMode>(1);std::memcpy(value.start,&current,12);
    const auto end=vector(instruction,hermite?5:2);std::memcpy(value.end,&end,12);
    if(hermite){const auto initial=vector(instruction,2),final=vector(instruction,8);std::memcpy(value.initial_tangent,&initial,12);std::memcpy(value.final_tangent,&final,12);}
    set_timer(value.timer,value.flags,rate,0);
}
}
// 0x403c80. STD control flow is independent of ECL: signed frame scheduling,
// byte-offset jumps, camera interpolation, fog and eight embedded animations.
i32 Stage::update_script(StageEnvironment& environment){
    while(instruction->time<=script_timer.current){
        const auto& command=*instruction;
        switch(command.opcode){
        case 0:goto interpolate;
        case 1:set_timer(script_timer,script_timer_flags,environment.rate,command.argument<i32>(1));instruction=reinterpret_cast<StageInstruction*>(reinterpret_cast<u8*>(script_begin)+instruction->argument<i32>(0));continue;
        case 2:{const auto previous=camera.position;camera.position=vector(command,0);camera.animation_delta={Scalar::sub(camera.position.x,previous.x),Scalar::sub(camera.position.y,previous.y),Scalar::sub(camera.position.z,previous.z)};break;}
        case 3:vector_interpolation(position_interpolation,camera.position,command,environment.rate,false);break;
        case 4:camera.target_offset=vector(command,0);break;
        case 5:vector_interpolation(target_interpolation,camera.target_offset,command,environment.rate,false);break;
        case 6:camera.up=vector(command,0);break;
        case 7:camera.field_of_view=command.argument<float>(0);break;
        case 8:{const auto color=command.argument<u32>(0);camera.fog.packed_color=color;for(u32 i=0;i<4;++i)camera.fog.color[i]=static_cast<float>((color>>(8*i))&255);camera.fog.near_distance=command.argument<float>(1);camera.fog.far_distance=command.argument<float>(2);break;}
        case 9:{const auto color=command.argument<u32>(2);AnmFog end{command.argument<float>(3),command.argument<float>(4),{},color};for(u32 i=0;i<4;++i)end.color[i]=static_cast<float>((color>>(8*i))&255);fog_interpolation.duration=command.argument<i32>(0);fog_interpolation.start=camera.fog;fog_interpolation.end=end;fog_interpolation.mode=command.argument<InterpolationMode>(1);set_timer(fog_interpolation.timer,fog_interpolation.flags,environment.rate,0);break;}
        case 10:vector_interpolation(position_interpolation,camera.position,command,environment.rate,true);break;
        case 11:vector_interpolation(target_interpolation,camera.target_offset,command,environment.rate,true);break;
        case 12:camera_effect=command.argument<u32>(0);set_timer(effect_timer,effect_timer_flags,environment.rate,0);break;
        case 13:*environment.background_color=command.argument<u32>(0);break;
        case 14:{auto& vm=script_animations[command.argument<i32>(0)];const auto script=command.argument<i32>(1);if(script<0)vm.flags&=~1u;else environment.initialize_animation(*animation_file,vm,script);break;}
        }
        instruction=instruction->next();
    }
    script_timer.tick();
interpolate:
    if(target_interpolation.duration)camera.target_offset=sample(target_interpolation,environment.rate);
    if(position_interpolation.duration)camera.position=sample(position_interpolation,environment.rate);
    if(fog_interpolation.duration)camera.fog=sample(fog_interpolation,environment.rate);
    if(camera_effect==1){camera.eye_offset.x=(sine(number(effect_timer.fractional)*number(0x1.921fb6p-7f)-number(0x1.921fb6p+1f))*number(-100)).to_float();effect_timer.tick();if(effect_timer.current>=512)set_timer(effect_timer,effect_timer_flags,environment.rate,0);}
    return 0;
}
// 0x403990. An object's animation-active bit clears only after all of its
// primitive scripts have stopped, including primitives of unknown draw types.
i32 Stage::update_objects(StageEnvironment& environment){
    for(i32 index=0;index<file->object_count;++index){auto& object=*objects[index];if(!(object.flags&1))continue;i32 running=0;
        for(auto* primitive=object.primitives();primitive->type>=0;primitive=primitive->next()){auto& vm=object_animations[primitive->animation];environment.update_animation(vm);if(vm.instruction)++running;}
        if(!running)object.flags&=~1u;
    }return 0;
}
// 0x402720. Scripted camera displacement resets each frame before STD runs.
i32 Stage::update(StageEnvironment& environment){
    if(!(draw_flags&8)&&(!(draw_flags&4)||fade_timer.current<60)){
        camera.animation_delta={0,0,0};camera.draw_offset={0,0};environment.normalize(camera.reserved_024,camera.target_offset);fade_color=0x00808080;
        update_objects(environment);update_script(environment);for(auto& vm:script_animations)environment.update_animation(vm);
        const auto rate=*environment.rate;if(frame_effect){*environment.rate=1;for(auto& vm:effect_animations)environment.update_animation(vm);}*environment.rate=rate;frame_effect=0;
        *environment.world=camera;++frame_count;
    }return 1;
}
// 0x404450. Each primitive receives its stable animation-array index.
void Stage::restart(StageEnvironment& environment){
    update_entry->flags|=2;draw_entry->flags|=2;foreground_entry->flags|=2;i32 animation=0;
    for(i32 index=0;index<file->object_count;++index){auto& object=*objects[index];object.flags=1;for(auto* primitive=object.primitives();primitive->type>=0;primitive=primitive->next()){
        environment.initialize_animation(*animation_file,object_animations[animation],primitive->script);primitive->animation=static_cast<std::int16_t>(animation++);
    }}instruction=script_begin;
}
void Stage::initialize() noexcept {std::memset(this,0,sizeof(*this));flags=2;}
}
