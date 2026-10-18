#include "Combo.hpp"
namespace th09 {
namespace {i32 multiply(i32 a,i32 b){return signed_bits(u32(a)*u32(b));}}
TransferParameters* Combo::transfer(const Vec3& p,bool spirit){
    auto& rng=world.random;Vec3 screen=geometry.to_screen(p);
    screen.x=float(rng.signed_unit())*32.f+screen.x;screen.y=float(rng.signed_unit())*32.f+screen.y;
    const float x=float(rng.signed_unit())*(opposing_width*.5f-8.f),y=float(rng.range(128));
    return actions.create_transfer(side+(spirit?3:1),screen,{x,y,0});
}
bool Combo::add(ComboState& s,const Vec3& position,i32 normal,i32 spirit,i32 character,i32 amount){
    if(actions.rewards_blocked())return true;const i32 previous=s.score,rank=world.rank;
    s.pending=0;s.hits=wrapping_add(s.hits,1);if(s.hits>s.best_hits)s.best_hits=s.hits;
    s.score=wrapping_add(wrapping_add(s.score,multiply(rank,amount)/10),amount);
    if(s.score%10)s.score=wrapping_add(wrapping_sub(s.score,s.score%10),10);
    if(s.score>999990){s.score=999990;actions.add_score(5000);}
    if(s.score>=500000&&previous<500000){actions.attack(1,1);if(!actions.opponent_has_boss())actions.attack(2,2);}
    else if(s.score>=300000&&previous<300000){if(!actions.opponent_has_boss())actions.attack(2,2);}
    else if(s.score>=100000&&previous<100000)actions.attack(2,2);
    const i32 interval=s.hits<10?2:s.hits<30?5:10;
    if(s.hits%interval==0)actions.score_popup(position,s.score>=999990?-1:s.score,s.score>=300000?0xffffff00u:s.score>=100000?0xffffffc0u:0xffffffffu);
    if(normal){
        if(world.difficulty==2)normal=multiply(normal,5)/4;else if(world.difficulty==3)normal=multiply(normal,4)/3;
        s.spirit_attack=wrapping_add(s.spirit_attack,spirit);s.normal_attack=wrapping_add(s.normal_attack,normal);s.character_attack=wrapping_add(s.character_attack,character);
        actions.character_meter(s,position);
        const i32 spirit_threshold=wrapping_sub(60,multiply(rank,3)/2),normal_threshold=multiply(wrapping_sub(30,rank),4);
        // These thresholds are positive throughout the original rank range.
        // Reject corrupt world state rather than entering a non-terminating loop.
        if(rank>=30)return false;
        while(s.spirit_attack>=spirit_threshold){
            if(actions.opposing_spirits()<25){
                auto t=transfer(position,true);if(!t)return false;t->source_kind=0;t->target_kind=1;
                const float random=float(world.random.range(.5f));t->speed=float(rank)*.0016666667f+random*.016666668f;t->source_side=i16(side);
            }
            s.spirit_attack=wrapping_add(s.spirit_attack,wrapping_sub(multiply(rank,2),60));s.normal_attack=wrapping_sub(s.normal_attack,10);if(s.normal_attack<0)s.normal_attack=0;
        }
        while(s.normal_attack>=normal_threshold){
            auto t=transfer(position,false);if(!t)return false;t->source_kind=0;t->target_kind=4;
            switch(world.difficulty){
            case 0:t->speed=float(rank)*.04f+.9f;break;
            case 1:t->speed=float(rank)*.05f+1.1f;break;
            case 2:t->speed=float(rank)*.08f+1.3f;break;
            case 3:t->speed=float(rank)*.11f+1.3f;break;
            case 4:t->speed=float(rank)*.04f+1.6f;break;
            default:break;
            }
            t->source_side=i16(side);s.normal_attack=wrapping_add(s.normal_attack,wrapping_sub(multiply(rank,4),120));
        }
    }
    const i32 time=s.chain_time.current;
    if(time<60)s.chain_time.advance(float(time<30?30:time<50?5:2),timing.rate,timing.force_step?32:0);
    if(s.display_score<=s.score){s.display_score=s.score;s.display_time.reset(wrapping_add(s.chain_time.current,45));}
    return true;
}
}
