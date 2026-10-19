#include "EffectSystem.hpp"
#include <cassert>
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
#include "PlayerEffectSlots.hpp"
#endif

using namespace th08;
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
using namespace th08::player_effect_slots;
#endif

int main(){
    EffectPoolState pool{};
    static_assert(offsetof(EffectPoolState,objects)==0x1c);
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    static_assert(object_count==sizeof(pool.objects)/sizeof(pool.objects[0]));
    static_assert(active_object_count==dummy_index);
    assert(fixed_pool_begin+fixed_count-1==active_object_count-1);

    bool occupied[37]{};
    for(i32 seat=0;seat<seat_count;++seat){
        for(i32 local=0;local<player_fixed_count;++local){
            const auto routed=fixed_for_player(seat,local);
            assert(routed&&routed.storage_index>=fixed_pool_begin&&routed.storage_index<dummy_index);
            const i32 relative=routed.relative_slot;
            assert(relative>=0&&relative<fixed_count);
            assert(!occupied[relative]);occupied[relative]=true;
            const auto actual=fixed_for_storage_index(routed.storage_index);
            assert(actual&&actual.seat==seat&&actual.local_slot==local);
            assert(actual.relative_slot==relative);
            auto& effect=pool.objects[routed.storage_index];
            effect.slot=local;
            assert(effect.slot==local);
            if(local>=4&&local<=7){
                assert(wrapping_sub(effect.slot,4)==local-4);
                assert((effect.slot&1)==(local&1));
            }
        }
    }

    for(i32 global=0;global<shared_fixed_count;++global){
        const auto routed=fixed_for_global(global);
        assert(routed&&routed.storage_index==fixed_pool_begin+global&&routed.local_slot==global);
    }
    assert(!fixed_for_global(shared_fixed_count));
    assert(local_slot_for_relative_slot(12)==12);
    assert(seat_for_relative_slot(12)==-1);
    pool.objects[fixed_pool_begin+12].slot=12; // shared moon keeps its native slot
    assert(pool.objects[fixed_pool_begin+12].slot==12);
    assert(!fixed_for_player(-1,0));
    assert(!fixed_for_player(seat_count,0));
    assert(!fixed_for_player(0,-1));
    assert(!fixed_for_player(0,player_fixed_count));
    assert(!fixed_for_storage_index(dummy_index));
    assert(!fixed_for_storage_index(fixed_pool_begin-1));

    assert(object_count==678&&dummy_index==677&&fixed_count==37);
    assert(fixed_for_player(0,0).storage_index==640);
    assert(fixed_for_player(1,0).storage_index==653);
    assert(fixed_for_player(2,0).storage_index==665);
    for(i32 relative=0;relative<fixed_count;++relative)
        if(relative!=12)assert(occupied[relative]);
    assert(pool.objects[fixed_pool_begin+13].slot==0);
    assert(pool.objects[fixed_pool_begin+25].slot==0);

    // Every real effect, including the last player's last fixed slot, lies in
    // the release/update domain; the dummy is the sole excluded object.
    pool.objects[active_object_count-1].active=1;
    pool.objects[dummy_index].active=1;
    assert(pool.objects[active_object_count-1].active);
    assert(dummy_index==active_object_count);
#else
    static_assert(effect_pool_layout::object_count==654&&effect_pool_layout::dummy_index==653);
    static_assert(effect_pool_layout::fixed_count==13&&effect_pool_layout::active_object_count==653);
    assert(sizeof(pool.objects)/sizeof(pool.objects[0])==654);
    pool.objects[effect_pool_layout::active_object_count-1].active=1;
    pool.objects[effect_pool_layout::dummy_index].active=1;
    assert(pool.objects[652].active&&effect_pool_layout::dummy_index==653);
#endif
}
