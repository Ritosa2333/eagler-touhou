#pragma once
#include <algorithm>
namespace th19 {
// 0x548c60 / 0x54572a: pure-spirit credit is independent of bullet credit.
struct SpiritMeter {
    int credit=0,bullet_credit=0,cooldown=0;
    float x=0,y=0;
    static float multiplier(int difficulty){return difficulty<=0?.5f:difficulty==1?1.f:difficulty==2?1.2f:1.5f;}
    void add(int amount,int difficulty,float px,float py){
        credit=std::min(700,credit+int(float(amount)*multiplier(difficulty)));x=px;y=py;
    }
    // 0x548bc0: both credit sources overwrite the same last-event position.
    void add_bullets(int amount,int difficulty,float px,float py){
        bullet_credit=std::min(1400,bullet_credit+int(float(amount)*multiplier(difficulty)));x=px;y=py;
    }
    // Shared rank/rank cap must come from the battle manager, not C3/C4 levels.
    int tick(int rank,int cap,int registered,int* bullets=nullptr){
        if(bullets)*bullets=0;
        if(cooldown>0){--cooldown;return 0;}
        const float level=float(std::clamp(rank,0,std::max(0,cap)))/200.f;
        // 0x5450e0 processes bullets first, then spirits in the same frame.
        // Both use the SAME cooldown, overwritten to 10 when spirits are sent.
        const int bulletThreshold=150-int(level*100.f),bulletCost=150+int(level*-100.f);
        if(bullets&&bulletThreshold>0&&bulletCost>0)while(bullet_credit>=bulletThreshold){
            bullet_credit-=bulletCost;++*bullets;cooldown=4;
        }
        const int threshold=120-int(level*40.f),cost=120+int(level*-40.f);
        if(threshold<=0||cost<=0)return 0; // malformed/unverified configuration
        int count=0;
        while(credit>=threshold){credit-=cost;cooldown=10;
            if(float(registered)<5.f+7.f*level)++count;
        }
        return count;
    }
};
}
