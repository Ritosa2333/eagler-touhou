#include "CollisionPipeline.hpp"
namespace th075 {
namespace {
i32 signed_bits(u32 v){i32 value;std::memcpy(&value,&v,4);return value;}
i16 decrement(i16 value){const u16 bits=u16(value)-u16(1);i16 result;std::memcpy(&result,&bits,2);return result;}
bool find_contacts(CollisionBounds& bounds,const std::vector<Rectangle>& a,const FighterMotion& am,const std::vector<Rectangle>& b,const FighterMotion& bm){
    bool found=false;for(const auto& first:a){const auto aw=transform_bounds(frame_bounds(first),am);for(const auto& second:b){const auto bw=transform_bounds(frame_bounds(second),bm);if(overlaps(aw,bw)){found=true;bounds.include_pair(aw,bw);}}}return found;
}
}
CollisionRectangle frame_bounds(const Rectangle& value){CollisionRectangle result;std::memcpy(&result,value.bits.data(),16);return result;}
bool collision_recovery_window(const FighterImpact& target,u32 flags){return target.motion.y>0&&target.movement.recoveryDelay==0&&target.combat.action>=50&&target.combat.action<=149&&!(flags&0x100000);}
bool resolve_fighter_contact(CollisionBounds& bounds,AttackImpact& attack,const AttackCollisionView& source,FighterImpact& owner,FighterImpact& target,const TargetCollisionView& targetFrame,DefenseView& defense,ReplayStreams& replay,bool story,bool activeRound,CollisionSignals& signals){
    const u32 flags=attack.frame.flags[1],body=targetFrame.snapshotFrame.flags[0];
    if(source.snapshotResponse!=0||attack.remainingHits<=0||collision_recovery_window(target,targetFrame.liveFlags))return false;
    if(!(flags&0x400000)&&(target.combat.recoveryTimers[0]!=0||(body&0x2000)))return false;
    if((flags&0x80)&&(target.combat.recoveryTimers[1]>0||(body&0x1800)))return false;
    bounds.intersection={-10000,-10000,10000,10000};
    if(!find_contacts(bounds,attack.frame.attackBounds,source.motion,targetFrame.snapshotFrame.bodyBounds,target.motion))return false;
    target.allowPush=!(flags&0x400000);target.collisionFlags=body;defense.collisionFlags=body;
    if(cancel_special_contact(attack,target)||apply_armor_hit(attack,owner,target,story,activeRound,signals)||reflect_contact(attack,owner,target,signals))return true;
    if(!target.allowPush&&graze_contact(attack,target,signals))return true;
    if(apply_guard_response(select_guard_response(defense,flags,replay),attack,owner,target,story,signals)||apply_ground_hit(attack,owner,target,story,activeRound,signals))return true;
    apply_normal_hit(attack,owner,target,story,activeRound,signals);return true;
}
bool resolve_projectile_contact(CollisionBounds& bounds,ProjectileCollisionView& a,ProjectileCollisionView& b){
    if(a.snapshotResponse!=0||b.snapshotResponse!=0||a.hits<=0||b.hits<=0||i16(a.snapshotSequence.mode)==0||i16(b.snapshotSequence.mode)==0)return false;
    bounds.intersection={-10000,-10000,10000,10000};
    // Include the base box as well as every attack box, including zero-box frames.
    bool found=false;const auto check=[&](const Rectangle& ar,const Rectangle& br){const auto aw=transform_bounds(frame_bounds(ar),a.motion),bw=transform_bounds(frame_bounds(br),b.motion);if(overlaps(aw,bw)){found=true;bounds.include_pair(aw,bw);}};
    for(const auto& ar:a.frame.attackBounds){for(const auto& br:b.frame.attackBounds)check(ar,br);check(ar,b.frame.baseBounds);}
    for(const auto& br:b.frame.attackBounds)check(a.frame.baseBounds,br);check(a.frame.baseBounds,b.frame.baseBounds);
    if(!found)return false;
    a.contactX=b.contactX=float(double(signed_bits(u32(bounds.intersection.left)+u32(bounds.intersection.right)))/2.0);
    a.contactY=b.contactY=float(double(signed_bits(u32(bounds.intersection.top)+u32(bounds.intersection.bottom)))/2.0);
    if(i16(a.sequence.next)<i16(b.sequence.mode)&&i16(b.sequence.next)<i16(a.sequence.mode)){
        if(i16(a.sequence.mode)<=i16(b.sequence.mode)){a.response=4;a.hits=decrement(a.hits);}
        if(i16(b.sequence.mode)<=i16(a.sequence.mode)){b.response=4;b.hits=decrement(b.hits);}
    }else a.response=b.response=5;
    return true;
}
}
