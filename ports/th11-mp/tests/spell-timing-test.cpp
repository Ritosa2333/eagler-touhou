#include "SpellTiming.hpp"
#include <cassert>
#include <cstdio>
using namespace th11;
int main(){
    for(i32 frames:{1,60,137,600,3599,5400}){
        SpellTiming live,peer;
        u32 flags=1,other=1;
        assert(live.update_multiplayer(flags,1));
        assert(peer.update_multiplayer(other,1));
        assert(flags&0x40);
        // Network stalls/pause/render callbacks do not call this simulation owner.
        const auto checkpoint=live;
        assert(live.update_multiplayer(flags,frames));
        flags&=~1u;other&=~1u;
        assert(live.update_multiplayer(flags,frames));
        assert(peer.update_multiplayer(other,frames));
        const auto decoded=decode_spell_time(live.encoded);
        assert(decoded.valid&&decoded.seconds!=999);
        assert(live.encoded==peer.encoded&&live.last_frames==frames&&live.index==1);
        SpellTiming original;u32 ordinary=1;
        assert(original.update(ordinary,1,0,false));ordinary&=~1u;
        assert(original.update(ordinary,frames,double(frames)/60.,false));
        assert(live.encoded==original.encoded);
        // Rollback/re-simulation and a checkpoint admitted mid-spell agree.
        auto restored=checkpoint;u32 restored_flags=0x40;
        assert(restored.update_multiplayer(restored_flags,frames));
        assert(restored.encoded==live.encoded);
        SpellTiming spectator;spectator.start=123456.;u32 spectator_flags=0x40;
        assert(spectator.update_multiplayer(spectator_flags,frames));
        assert(spectator.encoded==live.encoded);
        assert(live.update_multiplayer(flags,frames+600)&&live.index==1);
        for(int card=1;card<20;++card){flags=1;assert(live.update_multiplayer(flags,1));flags&=~1u;assert(live.update_multiplayer(flags,frames));assert(decode_spell_time(live.encoded).valid);}
        assert(live.index==20);
    }
    std::puts("PASS TH11 MP spell timing: native encoding, all cards, rollback, spectator, pause/network independence");
}
