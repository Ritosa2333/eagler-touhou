#include "EnemyCommands.hpp"
#include "BulletManager.hpp"
#include "LaserManager.hpp"
namespace th11 {
bool enemy_th16_laser_request(const EnemyState& e,u32 index,u8* request)noexcept{
#if PORT_TITLE == 16
 if(index>=16||!request)return false;std::memset(request,0,0x358);
 const auto& state=e.th16_emitters;const auto& emitter=state.records[index];const auto& base=state.bases[index];const auto& origin=state.origins[index];
 const Vec3 position=origin.z>.9f?Vec3{float(origin.x+base.x),float(base.y+origin.y),0}:Vec3{float(base.x+e.current.position.x),float(base.y+e.current.position.y),float(base.z+e.current.position.z)};
 auto write=[&](u32 offset,const auto& value){std::memcpy(request+offset,&value,sizeof value);};
 write(0,position);write(0xc,normalize_angle(emitter.angle));write(0x10,emitter.position.y);write(0x14,emitter.position.x);write(0x18,emitter.position.z);write(0x1c,emitter.values_340[0]);write(0x20,emitter.speed);write(0x24,emitter.sprite);write(0x28,emitter.color);write(0x2c,emitter.radius);
 const u32 flags=emitter.values_340[8]|1u;write(0x34,flags);std::memcpy(request+0x38,emitter.transforms,sizeof emitter.transforms);write(0x350,emitter.fire_sound);write(0x354,emitter.transform_sound);return true;
#else
 (void)e;(void)index;(void)request;return false;
#endif
}
bool enemy_th16_fire_position(EnemyState& e,u32 index)noexcept{
#if PORT_TITLE == 16
    if(index>=16)return false;
    auto& state=e.th16_emitters;auto& emitter=state.records[index];const auto& base=state.bases[index];const auto& origin=state.origins[index];
    // Retail4209cf..420aa5: two explicit SSE-coordinate branches.
    if(origin.z>.9f)emitter.position={float(origin.x+base.x),float(base.y+origin.y),0};
    else emitter.position={float(base.x+e.current.position.x),float(base.y+e.current.position.y),float(base.z+e.current.position.z)};
    return true;
#else
    (void)e;(void)index;return false;
#endif
}
i32 enemy_th16_fire_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& world)noexcept{
#if PORT_TITLE == 16
    const u16 op=c.instruction->opcode;if(op!=601&&op!=702)return -2;
    const u32 index=u32(c.integer_argument(0,g));if(index>=16)return -2;
    if(op==702){u8 request[0x358];if(!world.lasers||!enemy_th16_laser_request(e,index,request))return -2;
        world.lasers->spawn_th16_line(request);return world.lasers->last_error?-2:0;}
    if(!world.bullets)return -2;
    if(!enemy_th16_fire_position(e,index))return -2;auto& emitter=e.th16_emitters.records[index];
    world.bullets->exclusion_squared=e.exclusion_distance;const bool ok=world.bullets->fire_th16(emitter);world.bullets->exclusion_squared=0;return ok?0:-2;
#else
    (void)e;(void)c;(void)g;(void)world;return -2;
#endif
}
}
