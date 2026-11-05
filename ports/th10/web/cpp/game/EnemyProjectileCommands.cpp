#include "EnemyProjectileEnvironment.hpp"
#include "GameMath.hpp"
namespace th10 {
namespace {
Vec3 position(const EnemyState& enemy,i32 index){const auto& offset=enemy.emitter_offsets[index];return {Scalar::add(offset.x,enemy.current.position.x),Scalar::add(offset.y,enemy.current.position.y),Scalar::add(offset.z,enemy.current.position.z)};}
u32 rank_band(i32 rank,bool five){return five?(rank>=600?4:rank>=200?3:rank>=-200?2:rank>=-600?1:0):(rank>=512?2:rank>=-512?1:0);}
u32 difficulty_band(i32 value){return value>=0&&value<3?value:3;}
std::int16_t integer_blend(i32 start,i32 end,i32 rank){const i32 product=static_cast<i32>((static_cast<u32>(end)-static_cast<u32>(start))*(static_cast<u32>(rank)+1024));return static_cast<std::int16_t>(wrapping_add(product/2048,start));}
}
// 0x190..0x1b4 cases of 0x40e770. Argument evaluation is deliberately ordered:
// ECL stack references consume values and may alias the destinations below.
bool EnemyState::projectile_command(EclContext& context,EclGlobals& globals,EnemyProjectileEnvironment& env){
    const auto opcode=context.instruction->opcode;if(opcode<0x190||opcode>0x1b4)return false;
    const auto integer=[&](u32 index){return context.integer_argument(index,globals);};
    const auto floating=[&](u32 index){return context.float_argument(index,globals);};
    switch(opcode){
    case 0x190:{const auto index=integer(0);auto& e=emitters[index];e.initialize();e.count=e.layers=1;e.speed_start=2;e.shoot_sound=7;e.turn_sound=24;e.flags=0x203;emitter_offsets[index].x=emitter_offsets[index].y=0;break;}
    case 0x191:{
        const auto index=integer(0);auto& e=emitters[index];e.position=position(*this,index);const auto limit=number(shot_exclusion_distance_squared);
        const auto x=number(e.position.x)-number(env.player_position->x),y=number(e.position.y)-number(env.player_position->y),distance=x*x+y*y;
        if(shot_exclusion_distance_squared==0.f||limit<distance||limit==distance)env.fire(e);break;
    }
    case 0x192:{auto& e=emitters[integer(0)];e.sprite_type=static_cast<std::int16_t>(integer(1));e.color=static_cast<std::int16_t>(integer(2));break;}
    case 0x193:{auto& offset=emitter_offsets[integer(0)];offset.x=floating(1).to_float();offset.y=floating(2).to_float();break;}
    case 0x194:{auto& e=emitters[integer(0)];e.angle=floating(1).to_float();e.spread=floating(2).to_float();break;}
    case 0x195:{auto& e=emitters[integer(0)];e.speed_start=floating(1).to_float();e.speed_end=floating(2).to_float();break;}
    case 0x196:{auto& e=emitters[integer(0)];e.count=static_cast<std::int16_t>(integer(1));e.layers=static_cast<std::int16_t>(integer(2));break;}
    case 0x197:{auto& e=emitters[integer(0)];e.pattern=static_cast<std::int16_t>(integer(1));break;}
    case 0x198:{auto& e=emitters[integer(0)];e.shoot_sound=integer(1);e.turn_sound=integer(2);break;}
    case 0x199:{
        const auto emitter=integer(0),index=integer(1);auto& command=emitters[emitter].commands[index];command.concurrent=integer(2);command.type=integer(3);command.arguments[2]=integer(4);command.arguments[3]=integer(5);
        const float first=floating(6).to_float();__builtin_memcpy(command.arguments,&first,4);const float second=floating(7).to_float();__builtin_memcpy(command.arguments+1,&second,4);break;
    }
    case 0x19a:env.cancel_rectangle(true);break;
    case 0x19b:{const auto source=integer(1),destination=integer(0);if(source!=destination)__builtin_memcpy(&emitters[destination],&emitters[source],sizeof(BulletEmitter));break;}
    case 0x19c:case 0x1ac:case 0x1af:case 0x1b1:{
        StraightLaserParameters parameters{};if(opcode==0x1af||opcode==0x1b1)__builtin_memcpy(parameters.commands,emitters[0].commands,sizeof(parameters.commands));parameters.position=position(*this,0);
        parameters.sprite_type=static_cast<std::int16_t>(integer(0));parameters.color=static_cast<std::int16_t>(integer(1));parameters.angle=floating(2).to_float();parameters.speed=floating(3).to_float();
        parameters.initial_length=floating(4).to_float();parameters.target_length=floating(5).to_float();parameters.maximum_distance=floating(6).to_float();parameters.width=floating(7).to_float();parameters.flags=opcode==0x19c||opcode==0x1af?1:0;env.spawn_straight(parameters);break;
    }
    case 0x19d:case 0x1ad:case 0x1b0:case 0x1b2:{
        TimedLaserParameters parameters{};parameters.growth_speed=8;auto* output=context.integer_reference(0,globals);if(opcode==0x1b0||opcode==0x1b2)__builtin_memcpy(parameters.commands,emitters[0].commands,sizeof(parameters.commands));parameters.position=position(*this,0);
        parameters.sprite_type=static_cast<std::int16_t>(integer(1));parameters.color=static_cast<std::int16_t>(integer(2));parameters.angle=floating(3).to_float();parameters.initial_length=floating(4).to_float();parameters.target_length=floating(5).to_float();
        parameters.warning_frames=integer(6);parameters.grow_frames=integer(7);parameters.active_frames=integer(8);parameters.shrink_frames=integer(9);parameters.width=floating(10).to_float();const u32 flags=integer(11);parameters.flags=opcode==0x19d||opcode==0x1b0?flags|2:flags&~2u;
        const auto id=env.spawn_timed(parameters);if(output)*output=static_cast<i32>(id);break;
    }
    case 0x19e:case 0x19f:{
        auto* laser=env.laser(integer(0));if(!laser)break;const float y=floating(2).to_float(),x=floating(1).to_float();
        if(opcode==0x19e)laser->position={x,y,0};else reinterpret_cast<TimedLaser*>(laser)->parameters.velocity={x,y,0};break;
    }
    case 0x1a0:case 0x1a1:case 0x1a2:case 0x1a3:{
        auto* laser=env.laser(integer(0));if(!laser)break;const auto value=floating(1).to_float();if(opcode==0x1a0)laser->speed=value;else if(opcode==0x1a1)laser->width=value;else if(opcode==0x1a2)laser->angle=value;else reinterpret_cast<TimedLaser*>(laser)->parameters.angular_velocity=value;break;
    }
    case 0x1a4:case 0x1a5:{const auto radius=floating(0).to_float();env.cancel_circle(current.position,radius,opcode==0x1a4);break;}
    case 0x1a6:case 0x1a7:{auto& e=emitters[integer(0)];const u32 index=1+rank_band(*env.rank,opcode==0x1a7)*2;e.speed_start=floating(index).to_float();e.speed_end=floating(index+1).to_float();break;}
    case 0x1a8:{
        auto& e=emitters[integer(0)];const float start=floating(1).to_float(),end=floating(2).to_float(),other_start=floating(3).to_float();const auto other_end=floating(4);
        e.speed_start=((Extended::from_int(*env.rank)+number(1024.f))*(number(other_start)-number(start))*number(.00048828125f)+number(start)).to_float();
        e.speed_end=((other_end-number(end))*(Extended::from_int(*env.rank)+number(1024.f))*number(.00048828125f)+number(end)).to_float();break;
    }
    case 0x1a9:case 0x1aa:{auto& e=emitters[integer(0)];const u32 index=1+rank_band(*env.rank,opcode==0x1aa)*2;e.count=static_cast<std::int16_t>(integer(index));e.layers=static_cast<std::int16_t>(integer(index+1));break;}
    case 0x1ab:{auto& e=emitters[integer(0)];const auto count=integer(1),layers=integer(2),other_count=integer(3),other_layers=integer(4);e.count=integer_blend(count,other_count,*env.rank);e.layers=integer_blend(layers,other_layers,*env.rank);break;}
    case 0x1ae:{const float x=floating(1).to_float();const auto y=floating(2);const auto result=angle_to(number(env.player_position->y)-y,number(env.player_position->x)-number(x)).to_float();*context.float_reference(0,globals)=result;break;}
    case 0x1b3:{auto& e=emitters[integer(0)];e.speed_start=floating(1+difficulty_band(*env.difficulty)).to_float();e.speed_end=floating(5+difficulty_band(*env.difficulty)).to_float();break;}
    case 0x1b4:{auto& e=emitters[integer(0)];e.count=static_cast<std::int16_t>(integer(1+difficulty_band(*env.difficulty)));e.layers=static_cast<std::int16_t>(integer(5+difficulty_band(*env.difficulty)));break;}
    default:return false;
    }
    return true;
}
}
