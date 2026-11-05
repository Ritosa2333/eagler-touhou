#pragma once
#include "AnmExecutor.hpp"
namespace th09::presentation {
inline bool always_hitbox=false;
// Prepare the native focus marker with a private executor/RNG. It never enters
// the effect pool, consumes gameplay input, or writes the player's focus state.
class HitboxDisplay {
    Rng random;AnmExecutor executor{random};AnmVm marker{};
    const void* script=nullptr;i32 previous=-1;
public:
    void reset(){script=nullptr;previous=-1;}
    AnmVm sample(AnmLoaded& file,const Vec3& position,i32 frame){
        if(script!=file.scripts[63]||previous<0||frame<previous){
            random={};executor.invalid=false;executor.start(file,marker,file.scripts[63]);
            marker.flags|=0x2000;marker.color1.d3dColor=-1;marker.pos2={};
            script=file.scripts[63];previous=frame-1;
        }
        // Fresh logical time owns animation. Repeated drawings and pause never
        // tick the VM; skipped pictures retain the original animation cadence.
        while(previous<frame){executor.execute(marker);++previous;}
        auto copy=marker;copy.pos=position;copy.pos.z=.08f;return copy;
    }
};
}
