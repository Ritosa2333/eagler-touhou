#pragma once
#include "../game/BulletState.hpp"
#include <eagler/netplay/PartitionedPoolJournal.hpp>

namespace th08::multiplayer {
// TH07's exhaustive VM/tail layout and batched fixed-pool journal, adapted
// to TH08's 1536 slots plus sentinel. No compact/semantic-field encoding.
using LiveBulletJournal=Netplay::PartitionedPoolJournal<BulletState,1537,6>;
inline std::array<LiveBulletJournal::Part,6> LiveBulletParts(){
    static_assert(offsetof(BulletState,sprites)==0);
    static_assert(offsetof(BulletTemplate,animation)==0);
    return {{{0,sizeof(AnmVm)},{sizeof(AnmVm),sizeof(AnmVm)},
        {2*sizeof(AnmVm),sizeof(AnmVm)},{3*sizeof(AnmVm),sizeof(AnmVm)},
        {4*sizeof(AnmVm),sizeof(AnmVm)},
        {5*sizeof(AnmVm),sizeof(BulletState)-5*sizeof(AnmVm)}}};
}
inline u32 LiveBulletMask(u16 state){
    const u32 hot=(1u<<0)|(1u<<4)|(1u<<5);
    return state>=2&&state<=4?hot|(1u<<(state-1)):hot;
}
}
