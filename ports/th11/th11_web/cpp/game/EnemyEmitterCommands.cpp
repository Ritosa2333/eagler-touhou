#include "EnemyCommands.hpp"
#include "BulletManager.hpp"
namespace th11 {
void BulletEmitter::initialize()noexcept{std::memset(this,0,sizeof(*this));count=layers=1;speed=2;fire_sound=7;transform_sound=24;flags=0x83;}
namespace {
u32 rank_pair(i32 rank,bool five){return five?(rank>=600?9:rank>=200?7:rank>=-200?5:rank>=-600?3:1):(rank>=512?5:rank>=-512?3:1);}
i16 interpolate_count(i32 a,i32 b,i32 rank){return i16(wrapping_add(signed_bits(u32(wrapping_sub(b,a))*u32(wrapping_add(rank,1024)))/2048,a));}
}
bool enemy_emitter_command(EnemyState& e,EclContext& c,EnemyGlobals& g)noexcept{
    const u16 op=c.instruction->opcode;auto& world=g.environment;
    auto i=[&](u32 n){return c.integer_argument(n,g);};auto f=[&](u32 n){return float(c.float_argument(n,g));};
    switch(op){case 0x190:case 0x192:case 0x193:case 0x194:case 0x195:case 0x196:case 0x197:case 0x198:case 0x199:case 0x19b:case 0x1a6:case 0x1a7:case 0x1a8:case 0x1a9:case 0x1aa:case 0x1ab:case 0x1b3:case 0x1b4:case 0x1b5:case 0x1b6:case 0x1b7:break;default:return false;}
    if(op==0x19b){const i32 source=i(1),target=i(0);if(u32(source)>=8||u32(target)>=8)__builtin_trap();e.emitters[target]=e.emitters[source];return true;}
    const i32 index=i(0);if(u32(index)>=8)__builtin_trap();auto& b=e.emitters[index];
    switch(op){
    case 0x190:b.initialize();e.emitter_offset[index].x=e.emitter_offset[index].y=0;e.emitter_origin[index]={};break;
    case 0x192:b.sprite=i16(i(1));b.color=i16(i(2));break;
    case 0x193:e.emitter_offset[index].x=f(1);e.emitter_offset[index].y=f(2);break;
    case 0x194:b.angle=f(1);b.spread=f(2);break;
    case 0x195:b.speed=f(1);b.slow_speed=f(2);break;
    case 0x196:b.count=i16(i(1));b.layers=i16(i(2));break;
    case 0x197:b.aim=i16(i(1));break;
    case 0x198:b.fire_sound=i(1);b.transform_sound=i(2);break;
    case 0x199:{const i32 slot=i(1);if(u32(slot)>=18)__builtin_trap();auto& t=b.transforms[slot];t.concurrent=i(2);t.opcode=u32(i(3));t.c=i(4);t.d=i(5);t.a=f(6);t.b=f(7);break;}
    case 0x1a6:case 0x1a7:{const u32 arg=rank_pair(world.rank,op==0x1a7);b.speed=f(arg);b.slow_speed=f(arg+1);break;}
    case 0x1a8:{const double a=f(1),v=f(2),d=f(3),z=f(4),r=double(world.rank)+1024;b.speed=float((d-a)*r/2048+a);b.slow_speed=float((z-v)*r/2048+v);break;}
    case 0x1a9:case 0x1aa:{const u32 arg=rank_pair(world.rank,op==0x1aa);b.count=i16(i(arg));b.layers=i16(i(arg+1));break;}
    case 0x1ab:{const i32 a=i(1),v=i(2),d=i(3),z=i(4);b.count=interpolate_count(a,d,world.rank);b.layers=interpolate_count(v,z,world.rank);break;}
    case 0x1b3:{const u32 arg=world.difficulty==0?1:world.difficulty==1?2:world.difficulty==2?3:4;b.speed=f(arg);b.slow_speed=f(arg+4);break;}
    case 0x1b4:{const u32 arg=world.difficulty==0?1:world.difficulty==1?2:world.difficulty==2?3:4;b.count=i16(i(arg));b.layers=i16(i(arg+4));break;}
    case 0x1b5:{const float angle=f(1),radius=f(2);const Vec2 p=polar(angle,radius);e.emitter_offset[index].x=p.x;e.emitter_offset[index].y=p.y;break;}
    case 0x1b6:b.radius=f(1);break;
    case 0x1b7:e.emitter_origin[index].x=f(1);e.emitter_origin[index].y=f(2);e.emitter_origin[index].z=e.emitter_origin[index].x< -990.f?0:1;break;
    }
    return true;
}
i32 enemy_fire_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& w)noexcept{
    if(c.instruction->opcode!=0x191||!w.bullets)return -2;
    const i32 index=c.integer_argument(0,g);if(u32(index)>=8)return -2;
    auto& emitter=e.emitters[index];const auto& offset=e.emitter_offset[index];const auto& origin=e.emitter_origin[index];
    if(origin.z<=.9){const auto& p=e.current.position;emitter.position={float(double(offset.x)+p.x),float(double(offset.y)+p.y),float(double(offset.z)+p.z)};}
    else emitter.position={float(double(origin.x)+offset.x),float(double(origin.y)+offset.y),0};
    w.bullets->exclusion_squared=e.exclusion_distance;const bool ok=w.bullets->fire(emitter);w.bullets->exclusion_squared=0;return ok?0:-2;
}
}
