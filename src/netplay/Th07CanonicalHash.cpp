#include "Th07CanonicalHash.hpp"
#include <eagler/netplay/NetplayInput.hpp>

#include "BulletManager.hpp"
#include "Controller.hpp"
#include "EclManager.hpp"
#include "EnemyManager.hpp"
#include "GameManager.hpp"
#include "ItemManager.hpp"
#include "Player.hpp"
#include "Rng.hpp"
#include "Stage.hpp"
#include "Supervisor.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace Netplay::Th07CanonicalHash
{
namespace
{
constexpr std::uint64_t FNV_OFFSET = 1469598103934665603ull;
constexpr std::uint64_t FNV_PRIME = 1099511628211ull;

struct Hasher
{
    std::uint64_t value = FNV_OFFSET;

    void Bytes(const void *data, std::size_t size)
    {
        const auto *bytes = static_cast<const std::uint8_t *>(data);
        for (std::size_t i = 0; i < size; ++i)
        {
            value ^= bytes[i];
            value *= FNV_PRIME;
        }
    }

    template <typename T>
    void Scalar(const T &scalar)
    {
        Bytes(&scalar, sizeof(scalar));
    }
};

void HashFloat2(Hasher &hash, const Float2 &value)
{
    hash.Scalar(value.x);
    hash.Scalar(value.y);
}

void HashVec3(Hasher &hash, const ZunVec3 &value)
{
    hash.Scalar(value.x);
    hash.Scalar(value.y);
    hash.Scalar(value.z);
}

void HashTimer(Hasher &hash, const ZunTimer &timer)
{
    hash.Scalar(timer.previous);
    hash.Scalar(timer.subFrame);
    hash.Scalar(timer.current);
}

void HashGlobals(Hasher &hash, const ZunGlobals &globals)
{
    hash.Scalar(globals.guiScore);
    hash.Scalar(globals.score);
    hash.Scalar(globals.guiScoreDifference);
    hash.Scalar(globals.highScore);
    hash.Scalar(globals.grazeInStage);
    hash.Scalar(globals.grazeInTotal);
    hash.Scalar(globals.spellCardsCaptured);
    hash.Scalar(globals.numRetries);
    hash.Scalar(globals.pointItemsCollectedThisStage);
    hash.Scalar(globals.pointItemsCollectedForExtend);
    hash.Scalar(globals.extendsFromPointItems);
    hash.Scalar(globals.nextNeededPointItemsForExtend);
    for (i32 value : globals.rng1)
        hash.Scalar(value);
    hash.Scalar(globals.deaths);
    for (f32 value : globals.rngFloat1)
        hash.Scalar(value);
    hash.Scalar(globals.livesRemaining);
    for (f32 value : globals.rngFloat2)
        hash.Scalar(value);
    hash.Scalar(globals.bombsRemaining);
    hash.Scalar(globals.bombsUsed);
    for (f32 value : globals.rngFloat3)
        hash.Scalar(value);
    hash.Scalar(globals.currentPower);
    for (f32 value : globals.rngFloat4)
        hash.Scalar(value);
    hash.Scalar(globals.cherryStart);
    for (i32 value : globals.rng2)
        hash.Scalar(value);
    hash.Scalar(globals.curCsum);
    hash.Scalar(globals.csumAsSum);
    for (i32 value : globals.csumData)
        hash.Scalar(value);
}

void HashGameManager(Hasher &hash)
{
    hash.Scalar(g_GameManager.isTimeStopped);
    hash.Scalar(g_GameManager.slowModeSlowActive);
    hash.Scalar(g_GameManager.difficulty);
    hash.Scalar(g_GameManager.difficultyMask);
    hash.Scalar(g_GameManager.isPaused);
    hash.Scalar(g_GameManager.powerItemCountForScore);
    hash.Scalar(g_GameManager.character);
    hash.Scalar(g_GameManager.shotType);
    hash.Scalar(g_GameManager.shotTypeAndCharacter);
    hash.Scalar(g_GameManager.flags);
    hash.Scalar(g_GameManager.isInPauseMenu);
    hash.Scalar(g_GameManager.isInRetryMenu);
    hash.Scalar(g_GameManager.demoIdx);
    hash.Scalar(g_GameManager.replayStage);
    hash.Scalar(g_GameManager.demoFrames);
    hash.Scalar(g_GameManager.stageRngSeed);
    hash.Scalar(g_GameManager.framesThisStage);
    hash.Scalar(g_GameManager.currentStage);
    HashFloat2(hash, g_GameManager.arcadeRegionTopLeftPos);
    HashFloat2(hash, g_GameManager.arcadeRegionSize);
    HashFloat2(hash, g_GameManager.playerMovementAreaTopLeftPos);
    HashFloat2(hash, g_GameManager.playerMovementAreaSize);
    hash.Scalar(g_GameManager.csumFloat);
    hash.Scalar(g_GameManager.cherryMax);
    hash.Scalar(g_GameManager.cherry);
    hash.Scalar(g_GameManager.cherryPlus);
    hash.Scalar(g_GameManager.bulletLagTime);
    hash.Scalar(g_GameManager.rank.rank);
    hash.Scalar(g_GameManager.rank.maxRank);
    hash.Scalar(g_GameManager.rank.minRank);
    hash.Scalar(g_GameManager.subrank);
    if (g_GameManager.globals)
        HashGlobals(hash, *g_GameManager.globals);
}

void HashEclArgs(Hasher &hash, const EclContextArgs &args)
{
    for (i32 value : args.intVars1)
        hash.Scalar(value);
    for (f32 value : args.floatVars1)
        hash.Scalar(value);
    for (i32 value : args.intVars2)
        hash.Scalar(value);
    for (f32 value : args.floatVars2)
        hash.Scalar(value);
    for (i32 value : args.globalVars.intVars)
        hash.Scalar(value);
    for (f32 value : args.globalVars.floatVars)
        hash.Scalar(value);
}

void HashRawInstrIdentity(Hasher &hash, const EclRawInstr *instr)
{
    const std::uint8_t present = instr ? 1 : 0;
    hash.Scalar(present);
    if (!instr)
        return;
    hash.Scalar(instr->time);
    hash.Scalar(instr->id);
    hash.Scalar(instr->size);
    hash.Scalar(instr->unused_8);
    hash.Scalar(instr->skipInstrOnDifficulty);
    hash.Scalar(instr->paramMask);
}

void HashTimelineInstrIdentity(Hasher &hash, const EclTimelineInstr *instr)
{
    const std::uint8_t present = instr ? 1 : 0;
    hash.Scalar(present);
    if (!instr)
        return;
    hash.Scalar(instr->time);
    hash.Scalar(instr->arg0);
    hash.Scalar(instr->opcode);
    hash.Scalar(instr->size);
    for (const AnyArg &arg : instr->args.args)
        hash.Scalar(arg);
}

void HashEclContext(Hasher &hash, const EnemyEclContext &ctx)
{
    HashRawInstrIdentity(hash, ctx.curInstr);
    HashTimer(hash, ctx.time);
    hash.Scalar(static_cast<std::uint8_t>(ctx.func != nullptr));
    HashRawInstrIdentity(hash, ctx.eclExInstr);
    HashEclArgs(hash, ctx.eclContextArgs);
    HashTimer(hash, ctx.waitTimer);
    for (const EclInterp &interp : ctx.interps)
    {
        hash.Scalar(static_cast<std::uint8_t>(interp.fn != nullptr));
        HashTimer(hash, interp.timer);
        for (const AnyArg &arg : interp.args)
            hash.Scalar(arg);
    }
    hash.Scalar(ctx.laserNotInUse);
    hash.Scalar(ctx.isPeriodicSub);
    hash.Scalar(ctx.subId);
}

void HashBulletShooter(Hasher &hash, const EnemyBulletShooter &shooter)
{
    hash.Scalar(shooter.sprite);
    hash.Scalar(shooter.spriteOffset);
    HashVec3(hash, shooter.pos);
    hash.Scalar(shooter.angle1);
    hash.Scalar(shooter.angle2);
    hash.Scalar(shooter.speed1);
    hash.Scalar(shooter.speed2);
    for (const BulletCommand &command : shooter.commands)
    {
        hash.Scalar(command.speed);
        hash.Scalar(command.angle);
        hash.Scalar(command.duration);
        hash.Scalar(command.loopCount);
        hash.Scalar(command.type);
        hash.Scalar(command.flag);
    }
    for (i32 value : shooter.unused_b0)
        hash.Scalar(value);
    hash.Scalar(shooter.count1);
    hash.Scalar(shooter.count2);
    hash.Scalar(shooter.aimMode);
    hash.Scalar(shooter.unused_c2);
    hash.Scalar(shooter.flags);
    hash.Scalar(shooter.soundIdx);
    hash.Scalar(shooter.soundOverride);
}

void HashLaserShooter(Hasher &hash, const EnemyLaserShooter &shooter)
{
    hash.Scalar(shooter.sprite);
    hash.Scalar(shooter.spriteOffset);
    HashVec3(hash, shooter.pos);
    hash.Scalar(shooter.angle1);
    hash.Scalar(shooter.angle2);
    hash.Scalar(shooter.speed1);
    hash.Scalar(shooter.speed2);
    for (const BulletCommand &command : shooter.commands)
    {
        hash.Scalar(command.speed);
        hash.Scalar(command.angle);
        hash.Scalar(command.duration);
        hash.Scalar(command.loopCount);
        hash.Scalar(command.type);
        hash.Scalar(command.flag);
    }
    hash.Scalar(shooter.startOffset);
    hash.Scalar(shooter.endOffset);
    hash.Scalar(shooter.startLength);
    hash.Scalar(shooter.width);
    hash.Scalar(shooter.startTime);
    hash.Scalar(shooter.duration);
    hash.Scalar(shooter.endTime);
    hash.Scalar(shooter.hitboxStartTime);
    hash.Scalar(shooter.hitboxEndTime);
    hash.Scalar(shooter.unused_bc);
    hash.Scalar(shooter.type);
    hash.Scalar(shooter.unused_c2);
    hash.Scalar(shooter.flags);
    hash.Scalar(shooter.unused_c8);
    hash.Scalar(shooter.soundOverride);
    hash.Scalar(shooter.unused_d0);
}

i32 StableLaserIndex(const Laser *laser)
{
    if (!laser)
        return -1;
    const Laser *begin = &g_BulletManager.lasers[0];
    const Laser *end = begin + 64;
    return laser >= begin && laser < end ? static_cast<i32>(laser - begin) : -2;
}

void HashEnemy(Hasher &hash, i32 index, const Enemy &enemy)
{
    hash.Scalar(index);
    HashEclContext(hash, enemy.currentContext);
    hash.Scalar(enemy.stackDepth);
    const i32 stackDepth = std::clamp(enemy.stackDepth, 0, 16);
    for (i32 i = 0; i < stackDepth; ++i)
        HashEclContext(hash, enemy.savedContextStack[i]);
    hash.Scalar(enemy.unused_2a80);
    hash.Scalar(enemy.deathCallbackSub);
    for (i32 value : enemy.interrupts)
        hash.Scalar(value);
    hash.Scalar(enemy.runInterrupt);
    HashVec3(hash, enemy.pos);
    HashVec3(hash, enemy.prevRenderPos);
    HashVec3(hash, enemy.velocity);
    HashVec3(hash, enemy.prevPos);
    HashVec3(hash, enemy.deltaPos);
    HashVec3(hash, enemy.hitboxSize);
    HashVec3(hash, enemy.grazeSize);
    hash.Scalar(enemy.angle);
    hash.Scalar(enemy.angleVel);
    hash.Scalar(enemy.orbitAngle);
    hash.Scalar(enemy.orbitAngleVel);
    hash.Scalar(enemy.speed);
    hash.Scalar(enemy.accel);
    hash.Scalar(enemy.orbitRadius);
    hash.Scalar(enemy.orbitRadialVel);
    HashVec3(hash, enemy.shootOffset);
    HashVec3(hash, enemy.moveInterp);
    HashVec3(hash, enemy.moveInterpStartPos);
    HashTimer(hash, enemy.moveInterpTimer);
    hash.Scalar(enemy.moveInterpStartTime);
    hash.Scalar(enemy.bulletRankSpeedLow);
    hash.Scalar(enemy.bulletRankSpeedHigh);
    hash.Scalar(enemy.bulletRankAmount1Low);
    hash.Scalar(enemy.bulletRankAmount1High);
    hash.Scalar(enemy.bulletRankAmount2Low);
    hash.Scalar(enemy.bulletRankAmount2High);
    hash.Scalar(enemy.life);
    hash.Scalar(enemy.maxLife);
    hash.Scalar(enemy.score);
    HashTimer(hash, enemy.timer);
    HashBulletShooter(hash, enemy.bulletProps);
    hash.Scalar(enemy.shootInterval);
    HashTimer(hash, enemy.shootIntervalTimer);
    HashLaserShooter(hash, enemy.laserProps);
    for (const Laser *laser : enemy.lasers)
        hash.Scalar(StableLaserIndex(laser));
    hash.Scalar(enemy.laserIdx);
    hash.Scalar(enemy.itemDrop);
    hash.Scalar(enemy.deathAnm1);
    hash.Scalar(enemy.deathAnm2);
    hash.Scalar(enemy.deathAnm3);
    hash.Scalar(enemy.bossId);
    hash.Scalar(enemy.damageTintTimer);
    HashTimer(hash, enemy.unused_2e1c);
    hash.Scalar(enemy.flags1);
    hash.Scalar(enemy.flags2);
    hash.Scalar(enemy.flags3);
    hash.Scalar(enemy.flags4);
    hash.Scalar(enemy.spellcardDelayTimer);
    hash.Scalar(enemy.anmExFlags);
    hash.Scalar(enemy.zLayer);
    hash.Scalar(enemy.anmExDefaults);
    hash.Scalar(enemy.anmExFarLeft);
    hash.Scalar(enemy.anmExFarRight);
    hash.Scalar(enemy.anmExLeft);
    hash.Scalar(enemy.anmExRight);
    HashFloat2(hash, enemy.lowerMoveLimit);
    HashFloat2(hash, enemy.upperMoveLimit);
    hash.Scalar(enemy.lastDamage);
    hash.Scalar(enemy.effectsNum);
    hash.Scalar(enemy.effectDistance);
    for (i32 value : enemy.lifeCallbackThreshold)
        hash.Scalar(value);
    for (i32 value : enemy.lifeCallbackSub)
        hash.Scalar(value);
    hash.Scalar(enemy.timerCallbackThreshold);
    hash.Scalar(enemy.timerCallbackSub);
    hash.Scalar(enemy.periodicCallbackSub);
    HashEclArgs(hash, enemy.savedEclContextArgs);
    HashTimer(hash, enemy.periodicTimer);
    HashTimer(hash, enemy.periodicCounter);
    hash.Scalar(enemy.unused_2f68);
    HashTimer(hash, enemy.unused_2f6c);
    hash.Scalar(enemy.trailFlags);
    hash.Scalar(enemy.trailCount);
    hash.Scalar(enemy.trailInterval);
    hash.Scalar(enemy.trailNodeStep);
    HashTimer(hash, enemy.invincibilityTimer);
}

void HashPlayerBullet(Hasher &hash, i32 index, const PlayerBullet &bullet)
{
    hash.Scalar(index);
    HashVec3(hash, bullet.pos);
    HashVec3(hash, bullet.prevPos);
    for (const ZunVec3 &value : bullet.posHistory)
        HashVec3(hash, value);
    HashVec3(hash, bullet.hitboxSize);
    HashFloat2(hash, bullet.velocity);
    HashFloat2(hash, bullet.offset);
    hash.Scalar(bullet.speed);
    hash.Scalar(bullet.angle);
    hash.Scalar(bullet.prevAngle);
    HashTimer(hash, bullet.timer);
    hash.Scalar(bullet.damage);
    hash.Scalar(bullet.bulletState);
    hash.Scalar(bullet.bulletState2);
    hash.Scalar(bullet.timerIdx);
    hash.Scalar(bullet.optionId);
    hash.Scalar(bullet.trailLength);
}

i32 StablePlayerBulletIndex(const Player &player, const PlayerBullet *bullet)
{
    if (!bullet)
        return -1;
    const PlayerBullet *begin = &player.bullets[0];
    const PlayerBullet *end = begin + 96;
    return bullet >= begin && bullet < end ? static_cast<i32>(bullet - begin) : -2;
}

void HashPlayer(Hasher &hash, const Player &player)
{
    HashVec3(hash, player.pos);
    HashVec3(hash, player.prevPos);
    HashVec3(hash, player.hitboxTopLeft);
    HashVec3(hash, player.hitboxBottomRight);
    HashVec3(hash, player.grazeTopLeft);
    HashVec3(hash, player.grazeBottomRight);
    HashVec3(hash, player.grabItemTopLeft);
    HashVec3(hash, player.grabItemBottomRight);
    HashVec3(hash, player.hitboxSize);
    HashVec3(hash, player.grazeSize);
    HashVec3(hash, player.grabItemSize);
    for (const ZunVec3 &value : player.optionsPosition)
        HashVec3(hash, value);
    for (const ZunVec3 &value : player.prevOptionsPosition)
        HashVec3(hash, value);
    HashFloat2(hash, player.velocity);
    hash.Scalar(player.unused_9d4);
    for (const BombProjectile &box : player.bombDamageBoxes)
    {
        HashVec3(hash, box.pos);
        HashVec3(hash, box.size);
        hash.Scalar(box.lifetime);
        hash.Scalar(box.damage);
    }
    for (const BombClearBox &box : player.bombClearBoxes)
    {
        HashFloat2(hash, box.pos);
        HashFloat2(hash, box.size);
        hash.Scalar(box.radius);
        hash.Scalar(box.radiusGrowth);
        hash.Scalar(box.lifetime);
        hash.Scalar(box.itemType);
    }
    hash.Scalar(player.numActiveBombClearBoxes);
    hash.Scalar(player.dirtyBombBoxes);
    hash.Scalar(player.isBombing);
    hash.Scalar(player.horizontalMovementSpeedMultiplierDuringBomb);
    hash.Scalar(player.verticalMovementSpeedMultiplierDuringBomb);
    hash.Scalar(player.respawnTimer);
    hash.Scalar(player.borderInvulnerabilityTime);
    hash.Scalar(player.bulletGracePeriod);
    hash.Scalar(player.itemType);
    hash.Scalar(player.playerState);
    hash.Scalar(player.initParam);
    hash.Scalar(player.optionState);
    hash.Scalar(player.isFocus);
    hash.Scalar(player.bombParticleTime);
    hash.Scalar(player.hasBorder);
    HashTimer(hash, player.focusMovementTimer);
    hash.Scalar(player.playerDirection);
    hash.Scalar(player.previousHorizontalSpeed);
    hash.Scalar(player.previousVerticalSpeed);
    HashVec3(hash, player.positionOfLastEnemyHit);
    HashVec3(hash, player.sakuyaTargetPosition);
    hash.Scalar(player.targetingEnemy);
    for (i32 i = 0; i < 96; ++i)
        if (player.bullets[i].bulletState != 0)
            HashPlayerBullet(hash, i, player.bullets[i]);
    for (const PlayerBulletTimer &timer : player.timers)
    {
        HashTimer(hash, timer.timer);
        hash.Scalar(StablePlayerBulletIndex(player, timer.bullet));
    }
    HashTimer(hash, player.fireBulletTimer);
    HashTimer(hash, player.invulnerabilityTimer);
    HashTimer(hash, player.borderTimer);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    hash.Scalar(player.lifeGiveTimer);
    hash.Scalar(player.lifeGiveTargetToken);
#else
    hash.Scalar(player.unused_16a18);
    hash.Scalar(player.unused_16a1c);
#endif
    hash.Scalar(player.bombInfo.isInUse);
    hash.Scalar(player.bombInfo.isFocus);
    hash.Scalar(player.bombInfo.bombDuration);
    hash.Scalar(player.bombInfo.cherryDrain);
    HashTimer(hash, player.bombInfo.bombTimer);
    if (player.bombInfo.isInUse)
    {
        for (const PlayerBombSubInfo &sub : player.bombInfo.subInfo)
        {
            hash.Scalar(sub.state);
            hash.Scalar(sub.counter);
            hash.Scalar(sub.custom);
            hash.Scalar(sub.prevCustom);
            hash.Scalar(sub.speed);
            hash.Scalar(sub.angle);
            HashVec3(hash, sub.pos);
            HashVec3(hash, sub.prevPos);
            for (const ZunVec3 &trail : sub.posHistory)
                HashVec3(hash, trail);
            HashVec3(hash, sub.velocity);
            HashVec3(hash, sub.accel);
            HashTimer(hash, sub.timer);
        }
    }
    HashVec3(hash, player.bombStartPos);
    hash.Scalar(player.optionAngle);
}

void HashBulletCommand(Hasher &hash, const BulletCommand &command)
{
    hash.Scalar(command.speed);
    hash.Scalar(command.angle);
    hash.Scalar(command.duration);
    hash.Scalar(command.loopCount);
    hash.Scalar(command.type);
    hash.Scalar(command.flag);
}

void HashBulletCommandState(Hasher &hash, const BulletCommandState &state)
{
    HashTimer(hash, state.timer);
    hash.Scalar(state.speed);
    hash.Scalar(state.angle);
    HashVec3(hash, state.vec3);
    hash.Scalar(state.duration);
    hash.Scalar(state.maxTimes);
    hash.Scalar(state.minTimes);
}

void HashBullet(Hasher &hash, i32 index, const Bullet &bullet)
{
    hash.Scalar(index);
    HashVec3(hash, bullet.pos);
    HashVec3(hash, bullet.prevPos);
    HashVec3(hash, bullet.velocity);
    HashVec3(hash, bullet.unused_ba4);
    hash.Scalar(bullet.speed);
    hash.Scalar(bullet.accel);
    hash.Scalar(bullet.angleVel);
    hash.Scalar(bullet.angle);
    hash.Scalar(bullet.prevAngle);
    hash.Scalar(bullet.unused_bc0);
    hash.Scalar(bullet.unused_bc4);
    HashTimer(hash, bullet.timer1);
    HashTimer(hash, bullet.timer2);
    for (i32 value : bullet.unused_be0)
        hash.Scalar(value);
    hash.Scalar(bullet.spawnDelay);
    hash.Scalar(bullet.exFlags);
    hash.Scalar(bullet.moreFlags);
    hash.Scalar(bullet.spriteOffset);
    hash.Scalar(bullet.unused_bfa);
    hash.Scalar(bullet.state);
    hash.Scalar(bullet.outOfBoundsTime);
    hash.Scalar(bullet.spawned);
    hash.Scalar(bullet.grazed);
    hash.Scalar(bullet.state2);
    hash.Scalar(bullet.soundIdx);
    hash.Scalar(bullet.curCmdIdx);
    for (const BulletCommand &command : bullet.commands)
        HashBulletCommand(hash, command);
    for (const BulletCommandState &state : bullet.commandStates)
        HashBulletCommandState(hash, state);
}

void HashLaser(Hasher &hash, i32 index, const Laser &laser)
{
    hash.Scalar(index);
    HashVec3(hash, laser.pos);
    HashVec3(hash, laser.prevPos);
    hash.Scalar(laser.angle);
    hash.Scalar(laser.prevAngle);
    hash.Scalar(laser.startOffset);
    hash.Scalar(laser.prevStartOffset);
    hash.Scalar(laser.endOffset);
    hash.Scalar(laser.prevEndOffset);
    hash.Scalar(laser.startLength);
    hash.Scalar(laser.width);
    hash.Scalar(laser.targetWidth);
    hash.Scalar(laser.speed);
    hash.Scalar(laser.startTime);
    hash.Scalar(laser.hitboxStartTime);
    hash.Scalar(laser.duration);
    hash.Scalar(laser.endTime);
    hash.Scalar(laser.hitboxEndTime);
    hash.Scalar(laser.isInUse);
    HashTimer(hash, laser.timer);
    hash.Scalar(laser.flags);
    hash.Scalar(laser.color);
    hash.Scalar(laser.state);
    hash.Scalar(laser.hideWarning);
}

void HashItem(Hasher &hash, i32 index, const Item &item)
{
    hash.Scalar(index);
    HashVec3(hash, item.pos);
    HashVec3(hash, item.prevPos);
    HashVec3(hash, item.velocity);
    HashVec3(hash, item.targetPos);
    HashTimer(hash, item.timer);
    hash.Scalar(item.itemType);
    hash.Scalar(item.isInUse);
    hash.Scalar(item.isOnscreen);
    hash.Scalar(item.state);
    hash.Scalar(item.autoCollect);
}

void HashStage(Hasher &hash)
{
    HashTimer(hash, g_Stage.scriptTime);
    hash.Scalar(g_Stage.instructionIndex);
    hash.Scalar(g_Stage.stageFrameCounter);
    hash.Scalar(g_Stage.stage);
    HashVec3(hash, g_Stage.pos);
    HashVec3(hash, g_Stage.prevPos);
    hash.Scalar(g_Stage.color);
    hash.Scalar(g_Stage.skyFog.nearPlane);
    hash.Scalar(g_Stage.skyFog.farPlane);
    hash.Scalar(g_Stage.fogEnd.nearPlane);
    hash.Scalar(g_Stage.fogEnd.farPlane);
    hash.Scalar(g_Stage.fogStart.nearPlane);
    hash.Scalar(g_Stage.fogStart.farPlane);
    hash.Scalar(g_Stage.skyFogInterpDuration);
    HashTimer(hash, g_Stage.skyFogInterpTimer);
    hash.Scalar(g_Stage.spellCardState);
    hash.Scalar(g_Stage.ticksSinceSpellcardStarted);
    hash.Scalar(g_Stage.clearBackground);
    hash.Scalar(g_Stage.scriptWaitTime);
    for (i32 value : g_Stage.timersMax)
        hash.Scalar(value);
    for (const ZunTimer &timer : g_Stage.timers)
        HashTimer(hash, timer);
    for (i32 value : g_Stage.easeModes)
        hash.Scalar(value);
    HashVec3(hash, g_Stage.positionStart);
    hash.Scalar(g_Stage.positionInterpEndTime);
    HashVec3(hash, g_Stage.positionInterpInitial);
    hash.Scalar(g_Stage.positionInterpStartTime);
    hash.Scalar(g_Stage.cameraTeleported);
    hash.Scalar(g_Stage.isDarkening);

    for (const EclTimeline &timeline : g_EnemyManager.timelines)
    {
        HashTimer(hash, timeline.timelineTime);
        HashTimelineInstrIdentity(hash, timeline.timelineInstr);
    }
    HashTimer(hash, g_EnemyManager.timelineTime);
}

std::uint64_t MixComposite(const Sample &sample)
{
    Hasher hash;
    hash.Scalar(sample.meta);
    hash.Scalar(sample.multiplayer);
    hash.Scalar(sample.stage);
    hash.Scalar(sample.player);
    hash.Scalar(sample.enemies);
    hash.Scalar(sample.bullets);
    hash.Scalar(sample.items);
    hash.Scalar(sample.enemyCount);
    hash.Scalar(sample.bulletCount);
    hash.Scalar(sample.laserCount);
    hash.Scalar(sample.itemCount);
    return hash.value;
}
} // namespace

Sample Capture()
{
    Sample sample;

    Hasher rng;
    rng.Scalar(g_Rng.seed);
    rng.Scalar(g_Rng.seedBackup);
    rng.Scalar(g_Rng.generationCount);
    sample.metaRng = rng.value;

    Hasher game;
    HashGameManager(game);
    for (i32 value : g_GlobalEclVars.intVars)
        game.Scalar(value);
    for (f32 value : g_GlobalEclVars.floatVars)
        game.Scalar(value);
    sample.metaGame = game.value;

    Hasher input;
#ifdef TH_ENABLE_NETPLAY
    for (const auto &touch : Input::GetDirectTouchStates())
    {
        input.Scalar(touch.x);
        input.Scalar(touch.y);
        input.Scalar(touch.active);
    }
#endif
    input.Scalar(g_CurFrameRawInput);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (u16 value : g_CurFrameGameInputs)
        input.Scalar(value);
#else
    input.Scalar(g_CurFrameGameInput);
#endif
    input.Scalar(g_LastFrameRawInput);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (u16 value : g_LastFrameGameInputs)
        input.Scalar(value);
#else
    input.Scalar(g_LastFrameGameInput);
#endif
    input.Scalar(g_IsEighthFrameOfHeldInput);
    input.Scalar(g_NumOfFramesInputsWereHeld);
    sample.metaInput = input.value;

    Hasher supervisor;
    supervisor.Scalar(g_Supervisor.wantedState);
    supervisor.Scalar(g_Supervisor.curState);
    supervisor.Scalar(g_Supervisor.prevState);
    supervisor.Scalar(g_Supervisor.isInEnding);
    supervisor.Scalar(g_Supervisor.effectiveFramerateMultiplier);
    supervisor.Scalar(g_Supervisor.flags);
    sample.metaSupervisor = supervisor.value;

    Hasher meta;
    meta.Scalar(sample.metaRng);
    meta.Scalar(sample.metaGame);
    meta.Scalar(sample.metaInput);
    meta.Scalar(sample.metaSupervisor);
    sample.meta = meta.value;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    Hasher multiplayer;
    for (bool active : g_PlayerActive)
        multiplayer.Scalar(active);
    for (const MultiplayerPlayerResources &resources : g_MultiplayerPlayerResources)
    {
        multiplayer.Scalar(resources.livesRemaining);
        multiplayer.Scalar(resources.bombsRemaining);
        multiplayer.Scalar(resources.currentPower);
    }
    for (const MultiplayerContributionStats &stats : g_MultiplayerContributionStats)
    {
        multiplayer.Scalar(stats.enemiesDefeated);
        multiplayer.Scalar(stats.damageDealt);
    }
    for (i32 value : g_cherryMaxGrazeGrowth)
        multiplayer.Scalar(value);
    for (i32 value : g_cherryMaxBreakGrowth)
        multiplayer.Scalar(value);
    for (i32 value : g_powerGiveTaps)
        multiplayer.Scalar(value);
    for (i32 value : g_powerGiveWindow)
        multiplayer.Scalar(value);
    multiplayer.Scalar(g_teamWipeRetryFrames);
    sample.multiplayer = multiplayer.value;
#endif

    Hasher stage;
    HashStage(stage);
    sample.stage = stage.value;

    Hasher player;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        Hasher individual;
        individual.Scalar(playerId);
        individual.Scalar(g_PlayerActive[playerId]);
        if (g_PlayerActive[playerId])
            HashPlayer(individual, g_Players[playerId]);
        if (playerId == 0)
            sample.player0 = individual.value;
        else if (playerId == 1)
            sample.player1 = individual.value;
        else if (playerId == 2)
            sample.player2 = individual.value;

        player.Scalar(individual.value);
    }
#else
    HashPlayer(player, g_Player);
    sample.player0 = player.value;
#endif
    sample.player = player.value;

    Hasher enemies;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (i32 value : g_stage4ChainQueue)
        enemies.Scalar(value);
    enemies.Scalar(g_stage4ChainCount);
    enemies.Scalar(g_stage4ChainPos);
    enemies.Scalar(g_stage4ChainBossId);
    enemies.Scalar(g_stage4ChainCardActive);
    enemies.Scalar(g_stage4ChainPhaseLife);
    enemies.Scalar(g_stage4ChainSpellIdx);
#endif
    enemies.Scalar(g_EnemyManager.randomItemSpawnIdx);
    enemies.Scalar(g_EnemyManager.randomItemTableIdx);
    enemies.Scalar(g_EnemyManager.enemyCountReal);
    enemies.Scalar(g_EnemyManager.spellcardInfo.isCapturing);
    enemies.Scalar(g_EnemyManager.spellcardInfo.isActive);
    enemies.Scalar(g_EnemyManager.spellcardInfo.captureScore);
    enemies.Scalar(g_EnemyManager.spellcardInfo.grazeBonusScore);
    enemies.Scalar(g_EnemyManager.spellcardInfo.scoreDrainRate);
    enemies.Scalar(g_EnemyManager.spellcardInfo.spellcardIdx);
    enemies.Scalar(g_EnemyManager.spellcardInfo.usedBomb);
    HashTimer(enemies, g_EnemyManager.timer);
    for (i32 i = 0; i < 480; ++i)
    {
        if (!g_EnemyManager.enemies[i].active)
            continue;
        ++sample.enemyCount;
        HashEnemy(enemies, i, g_EnemyManager.enemies[i]);
    }
    sample.enemies = enemies.value;

    Hasher bullets;
    bullets.Scalar(g_BulletManager.bulletCount);
    bullets.Scalar(g_BulletManager.screenClearTime);
    HashTimer(bullets, g_BulletManager.time);
    bullets.Scalar(g_BulletManager.updateCount);
    bullets.Scalar(g_BulletManager.itemType);
    for (i32 i = 0; i < 1024; ++i)
    {
        if (g_BulletManager.bullets[i].state == BULLET_INACTIVE)
            continue;
        ++sample.bulletCount;
        HashBullet(bullets, i, g_BulletManager.bullets[i]);
    }
    for (i32 i = 0; i < 64; ++i)
    {
        if (!g_BulletManager.lasers[i].isInUse)
            continue;
        ++sample.laserCount;
        HashLaser(bullets, i, g_BulletManager.lasers[i]);
    }
    sample.bullets = bullets.value;

    Hasher items;
    items.Scalar(g_ItemManager.nextIndex);
    items.Scalar(g_ItemManager.activeItemCount);
    for (i32 i = 0; i < 1100; ++i)
    {
        if (!g_ItemManager.items[i].isInUse)
            continue;
        ++sample.itemCount;
        HashItem(items, i, g_ItemManager.items[i]);
    }
    sample.items = items.value;
    sample.composite = MixComposite(sample);
    return sample;
}
} // namespace Netplay::Th07CanonicalHash
