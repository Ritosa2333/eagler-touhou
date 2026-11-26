#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase16(){
    if(player.word(0x2c)){protection.boss_state_a7ea60=1;allocation.field_a7ea58=0;}else{protection.boss_state_a7ea60=0;allocation.field_a7ea58=1;}
    if(character==0)phase16_reimu();else if(character==1)phase16_marisa();else phase16_sanae();
}
inline void Engine::phase16_reimu(){
    const auto pulse=[&](unsigned mode,std::uint32_t radius,bool initial=false){const auto slot=allocate_projectile(pool,{29,3,{{255,255,255}},0,255,1,41});if(slot==-1)return;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,0);b.word(0x7c,mode);b.quad(0xa0,constant(radius));b.quad(0x40,initial?constant(0x6d1a60):real(0));b.quad(0x48,initial?constant(0x6d1cc0):real(1));b.word(8,168);b.word(0x3c,1);b.word(0x34,1);};
    const auto follow=[&](bool player_y,std::uint32_t rate){ex_boss_late_timeline_ease(real(0),player.quad(0x90),real(2),m.precision);ex_boss_late_timeline_ease(real(1),player_y?player.quad(0x98):constant(0x6b25b8),real(2),m.precision);ex_boss_late_timeline_ease(real(2),0x3ff0000000000000ull,constant(rate),m.precision);};
    const auto corners=[&](std::array<std::uint32_t,3> color){effect_at(10,color,0,0);effect_at(10,color,0,constant(0x6d19a0));effect_at(10,color,constant(0x6d1cb0),0);effect_at(10,color,constant(0x6d1cb0),constant(0x6d19a0));};
    const auto radial=[&](std::uint64_t angle,std::uint32_t speed,Number radius,std::array<std::uint32_t,3> color){const auto slot=allocate_projectile(pool,{12,0,color,0,255,0,41});if(slot==-1)return;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,0);
        auto value=math.cosine(angle);b.quad(0x50,m.store(m.mul(value,m.c(speed))));value=math.sine(angle);b.quad(0x58,m.store(m.mul(value,m.c(speed))));value=math.cosine(m.store(m.add(m.q(angle),m.c(0x6d1860))));b.quad(0x40,m.store(m.add(m.mul(value,radius),m.c(0x6d1a60))));value=math.sine(m.store(m.add(m.q(angle),m.c(0x6d1860))));b.quad(0x48,m.store(m.add(m.mul(value,radius),m.c(0x6d1af0))));b.word(8,169);b.quad(0x60,angle);
    };
    if(t.frame==1){target(0x6d1cc0,0x6d1a40);return;}
    if(t.frame==150){pulse(0,0x6d1a90,true);costume(3);return;}
    if(t.frame==210){presentation.entry_state=4;real(0)=enemy().quad(0x38);real(1)=enemy().quad(0x40);real(2)=constant(0x6d1ab0);enemy().word(8,0);enemy().quad(0x38,constant(0x6d2540));enemy().quad(0x40,constant(0x6d2540));return;}
    if(t.frame==300){ex_boss_late_timeline_name(context.spell_name,0x6c5e6c);services.configure_phase(1,100,0,143);return;}
    if(t.frame==380){for(auto x:{0x6d1aa8u,0x6d2398u,0x6d1a60u})effect_at(10,{{255,127,0}},constant(x),0);return;}
    if(frame()>=420&&frame()<=1999){
        if(remainder(t.frame,2)!=0){const auto slot=allocate_projectile(pool,{0,0,{{255,127,0}},0,255,0,41});if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,0);auto value=random.real_number(constant(0x6d1ab0),precision);b.quad(0x50,m.store(m.add(m.mul(value,m.c(0x6d1950)),m.c(0x6d22a0))));value=random.real_number(constant(0x6d2028),precision);b.quad(0x58,m.store(m.add(m.mul(value,m.c(0x6d1950)),m.c(0x6d1948))));value=random.real_number(constant(0x6d2f50),precision);b.quad(0x40,m.store(m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d19f0))));b.quad(0x48,constant(0x6d1a30));b.word(8,167);}}
        constexpr std::array<std::uint32_t,6> radii{{0x6d1a90,0x6b25b8,0x6d1d00,0x6d1f08,0x6d18c0,0x6d1ec8}};for(unsigned i=0;i<6;++i)if(t.frame==600u+i*200u){pulse(1,radii[i]);break;}
        if(t.frame==1999){clear_projectiles(pool,100);real(10)=constant(0x6d1c48);real(11)=constant(0x6d1c40);}if(frame()>=210)follow(false,0x6d1a88);return;
    }
    if(frame()>=2000&&frame()<=3999){
        if(t.frame==2000){effect_at(10,{{0,200,40}},constant(0x6d1898),0);effect_at(10,{{230,0,0}},constant(0x6d20e8),0);}
        if(remainder(t.frame,3)==0&&frame()<=3860)for(unsigned side=0;side<2;++side){const auto color=side?std::array<std::uint32_t,3>{{230,0,0}}:std::array<std::uint32_t,3>{{0,200,40}};const auto slot=allocate_projectile(pool,{16,0,color,0,255,0,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,0);b.quad(0x50,0);b.quad(0x58,constant(side?0x6d2180:0x6d1b28));b.quad(0x40,real(10u+side));b.quad(0x48,constant(0x6d1a30));b.word(8,167);b.quad(0x60,constant(0x6d1b60));}
        real(10)=m.store(m.add(m.q(real(10)),m.c(0x6d2e70)));if(compare(m.c(0x6d1c40),m.q(real(10)))<0)real(10)=m.store(m.sub(m.q(real(10)),m.c(0x6d2e68)));
        real(11)=m.store(m.sub(m.q(real(11)),m.c(0x6d2e60)));if(compare(m.c(0x6d1c48),m.q(real(11)))>0)real(11)=m.store(m.add(m.q(real(11)),m.c(0x6d2e68)));
        if(remainder(t.frame,200)==100&&frame()<=3860)pulse(1,0x6d1f08);if(t.frame==3999){clear_projectiles(pool,100);integer(0)=0;}follow(false,0x6d1a88);return;
    }
    if(frame()<4000)return;
    if(frame()<5500){
        if(remainder(t.frame,250)==90)corners(integer(0)==0?std::array<std::uint32_t,3>{{0,0,255}}:std::array<std::uint32_t,3>{{255,255,0}});
        else if(remainder(t.frame,250)==130){auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));const auto color=integer(0)==0?std::array<std::uint32_t,3>{{0,0,255}}:std::array<std::uint32_t,3>{{255,255,0}};
            for(unsigned i=0;i<80;++i){radial(angle,0x6d1f50,m.c(0x6d1bc0),color);angle=m.store(m.add(m.q(angle),m.c(0x6d2e58)));}integer(0)=1u-integer(0);
        }
    }else if(t.frame==5720)corners({{255,0,255}});
    else if(t.frame==5760){for(unsigned i=0;i<80;++i){if(i<=3){auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));for(unsigned j=0;j<110;++j){radial(angle,0x6d2e50,m.i(i*10u+310u),{{255,0,255}});angle=m.store(m.add(m.q(angle),m.c(0x6d2e48)));}}
            else for(unsigned j=0;j<6;++j)radial(m.store(m.mul(m.i(j),m.c(0x6d2750))),0x6d2e50,m.i(i*10u+310u),{{255,0,255}});
        }
    }
    if(remainder(t.frame,200)==100)pulse(2,0x6b25b8);if(t.frame==4000)real(2)=constant(0x6d1d00);follow(true,0x6d1a40);
}
}}
