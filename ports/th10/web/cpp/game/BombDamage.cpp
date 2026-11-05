#include "Bomb.hpp"
namespace th10 {
// 0x4059f0. Spell cards, bosses and character choice alter both the damage
// inside the moving bomb radius and the damage outside it.
i32 Bomb::damage(const Vec3& target,const BombDamageContext& context) const noexcept {
    if(!active)return 0;
    const auto x=number(target.x)-number(position.x),y=number(target.y)-number(position.y);
    if(x*x+y*y<number(radius)*number(radius)){
        if(context.spell_flags&1)return mode?38:3;
        return context.boss_active?38:5;
    }
    if(context.spell_flags&1)return mode==1?(context.character?23:26):0;
    return context.boss_active&&!mode?(context.character?16:20):0;
}
}
