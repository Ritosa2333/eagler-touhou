// 完整610020背景更新、65F590视口偏移与42F870矩阵构造。
#pragma once
#include "core/shared/BackgroundViewport.hpp"
#include "core/entities/EnemyDeathContext.hpp"
#include "core/math/Random.hpp"
#include "core/math/BinaryFloat32.hpp"
#include "core/math/FiniteSqrt.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/player/PlayerOptions.hpp"
namespace kaikeidou {
inline void update_stage_background(StageResourceContext& c,const EnemyDeathContext& life,
    const std::uint32_t& frame,const ObjectRecord<240>& player,Random& random,RandomPrecision precision,
    GameMath& math,StageBackgroundServices& services){
    using namespace binary_float;const auto bits=static_cast<unsigned>(precision);const options_motion_detail::Arithmetic m{bits};
    const auto signed_word=[](std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:std::int64_t(v);};
    const auto q=[&](unsigned i){return m.q(c.reals[i]);};
    const auto sum=[&](unsigned i,double amount){c.reals[i]=m.store(m.add(q(i),m.n(amount)));};
    const auto sub=[&](unsigned i,double amount){c.reals[i]=m.store(m.sub(q(i),m.n(amount)));};
    const auto scroll=[&](){sum(0,2.5);sum(1,2.5);
        if(crt_sse2::real(c.reals[0])>=648.)sub(0,848.);
        if(crt_sse2::real(c.reals[1])>=648.)sub(1,848.);
    };
    const auto sine=[&](unsigned angle,unsigned target){sum(angle,.0065);
        const auto value=math.sine(options_detail::stored_bits(c.reals[angle]));c.reals[target]=m.store(m.mul(value,m.n(192.)));
    };
    const auto orbit=[&](){sub(4,.002);
        const auto cosine=math.cosine(options_detail::stored_bits(c.reals[4]));c.reals[2]=m.store(m.sub(m.add(m.mul(cosine,m.n(500.)),m.n(192.)),m.n(1075.)));
        const auto sine_value=math.sine(options_detail::stored_bits(c.reals[4]));c.reals[3]=m.store(m.sub(m.add(m.mul(sine_value,m.n(500.)),m.n(224.)),m.n(1075.)));
    };
    const auto viewport=[&](unsigned x,unsigned y){const auto vy=options_detail::stored_bits(c.reals[y]),vx=options_detail::stored_bits(c.reals[x]);set_background_viewport(vx,vy,bits,services);};
    switch(c.stage){
    case 0:scroll();break;
    case 1:{sum(0,.02);const auto phase=c.integers[0];if(phase<1||phase>5)break;
        c.integers[1]-=1;if(c.integers[1]!=0)break;
        if(phase==1){c.integers[0]=2;c.integers[1]=1920;services.delete_graph(c.handles[2],0);}
        else if(phase==2){c.integers[0]=3;c.integers[1]=255;services.movie_seek(c.handles[4],0);services.movie_play(c.handles[4],1,0);}
        else if(phase==3){c.integers[0]=4;c.integers[1]=1920;services.movie_pause(c.handles[3],0);}
        else if(phase==4){c.integers[0]=5;c.integers[1]=255;services.movie_seek(c.handles[3],0);services.movie_play(c.handles[3],1,0);}
        else{c.integers[0]=2;c.integers[1]=1920;services.movie_pause(c.handles[4],0);}break;}
    case 3:
        if(life.boss_state==2)sine(1,2);
        else if(c.part==0&&signed_word(frame)>=450&&signed_word(frame)<=949){
            c.reals[0]=m.store(m.sub(q(0),divide(integer(frame-450u),m.n(2500.),bits)));
        }else if(signed_word(frame)>=950&&crt_sse2::real(c.reals[0])>-1552.)sub(0,.2);
        break;
    case 4:viewport(5,6);break;
    case 5:
        if(life.boss_state==2){if(life.subphase==1)sine(5,6);}
        else{
            const auto tx=m.store(m.sub(m.n(192.),m.mul(m.sub(m.q(player.quad(0x90)),m.n(192.)),m.n(1.2))));
            c.reals[0]=ease_option_value(c.reals[0],tx,crt_sse2::bits(40.),bits);
            c.reals[1]=ease_option_value(c.reals[1],options_detail::stored_bits(c.reals[0]),crt_sse2::bits(40.),bits);
            const auto ty=m.store(m.sub(q(4),m.mul(m.sub(m.q(player.quad(0x98)),m.n(224.)),m.n(.02))));
            c.reals[2]=ease_option_value(c.reals[2],ty,crt_sse2::bits(40.),bits);
            c.reals[3]=ease_option_value(c.reals[3],options_detail::stored_bits(c.reals[2]),crt_sse2::bits(40.),bits);
            viewport(1,3);
        }break;
    case 6:
        if(life.boss_state==2)orbit();
        else if(signed_word(c.part)<=1&&signed_word(life.subphase)<=3){
            for(unsigned i=0;i<2;++i){if(crt_sse2::real(c.reals[i])<768.)sum(i,1.2);
                else{sub(i,1536.);c.integers[i]=0u-random.integer_bits(300,precision,true);}}
            c.integers[2]-=1;
            if(c.integers[2]==0){c.integers[2]=60;std::uint32_t next;
                do{next=random.integer_bits(5,precision,true);}while(next==c.integers[3]);c.integers[3]=next;
            }
        }else{
            sum(5,.0039);sum(6,.0055);
            const auto wave=math.sine(options_detail::stored_bits(c.reals[5]));const auto amplitude=m.store(m.add(m.mul(wave,m.n(40.)),m.n(50.)));
            const auto dx=m.sub(m.q(player.quad(0x90)),m.n(192.)),dy=m.sub(m.q(player.quad(0x98)),m.n(224.));
            const auto squared=m.store(m.add(m.mul(dx,dx),m.mul(dy,dy)));
            const auto radius=m.store(m.mul(finite_square_root(squared,bits),m.n(.2)));
            const auto ax=m.store(m.sub(m.n(192.),m.q(player.quad(0x90)))),ay=m.store(m.sub(m.n(224.),m.q(player.quad(0x98))));
            const auto angle=m.store(math.atan2(ay,ax));
            const auto sa=math.sine(angle);const auto base_y=m.store(m.add(m.mul(sa,m.q(radius)),m.n(224.)));
            const auto sy=math.sine(options_detail::stored_bits(c.reals[6]));const auto y=m.store(m.add(m.mul(sy,m.q(amplitude)),m.q(base_y)));
            const auto ca=math.cosine(angle);const auto base_x=m.store(m.add(m.mul(ca,m.q(radius)),m.n(192.)));
            const auto cx=math.cosine(options_detail::stored_bits(c.reals[6]));const auto x=m.store(m.add(m.mul(cx,m.q(amplitude)),m.q(base_x)));
            set_background_viewport(x,y,bits,services);
        }break;
    case 7:switch(c.part){case 0:scroll();break;case 1:sum(0,.02);break;case 4:sine(1,2);break;case 7:orbit();break;default:break;}break;
    default:break;
    }
}
}
