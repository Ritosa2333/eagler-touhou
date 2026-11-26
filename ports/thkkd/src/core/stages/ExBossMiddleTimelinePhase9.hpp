#pragma once
#include "core/stages/ExBossMiddleTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_middle_timeline_detail {
inline void Engine::phase9(){
    if(player.word(0x2c)){protection.boss_state_a7ea60=1;allocation.field_a7ea58=0;}
    else{protection.boss_state_a7ea60=0;allocation.field_a7ea58=1;}
    if(character==0)phase9_reimu();else if(character==1)phase9_marisa();else phase9_sanae();
}
inline void Engine::phase9_reimu(){
    auto& s=pool.secondary[0];
    if(t.frame==1){ex_boss_middle_timeline_name(context.spell_name,0x6c5d68);services.configure_phase(1,76,extra.boss_hp_multiplier*25000u,134);enemy().word(0x28,1);}
    else if(t.frame==60){effect(12,{{200,0,200}});costume(3);}
    else if(t.frame==100){
        s.quad(0x28,constant(0x6d1858));pool.sound_triggers[18]=1;
        for(unsigned i=0;i<3;++i){const auto a=random.real_number(constant(0x6d1c60),precision);real(i)=m.store(m.mul(a,m.c(0x6d1950)));}
        real(3)=constant(0x6d1860);real(5)=0;real(6)=constant(0x6d18e8);real(7)=0;real(10)=constant(0x6d1b30);real(11)=constant(0x6d2f28);real(12)=constant(0x6d2f20);
    }else if(t.frame==160)target(0x6d1af0,0x6d1a40);
    else if(t.frame==220){
        s.word(4,147);s.quad(0x70,0);
        for(unsigned i=1;i<=9;++i)for(unsigned j=0;j<=9;++j){
            const auto slot=allocate_projectile(pool,{9,0,{{255u-i*25u,0,i*25u}},2,255,1,41});if(slot==-1)continue;
            auto value=math.cosine(m.store(m.mul(m.i(j),m.c(0x6d1dd8))));const auto cx=m.store(value);
            value=math.sine(m.store(m.mul(m.i(i),m.c(0x6d29b8))));ex_vectors.word((i*10u+j)*12u,binary_float::to_binary32(m.mul(value,m.q(cx))));
            value=math.sine(m.store(m.mul(m.i(j),m.c(0x6d1dd8))));const auto sy=m.store(value);
            value=math.sine(m.store(m.mul(m.i(i),m.c(0x6d29b8))));ex_vectors.word((i*10u+j)*12u+4u,binary_float::to_binary32(m.mul(value,m.q(sy))));
            value=math.cosine(m.store(m.mul(m.i(i),m.c(0x6d29b8))));ex_vectors.word((i*10u+j)*12u+8u,binary_float::to_binary32(value));
            // 原5CC6A0从j读取初位置，随后仍以i*10+j保存弹槽索引。
            auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0x40,m.store(m.add(m.mul(binary_float::finite_binary32(ex_vectors.word(j*12u)),m.q(real(5))),m.q(s.quad(0x10)))));
            b.quad(0x48,m.store(m.add(m.mul(binary_float::finite_binary32(ex_vectors.word(j*12u+4u)),m.q(real(5))),m.q(s.quad(0x18)))));b.word(8,25);b.word(0x34,1);bullet(i*10u+j).word(0x9c,static_cast<std::uint32_t>(slot));
        }
        for(unsigned i=0;i<2;++i){
            const auto slot=allocate_projectile(pool,{1,0,{{255u-i*255u,0,i*255u}},2,255,1,41});if(slot==-1)continue;
            ex_vectors.word(i*12u,0);ex_vectors.word(i*12u+4u,0);const auto value=math.cosine(m.store(m.mul(m.i(i),m.c(0x6d1860))));ex_vectors.word(i*12u+8u,binary_float::to_binary32(value));
            auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0x40,m.store(m.add(m.mul(binary_float::finite_binary32(ex_vectors.word(i*12u)),m.q(real(5))),m.q(s.quad(0x10)))));
            b.quad(0x48,m.store(m.add(m.mul(binary_float::finite_binary32(ex_vectors.word(i*12u+4u)),m.q(real(5))),m.q(s.quad(0x18)))));b.word(8,25);b.word(0x34,1);bullet(i).word(0x9c,static_cast<std::uint32_t>(slot));
        }
    }else if(t.frame==260)enemy().quad(0xb0,constant(0x6d1a88));
    if(frame()<220)return;
    constexpr std::array<std::uint32_t,4> increments{{0x6d1cf8,0x6d19e0,0x6b2620,0x6d2f18}};
    for(unsigned i=0;i<4;++i)real(i)=m.store(m.add(m.q(real(i)),m.c(increments[i])));
    ex_boss_middle_timeline_ease(real(5),real(6),constant(0x6d1cc0),m.precision);ex_boss_middle_timeline_ease(real(6),constant(0x6d1858),constant(0x6d1cc0),m.precision);
    const auto rotate=[&](unsigned index){
        for(unsigned axis=0;axis<3;++axis){const auto sine=math.sine(real(axis));const auto angle=m.store(m.add(m.mul(sine,m.c(0x6d2f10)),m.q(real(10u+axis))));stage6_rotate_float3(axis,ex_vectors.data.data()+index*12u,ex_vectors.data.data()+index*12u,angle,m.precision);}
        const auto slot=bullet(index).word(0x9c);auto b=bullet(slot);b.quad(0x40,m.store(m.add(m.mul(binary_float::finite_binary32(ex_vectors.word(index*12u)),m.q(real(5))),m.q(s.quad(0x10)))));
        b.quad(0x48,m.store(m.add(m.mul(binary_float::finite_binary32(ex_vectors.word(index*12u+4u)),m.q(real(5))),m.q(s.quad(0x18)))));
        const auto z=binary_float::finite_binary32(ex_vectors.word(index*12u+8u));b.word(0x30,index<2?ex_boss_middle_timeline_convert(m.add(m.mul(z,m.c(0x6b25b8)),m.c(0x6d2ef8))):ex_boss_middle_timeline_convert(m.div(m.mul(z,m.c(0x6d2f08)),m.c(0x6d2f00)))+168u);
    };
    for(unsigned i=1;i<=9;++i)for(unsigned j=0;j<=9;++j)rotate(i*10u+j);for(unsigned i=0;i<2;++i)rotate(i);
}
inline void Engine::phase9_marisa(){
    auto& s=pool.secondary[0];
    if(t.frame==1){ex_boss_middle_timeline_name(context.spell_name,0x6c5d80);services.configure_phase(1,76,extra.boss_hp_multiplier*19000u,135);enemy().word(0x28,1);}
    else if(t.frame==60){effect(12,{{255,255,255}});costume(3);}
    else if(t.frame==100){s.quad(0x28,constant(0x6d1d00));pool.sound_triggers[18]=1;}
    else if(t.frame==160){target(0x6d1af0,0x6d1a40);effect(12,{{255,0,0}});const auto a=random.real_number(constant(0x6d1c60),precision);real(0)=m.store(m.mul(a,m.c(0x6d1950)));real(1)=constant(0x6d1950);real(2)=constant(0x6d1ab0);real(3)=0;integer(10)=0xffffffffu;integer(11)=60;}
    else if(t.frame==200){
        for(unsigned i=0;i<5;++i){const auto slot=allocate_projectile(pool,{2,1,{{255,0,0}},2,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
            const auto angle=m.mul(m.i(i),m.c(0x6d1860));b.quad(0xa0,m.store(m.div(m.add(angle,angle),m.c(0x6d1910))));b.quad(0xb0,real(2));
            auto value=math.cosine(m.store(m.add(m.q(real(0)),m.q(b.quad(0xa0)))));b.quad(0x40,m.store(m.add(m.mul(value,m.q(b.quad(0xb0))),m.q(s.quad(0x10)))));
            value=math.sine(m.store(m.add(m.q(real(0)),m.q(b.quad(0xa0)))));b.quad(0x48,m.store(m.add(m.mul(value,m.q(b.quad(0xb0))),m.q(s.quad(0x18)))));b.word(8,148);b.word(0x34,1);b.word(0x78,0);b.word(0x7c,0);integer(i)=static_cast<std::uint32_t>(slot);
        }
        const auto slot=allocate_projectile(pool,{18,1,{{255,240,0}},0,255,1,41});if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0x40,constant(0x6d1a60));b.quad(0x48,constant(0x6d1af0));b.word(8,150);b.word(0x34,1);}enemy().quad(0xb0,constant(0x6d1aa8));
    }
    if(frame()>=211&&frame()<=270)real(1)=m.store(m.add(m.q(real(1)),m.c(0x6d2ef0)));
    if(frame()<211)return;
    const auto sine=math.sine(real(3));real(0)=m.store(m.add(m.add(m.mul(sine,m.c(0x6d1aa0)),m.q(real(1))),m.q(real(0))));real(3)=m.store(m.add(m.q(real(3)),m.c(0x6a7708)));
    if(--integer(11))return;
    if(++integer(10)==5){for(unsigned i=0;i<5;++i){bullet(integer(i)).word(0x78,1);bullet(integer(i)).word(0x7c,1);}integer(10)=0xffffffffu;integer(11)=90;}
    else{bullet(integer(integer(10))).word(0x78,1);integer(11)=integer(10)==4?90u:60u;}
}
inline void Engine::phase9_sanae(){
    auto& s=pool.secondary[0];
    if(t.frame==1){ex_boss_middle_timeline_name(context.spell_name,0x6c5d98);services.configure_phase(1,90,extra.boss_hp_multiplier*17000u,136);enemy().word(0x28,1);}
    else if(t.frame==60){effect(12,{{255,255,255}});costume(3);}
    else if(t.frame==100){s.quad(0x28,constant(0x6d18c0));pool.sound_triggers[18]=1;integer(2)=50;real(0)=constant(0x6d1b40);}
    if(frame()<150)return;
    if(signed_word(integer(2))>=1){
        --integer(2);if(integer(2)==40)effect_at(15,{{255,0,255}},s.quad(0x10),s.quad(0x18));
        else if(integer(2)==0){
            const auto source=ex_boss_middle_timeline_colored_effect(effects,effect_prefix,pool.sound_triggers,14,5,99999999,136,s.quad(0x10),s.quad(0x18),services);
            const auto slot=allocate_enemy(enemies);if(slot!=-1){auto& b=enemies.enemies.at(static_cast<std::uint32_t>(slot));auto& a=source==-1?effect_prefix:effects.effects.at(static_cast<std::uint32_t>(source));
                b.word(4,a.word(0x10));b.word(0x18,a.word(0x14));b.word(0x14,b.word(0x18));b.word(8,a.word(0x18));b.quad(0x38,a.quad(0x60));b.quad(0x40,a.quad(0x68));b.word(0x80,60);b.word(0x84,b.word(0x80));b.quad(0xa8,constant(0x6d2ee8));b.quad(0xb0,constant(0x6d27f8));
            }
            enemy().quad(0xa8,constant(compare(m.c(0x6d1a60),m.q(s.quad(0x10)))>=0?0x6d23e0:0x6d1ae8));
        }
    }else if(compare(m.c(0x6d1a40),m.q(s.quad(0x28)))<0)s.quad(0x28,m.store(m.sub(m.q(s.quad(0x28)),m.c(0x6d1968))));
}
}}
