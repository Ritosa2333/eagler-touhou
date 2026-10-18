#include "BulletCommands.hpp"
#include <cmath>
namespace th15 {
int BulletCommands::execute(EclContext& context,u16 opcode){
    const auto* instruction=context.instruction;if(!instruction){context.error="Bullet instruction unavailable";return -2;}bool ok=true;
    auto integer=[&](u32 index){i32 v=0;if(20+index*4>instruction->length){context.error="Truncated bullet arguments";ok=false;}else if(!context.integer(index,instruction->argument<i32>(index),v))ok=false;return v;};
    auto floating=[&](u32 index){float v=0;if(20+index*4>instruction->length){context.error="Truncated bullet arguments";ok=false;}else if(!context.floating(index,instruction->argument<float>(index),v))ok=false;return v;};
    i32 slot=integer(0);if(opcode>=609&&opcode<=612&&(slot<0||slot>=18))slot=0;
    if(!ok)return -2;if(slot<0||slot>=16){context.error="Bullet shooter index outside resource range";return -2;}
    auto& state=enemy.shooters;auto& shot=state.shooters[slot];auto& offset=state.offset[slot];auto& origin=state.origin[slot];auto& cursor=state.next_transform[slot];
    switch(opcode){
        case 600:shot.reset();offset.x=offset.y=0;origin={};cursor=0;break;
        case 601:{
            if(origin.z>.9f)shot.position={float(offset.x+origin.x),float(offset.y+origin.y),0};
            else shot.position={float(offset.x+enemy.motion.position.x),float(offset.y+enemy.motion.position.y),float(offset.z+enemy.motion.position.z)};
            if(!host||!host->emit(shot,enemy.minimum_bullet_distance_squared)){context.error="Bullet emission host unavailable";return -2;}break;
        }
        case 602:shot.sprite=integer(1);shot.color=integer(2);break;
        case 603:offset.x=floating(1);offset.y=floating(2);break;
        case 604:shot.angle=floating(1);shot.angle_step=floating(2);break;
        case 605:shot.speed=floating(1);shot.speed_step=floating(2);break;
        case 606:shot.count=i16(integer(1));shot.rows=i16(integer(2));break;
        case 607:shot.pattern=i16(integer(1));break;
        case 608:shot.shoot_sound=integer(1);shot.transform_sound=integer(2);break;
        case 609:case 610:case 611:case 612:{
            const bool append=opcode==611||opcode==612,full=opcode==610||opcode==612;const i32 index=append?cursor:integer(1);u32 arg=append?1:2;
            if(index<0||index>=18){context.error="Bullet transform index outside resource range";return -2;}
            auto& transform=shot.transforms[index];transform.active=integer(arg++);transform.type=u32(integer(arg++));transform.integers[0]=integer(arg++);transform.integers[1]=integer(arg++);
            if(full){transform.integers[2]=integer(arg++);transform.integers[3]=integer(arg++);}transform.floats[0]=floating(arg++);transform.floats[1]=floating(arg++);if(full){transform.floats[2]=floating(arg++);transform.floats[3]=floating(arg++);}cursor=wrapping_add(index,1);break;
        }
        case 614:{const i32 source=integer(1);if(!ok)return -2;if(source<0||source>=16){context.error="Bullet copy source outside resource range";return -2;}shot=state.shooters[source];offset=state.offset[source];origin=state.origin[source];break;}
        case 617:case 618:{
            u32 first=1;if(opcode==617){if(world.rank>=512)first=5;else if(world.rank>=-512)first=3;}
            else if(world.rank>=600)first=9;else if(world.rank>=200)first=7;else if(world.rank>=-200)first=5;else if(world.rank>=-600)first=3;
            shot.speed=floating(first);shot.speed_step=floating(first+1);break;
        }
        case 619:{const float a=floating(1),b=floating(2),c=floating(3),d=floating(4),rank=float(float(world.rank)+1024.f);shot.speed=float(float(float(float(c-a)*rank)*.00048828125f)+a);shot.speed_step=float(float(float(float(d-b)*rank)*.00048828125f)+b);break;}
        case 620:case 621:{
            u32 first=1;if(opcode==620){if(world.rank>=512)first=5;else if(world.rank>=-512)first=3;}
            else if(world.rank>=600)first=9;else if(world.rank>=200)first=7;else if(world.rank>=-200)first=5;else if(world.rank>=-600)first=3;
            shot.count=i16(integer(first));shot.rows=i16(integer(first+1));break;
        }
        case 622:{const i32 a=integer(1),b=integer(2),c=integer(3),d=integer(4),rank=wrapping_add(world.rank,1024);shot.count=i16(wrapping_add(wrapping_mul(rank,wrapping_sub(c,a))/2048,a));shot.rows=i16(wrapping_add(wrapping_mul(rank,wrapping_sub(d,b))/2048,b));break;}
        case 624:shot.speed=floating(1+(world.difficulty>=0&&world.difficulty<3?world.difficulty:3));shot.speed_step=floating(5+(world.difficulty>=0&&world.difficulty<3?world.difficulty:3));break;
        case 625:shot.count=i16(integer(1+(world.difficulty>=0&&world.difficulty<3?world.difficulty:3)));shot.rows=i16(integer(5+(world.difficulty>=0&&world.difficulty<3?world.difficulty:3)));break;
        case 626:{const float angle=floating(1),radius=floating(2);offset.x=float(std::cos(double(angle))*double(radius));offset.y=float(std::sin(double(angle))*double(radius));break;}
        case 627:shot.radius=floating(1);break;
        case 628:origin.x=floating(1);origin.y=floating(2);origin.z=-990.f>origin.x?0.f:1.f;break;
        case 700:shot.position={floating(1),floating(2),floating(3)};shot.extra[0]=float_to_bits(floating(4));break;
        case 701:for(u32 i=0;i<5;i++)shot.extra[4+i]=u32(integer(1+i));break;
        case 640:{const i32 index=integer(1);if(!ok)return -2;if(index<0||index>=18||instruction->length<29){context.error="Invalid bullet transform payload";return -2;}const auto* data=reinterpret_cast<const u8*>(instruction)+28;u32 n=0;while(28+n<instruction->length&&data[n])n++;if(28+n==instruction->length){context.error="Unterminated bullet transform payload";return -2;}shot.transforms[index].payload.assign(data,data+n+1);break;}
        case 641:cursor=wrapping_add(cursor,-1);if(cursor<0)cursor=0;break;
        default:context.error="Unimplemented bullet ECL opcode "+std::to_string(opcode);return -2;
    }
    return ok?0:-2;
}
}
