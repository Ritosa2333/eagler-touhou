#include "Th06CanonicalHash.hpp"

#include "BulletManager.hpp"
#include "EnemyEclInstr.hpp"
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

namespace Netplay::Th06CanonicalHash
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
    void Scalar(const T &value)
    {
        Bytes(&value, sizeof(value));
    }
};

void HashVec2(Hasher &hash, const ZunVec2 &value)
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

void HashGameManager(Hasher &hash)
{
    hash.Scalar(g_GameManager.guiScore);
    hash.Scalar(g_GameManager.score);
    hash.Scalar(g_GameManager.nextScoreIncrement);
    // highScore is initialized from this machine's score.dat and is only a
    // local HUD/persistence cache. It does not drive gameplay evolution, so
    // including it would report a false desync when two real peers have
    // different single-player save histories.
    hash.Scalar(g_GameManager.difficulty);
    hash.Scalar(g_GameManager.grazeInStage);
    hash.Scalar(g_GameManager.grazeInTotal);
    hash.Scalar(g_GameManager.isInReplay);
    hash.Scalar(g_GameManager.deaths);
    hash.Scalar(g_GameManager.bombsUsed);
    hash.Scalar(g_GameManager.spellcardsCaptured);
    hash.Scalar(g_GameManager.isTimeStopped);
    hash.Scalar(g_GameManager.currentPower);
    hash.Scalar(g_GameManager.pointItemsCollectedInStage);
    hash.Scalar(g_GameManager.pointItemsCollected);
    hash.Scalar(g_GameManager.numRetries);
    hash.Scalar(g_GameManager.powerItemCountForScore);
    hash.Scalar(g_GameManager.livesRemaining);
    hash.Scalar(g_GameManager.bombsRemaining);
    hash.Scalar(g_GameManager.extraLives);
    hash.Scalar(g_GameManager.character);
    hash.Scalar(g_GameManager.shotType);
    hash.Scalar(g_GameManager.isInGameMenu);
    hash.Scalar(g_GameManager.isInRetryMenu);
    hash.Scalar(g_GameManager.isInMenu);
    hash.Scalar(g_GameManager.isGameCompleted);
    hash.Scalar(g_GameManager.isInPracticeMode);
    hash.Scalar(g_GameManager.demoMode);
    hash.Scalar(g_GameManager.demoFrames);
    hash.Scalar(g_GameManager.randomSeed);
    hash.Scalar(g_GameManager.gameFrames);
    hash.Scalar(g_GameManager.currentStage);
    hash.Scalar(g_GameManager.menuCursorBackup);
    HashVec2(hash, g_GameManager.arcadeRegionTopLeftPos);
    HashVec2(hash, g_GameManager.arcadeRegionSize);
    HashVec2(hash, g_GameManager.playerMovementAreaTopLeftPos);
    HashVec2(hash, g_GameManager.playerMovementAreaSize);
    hash.Scalar(g_GameManager.cameraDistance);
    HashVec3(hash, g_GameManager.stageCameraFacingDir);
    hash.Scalar(g_GameManager.rank);
    hash.Scalar(g_GameManager.maxRank);
    hash.Scalar(g_GameManager.minRank);
    hash.Scalar(g_GameManager.subRank);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (const MultiplayerPlayerResources &resources : g_MultiplayerPlayerResources)
    {
        hash.Scalar(resources.livesRemaining);
        hash.Scalar(resources.bombsRemaining);
        hash.Scalar(resources.currentPower);
    }
    for (const MultiplayerContributionStats &stats : g_MultiplayerContributionStats)
    {
        hash.Scalar(stats.enemiesDefeated);
        hash.Scalar(stats.damageDealt);
    }
    hash.Scalar(g_teamWipeRetryFrames);
#endif
}

void HashRawInstrIdentity(Hasher &hash, const EclRawInstr *instr)
{
    const std::uint8_t present = instr ? 1 : 0;
    hash.Scalar(present);
    if (!instr)
        return;
    const i32 time = instr->time;
    const i16 opcode = instr->opCode;
    const i16 next = instr->offsetToNext;
    hash.Scalar(time);
    hash.Scalar(opcode);
    hash.Scalar(next);
    hash.Scalar(instr->skipForDifficulty);
}

void HashEclContext(Hasher &hash, const EnemyEclContext &ctx)
{
    HashRawInstrIdentity(hash, ctx.currentInstr);
    HashTimer(hash, ctx.time);
    hash.Scalar(ctx.var0);
    hash.Scalar(ctx.var1);
    hash.Scalar(ctx.var2);
    hash.Scalar(ctx.var3);
    hash.Scalar(ctx.float0);
    hash.Scalar(ctx.float1);
    hash.Scalar(ctx.float2);
    hash.Scalar(ctx.float3);
    hash.Scalar(ctx.var4);
    hash.Scalar(ctx.var5);
    hash.Scalar(ctx.var6);
    hash.Scalar(ctx.var7);
    hash.Scalar(ctx.compareRegister);
    hash.Scalar(ctx.subId);
}

i32 StableLaserIndex(const Laser *laser)
{
    if (!laser)
        return -1;
    const Laser *begin = &g_BulletManager.lasers[0];
    const Laser *end = begin + 64;
    return laser >= begin && laser < end ? static_cast<i32>(laser - begin) : -2;
}

void HashBulletShooter(Hasher &hash, const EnemyBulletShooter &shooter)
{
    hash.Scalar(shooter.sprite);
    hash.Scalar(shooter.spriteOffset);
    HashVec3(hash, shooter.position);
    hash.Scalar(shooter.angle1);
    hash.Scalar(shooter.angle2);
    hash.Scalar(shooter.speed1);
    hash.Scalar(shooter.speed2);
    for (f32 value : shooter.exFloats)
        hash.Scalar(value);
    for (i32 value : shooter.exInts)
        hash.Scalar(value);
    hash.Scalar(shooter.unk_40);
    hash.Scalar(shooter.count1);
    hash.Scalar(shooter.count2);
    hash.Scalar(shooter.aimMode);
    hash.Scalar(shooter.unk_4a);
    hash.Scalar(shooter.flags);
    hash.Scalar(shooter.sfx);
}

void HashLaserShooter(Hasher &hash, const EnemyLaserShooter &shooter)
{
    hash.Scalar(shooter.sprite);
    hash.Scalar(shooter.spriteOffset);
    HashVec3(hash, shooter.position);
    hash.Scalar(shooter.angle);
    hash.Scalar(shooter.unk_14);
    hash.Scalar(shooter.speed);
    hash.Scalar(shooter.unk_1c);
    hash.Scalar(shooter.startOffset);
    hash.Scalar(shooter.endOffset);
    hash.Scalar(shooter.startLength);
    hash.Scalar(shooter.width);
    hash.Scalar(shooter.startTime);
    hash.Scalar(shooter.duration);
    hash.Scalar(shooter.despawnDuration);
    hash.Scalar(shooter.hitboxStartTime);
    hash.Scalar(shooter.hitboxEndDelay);
    hash.Scalar(shooter.unk_44);
    hash.Scalar(shooter.type);
    hash.Scalar(shooter.flags);
    hash.Scalar(shooter.unk_50);
}

void HashEnemy(Hasher &hash, i32 index, const Enemy &enemy)
{
    hash.Scalar(index);
    HashEclContext(hash, enemy.currentContext);
    hash.Scalar(enemy.stackDepth);
    const i32 stackDepth = std::clamp(enemy.stackDepth, 0, 8);
    for (i32 i = 0; i < stackDepth; ++i)
        HashEclContext(hash, enemy.savedContextStack[i]);
    hash.Scalar(enemy.unk_c40);
    hash.Scalar(enemy.deathCallbackSub);
    for (i32 value : enemy.interrupts)
        hash.Scalar(value);
    hash.Scalar(enemy.runInterrupt);
    HashVec3(hash, enemy.position);
    HashVec3(hash, enemy.hitboxDimensions);
    HashVec3(hash, enemy.axisSpeed);
    hash.Scalar(enemy.angle);
    hash.Scalar(enemy.angularVelocity);
    hash.Scalar(enemy.speed);
    hash.Scalar(enemy.acceleration);
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
    HashTimer(hash, enemy.bossTimer);
    HashBulletShooter(hash, enemy.bulletProps);
    hash.Scalar(enemy.shootInterval);
    HashTimer(hash, enemy.shootIntervalTimer);
    HashLaserShooter(hash, enemy.laserProps);
    for (const Laser *laser : enemy.lasers)
        hash.Scalar(StableLaserIndex(laser));
    hash.Scalar(enemy.laserStore);
    hash.Scalar(enemy.deathAnm1);
    hash.Scalar(enemy.deathAnm2);
    hash.Scalar(enemy.deathAnm3);
    hash.Scalar(enemy.itemDrop);
    hash.Scalar(enemy.bossId);
    hash.Scalar(enemy.unk_e41);
    HashTimer(hash, enemy.exInsFunc10Timer);
    hash.Scalar(enemy.flags.movementMode);
    hash.Scalar(enemy.flags.movementEaseType);
    hash.Scalar(enemy.flags.shootingDisabled);
    hash.Scalar(enemy.flags.invertX);
    hash.Scalar(enemy.flags.isSlotOccupied);
    hash.Scalar(enemy.flags.isInteractable);
    hash.Scalar(enemy.flags.isCollidable);
    hash.Scalar(enemy.flags.hasBeenInBounds);
    hash.Scalar(enemy.flags.isBoss);
    hash.Scalar(enemy.flags.isDamageable);
    hash.Scalar(enemy.flags.deathMode);
    hash.Scalar(enemy.flags.shouldClampPos);
    hash.Scalar(enemy.flags.rotateAnm);
    hash.Scalar(enemy.flags.disableCallStack);
    hash.Scalar(enemy.flags.isInvisible);
    hash.Scalar(enemy.flags.isTimeoutSpell);
    hash.Scalar(enemy.anmExFlags);
    hash.Scalar(enemy.anmExDefaults);
    hash.Scalar(enemy.anmExFarLeft);
    hash.Scalar(enemy.anmExFarRight);
    hash.Scalar(enemy.anmExLeft);
    hash.Scalar(enemy.anmExRight);
    HashVec2(hash, enemy.lowerMoveLimit);
    HashVec2(hash, enemy.upperMoveLimit);
    hash.Scalar(enemy.effectIdx);
    hash.Scalar(enemy.effectDistance);
    hash.Scalar(enemy.lifeCallbackThreshold);
    hash.Scalar(enemy.lifeCallbackSub);
    hash.Scalar(enemy.timerCallbackThreshold);
    hash.Scalar(enemy.timerCallbackSub);
    hash.Scalar(enemy.exInsFunc6Angle);
    HashTimer(hash, enemy.exInsFunc6Timer);
}

void HashPlayerBullet(Hasher &hash, i32 index, const PlayerBullet &bullet)
{
    hash.Scalar(index);
    HashVec3(hash, bullet.position);
    HashVec3(hash, bullet.size);
    HashVec2(hash, bullet.velocity);
    hash.Scalar(bullet.sidewaysMotion);
    HashVec3(hash, bullet.unk_134);
    HashTimer(hash, bullet.unk_140);
    hash.Scalar(bullet.damage);
    hash.Scalar(bullet.bulletState);
    hash.Scalar(bullet.bulletType);
    hash.Scalar(bullet.unk_152);
    hash.Scalar(bullet.spawnPositionIdx);
}

void HashPlayer(Hasher &hash, const Player &player)
{
    HashVec3(hash, player.positionCenter);
    HashVec3(hash, player.unk_44c);
    HashVec3(hash, player.hitboxTopLeft);
    HashVec3(hash, player.hitboxBottomRight);
    HashVec3(hash, player.grabItemTopLeft);
    HashVec3(hash, player.grabItemBottomRight);
    HashVec3(hash, player.hitboxSize);
    HashVec3(hash, player.grabItemSize);
    for (const ZunVec3 &value : player.orbsPosition)
        HashVec3(hash, value);
    for (const ZunVec3 &value : player.bombRegionPositions)
        HashVec3(hash, value);
    for (const ZunVec3 &value : player.bombRegionSizes)
        HashVec3(hash, value);
    for (i32 value : player.bombRegionDamages)
        hash.Scalar(value);
    for (i32 value : player.unk_838)
        hash.Scalar(value);
    for (const PlayerRect &rect : player.bombProjectiles)
    {
        hash.Scalar(rect.posX);
        hash.Scalar(rect.posY);
        hash.Scalar(rect.sizeX);
        hash.Scalar(rect.sizeY);
    }
    for (const ZunTimer &timer : player.laserTimer)
        HashTimer(hash, timer);
    hash.Scalar(player.horizontalMovementSpeedMultiplierDuringBomb);
    hash.Scalar(player.verticalMovementSpeedMultiplierDuringBomb);
    hash.Scalar(player.respawnTimer);
    hash.Scalar(player.bulletGracePeriod);
    hash.Scalar(player.playerState);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    hash.Scalar(player.initParam);
    hash.Scalar(player.lifeGiveTimer);
    hash.Scalar(player.lifeGiveTargetToken);
    hash.Scalar(player.powerGiveTaps);
    hash.Scalar(player.powerGiveWindow);
#endif
    hash.Scalar(player.orbState);
    hash.Scalar(player.isFocus);
    hash.Scalar(player.unk_9e4);
    HashTimer(hash, player.focusMovementTimer);
    hash.Scalar(player.playerDirection);
    hash.Scalar(player.previousHorizontalSpeed);
    hash.Scalar(player.previousVerticalSpeed);
    hash.Scalar(player.previousFrameInput);
    HashVec3(hash, player.positionOfLastEnemyHit);
    for (i32 i = 0; i < 80; ++i)
        if (player.bullets[i].bulletState != BULLET_STATE_UNUSED)
            HashPlayerBullet(hash, i, player.bullets[i]);
    HashTimer(hash, player.fireBulletTimer);
    HashTimer(hash, player.invulnerabilityTimer);
    hash.Scalar(player.bombInfo.isInUse);
    hash.Scalar(player.bombInfo.duration);
    HashTimer(hash, player.bombInfo.timer);
    if (player.bombInfo.isInUse)
    {
        for (i32 value : player.bombInfo.reimuABombProjectilesState)
            hash.Scalar(value);
        for (f32 value : player.bombInfo.reimuABombProjectilesRelated)
            hash.Scalar(value);
        for (const ZunVec3 &value : player.bombInfo.bombRegionPositions)
            HashVec3(hash, value);
        for (const ZunVec3 &value : player.bombInfo.bombRegionVelocities)
            HashVec3(hash, value);
    }
}

void HashBullet(Hasher &hash, i32 index, const Bullet &bullet)
{
    hash.Scalar(index);
    HashVec3(hash, bullet.pos);
    HashVec3(hash, bullet.velocity);
    HashVec3(hash, bullet.ex4Acceleration);
    hash.Scalar(bullet.speed);
    hash.Scalar(bullet.ex5Float0);
    hash.Scalar(bullet.dirChangeSpeed);
    hash.Scalar(bullet.angle);
    hash.Scalar(bullet.ex5Float1);
    hash.Scalar(bullet.dirChangeRotation);
    HashTimer(hash, bullet.timer);
    hash.Scalar(bullet.ex5Int0);
    hash.Scalar(bullet.dirChangeInterval);
    hash.Scalar(bullet.dirChangeNumTimes);
    hash.Scalar(bullet.dirChangeMaxTimes);
    hash.Scalar(bullet.exFlags);
    hash.Scalar(bullet.spriteOffset);
    hash.Scalar(bullet.unk_5bc);
    hash.Scalar(bullet.state);
    hash.Scalar(bullet.unk_5c0);
    hash.Scalar(bullet.unk_5c2);
    hash.Scalar(bullet.isGrazed);
}

void HashLaser(Hasher &hash, i32 index, const Laser &laser)
{
    hash.Scalar(index);
    HashVec3(hash, laser.pos);
    hash.Scalar(laser.angle);
    hash.Scalar(laser.startOffset);
    hash.Scalar(laser.endOffset);
    hash.Scalar(laser.startLength);
    hash.Scalar(laser.width);
    hash.Scalar(laser.speed);
    hash.Scalar(laser.startTime);
    hash.Scalar(laser.hitboxStartTime);
    hash.Scalar(laser.duration);
    hash.Scalar(laser.despawnDuration);
    hash.Scalar(laser.hitboxEndDelay);
    hash.Scalar(laser.inUse);
    HashTimer(hash, laser.timer);
    hash.Scalar(laser.flags);
    hash.Scalar(laser.color);
    hash.Scalar(laser.state);
}

void HashItem(Hasher &hash, i32 index, const Item &item)
{
    hash.Scalar(index);
    HashVec3(hash, item.currentPosition);
    HashVec3(hash, item.startPosition);
    HashVec3(hash, item.targetPosition);
    HashTimer(hash, item.timer);
    hash.Scalar(item.itemType);
    hash.Scalar(item.isInUse);
    hash.Scalar(item.state);
}

void HashTimelineInstrIdentity(Hasher &hash, const EclTimelineInstr *instr)
{
    const std::uint8_t present = instr ? 1 : 0;
    hash.Scalar(present);
    if (!instr)
        return;
    const i16 time = instr->time;
    const i16 arg0 = instr->arg0;
    const i16 opcode = instr->opCode;
    const i16 size = instr->size;
    hash.Scalar(time);
    hash.Scalar(arg0);
    hash.Scalar(opcode);
    hash.Scalar(size);
}

void HashStage(Hasher &hash)
{
    HashTimer(hash, g_Stage.scriptTime);
    hash.Scalar(g_Stage.instructionIndex);
    HashTimer(hash, g_Stage.timer);
    hash.Scalar(g_Stage.stage);
    HashVec3(hash, g_Stage.position);
    hash.Scalar(g_Stage.skyFog.nearPlane);
    hash.Scalar(g_Stage.skyFog.farPlane);
    hash.Scalar(g_Stage.skyFogInterpInitial.nearPlane);
    hash.Scalar(g_Stage.skyFogInterpInitial.farPlane);
    hash.Scalar(g_Stage.skyFogInterpFinal.nearPlane);
    hash.Scalar(g_Stage.skyFogInterpFinal.farPlane);
    hash.Scalar(g_Stage.skyFogInterpDuration);
    HashTimer(hash, g_Stage.skyFogInterpTimer);
    hash.Scalar(g_Stage.skyFogNeedsSetup);
    hash.Scalar(g_Stage.spellcardState);
    hash.Scalar(g_Stage.ticksSinceSpellcardStarted);
    hash.Scalar(g_Stage.unpauseFlag);
    HashVec3(hash, g_Stage.facingDirInterpInitial);
    HashVec3(hash, g_Stage.facingDirInterpFinal);
    hash.Scalar(g_Stage.facingDirInterpDuration);
    HashTimer(hash, g_Stage.facingDirInterpTimer);
    HashVec3(hash, g_Stage.positionInterpFinal);
    hash.Scalar(g_Stage.positionInterpEndTime);
    HashVec3(hash, g_Stage.positionInterpInitial);
    hash.Scalar(g_Stage.positionInterpStartTime);
    hash.Scalar(g_Stage.quadCount);
    hash.Scalar(g_Stage.objectsCount);
    for (i32 i = 0; i < g_Stage.objectsCount; ++i)
    {
        const std::uint8_t present = g_Stage.objects && g_Stage.objects[i] ? 1 : 0;
        hash.Scalar(present);
        if (present)
            hash.Scalar(g_Stage.objects[i]->flags);
    }
    hash.Scalar(g_EnemyManager.randomItemSpawnIndex);
    hash.Scalar(g_EnemyManager.randomItemTableIndex);
    HashTimelineInstrIdentity(hash, g_EnemyManager.timelineInstr);
    HashTimer(hash, g_EnemyManager.timelineTime);

    const EnemyEclInstr::RollbackState &eclState = *EnemyEclInstr::GetRollbackState();
    for (f32 angle : eclState.starAngleTable)
        hash.Scalar(angle);
    HashVec3(hash, eclState.enemyPosVector);
    HashVec3(hash, eclState.playerPosVector);
}

std::uint64_t MixComposite(const Sample &sample)
{
    Hasher hash;
    hash.Scalar(sample.meta);
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
    rng.Scalar(g_Rng.generationCount);
    sample.metaRng = rng.value;

    Hasher game;
    HashGameManager(game);
    sample.metaGame = game.value;

    Hasher input;
    input.Scalar(g_CurFrameInput);
    input.Scalar(g_LastFrameInput);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (u16 value : g_CurFrameGameInputs)
        input.Scalar(value);
    for (u16 value : g_LastFrameGameInputs)
        input.Scalar(value);
#endif
    input.Scalar(g_IsEigthFrameOfHeldInput);
    input.Scalar(g_NumOfFramesInputsWereHeld);
    sample.metaInput = input.value;

    Hasher supervisor;
    supervisor.Scalar(g_Supervisor.wantedState);
    supervisor.Scalar(g_Supervisor.curState);
    supervisor.Scalar(g_Supervisor.wantedState2);
    supervisor.Scalar(g_Supervisor.isInEnding);
    supervisor.Scalar(g_Supervisor.effectiveFramerateMultiplier);
    sample.metaSupervisor = supervisor.value;

    Hasher meta;
    meta.Scalar(sample.metaRng);
    meta.Scalar(sample.metaGame);
    meta.Scalar(sample.metaInput);
    meta.Scalar(sample.metaSupervisor);
    sample.meta = meta.value;

    Hasher stage;
    HashStage(stage);
    sample.stage = stage.value;

    Hasher player;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
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
        else
            sample.player2 = individual.value;

        player.Scalar(individual.value);
    }
#else
    HashPlayer(player, g_Player);
    sample.player0 = player.value;
#endif
    sample.player = player.value;

    Hasher enemies;
    enemies.Scalar(g_EnemyManager.randomItemSpawnIndex);
    enemies.Scalar(g_EnemyManager.randomItemTableIndex);
    enemies.Scalar(g_EnemyManager.enemyCount);
    enemies.Scalar(g_EnemyManager.spellcardInfo.isCapturing);
    enemies.Scalar(g_EnemyManager.spellcardInfo.isActive);
    enemies.Scalar(g_EnemyManager.spellcardInfo.captureScore);
    enemies.Scalar(g_EnemyManager.spellcardInfo.idx);
    enemies.Scalar(g_EnemyManager.spellcardInfo.usedBomb);
    for (i32 i = 0; i < 257; ++i)
    {
        if (!g_EnemyManager.enemies[i].flags.isSlotOccupied)
            continue;
        ++sample.enemyCount;
        HashEnemy(enemies, i, g_EnemyManager.enemies[i]);
    }
    sample.enemies = enemies.value;

    Hasher bullets;
    bullets.Scalar(g_BulletManager.nextBulletIndex);
    bullets.Scalar(g_BulletManager.bulletCount);
    HashTimer(bullets, g_BulletManager.time);
    for (i32 i = 0; i < 640; ++i)
    {
        if (g_BulletManager.bullets[i].state == 0)
            continue;
        ++sample.bulletCount;
        HashBullet(bullets, i, g_BulletManager.bullets[i]);
    }
    for (i32 i = 0; i < 64; ++i)
    {
        if (!g_BulletManager.lasers[i].inUse)
            continue;
        ++sample.laserCount;
        HashLaser(bullets, i, g_BulletManager.lasers[i]);
    }
    sample.bullets = bullets.value;

    Hasher items;
    items.Scalar(g_ItemManager.nextIndex);
    items.Scalar(g_ItemManager.itemCount);
    for (i32 i = 0; i < 513; ++i)
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
} // namespace Netplay::Th06CanonicalHash
