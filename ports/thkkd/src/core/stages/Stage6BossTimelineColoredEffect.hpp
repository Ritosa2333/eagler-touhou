// 原660B10..660CB5：color自然域0..5；分配失败仍写-1前置槽的元数据。
#pragma once
#include "core/shared/EffectPool.hpp"
#include "core/stages/StageBossServices.hpp"
#include <stdexcept>
namespace kaikeidou {
inline std::int32_t stage6_boss_colored_effect(EffectPool& effects,Effect& prefix,std::array<std::uint32_t,100>& sounds,
 std::uint32_t kind,std::uint32_t color,std::uint32_t hp,std::uint32_t enemy_behavior,std::uint64_t x,std::uint64_t y,StageBossServices& services){
 constexpr std::array<std::array<std::uint32_t,3>,6> colors{{{{255,0,0}},{{0,0,255}},{{0,255,0}},{{0,128,128}},{{128,128,0}},{{128,0,128}}}};
 if(color>=colors.size())throw std::out_of_range("Stage6 colored effect outside original initialized-local domain");
 const auto packed=services.color(colors[color][0],colors[color][1],colors[color][2]);const auto index=allocate_effect(effects,sounds,{kind,packed,x,y});
 auto& effect=index==-1?prefix:effects.effects.at(static_cast<std::uint32_t>(index));effect.word(0x10,color+100u);effect.word(0x14,hp);effect.word(0x18,enemy_behavior);return index;
}
}
