// 原61D450调度及三状态绘制函数的持久状态切片；绘图边界按无状态处理。
#pragma once
#include "core/entities/ProjectilePool.hpp"
#include "core/math/Random.hpp"
namespace kaikeidou {
namespace projectile_presentation_detail {
inline void present(Projectile& bullet,Random& random,RandomPrecision precision,bool sse_conversion){
    if(bullet.word(4)==22){
        bullet.word(0x98,bullet.word(0x98)+1u);
        if(bullet.word(0x98)==10){
            bullet.word(0x9c,bullet.word(0x9c)+1u);
            if(bullet.word(0x9c)==4)bullet.word(0x9c,0);
            const auto next=random.integer_bits(5,precision,sse_conversion);
            bullet.word(0x98,next);
        }
    }else if(bullet.word(0)==1&&bullet.word(4)==37)bullet.word(0x30,255);
}
}
inline void update_projectile_presentation(ProjectilePools& pools,Random& random,RandomPrecision precision,bool sse_conversion=true){
    for(std::size_t group=0;group<3;++group){
        const auto begin=group*1000,end=begin+1000;
        for(auto i=begin;i<end;++i){auto& b=pools.bullets[i];if(b.word(0)==1&&b.word(0x2c)==0)projectile_presentation_detail::present(b,random,precision,sse_conversion);}
        for(auto i=begin;i<end;++i){auto& b=pools.bullets[i];if(b.word(0)==1&&b.word(0x2c)==2&&b.word(0x30)==255)projectile_presentation_detail::present(b,random,precision,sse_conversion);}
        for(auto i=begin;i<end;++i){auto& b=pools.bullets[i];const auto state=b.word(0);
            if(state==2||state==3||(state==1&&b.word(0x2c)!=0&&(b.word(0x2c)!=2||b.word(0x30)!=255)))
                projectile_presentation_detail::present(b,random,precision,sse_conversion);
        }
    }
}
}
