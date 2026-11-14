#include "EffectTh16Slots.hpp"
namespace th11 {
i32 Th16EffectSlots::choose(Th16EffectLookup& lookup)noexcept{
 // Complete40e6c0: inspect the following slot, return the previous cursor.
 // A live following ID is also an immediate return, as in the retail body.
 for(u32 n=0;n<1024;++n){const i32 old=cursor;cursor=wrapping_add(cursor,1)%1024;
  if(cursor<0)return -1;auto& id=ids[u32(cursor)];if(!id||lookup.alive(id))return old;id=0;
 }return -1;
}
}
