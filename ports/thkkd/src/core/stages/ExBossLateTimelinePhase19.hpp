#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase19(){
    const auto satellites=[&](std::uint32_t parent,unsigned count_index,unsigned layers,std::uint32_t radial_step,std::uint32_t radial_base,std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,bool track){
        for(std::uint32_t i=0;signed_word(i)<signed_word(integer(count_index));++i)for(unsigned j=0;j<layers;++j){const auto slot=allocate_projectile(pool,{kind,group,color,0,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,parent);b.word(0x7c,i);
            b.quad(0xa0,m.store(m.mul(m.div(m.c(0x6d2208),m.i(integer(count_index))),m.i(b.word(0x7c)))));b.quad(0xa8,m.store(m.add(m.mul(m.i(j),m.c(radial_step)),m.c(radial_base))));b.quad(0x60,m.store(m.add(m.q(bullet(b.word(0x78)).quad(0x60)),m.q(b.quad(0xa0)))));
            const auto px=bullet(b.word(0x78));auto value=math.cosine(b.quad(0x60));b.quad(0x40,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(px.quad(0x40)))));const auto py=bullet(b.word(0x78));value=math.sine(b.quad(0x60));b.quad(0x48,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(py.quad(0x48)))));b.word(8,118);b.word(0x34,1);
            if(track){if(i==0&&j==0)integer(3)=static_cast<std::uint32_t>(slot);else if(i==3&&j==0)integer(4)=static_cast<std::uint32_t>(slot);}
        }
    };
    constexpr std::array<std::array<std::uint32_t,3>,3> colors{{{{0,255,0}},{{255,0,0}},{{0,0,255}}}};
    const auto outer_position=[&](bool vertical,bool extended_position){auto value=vertical?math.sine(constant(0x6d2948)):math.cosine(constant(0x6d2948));const auto first=m.store(m.add(m.mul(value,m.c(0x6d2240)),m.c(vertical?0x6d1af0:0x6d1a60)));if(!extended_position)return first;
        value=vertical?math.sine(constant(0x6d2da0)):math.cosine(constant(0x6d2da0));return m.store(m.add(m.mul(value,m.c(0x6d1f08)),m.q(first)));
    };
    if(t.frame==40){
        pool.cursors[0]=0;pool.cursors[1]=0;pool.cursors[2]=1000;pool.cursors[3]=2000;target(0x6d1af0,0x6d1a40);services.configure_phase(0,70,extra.boss_hp_multiplier*23000u,0xfffffff6u);
        real(0)=constant(0x6d2238);real(3)=constant(0x6d2da8);real(10)=constant(0x6d1ac8);integer(0)=18;integer(1)=6;
        real(1)=m.store(m.sub(m.q(real(3)),m.div(m.mul(m.sub(m.q(real(0)),m.q(real(3))),m.i(integer(0))),m.i(integer(1)))));
        // 原masked FIDIV零除数正常存负无穷，见divide-r3双版三精度原片段结果。
        real(2)=integer(2)==0?0xfff0000000000000ull:m.store(m.sub(m.q(real(3)),m.div(m.mul(m.sub(m.q(real(0)),m.q(real(3))),m.i(integer(0))),m.i(integer(2)))));real(4)=0;effect_at(10,{{32,32,32}},constant(0x6d1a60),constant(0x6d1af0));costume(3);
    }else if(t.frame==80){ex_boss_late_timeline_emit(pool,{{2,3,{{0,0,0}},0,255,1,41},constant(0x6d1a60),constant(0x6d1af0),16,constant(0x6d27f8),constant(0x6d1f50),0,0,0},player,p,random,precision,math);}
    else if(t.frame==170){effect_at(10,{{255,255,255}},constant(0x6d1a60),constant(0x6d1af0));costume(3);}
    else if(t.frame==210){const auto parent=static_cast<std::uint32_t>(allocate_projectile(pool,{26,1,{{255,255,255}},2,255,1,41}));if(parent!=0xffffffffu){auto b=bullet(parent);b.quad(0x40,constant(0x6d1a60));b.quad(0x48,constant(0x6d1af0));b.word(8,123);b.quad(0x60,0);b.word(0x34,1);}satellites(parent,0,3,0x6d1a40,0x6b25b8,1,2,{{255,255,255}},false);}
    else if(t.frame==260){
        for(unsigned i=0;i<3;++i){const auto angle=m.store(m.add(m.mul(m.i(i),m.c(0x6d2138)),m.c(0x6d1de0)));auto value=math.sine(angle);const auto y=m.store(m.add(m.mul(value,m.q(real(10))),m.c(0x6d1af0)));value=math.cosine(angle);const auto x=m.store(m.add(m.mul(value,m.q(real(10))),m.c(0x6d1a60)));effect_at(10,colors[i],x,y);}
        effect_at(10,{{0,0,0}},constant(0x6d1a60),constant(0x6d1af0));auto y=outer_position(true,false);auto x=outer_position(false,false);effect_at(10,{{0,255,255}},x,y);y=outer_position(true,true);x=outer_position(false,true);effect_at(10,{{255,255,0}},x,y);effect_at(10,{{255,0,255}},constant(0x6d2d90),constant(0x6d2d98));
    }else if(t.frame==300){
        for(unsigned i=0;i<3;++i){const auto angle=m.store(m.add(m.mul(m.i(i),m.c(0x6d2138)),m.c(0x6d1de0)));const auto parent=static_cast<std::uint32_t>(allocate_projectile(pool,{18,1,colors[i],2,255,1,41}));if(parent!=0xffffffffu){auto b=bullet(parent);auto value=math.cosine(angle);b.quad(0x40,m.store(m.add(m.mul(value,m.q(real(10))),m.c(0x6d1a60))));value=math.sine(angle);b.quad(0x48,m.store(m.add(m.mul(value,m.q(real(10))),m.c(0x6d1af0))));b.word(8,124);b.quad(0x60,angle);b.quad(0xa0,angle);b.word(0x34,1);}satellites(parent,1,3,0x6d1a40,0x6d1a88,1,2,colors[i],false);}
        auto parent=static_cast<std::uint32_t>(allocate_projectile(pool,{18,1,{{0,0,0}},0,255,1,41}));if(parent!=0xffffffffu){auto b=bullet(parent);b.quad(0x40,constant(0x6d1a60));b.quad(0x48,constant(0x6d1af0));b.word(8,125);b.quad(0x60,0);b.word(0x34,1);}satellites(parent,1,4,0x6d1a40,0x6d1a88,1,2,{{0,0,0}},true);
        parent=static_cast<std::uint32_t>(allocate_projectile(pool,{18,2,{{0,255,255}},0,255,1,41}));if(parent!=0xffffffffu){auto b=bullet(parent);b.quad(0x40,outer_position(false,false));b.quad(0x48,outer_position(true,false));b.word(8,126);b.quad(0x60,constant(0x6d2140));b.word(0x34,1);}satellites(parent,1,3,0x6d1a40,0x6d1a88,1,3,{{0,255,255}},false);
        parent=static_cast<std::uint32_t>(allocate_projectile(pool,{2,2,{{255,255,0}},0,255,1,41}));if(parent!=0xffffffffu){auto b=bullet(parent);b.quad(0x40,outer_position(false,true));b.quad(0x48,outer_position(true,true));b.word(8,125);b.quad(0x60,0);b.word(0x34,1);}satellites(parent,1,3,0x6d1898,0x6b25f8,0,3,{{255,255,0}},false);
        for(unsigned side=0;side<2;++side)for(unsigned i=1;i<12;++i){const auto slot=allocate_projectile(pool,{0,3,{{255,0,255}},0,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,integer(3u+side));b.quad(0x40,constant(0x6d2fa8));b.quad(0x48,constant(0x6d2fa8));b.quad(0xa0,m.store(m.i(i)));b.word(8,127);b.word(0x34,1);}
    }
    if(frame()>=331&&frame()<=1000){if(compare(m.i(1),m.q(real(4)))>0)real(4)=m.store(m.add(m.q(real(4)),m.c(0x6d19e0)));}
    else if(frame()>=1600&&compare(m.c(0x6d2a40),m.q(real(4)))<0)real(4)=m.store(m.sub(m.q(real(4)),m.c(0x6a7708)));
}
}}
