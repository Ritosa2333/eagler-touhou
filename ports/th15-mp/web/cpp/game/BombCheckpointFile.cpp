#include "BombCheckpoint.hpp"
namespace th15 {
namespace {
template<class T>void put(u8* p,u32 at,const T& v){std::memcpy(p+at,&v,sizeof v);}
template<class T>void get(const u8* p,u32 at,T& v){std::memcpy(&v,p+at,sizeof v);}
u32 word(const u8* p,u32 at){u32 v;get(p,at,v);return v;}
template<class O,class Copy>void orb_fields(O& o,Copy copy){copy(0,o.animation);copy(4,o.motion);copy(0xa0,o.active);copy(0xa4,o.age);copy(0xb8,o.displacement);copy(0xc4,o.target);copy(0xcc,o.index);copy(0xd0,o.damage_source);}
}
bool BombCheckpoint::write_file(std::vector<u8>& out){
    error.clear();out.clear();if(!available){error="No bomb checkpoint to serialize";return false;}
    out.resize(0x54,0);auto* b=out.data();put(b,0,position);put(b,0xc,angle);put(b,0x10,state);put(b,0x14,age);put(b,0x28,secondary_age);put(b,0x3c,first_handle);put(b,0x44,aura_handle);put(b,0x48,u32(effective));put(b,0x4c,saved_charges);
    if(!state)return true;
    auto tree=[&](u32 h)->bool{if(!h)return true;if(animations.write_tree(h,bomb.context.animations,out))return true;error=animations.error;out.clear();return false;};
    if(reimu){const u32 at=out.size();out.resize(at+0x6c0,0);for(u32 i=0;i<8;i++){const auto& o=orbs[i];orb_fields(o,[&](u32 offset,const auto& value){put(out.data()+at+i*0xd8,offset,value);});put(out.data()+at+i*0xd8,0xd4,u32(o.archived));}
        // Registry slots and checkpoint slots are independent in the web port.
        // Serialize the captured aura, never the live handle's unrelated slot.
        if(!tree(aura_handle))return false;
        for(const auto& o:orbs)if(!tree(o.animation))return false;
    }else if(!tree(first_handle)||!tree(aura_handle))return false;
    return true;
}
bool BombCheckpoint::read_file(const u8* data,u32 size,u32& consumed){
    error.clear();consumed=0;if(!data||size<0x54){error="Truncated bomb checkpoint state";return false;}
    Vec3 next_position;float next_angle;i32 next_state;Timer next_age,next_secondary;get(data,0,next_position);get(data,0xc,next_angle);get(data,0x10,next_state);get(data,0x14,next_age);get(data,0x28,next_secondary);u32 next_first=word(data,0x3c),next_aura=word(data,0x44),imported_aura=0;const bool next_effective=word(data,0x48)!=0;const i32 next_charges=signed_bits(word(data,0x4c));std::array<ReimuOrb,8> next_orbs{};u32 offset=0x54;
    struct Guard{AnmCheckpoint& pool;AnmCheckpoint::Mark mark;bool committed=false;~Guard(){if(!committed)pool.rollback(mark);}} guard{animations,animations.mark()};
    auto tree=[&](u32& h)->bool{if(!h)return true;u32 used=0;const u32 read=animations.read_tree(data+offset,size-offset,used,bomb.context.animations);if(!read){error=animations.error;return false;}h=read;offset+=used;return true;};
    if(next_state){if(reimu){if(size-offset<0x6c0){error="Truncated Reimu bomb orb records";return false;}for(u32 i=0;i<8;i++){auto& o=next_orbs[i];orb_fields(o,[&](u32 at,auto& value){get(data+offset+i*0xd8,at,value);});o.archived=word(data+offset+i*0xd8,0xd4)!=0;}offset+=0x6c0;
            imported_aura=next_aura;if(!tree(imported_aura))return false;for(auto& o:next_orbs)if(!tree(o.animation))return false;
        }else if(!tree(next_first)||!tree(next_aura))return false;}
    position=next_position;angle=next_angle;state=next_state;age=next_age;secondary_age=next_secondary;first_handle=next_first;aura_handle=reimu&&next_state?imported_aura:next_aura;effective=next_effective;saved_charges=next_charges;if(reimu&&next_state)orbs=next_orbs;available=true;consumed=offset;guard.committed=true;return true;
}
}
