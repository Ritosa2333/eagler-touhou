#pragma once
#include "core/stages/Stage2MidbossArithmetic.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
namespace kaikeidou {
inline std::int32_t stage2_boss_remove_projectile(ProjectilePools& pool,std::uint32_t index){
    if(index!=0xffffffffu)return remove_projectile(pool,index,0);
    final_projectile_detail::Record p{pool,index};
    if(!p.word(0x34)&&(p.word(0)==1||p.word(0)==2)){p.word(0,3);p.word(0x10,0);p.word(8,0);if(!p.word(0x2c))p.word(0x2c,1);}
    return -240;
}
}
