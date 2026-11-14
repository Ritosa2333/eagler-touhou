#include "EnemyCommands.hpp"
#include "BulletManager.hpp"
namespace th11 {
i32 enemy_th14_fire_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& world)noexcept{
#if PORT_TITLE == 14
    if(c.instruction->opcode!=601||!world.bullets){g_th14_fire_stage=1;return -2;}
    const u32 index=u32(c.integer_argument(0,g));if(index>=16){g_th14_fire_stage=2;return -2;}
    auto& emitter=e.th14_emitters[index];
    // 0x427ba9 measured: the position is the branch's vector and is written into the record's head
    // (native +0x5a0), and the pointer handed to 0x4191a0 is the record itself - which is what
    // firing below does, because the port's emitter is that record.
    enemy_th14_fire_position(e,index);
    world.bullets->exclusion_squared=e.exclusion_distance;const bool ok=world.bullets->fire_th14(emitter);world.bullets->exclusion_squared=0;
    g_th14_fire_stage=ok?0:3;g_th14_fire_bullet_error=world.bullets->last_error;return ok?0:-2;
#else
    (void)e;(void)c;(void)g;(void)world;return -2;
#endif
}
}
