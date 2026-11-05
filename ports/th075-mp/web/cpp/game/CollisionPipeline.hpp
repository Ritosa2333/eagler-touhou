#pragma once
#include "CollisionResponse.hpp"
namespace th075 {
struct AttackCollisionView {const FighterMotion& motion;i32 snapshotResponse=0;};
struct TargetCollisionView {const PatternFrame& snapshotFrame;u32 liveFlags=0;};
// The original tests live recovery and movement against a frozen frame snapshot.
bool collision_recovery_window(const FighterImpact&,u32 liveFlags);
bool resolve_fighter_contact(CollisionBounds&,AttackImpact&,const AttackCollisionView&,FighterImpact& owner,FighterImpact& target,const TargetCollisionView&,DefenseView&,ReplayStreams&,bool story,bool activeRound,CollisionSignals&);
struct ProjectileCollisionView {
    const PatternFrame& frame;const PatternSequence& sequence;const PatternSequence& snapshotSequence;
    const FighterMotion& motion;i32 snapshotResponse=0;i32& response;i16& hits;float& contactX;float& contactY;
};
bool resolve_projectile_contact(CollisionBounds&,ProjectileCollisionView&,ProjectileCollisionView&);
CollisionRectangle frame_bounds(const Rectangle&);
}
