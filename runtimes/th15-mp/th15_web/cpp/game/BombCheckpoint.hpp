#pragma once
#include "BombReimu.hpp"
#include "BombMarisa.hpp"
#include "BombSanae.hpp"
#include "BombReisen.hpp"
#include "AnmCheckpoint.hpp"
namespace th15 {
// Each character owns its checkpoint. Animation clones belong to the shared
// checkpoint pool; live animations are retired, rather than rewound in place.
class BombCheckpoint {
    enum class Kind {Reimu,Marisa,Sanae,Reisen};
    Kind kind;BombController& bomb;AnmCheckpoint& animations;
    BombReimu* reimu=nullptr;u32* first=nullptr;u32& aura;i32* charges=nullptr;
    Vec3 position{};float angle=0;Timer age,secondary_age;i32 state=0,saved_charges=0;
    u32 first_handle=0,aura_handle=0;bool effective=false,available=false;
    std::array<ReimuOrb,8> orbs;
    bool clone(u32&);bool restore_handle(u32&);bool retire(u32&);
public:
    std::string error;
    BombCheckpoint(BombReimu& b,AnmCheckpoint& a):kind(Kind::Reimu),bomb(b),animations(a),reimu(&b),aura(b.aura){}
    BombCheckpoint(BombMarisa& b,AnmCheckpoint& a):kind(Kind::Marisa),bomb(b),animations(a),first(&b.beam),aura(b.aura){}
    BombCheckpoint(BombSanae& b,AnmCheckpoint& a):kind(Kind::Sanae),bomb(b),animations(a),first(&b.field),aura(b.aura){}
    BombCheckpoint(BombReisen& b,AnmCheckpoint& a):kind(Kind::Reisen),bomb(b),animations(a),first(&b.barrier),aura(b.aura),charges(&b.charges){}
    bool capture();bool restore();bool ready()const noexcept{return available;}
    bool write_file(std::vector<u8>&);
    bool read_file(const u8*,u32,u32& consumed);
    const ReimuOrb& orb(u32 i)const noexcept{return orbs[i];}
    u32 saved_handle(u32 i)const noexcept{return i?aura_handle:first_handle;}
    const Timer& saved_age(bool secondary)const noexcept{return secondary?secondary_age:age;}
};
}
