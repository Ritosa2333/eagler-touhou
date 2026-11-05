#include "CpuPlayer.hpp"
#include "HazardTrace.hpp"
#include <algorithm>
namespace th09 {
namespace {
// Original 1.50a difficulty policy: full-health, injured, critical-health grace
// periods, followed by the corresponding error periods and damage modifier.
struct Policy {i32 grace[3],error[3],damage;};
constexpr Policy policies[]={
{{0,0,0},{0,5,10},3},{{0,0,0},{0,6,10},3},{{0,0,0},{0,7,15},3},{{0,0,0},{0,8,15},3},{{0,0,0},{0,9,15},3},
{{0,0,10},{0,10,10},3},{{0,0,10},{0,10,10},3},{{0,0,10},{0,10,15},3},{{0,0,10},{0,15,15},3},{{0,0,10},{0,15,20},3},
{{10,5,10},{5,10,10},2},{{10,5,10},{5,10,10},2},{{10,5,10},{5,10,15},2},{{10,5,10},{5,15,15},2},{{10,5,10},{5,15,20},2},
{{10,5,15},{5,10,10},2},{{10,5,15},{5,10,10},2},{{10,5,15},{5,10,15},2},{{10,5,15},{5,15,15},2},{{10,5,15},{5,15,20},2},
{{15,10,20},{25,10,20},1},{{15,10,20},{30,10,20},1},{{15,10,20},{30,10,25},1},{{15,10,20},{30,15,25},1},{{15,10,20},{30,15,30},1},
{{20,10,20},{25,10,20},1},{{20,10,20},{30,10,20},1},{{20,10,20},{30,10,25},1},{{20,10,20},{30,15,25},1},{{20,10,20},{30,15,30},1},
{{20,15,30},{25,10,20},0},{{20,15,30},{30,10,20},0},{{20,15,30},{30,10,25},0},{{20,15,30},{30,15,25},0},{{20,15,30},{30,15,30},0},
{{20,15,30},{25,20,20},0},{{20,15,30},{30,20,20},0},{{20,15,30},{30,20,25},0},{{20,15,30},{30,25,25},0},{{20,15,30},{30,25,30},0},
{{30,20,50},{25,10,20},0},{{30,20,50},{30,10,20},0},{{30,20,50},{30,10,25},0},{{30,20,50},{30,10,25},0},{{30,20,50},{30,10,30},0},
{{30,20,60},{25,10,20},0},{{30,20,60},{30,10,20},0},{{30,20,60},{30,10,25},0},{{30,20,60},{30,10,25},0},{{30,20,60},{30,10,30},0},
{{40,20,60},{25,10,20},0},{{40,20,60},{30,10,20},0},{{40,20,70},{30,10,20},0},{{40,20,70},{30,10,20},0},{{40,20,70},{30,10,20},0},
{{50,20,80},{25,10,20},0},{{50,20,90},{30,10,20},0},{{50,20,100},{30,10,20},0},{{50,20,110},{30,10,20},0},{{50,20,120},{30,10,20},0},
{{9999999,9999999,9999999},{9999999,9999999,9999999},0},
{{20,30,20},{20,20,0},0},
{{20,30,22},{20,20,0},0},
{{20,30,24},{23,20,0},0},
{{25,30,26},{23,25,0},0},
{{40,30,28},{25,25,0},0},
{{45,30,40},{25,10,0},0},
{{50,30,45},{10,10,0},0},
{{60,30,50},{10,10,0},0},
{{70,30,55},{10,15,0},0},
{{70,30,120},{10,15,0},0}};
constexpr i32 preferences[12][10]={
{0,0,4,6,8,1,5,3,7,2},{0,0,1,6,5,4,3,8,7,2},{0,0,1,5,6,3,4,7,8,2},{0,0,3,5,7,1,6,4,8,2},
{0,0,4,8,6,2,7,3,5,1},{0,0,4,3,2,8,7,6,5,1},{0,0,3,4,2,7,8,5,6,1},{0,0,3,7,5,2,8,4,6,1},
{0,0,4,8,8,2,7,3,7,2},{0,0,4,3,2,8,7,8,7,2},{0,0,3,4,2,7,8,7,8,2},{0,0,3,7,7,2,8,4,8,2}};
constexpr u16 keys[]={0,16,32,64,128,80,144,96,160};
constexpr i32 alternatives[2][9]={{0,3,4,2,2,6,5,8,7},{0,4,3,1,1,7,8,5,6}};
bool boss_target(u32 flags){return (flags&0xc00)==0xc00||(flags&0x2000);}
}
i32 CpuPlayer::survival_seconds(i32 level){return policies[std::clamp(level,0,70)].grace[2];}
Vec2 CpuPlayer::velocity(i32 direction,bool focus,const CpuContext& c){
    const float speed=focus?c.speeds.focused:c.speeds.normal,diagonal=focus?c.speeds.focused_diagonal:c.speeds.diagonal;Vec2 v;
    switch(direction){case 1:v.y=-speed;break;case 2:v.y=speed;break;case 3:v.x=-speed;break;case 4:v.x=speed;break;case 5:v={-diagonal,-diagonal};break;case 6:v={diagonal,-diagonal};break;case 7:v={-diagonal,diagonal};break;case 8:v={diagonal,diagonal};break;}
    v.x=(c.effect_scale.x*c.base_scale.x)*v.x;v.y=(c.effect_scale.y*c.base_scale.y)*v.y;return v;
}
void CpuPlayer::update(GameInput& input,CpuContext& c,ShotControlState& shot,const FrameTiming& timing){
    input={};if(c.scene_locked||c.dialogue)return;
    auto& s=state;const auto& policy=policies[std::clamp(c.level,0,70)];const i32 health_band=c.health>=10?0:c.health>=2?1:2;
    if(c.extra_mode)actions.opponent_survival_display(wrapping_sub(policy.grace[2]*60,s.survival.current));
    if(!(s.flags&1)){
        if(!(c.field_flags&1)&&c.opponent_pending<361)s.survival.tick(timing);
        if(c.extra_mode&&shot.player_state!=1){shot.player_state=3;shot.protection.reset(2);c.player.state=3;}
        if(s.survival.current/60>=policy.grace[health_band]){s.flags|=1;if(c.extra_mode){actions.opponent_survival_expired();actions.opponent_survival_display(-1);}}
    }else if(!(s.flags&2)){s.mistakes.tick(timing);if(s.mistakes.current/60>=policy.error[health_band])s.flags|=2;}
    c.extra_damage=policy.damage;
    if(shot.available<s.charge_goal){if(s.charging){input.released|=1;s.charging=0;}}else s.charging=1;
    const auto p=c.player.position;
    const i32 region=(p.x<-96?0:p.x<0?1:p.x<96?2:3)+(p.y<288?4:0)+(p.y<192?4:0);
#if TH09_REPLAY_DIAGNOSTICS
    CpuDecision* trace_out=&cpu_decisions[u32(std::clamp(c.side,0,2))];*trace_out=CpuDecision{};
#else
    // Unobserved local stores are eliminated in the optimized game build.
    CpuDecision unused_trace;CpuDecision* trace_out=&unused_trace;
#endif
    trace_out->side=c.side;trace_out->frame=u32(s.frame.current);trace_out->flags=i32(s.flags);trace_out->hold=s.hold_direction;trace_out->hazard_count=hazards.count;trace_out->px=p.x;trace_out->py=p.y;trace_out->region=region;
    const Vec3 extents[3]={{28,28,0},{10,10,0},c.player.half_extent};const float radii[3]={48,16,0};
    bool focus=c.spirit_count>3;if(focus)input.held|=4;
    trace_out->focus=focus?1:0;
    auto predict=[&](i32 direction){auto v=velocity(direction,focus,c);return Vec3{std::clamp(p.x+v.x,c.limits.origin.x,c.limits.origin.x+c.limits.extent.x),std::clamp(p.y+v.y,c.limits.origin.y,c.limits.origin.y+c.limits.extent.y),p.z};};
    auto safe=[&](i32 direction,i32 tier){return hazards.sample(c.player,predict(direction),extents[tier],radii[tier])==nullptr;};
    i32 chosen=0,tier=0;bool found=(s.flags&2)&&c.opponent_pending<=300;
    if(!found){
        if(s.hold_direction){--s.hold_direction;chosen=s.direction;for(tier=0;tier<2;++tier)if(safe(chosen,tier)){found=true;break;}}
        while(!found){
            for(tier=0;tier<3&&!found;++tier){for(i32 candidate=tier;candidate<10;++candidate){chosen=candidate==1?s.direction:preferences[region][candidate];if(safe(chosen,tier)){found=true;break;}}if(found)break;}
            if(found)break;
            if(focus){input.held&=~4;focus=false;continue;}
            if(!(s.flags&1)&&shot.player_state==0&&shot.shock.current==0){if(s.charging&&shot.charge>=100){trace_out->path=3;trace_out->chosen=chosen;trace_out->keys=keys[chosen];input.released|=1;s.charging=0;return;}input.pressed|=2;input.held|=2;}
            trace_out->path=3;trace_out->chosen=chosen;trace_out->keys=keys[chosen];s.direction=chosen;return;
        }
    }
    if(shot.charge<s.charge_goal){if(s.charging)input.held|=1;}
    else {input.released|=1;s.charging=0;s.charge_goal=std::min((float(random.bounded32(4))+1.f)*100.f,400.f);}
    if(keys[chosen]&&!(s.flags&2)){trace_out->path=1;trace_out->chosen=chosen;trace_out->keys=keys[chosen];trace_out->tier=tier;
        if(s.hold_direction==0&&s.evasive_directions[0]==s.evasive_directions[1]&&s.evasive_directions[1]==s.evasive_directions[2]&&s.evasive_directions[2]==s.evasive_directions[3]&&s.evasive_directions[3]==s.evasive_directions[4]&&s.evasive_directions[4]==s.evasive_directions[5]){
            const i32 alternate=alternatives[random.below(2)][chosen];if(safe(alternate,tier<2?tier+1:tier)){s.hold_direction=8;chosen=alternate;}
        }
        input.held|=keys[chosen];if(s.hold_direction==0)for(i32 i=7;i>0;--i)s.evasive_directions[i]=s.evasive_directions[i-1];s.evasive_directions[0]=keys[chosen];
    }else{
        trace_out->path=2;trace_out->chosen=chosen;trace_out->keys=keys[chosen];trace_out->tier=tier;
        Vec3 target{-1000,16,0};
        if(c.item_target.x>-999)target=c.item_target;
        else if(c.has_priority){target=c.priority_target;target.y+=128;if(boss_target(c.target_flags))target.y=400;}
        else if(c.has_first){target=c.first_target;target.y+=128;}
        if(s.directions[0]==s.directions[2]&&s.directions[1]==s.directions[3]&&s.directions[0]!=s.directions[1]){target.x=-target.x;target.y=float(random.range(448));trace_out->mirror=1;}
        trace_out->tx=target.x;trace_out->ty=target.y;
        i32 move=chosen;
        if(target.x>-999&&target.y<448){
            i32 desired=-1,probe=chosen;
            if(p.x>target.x+6){desired=(p.y>target.y+6&&p.y>48)?5:7;probe=desired;trace_out->branch=4;}
            else if(p.x<target.x-6){desired=(p.y>target.y+6&&p.y>48)?6:8;trace_out->branch=5;/* Original probes the retained direction on this branch. */}
            else trace_out->branch=3;
            trace_out->probe=probe;trace_out->desired=desired;
            if(desired>=0){auto* hit=hazards.sample(c.player,predict(probe),extents[1],0);trace_out->sampled=hit?1:0;if(hit==nullptr)move=desired;}
        }else trace_out->branch=1;
        trace_out->move=move;
        input.held|=keys[move];
    }
    for(i32 i=7;i>0;--i)s.directions[i]=s.directions[i-1];s.directions[0]=keys[chosen];
    if((c.has_priority&&boss_target(c.target_flags))||c.attack_areas==0){const i32 interval=c.normal_enemies?10:30;if(s.frame.current!=s.frame.previous&&s.frame.current%interval==0)input.pressed|=1;}
    if(c.extra_mode&&(s.flags&2)&&c.opponent_pending<301)input.pressed&=~1;
    s.direction=chosen;s.frame.tick(timing);
}
}
