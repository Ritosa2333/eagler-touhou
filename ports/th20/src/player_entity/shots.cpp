#include "shots.hpp"
namespace th20::source::player_entity {
void recycle_shot(ShotController& owner,Shot& shot) noexcept {
    if(shot.flags&0x1000000u)return;
    scheduler::insert_after(owner.free.sentinel,shot.link);shot.link.owner=&owner.free;
    if(owner.free.tail==&owner.free.sentinel)owner.free.tail=&shot.link;
}
void retire_shot(Shot& shot,ShotServices& services){
    shot.fields_98[1]=0;services.delete_animation(shot.handle_18);
    //The guard uses this shot's context, but4c0e10 deliberately resolves the
    //damage handle against global context0. The adapter retains that behavior.
    if(shot.context->object_28)services.retire_damage(shot.fields_b8[(0xdc-0xb8)/4]);
    scheduler::unlink(shot.link);recycle_shot(*shot.owner,shot);
    const auto flags=shot.flags;shot.flags=0;
    if(flags&0x1000000u)services.release(&shot);
}
}
