// 一面关底完整6941C0..6967D1；场景服务保留原调用边界，弹池失败写入映射副弹池末尾。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/stages/StageBossServices.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/EnemyDeathContext.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/stages/Stage1BossConstants.hpp"

namespace kaikeidou {
namespace stage1_boss_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline double host(std::uint64_t v){double d;std::memcpy(&d,&v,8);return d;}
// -1槽从A8E570开始，覆盖secondary[30]末48字节和secondary[31]全部192字节。
struct BulletView {
    ProjectilePools& pools;std::int32_t index;
    std::uint8_t& byte(std::size_t offset){if(index!=-1)return pools.bullets[static_cast<std::size_t>(index)].data[offset];const auto at=5904+offset;return pools.secondary[at/192].data[at%192];}
    void word(std::size_t offset,std::uint32_t value){for(unsigned i=0;i<4;++i)byte(offset+i)=static_cast<std::uint8_t>(value>>(8*i));}
    void quad(std::size_t offset,std::uint64_t value){for(unsigned i=0;i<8;++i)byte(offset+i)=static_cast<std::uint8_t>(value>>(8*i));}
    std::uint64_t quad(std::size_t offset){std::uint64_t v=0;for(unsigned i=0;i<8;++i)v|=std::uint64_t(byte(offset+i))<<(8*i);return v;}
};
}
inline void stage1_boss_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,
    EffectPool& effects,const ObjectRecord<240>& player,PatternParameters& parameters,BossPhaseContext& phase,
    BossStageContext& context,StagePatternContext& extra,EnemyDeathContext& death,
    Random& random,GameMath& math,RandomPrecision precision,StageBossServices& services){
    using namespace binary_float;using namespace stage1_boss_detail;
    const auto bits=static_cast<unsigned>(precision);const auto boss=phase.boss_index;
    const auto enemy_at=[&](std::uint32_t slot)->Enemy&{return slot==0xffffffffu?context.enemy_prefix:enemies.enemies[slot];};
    auto& enemy=enemy_at(boss);auto& ints=parameters.integers;
    const auto pick=[&](std::array<std::uint32_t,6> a){return a[parameters.difficulty<6?parameters.difficulty:3];};
    const auto real_pick=[&](std::array<std::uint64_t,6> a){return a[parameters.difficulty<6?parameters.difficulty:3];};
    const auto c=[](std::uint32_t a){return constant(a);};
    const auto n=[&](std::uint32_t a){return binary64(c(a));};
    const auto sub=[&](Number a,Number b){b.negative=!b.negative;return add(a,b,bits);};
    const auto deg=[&](std::uint64_t value){return to_binary64(divide(multiply(binary64(value),n(0x6d1860),bits),integer(180),bits));};
    const auto rad=[&](std::uint64_t value){return divide(multiply(binary64(value),integer(180),bits),n(0x6d1860),bits);};
    const auto frame=[&](){return signed_word(timeline.frame);};
    const auto set_frame=[&](std::uint32_t value){timeline.frame=value;parameters.game_frame=value;};
    const auto effect=[&](std::uint32_t kind,std::uint64_t x,std::uint64_t y,bool red=false){const auto color=services.color(red?255u:0u,red?0u:255u,0);allocate_effect(effects,projectiles.sound_triggers,{kind,color,x,y});};
    const auto transition=[&](std::uint64_t x,std::uint64_t y){effect(12,x,y);enemy.word(0x2c,3);enemy.word(0x30,0);enemy.word(0x34,0);};
    const auto emit=[&](const ProjectileEmission& input){emit_projectiles(projectiles,input,player.quad(0x90),player.quad(0x98),parameters,random,precision,math);};
    const auto wave=[&](std::int64_t remainder,bool second){
        constexpr std::array<std::uint32_t,8> widths{{0,1,2,3,3,2,1,0}};
        if(remainder>=0&&remainder<8)ints[2]=widths[static_cast<std::size_t>(remainder)];
        else if(remainder>=14&&remainder<22)ints[2]=widths[static_cast<std::size_t>(remainder-14)];
        if(!second){if(remainder==0||remainder==14)ints[1]=random.integer_bits(40,precision,true)+250;}
        else if(remainder==0){
            const auto dx=to_binary64(sub(binary64(player.quad(0x90)),binary64(enemy.quad(0x38))));
            const auto dy=to_binary64(sub(binary64(player.quad(0x98)),binary64(enemy.quad(0x40))));
            const auto aim=to_binary64(add(rad(to_binary64(math.atan2(dy,dx))),n(0x6d1858),bits));
            const auto factor=real_pick({{c(0x6d1868),c(0x6d19d8),c(0x6d1f50),c(0x6d1868),0,0}});
            parameters.reals[0]=to_binary64(add(multiply(binary64(factor),integer(ints[3]),bits),binary64(aim),bits));
            ints[3]+=signed_word(parameters.difficulty)>1?30u:60u;if(signed_word(ints[3])>=60)ints[3]=0xffffffe2u;
        }else if(remainder==14)parameters.reals[0]=to_binary64(add(random.real_number(c(0x6d1ae8),precision),n(0x6d1ec8),bits));
        const auto across=deg(second?to_binary64(sub(binary64(parameters.reals[0]),n(0x6d1b38))):to_binary64(integer(ints[1]-90)));
        const auto speed=to_binary64(multiply(integer(static_cast<std::uint32_t>(remainder*2+(remainder>7?-25:3))),n(0x6d19c0),bits));
        const std::uint32_t group=remainder>7?1u:(second?2u:0u);
        for(std::uint32_t i=0;signed_word(i)<=signed_word(ints[2]);++i){
            const auto slot=allocate_projectile(projectiles,{10,0,{{0,255,10}},0,255,1,41});if(slot==-1)continue;
            auto& bullet=projectiles.bullets[static_cast<std::size_t>(slot)];
            const auto offset=to_binary64(multiply(sub(multiply(integer(ints[2]),n(0x6d19c0),bits),integer(i)),n(0x6d1a88),bits));
            bullet.quad(0x40,to_binary64(add(multiply(math.cosine(across),binary64(offset),bits),binary64(enemy.quad(0x38)),bits)));
            bullet.quad(0x48,to_binary64(add(multiply(math.sine(across),binary64(offset),bits),binary64(enemy.quad(0x40)),bits)));
            bullet.quad(0x60,deg(second?parameters.reals[0]:to_binary64(integer(ints[1]))));
            bullet.quad(0x50,to_binary64(multiply(math.cosine(bullet.quad(0x60)),binary64(speed),bits)));
            bullet.quad(0x58,to_binary64(multiply(math.sine(bullet.quad(0x60)),binary64(speed),bits)));
            bullet.word(0x78,group);if(second&&group==2)bullet.quad(0xb0,parameters.reals[0]);
            bullet.word(0xc,static_cast<std::uint32_t>(remainder>7?remainder-14:remainder));bullet.word(8,17);
        }
    };
    switch(timeline.subphase){
    case 0:if(timeline.frame==1)services.dialog(0);break;
    case 1:{
        const auto remainder=frame()%signed_word(pick({{250,200,100,80,0,0}}));
        if(frame()<34){
            if(timeline.frame==1)services.begin_boss_effect();
            else if(timeline.frame==12)transition(enemy.quad(0x38),enemy.quad(0x40));
            else if(timeline.frame==33){set_frame(pick({{229,179,79,51,0,0}}));ints[0]=0;services.configure_phase(0,30,extra.boss_hp_multiplier*11000u,0xfffffff6u);enemy.word(8,14);enemy.word(0xc,40);enemy.word(0x2c,0);}
        }else{
            if(remainder<8||(remainder>13&&remainder<22))wave(remainder,false);
            if(remainder==signed_word(pick({{148,112,26,8,0,0}}))&&++ints[0]==2){
                ints[0]=0;enemy.word(0xc,0);auto target=player.quad(0x90);
                if(host(target)<host(c(0x6d1d00)))target=c(0x6d1d00);else if(host(target)>host(c(0x6d23a0)))target=c(0x6d23a0);
                if(host(enemy.quad(0x38))<=host(target))enemy.quad(0xa8,to_binary64(sub(add(random.real_number(c(0x6d1a90),precision),binary64(enemy.quad(0x38)),bits),n(0x6d1a40))));
                else enemy.quad(0xa8,to_binary64(add(sub(binary64(enemy.quad(0x38)),random.real_number(c(0x6d1a90),precision)),n(0x6d1a40),bits)));
                if(host(enemy.quad(0xa8))<host(c(0x6d1aa8)))enemy.quad(0xa8,to_binary64(add(random.real_number(c(0x6b25f8),precision),n(0x6d1aa8),bits)));
                else if(host(enemy.quad(0xa8))>host(c(0x6d2398)))enemy.quad(0xa8,to_binary64(sub(n(0x6d2398),random.real_number(c(0x6b25f8),precision))));
                enemy.quad(0xb0,to_binary64(add(random.real_number(c(0x6d1d50),precision),n(0x6d2780),bits)));
            }
        }break;
    }
    case 2:
        if(timeline.frame==32){
            spell_name(context.spell_name.data(),0,parameters.difficulty);services.configure_phase(1,40,extra.boss_hp_multiplier*9500u,3);
            enemy.word(0xc,0);enemy.word(0x80,0);enemy.quad(0xa8,c(0x6d1a60));enemy.quad(0xb0,c(0x6d1b38));enemy.word(8,22);
            ints[4]=10;death.pattern_integer_5=pick({{20,20,32,32,0,0}})+1;extra.integers_6_to_19[0]=pick({{40,30,19,16,0,0}});extra.integers_6_to_19[1]=pick({{8,8,8,8,0,0}});
            extra.real_4=real_pick({{c(0x6d1948),c(0x6d1948),c(0x6d1b18),c(0x6d1b18),0,0}});for(unsigned i=10;i<20;++i)extra.integers_6_to_19[i-6]=0;
        }else if(timeline.frame==80){enemy_at(phase.boss_index).quad(0xa8,c(0x6d1828));enemy.quad(0xb0,c(0x6d1aa8));}
        else if(frame()>137){
            if(frame()%signed_word(extra.integers_6_to_19[0])==0){extra.integers_6_to_19[ints[4]-6]=death.pattern_integer_5;extra.reals_10_to_19[ints[4]-10]=to_binary64(multiply(random.real_number(c(0x6d22a8),precision),n(0x6d1960),bits));if(++ints[4]==20)ints[4]=10;}
            for(unsigned i=10;i<20;++i){auto& count=extra.integers_6_to_19[i-6];if(count&&signed_word(--count)%signed_word(extra.integers_6_to_19[1])==0){
                const auto slot=allocate_projectile(projectiles,{9,0,{{0,128,255}},2,255,1,41});if(slot==-1)continue;auto& bullet=projectiles.bullets[static_cast<std::size_t>(slot)];
                bullet.quad(0x40,extra.reals_10_to_19[i-10]);bullet.quad(0x48,c(0x6d1c18));bullet.quad(0x58,extra.real_4);bullet.word(8,18);
            }}
            const auto cycle=signed_word(timeline.frame-138)%signed_word(pick({{350,300,228,210,0,0}}));
            if(timeline.frame==138){enemy.word(0xc,0);enemy.quad(0xa8,c(0x6d1828));enemy.quad(0xb0,c(0x6d1aa8));}
            else if(cycle==6&&signed_word(parameters.difficulty)>=2){
                const bool right=host(enemy.quad(0x38))>=192.;const auto x=c(right?0x6d3190:0x6d1e20),angle=right?c(0x6d1860):0;
                const auto offset=integer((4-parameters.difficulty)<<6);const auto base=to_binary64(host(player.quad(0x98))>=host(c(0x6d1af0))?sub(binary64(player.quad(0x98)),offset):add(offset,binary64(player.quad(0x98)),bits));
                const auto y=to_binary64(sub(add(random.real_number(c(0x6d1ae8),precision),binary64(base),bits),n(0x6d1a40)));
                for(unsigned i=0;i<=12;++i){ints[0]=i*6;ints[1]=i*6+1;emit({{11,1,{{0,255,0}},2,255,0,41},x,y,1,0,c(0x6d3188),angle,1,2});}
            }else if(cycle==16){
                const bool right=host(enemy.quad(0x38))>=192.;const auto x=c(right?0x6d1a78:0x6d1a80),angle=right?c(0x6d1860):0;ints[2]=pick({{78,60,24,24,0,0}});
                for(unsigned i=0;i<6;++i){ints[0]=i*5;ints[1]=i*5+1;ints[3]=static_cast<std::uint32_t>(signed_word(i*ints[2])/6);emit({{11,1,{{187,51,0}},2,255,0,41},x,c(0x6d2748),1,0,c(0x6d2130),angle,1,19});}
            }else if(cycle==48){enemy.word(0x80,1);const bool right=host(enemy.quad(0x38))>=192.;enemy.quad(0x38,c(right?0x6d1f70:0x6d1828));enemy.quad(0xb8,c(right?0x6d3180:0x6d2130));}
            else if(cycle==208){enemy.quad(0xb8,0);enemy.quad(0x38,to_binary64(integer(host(enemy.quad(0x38))>=192.?360u:24u)));}
        }break;
    case 3:{
        const auto remainder=signed_word(timeline.frame-40)%signed_word(pick({{195,155,87,64,0,0}}));
        if(timeline.frame==1){enemy.word(8,14);enemy.quad(0xa8,c(0x6d1a60));enemy.quad(0xb0,c(0x6d1f08));}
        else if(timeline.frame==60)transition(c(0x6d1a60),c(0x6d1f08));
        else if(timeline.frame==100){set_frame(pick({{194,154,86,63,0,0}})+40);ints[4]=0;ints[3]=0xffffffe2u;services.configure_phase(0,30,extra.boss_hp_multiplier*11000u,0xfffffff6u);enemy.word(0xc,40);enemy.word(8,14);enemy.quad(0xa8,enemy.quad(0x38));enemy.quad(0xb0,enemy.quad(0x40));enemy.word(0x2c,0);}
        else if(frame()>100){
            if(remainder<8||(remainder>13&&remainder<22))wave(remainder,true);
            if(remainder==signed_word(pick({{118,102,26,8,0,0}}))&&++ints[4]==2){ints[4]=0;enemy.word(0xc,0);enemy.quad(0xa8,to_binary64(sub(add(random.real_number(c(0x6b25b8),precision),n(0x6d1a60),bits),n(0x6d1a88))));enemy.quad(0xb0,to_binary64(sub(add(random.real_number(c(0x6d1aa8),precision),n(0x6d1f08),bits),n(0x6b25f8))));}
        }break;
    }
    case 4:{
        if(timeline.frame==1)ints[0]=pick({{502,452,268,208,0,0}});
        if(timeline.frame==32){spell_name(context.spell_name.data(),1,parameters.difficulty);services.configure_phase(1,41,pick({{25000,19500,19500,24000,0,0}}),7);enemy.word(0xc,0);enemy.quad(0xa8,c(0x6d1a60));enemy.quad(0xb0,c(0x6d1aa8));enemy.word(8,parameters.difficulty==3?14u:23u);ints[1]=pick({{660,660,380,380,0,0}});}
        if(parameters.difficulty==0){
            const auto easy=[&](std::uint32_t first,bool second){
                const auto r=frame()%20;const bool left=r==(second?10:0),right=r==(second?0:10);
                if(frame()>=first&&frame()<first+131&&(left||right)){const auto y=to_binary64(multiply(integer(timeline.frame-first),n(0x6d2480),bits));effect(10,left?0:c(0x6d1cb0),y,!left);}
                if(frame()>=first+40&&frame()<first+171&&(left||right)){
                    const auto slot=allocate_projectile(projectiles,{2,1,left?std::array<std::uint32_t,3>{{0,255,0}}:std::array<std::uint32_t,3>{{255,0,0}},2,255,0,41});BulletView bullet{projectiles,slot};
                    bullet.quad(0xa0,left?c(0x6d1860):0);bullet.quad(0xa8,c(left?0x6d2748:0x6d2998));bullet.quad(0xb0,c(0x6d3178));
                    bullet.quad(0x40,to_binary64(add(multiply(math.cosine(bullet.quad(0xa0)),n(0x6d2748),bits),binary64(bullet.quad(0xa8)),bits)));
                    bullet.quad(0x48,to_binary64(multiply(integer(timeline.frame-first-40),n(0x6d2480),bits)));bullet.word(0x34,1);bullet.word(8,160);
                }
            };easy(130,false);easy(370,true);
        }else if(parameters.difficulty==1||parameters.difficulty==2){
            if(timeline.frame==88||timeline.frame==ints[0]-40){
                for(unsigned i=0;i<14;++i){const bool selected=parameters.difficulty==1?((i%2)==(timeline.frame==88?0u:1u)):(timeline.frame==88?i<8:i>7);if(!selected)continue;const auto y=to_binary64(integer(i*34));effect(10,0,y,true);effect(10,c(0x6d1cb0),y);}
            }else if(timeline.frame==128||timeline.frame==ints[0]){
                for(unsigned i=0;i<14;++i){const bool selected=parameters.difficulty==1?((i%2)==(timeline.frame==128?0u:1u)):(timeline.frame==128?i<8:i>7);if(!selected)continue;
                    const auto first=allocate_projectile(projectiles,{2,1,{{255,0,0}},2,255,0,41});const auto second=allocate_projectile(projectiles,{2,1,{{0,255,0}},2,255,0,41});BulletView a{projectiles,first},b{projectiles,second};
                    if(parameters.difficulty==1)b.quad(0xd8,c(0x6d3170));else if(parameters.difficulty==2)b.quad(0xd8,i%2?c(0x6d1ee8):c(0x6d1868));
                    a.quad(0x40,c(0x6d1840));a.quad(0x48,to_binary64(integer(i*34)));a.quad(0x50,real_pick({{c(0x6d2488),c(0x6d1ad0),c(0x6d1b18),c(0x6d1b18),0,0}}));a.word(0x34,1);a.word(8,25);
                    b.quad(0x40,c(0x6d1a38));b.quad(0x48,a.quad(0x48));b.quad(0x50,real_pick({{c(0x6d1fc0),c(0x6d2640),c(0x6d2248),c(0x6d2248),0,0}}));b.word(0x34,1);b.word(0x78,static_cast<std::uint32_t>(first));b.quad(0xa0,a.quad(0x50));b.word(0x7c,0);b.quad(0xa8,0);b.quad(0xb0,to_binary64(integer(pick({{180,190,210,210,0,0}}))));b.quad(0xb8,c(0x6d1900));b.word(8,21);
                }
            }
        }else{
            const auto orbit=[&](bool second){
                const auto slot=allocate_projectile(projectiles,{10,2,second?std::array<std::uint32_t,3>{{200,0,0}}:std::array<std::uint32_t,3>{{0,200,0}},0,255,1,41});BulletView b{projectiles,slot};
                b.quad(0xa0,to_binary64(multiply(random.real_number(c(0x6d1c60),precision),n(0x6d1950),bits)));b.quad(0xa8,c(0x6d1868));
                b.quad(0x40,to_binary64(add(multiply(math.cosine(b.quad(0xa0)),n(second?0x6d1878:0x6d1e20),bits),binary64(second?player.quad(0x90):enemy.quad(0x38)),bits)));
                b.quad(0x48,to_binary64(add(multiply(math.sine(b.quad(0xa0)),n(second?0x6d1878:0x6d1e20),bits),binary64(second?player.quad(0x98):enemy.quad(0x40)),bits)));
                b.quad(0x50,to_binary64(multiply(math.cosine(b.quad(0xa0)),n(0x6d1868),bits)));
                const auto sine=to_binary64(multiply(math.sine(b.quad(0xa0)),n(0x6d1868),bits));const auto scale=add(random.real_number(c(0x6d1aa8),precision),n(0x6d1960),bits);
                b.quad(0x58,to_binary64(multiply(multiply(scale,binary64(sine),bits),n(0x6a7708),bits)));b.quad(0x60,to_binary64(math.atan2(b.quad(0x58),b.quad(0x50))));b.word(8,second?162u:161u);
            };
            if(timeline.frame==130)effect(10,enemy.quad(0x38),enemy.quad(0x40));
            if(frame()>169&&frame()%4==0)orbit(false);
            if(timeline.frame==290)effect(13,enemy.quad(0x38),enemy.quad(0x40),true);
            if(frame()>329&&frame()%5==2)orbit(true);
            if(frame()>399){if(frame()%300==100)effect(10,enemy.quad(0x38),enemy.quad(0x40));else if(frame()%300==140)emit({{2,1,{{0,255,0}},2,255,1,41},enemy.quad(0x38),enemy.quad(0x40),1,0,c(0x6d2480),0,0,0});}
        }break;
    }
    case 5:if(timeline.frame==32)services.retire_boss(0);else if(timeline.frame==196)services.dialog(1);break;
    case 6:if(timeline.frame==1)services.award_stage_clear();else if(timeline.frame==200){++death.stage;timeline.part=0;timeline.subphase=0;set_frame(0);services.change_stage_resources(death.stage);services.restore_stage_music(1);}break;
    default:break;
    }
}
}
