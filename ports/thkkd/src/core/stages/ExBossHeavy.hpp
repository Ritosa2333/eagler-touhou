// 普通170..176；入口行为号与对象字段分离，固定槽写入保持原顺序。
#pragma once
#include "core/stages/ExBossHeavy170.hpp"
#include "core/stages/ExBossHeavy171.hpp"
#include "core/stages/ExBossHeavyChildren.hpp"
namespace kaikeidou {
inline void update_ex_boss_heavy_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,const ObjectRecord<240>& player,const PatternParameters& p,EffectPool& effects,Random& random,RandomPrecision precision,GameMath& math,ExBossHeavyServices& services){
    ex_boss_heavy_detail::Engine e{pool,index,player,p,effects,random,precision,math,services,{{static_cast<unsigned>(precision)}}};
    switch(behavior){case 170:e.normal170();break;case 171:e.normal171();break;case 172:e.normal172();break;case 173:e.normal173();break;case 174:e.normal174();break;case 175:e.normal175();break;case 176:e.normal176();break;default:throw std::out_of_range("EX heavy behavior outside170..176");}
}
}
