// 二面part0完整67C8F0..67D0D1，保留失败分配后的参数推进和字段写入次序。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/math/GameMath.hpp"
#include "core/stages/Stage2FirstArithmetic.hpp"
namespace kaikeidou {
inline void stage2_first_segment(StageTimeline& timeline,EnemyPool& pool,PatternParameters& parameters,
    StageResourceContext& resources,StageServices& services,GameMath& math,unsigned precision){
    using namespace binary_float;using stage2_first_detail::constant;
    const auto c=[](std::uint32_t a){return constant(a);};
    const auto n=[&](std::uint32_t a){return binary64(c(a));};
    const auto add=[&](Number a,Number b){return binary_float::add(a,b,precision);};
    const auto sub=[&](Number a,Number b){b.negative=!b.negative;return add(a,b);};
    const auto mul=[&](Number a,Number b){return multiply(a,b,precision);};
    const auto spawn=[&]()->Enemy*{const auto slot=allocate_enemy(pool);return slot==-1?nullptr:&pool.enemies[static_cast<std::size_t>(slot)];};
    const auto initialize=[](Enemy& e,std::uint32_t kind,std::uint32_t hp,std::uint32_t items,std::uint32_t behavior){
        e.word(4,kind);e.word(0x18,hp);e.word(0x14,hp);e.word(0x1c,items);e.word(8,behavior);
    };
    const auto frame=timeline.frame;
    const auto signed_frame=frame>>31?std::int64_t(frame)-0x100000000ll:std::int64_t(frame);
    if(frame==1){services.music(3,1);services.title();resources.integers[0]=0;parameters.integers[0]=0;}
    else if(signed_frame>=180&&signed_frame<=189){
        if(auto* e=spawn()){
            initialize(*e,(timeline.frame&1u)?0u:2u,40,1,24);
            e->quad(0x38,to_binary64(add(mul(integer(timeline.frame-180u),n(0x6d2778)),n(0x6d2770))));
            e->quad(0x40,c(0x6d22c0));e->quad(0x50,c(0x6d1ad8));e->word(0x80,0);e->word(0x84,parameters.integers[0]);
        }
        ++parameters.integers[0];
    }else if(signed_frame>=320&&signed_frame<=329){
        if(auto* e=spawn()){
            initialize(*e,(timeline.frame&1u)?0u:1u,40,1,24);
            e->quad(0x38,to_binary64(add(mul(integer(329u-timeline.frame),n(0x6d2778)),n(0x6d2770))));
            e->quad(0x40,c(0x6d22c0));e->quad(0x50,c(0x6d1ad8));e->word(0x80,1);e->word(0x84,parameters.integers[0]);
        }
        parameters.reals[0]=0;++parameters.integers[0];
    }else if(signed_frame>=420&&signed_frame<=490){
        if(signed_frame%10==0)if(auto* e=spawn()){
            initialize(*e,0,40,3,25);e->quad(0x38,c(0x6d1a60));e->quad(0x40,c(0x6d19f0));e->quad(0xa8,parameters.reals[0]);
            parameters.reals[0]=to_binary64(sub(binary64(parameters.reals[0]),n(0x6d2768)));
        }
    }else if(frame==620||frame==650||frame==680||frame==710){
        const bool earlier=frame==620||frame==650;
        for(std::uint32_t side=0;side<2;++side)if(auto* e=spawn()){
            initialize(*e,1,40,2,earlier?26u:27u);
            e->quad(0xa8,to_binary64(integer(earlier?120u-side*60u:side*240u-30u)));
            e->quad(0xb0,to_binary64(earlier?add(mul(integer(side),n(0x6d1b18)),n(0x6d2760)):sub(integer(1),add(integer(side),integer(side)))));
            e->quad(0xb8,to_binary64(integer(earlier?side*300u+42u:side*240u+72u)));
            const auto angle=stage2_first_degrees(e->quad(0xa8),precision);
            const auto cosine=math.cosine(angle);
            e->quad(0x38,to_binary64(add(mul(cosine,n(0x6d1d78)),binary64(e->quad(0xb8)))));
            e->quad(0x40,to_binary64(add(mul(math.sine(angle),n(0x6d1d78)),n(0x6d22c0))));
        }
    }else if(frame==780){
        if(auto* e=spawn()){
            initialize(*e,5,150000,1,28);e->quad(0xa8,c(0x6d1ad8));e->word(0x80,0);e->quad(0x38,c(0x6d1a60));e->quad(0x40,c(0x6d2758));
        }
    }else if(frame==1300){
        services.clear(100);++timeline.part;timeline.subphase=0;timeline.frame=0;parameters.game_frame=0;
    }
}
}
