#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase6_reimu(){
    if(t.frame==1){enemy().word(8,17);enemy().quad(0xa8,constant(0x6d23e0));enemy().quad(0xb0,constant(0x6d1a90));enemy().quad(0xb8,constant(0x6b25f8));enemy().quad(0xc0,constant(0x6b25f8));}
    else if(t.frame==150){ex_boss_early_timeline_name(context.spell_name,0x6c5d08);services.configure_phase(1,60,extra.boss_hp_multiplier*20000u,131);}
    else if(t.frame==210){effect(12,{{255,0,0}});real(0)=constant(0x6d1a08);costume(3);}
    else if(t.frame==250){const auto slot=allocate_projectile(pool,{26,1,{{255,0,0}},2,255,1,41});if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,140);b.word(0x34,1);}enemy().word(8,0);}
}
}}
