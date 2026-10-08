#pragma once
#include "GameTypes.hpp"

namespace th12 {
extern bool playerFocused; // Native Player+c598, distinct from the raw held key.
bool movementFocus(int held, int totalAge, bool liveEnemies);
void explodeReimuBOrb(pw::ReimuBOrb& orb);
void awardPlayerGraze(float x, float y, float z = 0);
bool startPlayerBomb();
void tickPlayerBomb();
int playerBombDamage(const Enemy& e);
bool tickPlayerBombEffect(int profile, u32 id);
void buildPlayerBombTrails();
void tickPlayerBombEffects();
int friendlyImpactDuration(int script);
void initializeFriendlyProjectile(Projectile& b, const ShotSpec& s);
void tickPlayerDamageSources();
int playerSourceDamage(const Enemy& e);
float playerAngle(float a);
Enemy* playerTarget(float x, float y, float radius, u32 existing = 0);
void updateFriendlyProjectile(Projectile& b);
bool playerProjectileHits(const Projectile& b, const Enemy& e);
void sanaeBExplosion(const Projectile& parent);
int friendlyProjectileDamage(Projectile& b, const Enemy& e);
} // namespace th12
