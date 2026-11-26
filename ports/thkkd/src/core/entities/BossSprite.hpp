// 恢复0x619b80的Boss分支（kind10..99）；保留绘制时写入与眨眼随机调用。
#pragma once
#include "core/entities/BossSpriteServices.hpp"
#include "core/entities/BossAllocationContext.hpp"
#include "core/entities/EnemyDeathContext.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/player/PlayerOptions.hpp"
namespace kaikeidou {
namespace boss_sprite_detail {
inline bool less(binary_float::Number a,binary_float::Number b){
    if(!a.mantissa)a.negative=false;
    if(!b.mantissa)b.negative=false;
    if(a.negative!=b.negative)return a.negative;
    const int cmp=!a.mantissa?(!b.mantissa?0:-1):!b.mantissa?1:
        a.exponent!=b.exponent?(a.exponent<b.exponent?-1:1):a.mantissa==b.mantissa?0:(a.mantissa<b.mantissa?-1:1);
    return a.negative?cmp>0:cmp<0;
}
}
inline void draw_boss_sprites(EnemyPool& pool,const BossAllocationContext& allocation,const EnemyDeathContext& death,
    const PatternParameters& parameters,Random& random,BossSpriteServices& services,unsigned precision){
    using namespace binary_float;
    for(auto& e:pool.enemies){
        if(e.word(0)!=1||options_detail::signed_word(e.word(4))<10||options_detail::signed_word(e.word(4))>=100)continue;
        const auto kind=e.word(4)-10u;
        services.blend(1,allocation.field_a7ea5c);
        if(kind<8){
            const auto scale=kind==4&&death.stage==3&&death.part==3&&death.subphase==6?options_detail::stored_bits(parameters.reals[0]):0x3ff0000000000000ull;
            const auto draw=[&](std::uint32_t bank,std::uint32_t flip=0,bool blink=false){
                const auto address=0xa7e800u+kind*0x40u+bank+(blink?4u:e.word(0x34)*4u);
                const auto handle=services.sprite_handle(address);
                services.sprite(options_detail::stored_bits(e.quad(0x38)),options_detail::stored_bits(e.quad(0x40)),scale,0,handle,1,flip);
            };
            const auto tick=[&](std::uint32_t limit){e.word(0x30,e.word(0x30)+1u);return e.word(0x30)==limit;};
            const auto advance=[&](){e.word(0x34,e.word(0x34)+1u);};
            const auto saturating=[&](){if(tick(5)){if(options_detail::signed_word(e.word(0x34))<3)advance();e.word(0x30,0);}};
            const auto idle=[&](std::uint32_t period,bool blink_enabled){
                if(blink_enabled&&e.word(0x34)==0xffffffffu){draw(0,0,true);if(tick(period)){e.word(0x34,0);e.word(0x30,0);}}
                else{draw(0);if(tick(period)){advance();if(e.word(0x34)==4)e.word(0x34,0);
                    else if(blink_enabled&&e.word(0x34)==3&&random.integer_bits(5,static_cast<RandomPrecision>(precision),true)!=0)e.word(0x34,0xffffffffu);
                    e.word(0x30,0);}}
            };
            if(kind==2)idle(10,true);
            else if(kind==5&&(e.word(0x2c)==3||e.word(0x2c)==4||e.word(0x2c)==5)){
                const auto pose=e.word(0x2c);draw(0x30,pose==5?1u:0u);
                if(options_detail::signed_word(e.word(0x34))<3){if(tick(pose==3?8u:10u)){advance();e.word(0x30,0);}}
                else if(tick(pose==3?16u:40u)){e.word(0x2c,pose==3?0u:pose==4?5u:4u);e.word(0x34,0);e.word(0x30,0);}
            }else if(e.word(0x2c)==3){
                draw(0x30);
                if(kind==0)saturating();
                else if(kind==6){
                    if(options_detail::signed_word(e.word(0x34))<2){if(tick(10)){advance();e.word(0x30,0);}}
                    else if(e.word(0x34)==2){if(tick(3)){e.word(0x34,3);e.word(0x30,0);}}
                    else if(tick(3)){e.word(0x34,2);e.word(0x30,0);}
                }else if(tick(kind==1?8u:10u)){advance();if(e.word(0x34)==4){if(kind!=1)e.word(0x2c,0);e.word(0x34,0);}e.word(0x30,0);}
            }else{
                const auto old=binary64(e.quad(0x70)),now=binary64(e.quad(0x38));
                const auto right=boss_sprite_detail::less(add(old,binary64(0x3fb999999999999aull),precision),now);
                const auto left=!right&&boss_sprite_detail::less(now,add(old,binary64(0xbfb999999999999aull),precision));
                if(right||left){if(e.word(0x2c)==0){e.word(0x30,0);e.word(0x34,0);}e.word(0x2c,right?1u:2u);draw(right?0x10u:0x20u);saturating();}
                else{if(e.word(0x2c)==1||e.word(0x2c)==2){e.word(0x30,0);e.word(0x34,0);}e.word(0x2c,0);idle(kind==1||kind==5?8u:10u,kind!=5);}
            }
        }
        e.quad(0x70,options_detail::stored_bits(e.quad(0x38)));
        services.blend(0,255);
    }
}
}
