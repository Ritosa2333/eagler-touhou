// 完整65e330/65d7b0炸弹更新和65d470初始化，原屏震、速度及屏内筛选依赖同序执行。
#pragma once
#include "core/player/PlayerBombPool.hpp"
#include "core/player/PlayerBombContext.hpp"
#include "core/player/PlayerShotFireContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/EnemyDamage.hpp"
#include "core/shared/EffectUpdate.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/player/PlayerOptions.hpp"
#include "core/math/FiniteSqrt.hpp"
#include "core/shared/FloorToInt32.hpp"
namespace kaikeidou {
inline void reset_player_bomb_speed(ObjectRecord<240>& player,std::uint32_t character,std::uint64_t multiplier,unsigned precision){
    const options_motion_detail::Arithmetic m{precision};
    player.quad(0xb0,m.store(m.mul(m.n(character==1?5.:4.5),m.q(multiplier))));
    player.quad(0xc0,m.store(m.mul(m.n(2.),m.q(multiplier))));
    player.quad(0xb8,m.store(m.div(m.q(player.quad(0xb0)),finite_square_root(crt_sse2::bits(2.),precision))));
    player.quad(0xc8,m.store(m.div(m.q(player.quad(0xc0)),finite_square_root(crt_sse2::bits(2.),precision))));
}
inline bool player_bomb_enemy_visible(const Enemy& enemy){
    const auto x=crt_sse2::real(enemy.quad(0x38)),y=crt_sse2::real(enemy.quad(0x40));return x>0.&&x<384.&&y>0.&&y<448.;
}
inline void initialize_player_bomb(ObjectRecord<240>& player,PlayerBombPool& pool,const PlayerShotFireContext& fire,
    BossPhaseContext& boss,EffectShake& shake,GameMath& math,unsigned precision){
    const options_motion_detail::Arithmetic m{precision};auto& b=pool.bombs[0];
    player.word(0x18,player.word(0x18)-1u);player.word(0,0);
    player.word(0x2c,player_bomb_durations.at(fire.options.character*3u+fire.options.weapon));
    player.quad(0x30,crt_sse2::bits(-384.));player.quad(0x38,crt_sse2::bits(128.));boss.bomb_flag=1;
    const auto selector=fire.options.character*10u+fire.options.weapon;
    const auto q=[&](std::size_t o,double v){b.quad(o,crt_sse2::bits(v));};
    const auto at_player=[&](){b.quad(0x10,options_detail::stored_bits(player.quad(0x90)));b.quad(0x18,options_detail::stored_bits(player.quad(0x98)));};
    if(selector==0){
        player.word(0x28,player.word(0x2c)+20u);at_player();q(0x20,0.);q(0x28,.1);q(0x30,0.);q(0x38,14.);q(0x40,255.);
        b.quad(0x48,m.store(m.mul(math.sine(b.quad(0x30)),m.q(b.quad(0x38)))));
    }else if(selector==1){
        player.word(0x28,player.word(0x2c)+20u);b.quad(0x58,options_detail::stored_bits(player.quad(0x90)));b.quad(0x60,options_detail::stored_bits(player.quad(0x98)));
        b.quad(0x20,0x3fe921fb54442d28ull);q(0x28,128.);q(0x30,10.);q(0x38,0.);q(0x40,10.);q(0x48,.18);q(0x50,1.);
    }else if(selector==2){player.word(0x28,player.word(0x2c)+20u);at_player();q(0x20,0.);q(0x28,0.);}
    else if(selector==10){player.word(0x28,player.word(0x2c)+20u);q(0x20,0.);pool.bombs[1].quad(0x20,crt_sse2::bits(2.));q(0x30,0.);q(0x38,0.);q(0x40,0.);q(0x48,0.);}
    else if(selector==11||selector==12||selector==20){
        player.word(0x28,player.word(0x2c)+60u);reset_player_bomb_speed(player,fire.options.character,crt_sse2::bits(selector==11?.1:.3),precision);
        set_effect_screen_shake(shake,4,player_bomb_durations[selector==11?4:selector==12?5:6]);
    }else if(selector==21)player.word(0x28,player.word(0x2c)+20u);
    else if(selector==22){player.word(0x28,player.word(0x2c)+20u);at_player();q(0x20,0.);q(0x28,0.);b.word(4,32);}
}
inline void update_player_bombs(ObjectRecord<240>& player,PlayerBombPool& pool,PlayerBombContext& context,
    PlayerShotFireContext& fire,BossPhaseContext& boss,const EnemyDamageContext& damage,const EnemyPool& enemies,
    EffectShake& shake,Random& random,PlayerShotFireInput& input,GameMath& math,unsigned precision){
    using options_detail::signed_word;const options_motion_detail::Arithmetic m{precision};auto& b=pool.bombs[0];
    if(player.word(0x18)!=0&&(player.word(0)==0||player.word(0)==1||(player.word(0)==2&&signed_word(player.word(4))>=40))&&player.word(0x2c)==0){
        auto key=input.read_counter(context.bomb_binding);
        if(fire.replay_variant){key=key!=0?1u:0u;if(fire.replay_mode!=0)key=(fire.replay_buttons>>4)&1u;else fire.replay_buttons|=key*16u;}
        if(key==1&&fire.fire_lock==0)initialize_player_bomb(player,pool,fire,boss,shake,math,precision);
    }
    if(player.word(0x2c)==0)return;
    if(signed_word(damage.field_a7eaac-damage.field_a7eaa4)>signed_word(damage.field_a7eab0))boss.bomb_flag=1;
    if(signed_word(player.word(0x2c))<30)player.quad(0x30,ease_option_value(player.quad(0x30),crt_sse2::bits(-384.),crt_sse2::bits(20.),precision));
    else if(crt_sse2::real(player.quad(0x38))<428.){
        if(crt_sse2::real(player.quad(0x30))<0.)player.quad(0x30,ease_option_value(player.quad(0x30),crt_sse2::bits(1.),crt_sse2::bits(12.),precision));
        else player.quad(0x38,ease_option_value(player.quad(0x38),crt_sse2::bits(429.),crt_sse2::bits(12.),precision));
    }
    const auto duration=player_bomb_durations.at(fire.options.character*3u+fire.options.weapon);
    if(signed_word(player.word(0x2c))>signed_word(duration-32u))context.screen_tint=static_cast<std::uint8_t>((player.word(0x2c)-(duration-32u))*6u+63u);
    else if(signed_word(player.word(0x2c))<32)context.screen_tint=static_cast<std::uint8_t>((32u-player.word(0x2c))*6u+63u);
    const auto add=[&](PlayerBomb& bomb,std::size_t o,binary_float::Number v){bomb.quad(o,m.store(m.add(m.q(bomb.quad(o)),v)));};
    const auto addc=[&](std::size_t o,double v){add(b,o,m.n(v));};
    const auto mulc=[&](std::size_t o,double v){b.quad(o,m.store(m.mul(m.q(b.quad(o)),m.n(v))));};
    const auto selector=fire.options.character*10u+fire.options.weapon;const auto timer=signed_word(player.word(0x2c));
    if(selector==0){
        if(timer<=40){mulc(0x38,.707);addc(0x40,-6.2);}
        else{add(b,0x20,m.q(b.quad(0x28)));mulc(0x28,.96);addc(0x30,.043);}
        b.quad(0x48,m.store(m.mul(math.sine(b.quad(0x30)),m.q(b.quad(0x38)))));
    }else if(selector==1){
        if(timer<=signed_word(player_bomb_durations[1]-10u)){addc(0x48,.0001);if(timer<20)addc(0x50,-.05);}
        for(const auto& e:enemies.enemies)if(e.word(0)==1&&player_bomb_enemy_visible(e)){
            const auto x=m.store(m.sub(m.q(e.quad(0x38)),m.q(b.quad(0x58))));
            const auto y=m.store(m.sub(m.q(e.quad(0x40)),m.q(b.quad(0x60))));const auto angle=m.store(math.atan2(y,x));
            add(b,0x58,m.mul(math.cosine(angle),m.q(b.quad(0x40))));add(b,0x60,m.mul(math.sine(angle),m.q(b.quad(0x40))));break;
        }
        for(std::uint32_t i=0;i<8;++i){
            const auto angle=m.store(m.add(m.div(m.mul(m.i(i),m.q(0x400921fb54442d28ull)),m.n(4.)),m.q(b.quad(0x20))));
            const auto radius=m.store(m.mul(math.sine(b.quad(0x38)),m.q(b.quad(0x28))));
            pool.bombs[i].quad(0x10,m.store(m.add(m.mul(math.cosine(angle),m.q(radius)),m.q(b.quad(0x58)))));
            pool.bombs[i].quad(0x18,m.store(m.add(m.mul(math.sine(angle),m.q(radius)),m.q(b.quad(0x60)))));
        }
        add(b,0x20,m.q(b.quad(0x48)));addc(0x38,.05);
    }else if(selector==2){addc(0x18,-2.);addc(0x20,-.07);if(timer>=signed_word(player_bomb_durations[2]-20u))addc(0x28,.05);else if(timer<20)addc(0x28,-.05);}
    else if(selector==10){
        if(timer>=signed_word(player_bomb_durations[3]-19u))addc(0x30,12.75);else if(timer<20)addc(0x30,-12.75);
        const auto dx=m.store(m.mul(math.cosine(b.quad(0x38)),m.n(128.))),dy=m.store(m.mul(math.sine(b.quad(0x38)),m.n(128.)));
        b.quad(0x10,m.store(m.add(m.q(player.quad(0x90)),m.q(dx))));b.quad(0x18,m.store(m.add(m.q(player.quad(0x98)),m.q(dy))));
        pool.bombs[1].quad(0x10,m.store(m.sub(m.q(player.quad(0x90)),m.q(dx))));pool.bombs[1].quad(0x18,m.store(m.sub(m.q(player.quad(0x98)),m.q(dy))));
        addc(0x38,-.1);addc(0x20,.00714);add(pool.bombs[1],0x20,m.n(-.00714));
    }else if(selector==11){
        for(auto& v:pool.bombs)if(v.word(0)==1){add(v,0x18,m.n(-32.));add(v,0x20,m.q(v.quad(0x28)));v.quad(0x28,m.store(m.mul(m.q(v.quad(0x28)),m.n(.6))));if(crt_sse2::real(v.quad(0x18))<-256.)v.word(0,0);}
        if(timer>=20)for(auto& v:pool.bombs)if(v.word(0)==0){v.word(0,1);v.quad(0x10,options_detail::stored_bits(player.quad(0x90)));v.quad(0x18,options_detail::stored_bits(player.quad(0x98)));v.quad(0x20,crt_sse2::bits(.6));v.quad(0x28,crt_sse2::bits(.4));return;}
    }else if(selector==12){
        b.word(4,floor_to_int32(m.store(m.div(m.add(m.q(player.quad(0x90)),m.n(32.)),m.n(64.)))));
        b.word(8,floor_to_int32(m.store(m.div(m.q(player.quad(0x98)),m.n(64.)))));
    }else if(selector==20){
        for(auto& v:pool.bombs)if(v.word(0)==1){add(v,0x10,m.q(v.quad(0x28)));add(v,0x18,m.q(v.quad(0x30)));if(crt_sse2::real(v.quad(0x10))<-32.)v.word(0,0);}
        if(timer>=40)for(auto& v:pool.bombs)if(v.word(0)==0){
            v.word(0,1);v.quad(0x10,crt_sse2::bits(416.));
            const auto offset=m.store(m.sub(random.real_number(crt_sse2::bits(128.),static_cast<RandomPrecision>(precision)),m.n(64.)));
            v.quad(0x18,m.store(m.add(m.q(player.quad(0x98)),m.q(offset))));
            const auto angle=m.store(m.mul(m.i(player.word(0x2c)),m.n(.1)));
            v.quad(0x20,m.store(m.add(m.mul(math.sine(angle),m.n(.2)),m.q(0x400921fb54442d28ull))));
            v.quad(0x28,m.store(m.mul(math.cosine(v.quad(0x20)),m.n(16.))));v.quad(0x30,m.store(m.mul(math.sine(v.quad(0x20)),m.n(16.))));
            v.quad(0x38,m.store(m.sub(m.n(255.),m.mul(m.q(offset&0x7fffffffffffffffull),m.n(3.)))));return;
        }
    }else if(selector==21){
        for(auto& v:pool.bombs)if(v.word(0)==1){add(v,0x18,m.q(v.quad(0x20)));if(crt_sse2::real(v.quad(0x18))<-90.)v.word(0,0);}
        if(timer>=20&&player.word(0x2c)%4u!=0)for(auto& v:pool.bombs)if(v.word(0)==0){
            v.word(0,1);v.quad(0x10,m.store(m.add(random.real_number(crt_sse2::bits(400.),static_cast<RandomPrecision>(precision)),m.n(-8.))));v.quad(0x18,crt_sse2::bits(538.));
            v.quad(0x20,m.store(m.sub(m.mul(m.i(player.word(0x2c)-player_bomb_durations[7]),m.n(.07)),m.n(17.))));v.quad(0x28,crt_sse2::bits(90.));return;
        }
    }else if(selector==22){
        if(timer>=signed_word(player_bomb_durations[8]-99u)){add(b,0x20,m.q(b.quad(0x28)));addc(0x28,.00015);}
        else if(timer>=signed_word(player_bomb_durations[8]-119u)){mulc(0x20,1.147);if(player.word(0x2c)==player_bomb_durations[8]-100u)set_effect_screen_shake(shake,4,player.word(0x2c));}
        else if(timer<21){mulc(0x20,.85);if(player.word(0x2c)==20)b.word(4,16);}
        else if(player.word(0x2c)==player_bomb_durations[8]-120u)b.word(4,20);
    }
}
}
