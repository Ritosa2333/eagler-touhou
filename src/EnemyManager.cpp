#include "EnemyManager.hpp"

#include "AnmIdx.hpp"
#include "AsciiManager.hpp"
#include "Chain.hpp"
#include "EclManager.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "Gui.hpp"
#include "Player.hpp"
#include "PracticeRuntime.hpp"
#include "Rng.hpp"
#include "SoundPlayer.hpp"
#include "ZunResult.hpp"
#include "utils.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif
#ifdef TH_ENABLE_NETPLAY
#include "netplay/Th07RollbackState.hpp"
#endif

u32 g_SpellcardScore[141] = {
    0x1E8480, 0x1E8480, 0x2191C0, 0x2191C0, 0x249F00, 0x249F00, 0x249F00, 0x249F00, 0x249F00,
    0x249F00, 0x27AC40, 0x27AC40, 0x27AC40, 0x27AC40, 0x27AC40, 0x27AC40, 0x27AC40, 0x27AC40,
    0x27AC40, 0x27AC40, 0x27AC40, 0x27AC40, 0x27AC40, 0x27AC40, 0x27AC40, 0x27AC40, 0x2DC6C0,
    0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0,
    0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x3567E0,
    0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0,
    0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0,
    0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3567E0, 0x3D0900, 0x3D0900, 0x3D0900, 0x3D0900,
    0x3D0900, 0x3D0900, 0x3D0900, 0x3D0900, 0x3D0900, 0x3D0900, 0x3D0900, 0x3D0900, 0x3D0900,
    0x3D0900, 0x3D0900, 0x3D0900, 0x3D0900, 0x3D0900, 0x3D0900, 0x3D0900, 0x4C4B40, 0x4C4B40,
    0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40,
    0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40,
    0x4C4B40, 0x4C4B40, 0x4C4B40, 0x4C4B40, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x2DC6C0, 0x5B8D80,
    0x5B8D80, 0x6ACFC0, 0x6ACFC0, 0x6ACFC0, 0x6ACFC0, 0x6ACFC0, 0x6ACFC0, 0x6ACFC0, 0x6ACFC0,
    0x3D0900, 0x6ACFC0, 0x6ACFC0, 0x6ACFC0, 0x7A1200, 0x7A1200, 0x7A1200, 0x7A1200, 0x7A1200,
    0x7A1200, 0x7A1200, 0x7A1200, 0x3D0900, 0x7A1200, 0x3D0900};

ChainElem g_EnemyManagerDrawChain1;

EnemyManager g_EnemyManager;

ChainElem g_EnemyManagerCalcChain;

ChainElem g_EnemyManagerDrawChain2;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
static bool ShouldFreezeEnemyEclForPlayers()
{
    if (IsAnyActivePlayerBombing())
        return true;

    bool hasParticipant = false;
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (!IsPlayerSlotActive(playerId) ||
            MultiplayerGameplay::IsPlayerTemporarilyAbsent(playerId))
        {
            continue;
        }

        hasParticipant = true;
        if (g_Players[playerId].playerState == PLAYER_STATE_ALIVE)
            return false;
    }

    // If nobody can currently participate, holding the ECL is safer than
    // advancing a bomb/death-sensitive script with no live player. With one
    // player this preserves TH07's original non-ALIVE freeze semantics.
    return hasParticipant || MultiplayerGameplay::IsMultiplayer();
}
#endif

void Enemy::Move()
{
    this->deltaPos = this->pos - this->prevPos;
    this->prevPos = this->pos;
    if (!this->mirror)
    {
        this->pos.x += g_Supervisor.effectiveFramerateMultiplier * this->velocity.x;
    }
    else
    {
        this->pos.x -= g_Supervisor.effectiveFramerateMultiplier * this->velocity.x;
    }
    this->pos.y += g_Supervisor.effectiveFramerateMultiplier * this->velocity.y;
    this->pos.z += g_Supervisor.effectiveFramerateMultiplier * this->velocity.z;
}

EnemyManager::EnemyManager()
{
    Initialize();
}

Enemy::Enemy()
{
}

EnemyEclContext::EnemyEclContext()
{
}

Enemy *EnemyManager::SpawnEnemy(i32 eclSubId, ZunVec3 *pos, i32 life, i32 itemDrop, i32 score,
                                u8 mirror)
{
    Enemy *enemy;
    i32 i;

    enemy = this->enemies;
    for (i = 0; i < MAX_ENEMIES; i++, enemy++)
    {
        if (enemy->active)
        {
            continue;
        }

#ifdef TH_ENABLE_NETPLAY
        Netplay::Th07Rollback::TouchEnemy(enemy);
#endif

        *enemy = this->enemyTemplate;
        enemy->mirror = mirror;
        if (life >= 0)
        {
            enemy->life = life;
        }
        enemy->prevRenderPos = enemy->pos = *pos;
        g_EclManager.CallEclSub(&enemy->currentContext, eclSubId);
        if (g_EclManager.RunEcl(enemy) == ZUN_ERROR)
        {
            enemy->active = 0;
        }
        else
        {
            enemy->UpdatePrev();
            enemy->color.color = enemy->primaryVm.color.color;
            enemy->itemDrop = (i8)itemDrop;
            if (score >= 0)
            {
                enemy->score = score;
            }
            enemy->maxLife = enemy->life;
        }
        break;
    }
    return enemy;
}

Enemy *EnemyManager::SpawnEnemyEx(i32 eclSubId, ZunVec3 *pos, i32 life, i32 itemDrop, i32 score,
                                  EclContextArgs *args)
{
    Enemy *enemy;
    i32 i;

    enemy = this->enemies;
    for (i = 0; i < MAX_ENEMIES; i++, enemy++)
    {
        if (enemy->active)
        {
            continue;
        }

#ifdef TH_ENABLE_NETPLAY
        Netplay::Th07Rollback::TouchEnemy(enemy);
#endif

        *enemy = this->enemyTemplate;
        if (life >= 0)
        {
            enemy->life = life;
        }
        enemy->prevRenderPos = enemy->pos = *pos;
        g_EclManager.CallEclSub(&enemy->currentContext, eclSubId);
        enemy->currentContext.eclContextArgs = *args;
        if (g_EclManager.RunEcl(enemy) == ZUN_ERROR)
        {
            enemy->active = 0;
        }
        else
        {
            enemy->UpdatePrev();
            enemy->color.color = enemy->primaryVm.color.color;
            enemy->itemDrop = (i8)itemDrop;
            if (life >= 0)
            {
                enemy->life = life;
            }
            if (score >= 0)
            {
                enemy->score = score;
            }
            enemy->maxLife = enemy->life;
        }
        break;
    }
    return enemy;
}

void Enemy::UpdateEffects()
{
    Effect *effect;

    for (i32 i = 0; i < this->effectsNum; i++)
    {
        effect = this->effects[i];
        if (!effect)
        {
            continue;
        }

        effect->vm.active = !this->hasNoCollision;
        effect->emitterPos = this->pos;
        if (effect->radius < this->effectDistance)
        {
            effect->radius = effect->radius + 0.3f;
        }
        effect->angleVel = utils::AddNormalizeAngle(effect->angleVel, ZUN_PI / 100.0f);
    }
}

void Enemy::ResetEffectArray()
{
    for (i32 i = 0; i < this->effectsNum; i++)
    {
        if (!this->effects[i])
        {
            continue;
        }

        this->effects[i]->isFadingOut = 1;
        this->effects[i] = NULL;
    }
    this->effectsNum = 0;
}

void EnemyManager::RunEclTimeline(EclTimeline *timeline)
{
    ZunVec3 pos4;
    ZunVec3 pos3;
    EclTimelineInstrArgs *args4;
    ZunVec3 pos2;
    ZunVec3 pos;
    EclTimelineInstrArgs *args3;
    EclTimelineInstrArgs *args2;
    EclTimelineInstrArgs *args1;
    Enemy *enemy;

    while (timeline->timelineInstr->time >= 0)
    {
        if (timeline->timelineTime == timeline->timelineInstr->time)
        {
            switch (timeline->timelineInstr->opcode)
            {
            case 0:
                if (!g_Gui.BossPresent())
                {
                    args1 = &timeline->timelineInstr->args;
                    g_EnemyManager.SpawnEnemy(timeline->timelineInstr->arg0, args1->AsVec(),
                                              args1->args[3].i, args1->args[4].i, args1->args[5].i,
                                              0);
                }
                break;
            case 1:
                if (!g_Gui.BossPresent())
                {
                    g_EnemyManager.SpawnEnemy(timeline->timelineInstr->arg0,
                                              timeline->timelineInstr->args.AsVec(), -1, -1, -1, 0);
                }
                break;
            case 2:
                if (!g_Gui.BossPresent())
                {
                    args2 = &timeline->timelineInstr->args;
                    enemy = g_EnemyManager.SpawnEnemy(timeline->timelineInstr->arg0, args2->AsVec(),
                                                      args2->args[3].i, args2->args[4].i,
                                                      args2->args[5].i, 1);
                }
                break;
            case 3:
                if (!g_Gui.BossPresent())
                {
                    enemy = g_EnemyManager.SpawnEnemy(timeline->timelineInstr->arg0,
                                                      timeline->timelineInstr->args.AsVec(), -1, -1,
                                                      -1, 1);
                }
                break;
            case 4:
                if (!g_Gui.BossPresent())
                {
                    args3 = &timeline->timelineInstr->args;
                    pos = *args3->AsVec();
                    if (args3->AsVec()->x <= -990.0f)
                    {
                        pos.x = g_Rng.GetRandomFloatInRange(g_GameManager.playerMovementAreaSize.x);
                    }
                    if (args3->AsVec()->y <= -990.0f)
                    {
                        pos.y = g_Rng.GetRandomFloatInRange(g_GameManager.playerMovementAreaSize.y);
                    }
                    if (args3->AsVec()->z <= -990.0f)
                    {
                        pos.z = g_Rng.GetRandomFloatInRange(800.0f);
                    }
                    g_EnemyManager.SpawnEnemy(timeline->timelineInstr->arg0, &pos, args3->args[3].i,
                                              args3->args[4].i, args3->args[5].i, 0);
                }
                break;
            case 5:
                if (!g_Gui.BossPresent())
                {
                    pos2 = *timeline->timelineInstr->args.AsVec();
                    if (pos2.x <= -990.0f)
                    {
                        pos2.x =
                            g_Rng.GetRandomFloatInRange(g_GameManager.playerMovementAreaSize.x);
                    }
                    if (pos2.y <= -990.0f)
                    {
                        pos2.y =
                            g_Rng.GetRandomFloatInRange(g_GameManager.playerMovementAreaSize.y);
                    }
                    if (pos2.z <= -990.0f)
                    {
                        pos2.z = g_Rng.GetRandomFloatInRange(800.0f);
                    }
                    g_EnemyManager.SpawnEnemy(timeline->timelineInstr->arg0, &pos2, -1, -1, -1, 0);
                }
                break;
            case 6:
                if (!g_Gui.BossPresent())
                {
                    args4 = &timeline->timelineInstr->args;
                    pos3 = *args4->AsVec();
                    if (args4->AsVec()->x <= -990.0f)
                    {
                        pos3.x =
                            g_Rng.GetRandomFloatInRange(g_GameManager.playerMovementAreaSize.x);
                    }
                    if (args4->AsVec()->y <= -990.0f)
                    {
                        pos3.y =
                            g_Rng.GetRandomFloatInRange(g_GameManager.playerMovementAreaSize.y);
                    }
                    if (args4->AsVec()->z <= -990.0f)
                    {
                        pos3.z = g_Rng.GetRandomFloatInRange(800.0f);
                    }
                    enemy = g_EnemyManager.SpawnEnemy(timeline->timelineInstr->arg0, &pos3,
                                                      args4->args[3].i, args4->args[4].i,
                                                      args4->args[5].i, 0);
                    enemy->mirror = 1;
                }
                break;
            case 7:
                if (!g_Gui.BossPresent())
                {
                    pos4 = *timeline->timelineInstr->args.AsVec();
                    if (pos4.x <= -990.0f)
                    {
                        pos4.x =
                            g_Rng.GetRandomFloatInRange(g_GameManager.playerMovementAreaSize.x);
                    }
                    if (pos4.y <= -990.0f)
                    {
                        pos4.y =
                            g_Rng.GetRandomFloatInRange(g_GameManager.playerMovementAreaSize.y);
                    }
                    if (pos4.z <= -990.0f)
                    {
                        pos4.z = g_Rng.GetRandomFloatInRange(800.0f);
                    }
                    enemy = g_EnemyManager.SpawnEnemy(timeline->timelineInstr->arg0, &pos4, -1, -1,
                                                      -1, 0);
                    enemy->mirror = 1;
                }
                break;
            case 8:
                g_Gui.MsgRead(timeline->timelineInstr->arg0 + g_GameManager.character * 10);
                break;
            case 9:
                if (g_Gui.MsgWait())
                {
                    timeline->timelineTime--;
                    goto stop;
                }
                break;
            case 10:
                g_EnemyManager.bosses[timeline->timelineInstr->args.args[0].i]->runInterrupt =
                    timeline->timelineInstr->args.args[1].i;
                break;
            case 11:
                g_GameManager.SetCurrentPower(timeline->timelineInstr->arg0);
                g_GameManager.RegenerateGameIntegrityCsum();
                break;
            case 12:
                if (g_EnemyManager.bosses[timeline->timelineInstr->arg0] &&
                    g_EnemyManager.bosses[timeline->timelineInstr->arg0]->active)
                {
                    timeline->timelineTime--;
                    goto stop;
                }
            }
        }
        else if (timeline->timelineTime < timeline->timelineInstr->time)
        {
            break;
        }
        timeline->timelineInstr =
            (EclTimelineInstr *)((u8 *)timeline->timelineInstr + timeline->timelineInstr->size);
    }
stop:
    timeline->timelineTime++;
}

i32 Enemy::HandleLifeCallback()
{
    i32 j;
    i32 i;
    Enemy *enemy;

    enemy = g_EnemyManager.enemies;
    for (i = 0; i < ARRAY_SIZE_SIGNED(enemy->lifeCallbackThreshold); i++)
    {
        if (this->lifeCallbackThreshold[i] < 0)
        {
            continue;
        }

        if (this->life < this->lifeCallbackThreshold[i])
        {
            this->life = this->lifeCallbackThreshold[i];
            g_EclManager.CallEclSub(&this->currentContext, (i16)this->lifeCallbackSub[i]);
            this->lifeCallbackThreshold[i] = -1;
            this->timerCallbackThreshold = -1;
            this->periodicCallbackSub = -1;
            this->bulletRankSpeedLow = -0.5f;
            this->bulletRankSpeedHigh = 0.5f;
            this->bulletRankAmount1Low = 0;
            this->bulletRankAmount1High = 0;
            this->bulletRankAmount2Low = 0;
            this->bulletRankAmount2High = 0;
            this->stackDepth = 0;
            this->bulletProps = g_EnemyManager.enemyTemplate.bulletProps;
            this->shootInterval = 0;
            for (j = 0; j < MAX_ENEMIES; j++, enemy++)
            {
                if (!enemy->active)
                {
                    continue;
                }

                if (enemy->isBoss)
                {
                    continue;
                }

                enemy->life = 0;
                if (!enemy->canDie && enemy->deathCallbackSub >= 0)
                {
                    g_EclManager.CallEclSub(&enemy->currentContext, (i16)enemy->deathCallbackSub);
                    enemy->deathCallbackSub = -1;
                }
            }
            return TRUE;
        }
    }

    return FALSE;
}

i32 Enemy::HandleTimerCallback()
{
    i32 j;
    Enemy *enemy;
    u32 cherryPenalty;
    i32 maxIdx;
    i32 max;
    i32 i;

    if (this->isBoss && this->bossId == 0)
    {
        g_Gui.SetSpellcardSecondsRemaining(
            (this->timerCallbackThreshold - this->timer.GetCurrent()) / 60);
    }
    if (this->timer >= this->timerCallbackThreshold)
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (Stage4ChainRestartPhase(this, 1))
            return 1;
#endif
        max = 0;
        for (i = 0; i < 4; i++)
        {
            if (this->lifeCallbackThreshold[i] < 0)
            {
                continue;
            }
            if (max < this->lifeCallbackThreshold[i])
            {
                max = this->lifeCallbackThreshold[i];
                maxIdx = i;
            }
        }
        if (max > 0)
        {
            this->life = this->lifeCallbackThreshold[maxIdx];
            this->lifeCallbackThreshold[maxIdx] = -1;
        }
        g_EclManager.CallEclSub(&this->currentContext, (i16)this->timerCallbackSub);
        this->timerCallbackThreshold = -1;
        this->timerCallbackSub = this->deathCallbackSub;
        this->timer = 0;
        if (!this->isSurvivalSpellcard)
        {
            g_EnemyManager.spellcardInfo.captureScore = 0;
            g_EnemyManager.spellcardInfo.isCapturing = 0;
            if (g_EnemyManager.spellcardInfo.isActive)
            {
                g_EnemyManager.spellcardInfo.isActive++;
            }
            g_BulletManager.RemoveAllBullets(10);
            cherryPenalty =
                (f32)(g_GameManager.cherry - g_GameManager.globals->cherryStart) * 0.25f;
            cherryPenalty -= (i32)cherryPenalty % 10;
            g_GameManager.cherry -= cherryPenalty;
        }
        enemy = g_EnemyManager.enemies;
        for (j = 0; j < MAX_ENEMIES; j++, enemy++)
        {
            if (!enemy->active)
            {
                continue;
            }

            if (enemy->isBoss)
            {
                continue;
            }

            enemy->life = 0;
            if (!enemy->canDie && enemy->deathCallbackSub >= 0)
            {
                g_EclManager.CallEclSub(&enemy->currentContext, (i16)enemy->deathCallbackSub);
                enemy->deathCallbackSub = -1;
            }
        }
        this->periodicCallbackSub = -1;
        this->bulletProps = g_EnemyManager.enemyTemplate.bulletProps;
        this->shootInterval = 0;
        this->bulletRankSpeedLow = -0.5f;
        this->bulletRankSpeedHigh = 0.5f;
        this->bulletRankAmount1Low = 0;
        this->bulletRankAmount1High = 0;
        this->bulletRankAmount2Low = 0;
        this->bulletRankAmount2High = 0;
        this->stackDepth = 0;
        return TRUE;
    }

    return FALSE;
}

void Enemy::Despawn()
{
    if (this->deathType == ENEMY_DEATH_DESPAWN)
    {
        this->active = 0;
    }
    else
    {
        this->canDie = 0;
    }
    if (this->isBoss && this->bossId < 4)
    {
        g_Gui.bossPresent = 0;
    }
    if (this->effectsNum != 0)
    {
        ResetEffectArray();
    }
    if (this->isBoss)
    {
        g_EnemyManager.bosses[this->bossId] = NULL;
    }
    g_ReplayManager->replayEventFlags |= 0x20;
}

void Enemy::ClampPos()
{
    if (this->hasMovementBounds)
    {
        if (this->pos.x < this->lowerMoveLimit.x)
        {
            this->pos.x = this->lowerMoveLimit.x;
        }
        else if (this->pos.x > this->upperMoveLimit.x)
        {
            this->pos.x = this->upperMoveLimit.x;
        }

        if (this->pos.y < this->lowerMoveLimit.y)
        {
            this->pos.y = this->lowerMoveLimit.y;
        }
        else if (this->pos.y > this->upperMoveLimit.y)
        {
            this->pos.y = this->upperMoveLimit.y;
        }
    }
}

void Enemy::CheckBulletPlayerCollision(ZunVec3 *bulletCenter, ZunVec3 *bulletSize)
{
    ZunVec3 grazeSize;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    bool hitPlayer = false;
#endif

    grazeSize = *bulletSize / 0.7f;
    if (this->isProjectile && this->timer.HasTicked() && this->timer.current % 6 == 0)
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            if (IsPlayerSlotActive(playerId))
                g_Players[playerId].CheckGraze(bulletCenter, &grazeSize);
        }
#else
        g_Player.CheckGraze(bulletCenter, &grazeSize);
#endif
    }
    grazeSize = *bulletSize / 1.5f;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (IsPlayerSlotActive(playerId) &&
            g_Players[playerId].CalcKillboxCollision(bulletCenter, &grazeSize) == 1)
            hitPlayer = true;
    }
    if (hitPlayer && this->canDie && (!this->isBoss && !this->isProjectile))
#else
    if (g_Player.CalcKillboxCollision(bulletCenter, &grazeSize) == 1 && this->canDie &&
        (!this->isBoss && !this->isProjectile))
#endif
    {
        this->life = this->life - 10;
    }
}

u32 EnemyManager::OnUpdate(EnemyManager *arg)
{
    ZunVec3 bossMarkerPos;
    i32 l;
    i32 removedScore;
    i32 k;
    i32 timerLimit;
    ZunVec3 diffToPlayer;
    i32 cherryGain;
    Enemy *enemy;
    i32 playedDamageSound;
    i32 j;
    i32 grazeDamage;
    ZunVec3 currentHitbox;
    f32 angle;
    i32 i;
    i32 damage;
    i32 collisionOut;
    i32 stageFactor;
    ZunVec3 enemyDiff;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    i32 playerDamage[TH07_MULTI_MAX_PLAYERS] = {};
    i32 playerCollision[TH07_MULTI_MAX_PLAYERS] = {};
    i32 damageOwnerId;
    i32 damageTotal;
    i32 damageAttributed;
    i32 contribution;
    Player *targetingPlayer;
#endif

    collisionOut = 0;
    stageFactor = g_GameManager.currentStage >= 5 ? 10 : g_GameManager.currentStage * 2;
    if (!g_Gui.HasCurrentMsgIdx())
    {
        timerLimit = 2400;
        timerLimit -= (i32)g_GameManager.globals->livesRemaining * 4 * 60;
        if (arg->timelineTime.HasTicked() && arg->timelineTime.GetCurrent() % timerLimit == 0)
        {
            g_GameManager.IncreaseSubrank(100);
        }
        g_GameManager.playTimeAll++;
    }
    for (i = 0; i < ARRAY_SIZE_SIGNED(arg->enemyHead); i++)
    {
        arg->enemyHead[i] = NULL;
    }
    for (i = 0; i < g_EclManager.eclFile->timelineCount; i++)
    {
        if (!arg->timelines[i].timelineInstr)
        {
            arg->timelines[i].timelineInstr = g_EclManager.GetTimeline(i);
        }

        if (PracticeRuntime::OverlayTimeLock() && g_GameManager.currentStage == 4 &&
            arg->bosses[0] != nullptr)
        {
            constexpr i32 lilyStart = 7122;
            constexpr i32 lilyLatest = lilyStart + 60 * 50;
            constexpr i32 lilySkipTo = lilyStart + 60 * 10;
            const i32 curTime = arg->timelines[i].timelineTime.GetCurrent();
            if (curTime >= lilyStart && curTime < lilyLatest)
            {
                if (curTime < lilySkipTo)
                    arg->timelines[i].timelineTime.current = lilySkipTo;
                continue;
            }
        }
        RunEclTimeline(&arg->timelines[i]);
    }

    enemy = arg->enemies;
    arg->enemyCountReal = 0;
    for (i = 0; i < 480; i++, enemy++)
    {
        if (!enemy->active)
        {
            continue;
        }
        enemy->UpdatePrev();
        enemy->prevRenderPos = enemy->pos;
        arg->enemyCountReal++;
        if (enemy->freezeEclDuringBombs &&
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            ShouldFreezeEnemyEclForPlayers())
#else
            (g_Player.bombInfo.isInUse || g_Player.playerState != PLAYER_STATE_ALIVE))
#endif
        {
            enemy->timer--;
            goto LAB_00421da7;
        }
    HUH:
        if (g_EclManager.RunEcl(enemy) == ZUN_ERROR)
        {
            enemy->active = 0;
            enemy->Despawn();
            continue;
        }
        if (!enemy->disableMovement)
        {
            enemy->ClampPos();
            enemy->Move();
            enemy->ClampPos();
            if (enemy->specialEffect && !enemy->customSpecialEffectPos)
            {
                enemy->specialEffect->pos =
                    enemy->specialEffect->pos + (enemy->pos - enemy->specialEffect->pos) / 16.0f;
            }
        }
        if (enemy->trailFlags != 0)
        {
            for (j = enemy->trailCount - 1; j > 0; j--)
            {
                enemy->enemyHistory[j].pos = enemy->enemyHistory[j - 1].pos;
                enemy->enemyHistory[j].velocity = enemy->enemyHistory[j - 1].velocity;
                enemy->enemyHistory[j].angle = enemy->enemyHistory[j - 1].angle;
            }
            enemy->enemyHistory[0].pos = enemy->pos;
            enemy->enemyHistory[0].velocity = enemy->velocity;
            enemy->enemyHistory[0].angle = enemy->angle;
        }
        if (!enemy->primaryVm.sprite)
        {
            enemy->hasNoCollision = 1;
        }
        if (!enemy->hasNoCollision && !enemy->isInBounds &&
            g_GameManager.IsInBounds(enemy->pos.x, enemy->pos.y, enemy->primaryVm.sprite->widthPx,
                                     enemy->primaryVm.sprite->heightPx))
        {
            enemy->isInBounds = 1;
        }
        if (enemy->isInBounds == 1 &&
            ((enemy->trailFlags == 0 &&
              !g_GameManager.IsInBounds(enemy->pos.x, enemy->pos.y,
                                        enemy->primaryVm.sprite->widthPx,
                                        enemy->primaryVm.sprite->heightPx)) ||
             (enemy->trailFlags != 0 &&
              !g_GameManager.IsInBounds(enemy->pos.x, enemy->pos.y,
                                        enemy->primaryVm.sprite->widthPx,
                                        enemy->primaryVm.sprite->heightPx) &&
              !g_GameManager.IsInBounds(enemy->enemyHistory[enemy->trailCount - 1].pos.x,
                                        enemy->enemyHistory[enemy->trailCount - 1].pos.y,
                                        enemy->primaryVm.sprite->widthPx,
                                        enemy->primaryVm.sprite->heightPx))) &&
            !enemy->disableOOBDespawn)
        {
            enemy->active = 0;
            enemy->Despawn();
            continue;
        }
        if (enemy->HandleLifeCallback())
        {
            goto HUH;
        }
        if (enemy->timerCallbackThreshold >= 0 && enemy->HandleTimerCallback())
        {
            goto HUH;
        }
        enemy->primaryVm.color.color = enemy->color.color;
        g_AnmManager->ExecuteScript(&enemy->primaryVm);
        enemy->color.color = enemy->primaryVm.color.color;
        for (j = 0; j < ARRAY_SIZE_SIGNED(enemy->vms); j++)
        {
            if (enemy->vms[j].anmFileIdx >= 0 && g_AnmManager->ExecuteScript(enemy->vms + j))
            {
                enemy->vms[j].anmFileIdx = -1;
            }
        }
        collisionOut = 0;
        playedDamageSound = 0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        damage = 0;
        damageOwnerId = 0;
        damageTotal = 0;
        damageAttributed = 0;
#endif
        if (!enemy->hasNoCollision && !enemy->invisibleOnBomb)
        {
            if (enemy->canDie && enemy->hasContactHitbox)
            {
                enemy->CheckBulletPlayerCollision(&enemy->pos, &enemy->hitboxSize);
                if (enemy->trailFlags != 0)
                {
                    currentHitbox = enemy->hitboxSize;
                    for (j = 1; j < enemy->trailInterval; j += 6)
                    {
                        if ((enemy->trailFlags & 2) != 0)
                        {
                            currentHitbox = enemy->hitboxSize - enemy->hitboxSize * (f32)j /
                                                                    (f32)(i32)enemy->trailInterval;
                        }
                        enemy->CheckBulletPlayerCollision(&enemy->enemyHistory[j].pos,
                                                          &currentHitbox);
                    }
                }
            }
            enemy->lastDamage = 0;
            if (enemy->canDie && enemy->isHittable)
            {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                damage = 0;
                collisionOut = 0;
                damageOwnerId = 0;
                for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
                {
                    playerDamage[playerId] = 0;
                    playerCollision[playerId] = 0;
                    if (!IsPlayerSlotActive(playerId))
                        continue;

                    playerDamage[playerId] = g_Players[playerId].CalcDamageToEnemy(
                        &enemy->pos, &enemy->hitboxSize, &playerCollision[playerId]);
                    if (enemy->grazeSize.x > 0.0f)
                    {
                        grazeDamage = g_Players[playerId].CalcDamageToEnemy(
                            &enemy->pos, &enemy->grazeSize, &playerCollision[playerId]);
                        if (playerCollision[playerId] == 0)
                            playerDamage[playerId] = (i32)((f32)playerDamage[playerId] +
                                                           (f32)grazeDamage / 2.5f);
                    }
                    damage += playerDamage[playerId];
                    if (playerCollision[playerId] != 0)
                        collisionOut = playerCollision[playerId];
                    // Keep exact ties with the lower slot, matching upstream.
                    if (playerDamage[playerId] > playerDamage[damageOwnerId])
                        damageOwnerId = playerId;
                }
                // Preserve the raw per-player split for contribution
                // attribution after spell/invulnerability/boss scaling has
                // produced the actual damage applied to the enemy.
                damageTotal = damage;
#else
                damage = g_Player.CalcDamageToEnemy(&enemy->pos, &enemy->hitboxSize, &collisionOut);
                if (enemy->grazeSize.x > 0.0f)
                {
                    grazeDamage =
                        g_Player.CalcDamageToEnemy(&enemy->pos, &enemy->grazeSize, &collisionOut);
                    if (collisionOut == 0)
                    {
                        damage = (i32)((f32)damage + (f32)grazeDamage / 2.5f);
                    }
                }
#endif
                if (damage > 0)
                {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    Player &damageOwner = g_Players[damageOwnerId];
                    if ((enemy->isBoss || !damageOwner.isFocus) && damageOwner.bombInfo.isInUse == 0)
                    {
                        if (enemy->isBoss && !damageOwner.isFocus)
                            cherryGain = damage / (10 - stageFactor / 3) * 10;
                        else
                            cherryGain = damage / (30 - stageFactor) * 10;
                        if (cherryGain > 70)
                            cherryGain = 70;
                        if (cherryGain == 0 &&
                            (damageOwner.isFocus == 0 || (enemy->timer.GetCurrent() & 1) != 0))
                            cherryGain = 10;

                        switch (MultiplayerGameplay::GetPlayerCharacter((u8)damageOwnerId) * 2 +
                                MultiplayerGameplay::GetPlayerShot((u8)damageOwnerId))
                        {
                        default:
                            break;
                        case SHOT_REIMU_A:
                            if ((cherryGain == 20 || cherryGain == 30) &&
                                (enemy->timer.GetCurrent() & 1) != 0)
                                cherryGain -= 10;
                            if (g_GameManager.currentStage >= 5 &&
                                g_GameManager.currentStage <= 6 && !enemy->isBoss)
                                damage = damage / 2;
                            if (g_GameManager.currentStage == 4 && !enemy->isBoss)
                                damage -= damage / 4 + damage / 16;
                        }
                        if (cherryGain != 0)
                            g_GameManager.AddCherryPlusForPlayer(cherryGain, (u8)damageOwnerId);
                    }
#else
                    if ((enemy->isBoss || !g_Player.isFocus) && g_Player.bombInfo.isInUse == 0)
                    {
                        if (enemy->isBoss && !g_Player.isFocus)
                        {
                            cherryGain = damage / (10 - stageFactor / 3) * 10;
                        }
                        else
                        {
                            cherryGain = damage / (30 - stageFactor) * 10;
                        }
                        if (cherryGain > 70)
                        {
                            cherryGain = 70;
                        }
                        if (cherryGain == 0 &&
                            (!g_Player.isFocus || (enemy->timer.GetCurrent() & 1) != 0))
                        {
                            cherryGain = 10;
                        }

                        // ABSOLUTELY no reason for this to be a switch statement
                        switch (g_GameManager.shotTypeAndCharacter)
                        {
                        default:
                            break;
                        case SHOT_REIMU_A:
                            if ((cherryGain == 20 || cherryGain == 30) &&
                                (enemy->timer.GetCurrent() & 1) != 0)
                            {
                                cherryGain -= 10;
                            }
                            if (g_GameManager.currentStage >= STAGE5 &&
                                g_GameManager.currentStage <= STAGE6 && !enemy->isBoss)
                            {
                                damage = damage / 2;
                            }
                            if (g_GameManager.currentStage == STAGE4 && !enemy->isBoss)
                            {
                                damage -= damage / 4 + damage / 16;
                            }
                        }
                        if (cherryGain != 0)
                        {
                            g_GameManager.AddCherryPlus(cherryGain);
                        }
                    }
#endif
                    if (damage >= 70)
                    {
                        damage = 70;
                    }
                    g_GameManager.AddScore(damage / 5 * 10);
                    if (enemy->canBeDamaged)
                    {
                        if (arg->spellcardInfo.isActive)
                        {
                            if (collisionOut == 0)
                            {
                                if (damage > 7)
                                {
                                    damage = damage / 7;
                                }
                                else if (damage != 0)
                                {
                                    damage = 1;
                                }
                            }
                            else if (arg->spellcardInfo.usedBomb)
                            {
                                if (damage > 2)
                                {
                                    damage = (i32)((f32)damage / 2.5f);
                                }
                                else if (damage != 0)
                                {
                                    damage = 1;
                                }
                            }
                            else
                            {
                                damage = 0;
                            }
                        }
                        if (enemy->invincibilityTimer > 0)
                        {
                            if (enemy->isBoss)
                            {
                                damage /= 9;
                            }
                            else
                            {
                                damage = 0;
                            }
                        }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                        // Multiple ships add firepower, but boss phases should
                        // not collapse proportionally faster.  The Stage 4
                        // character chain is already lengthened by fighting
                        // each distinct character card, so those copies retain
                        // the original single-player damage scale.
                        if (enemy->isBoss && !IsStage4ChainedCardActive())
                            damage = (i32)((f32)damage * GetMultiplayerBossDamageMultiplier());

                        if (damage > 0 && damageTotal > 0)
                        {
                            damageAttributed = 0;
                            for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
                            {
                                if (!IsPlayerSlotActive(playerId) || playerDamage[playerId] <= 0)
                                    continue;
                                contribution = (i32)((i64)damage * (i64)playerDamage[playerId] /
                                                     (i64)damageTotal);
                                if (contribution > 0)
                                {
                                    AddPlayerDamageDealt(playerId, (u32)contribution);
                                    damageAttributed += contribution;
                                }
                            }
                            // Integer division can leave a small remainder.
                            // Give it to the same deterministic owner used for
                            // cherry attribution and exact-tie handling.
                            if (damageAttributed < damage)
                                AddPlayerDamageDealt((u8)damageOwnerId,
                                                     (u32)(damage - damageAttributed));
                        }
#endif
                        enemy->life -= damage;
                        enemy->lastDamage = damage;
                    }
                    playedDamageSound = 1;
                }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
                {
                    if (!IsPlayerSlotActive(playerId))
                        continue;
                    targetingPlayer = &g_Players[playerId];
                    if (enemy->isBoss)
                    {
                        diffToPlayer = targetingPlayer->positionOfLastEnemyHit -
                                       targetingPlayer->pos;
                        enemyDiff = enemy->pos - targetingPlayer->pos;
                        if (!targetingPlayer->targetingEnemy ||
                            fabsf(diffToPlayer.x) > fabsf(enemyDiff.x))
                        {
                            targetingPlayer->positionOfLastEnemyHit = enemy->pos;
                        }

                        if (MultiplayerGameplay::GetPlayerCharacter(playerId) == CHAR_SAKUYA)
                        {
                            diffToPlayer = targetingPlayer->sakuyaTargetPosition -
                                           targetingPlayer->pos;
                            angle = atan2f(enemy->pos.y - targetingPlayer->pos.y,
                                           enemy->pos.x - targetingPlayer->pos.x);
                            if (angle >= -2.0943952f && angle <= -1.0471976f &&
                                (!targetingPlayer->targetingEnemy ||
                                 fabsf(diffToPlayer.x) > fabsf(enemyDiff.x)))
                            {
                                targetingPlayer->sakuyaTargetPosition = enemy->pos;
                                targetingPlayer->targetingEnemy = 1;
                            }
                        }
                        else
                        {
                            targetingPlayer->targetingEnemy = 1;
                        }
                    }
                    if (!targetingPlayer->targetingEnemy)
                    {
                        if (targetingPlayer->positionOfLastEnemyHit.y < enemy->pos.y)
                            targetingPlayer->positionOfLastEnemyHit = enemy->pos;
                        if (MultiplayerGameplay::GetPlayerCharacter(playerId) == CHAR_SAKUYA &&
                            targetingPlayer->sakuyaTargetPosition.y < -900.0f)
                        {
                            angle = atan2f(enemy->pos.y - targetingPlayer->pos.y,
                                           enemy->pos.x - targetingPlayer->pos.x);
                            if (angle >= -2.0943952f && angle <= -1.0471976f)
                                targetingPlayer->sakuyaTargetPosition = enemy->pos;
                        }
                    }
                }
#else
                if (enemy->isBoss)
                {
                    diffToPlayer = g_Player.positionOfLastEnemyHit - g_Player.pos;
                    enemyDiff = enemy->pos - g_Player.pos;

                    if (!g_Player.targetingEnemy || fabsf(diffToPlayer.x) > fabsf(enemyDiff.x))
                    {
                        g_Player.positionOfLastEnemyHit = enemy->pos;
                    }

                    if (g_GameManager.character == CHAR_SAKUYA)
                    {
                        diffToPlayer = g_Player.sakuyaTargetPosition - g_Player.pos;
                        angle =
                            atan2f(enemy->pos.y - g_Player.pos.y, enemy->pos.x - g_Player.pos.x);

                        if (angle >= -ZUN_2PI / 3.0f && angle <= -ZUN_PI / 3.0f &&
                            (!g_Player.targetingEnemy ||
                             fabsf(diffToPlayer.x) > fabsf(enemyDiff.x)))
                        {
                            g_Player.sakuyaTargetPosition = enemy->pos;
                            g_Player.targetingEnemy = TRUE;
                        }
                    }
                    else
                    {
                        g_Player.targetingEnemy = TRUE;
                    }
                }
                if (!g_Player.targetingEnemy)
                {
                    if (g_Player.positionOfLastEnemyHit.y < enemy->pos.y)
                    {
                        g_Player.positionOfLastEnemyHit = enemy->pos;
                    }
                    if (g_GameManager.character == CHAR_SAKUYA &&
                        g_Player.sakuyaTargetPosition.y < -900.0f)
                    {
                        angle =
                            atan2f(enemy->pos.y - g_Player.pos.y, enemy->pos.x - g_Player.pos.x);
                        if (angle >= -ZUN_2PI / 3.0f && angle <= -ZUN_PI / 3.0f)
                        {
                            g_Player.sakuyaTargetPosition = enemy->pos;
                        }
                    }
                }
#endif
            }
        }
        if (enemy->life <= 0 && enemy->canDie)
        {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            if (enemy->deathType != 3 && enemy->canBeDamaged && damage > 0)
                AddPlayerEnemiesDefeated((u8)damageOwnerId, 1);
#endif
            for (k = 0; k < 4; k++)
            {
                enemy->lifeCallbackThreshold[k] = -1;
            }
            enemy->timerCallbackThreshold = -1;
            enemy->periodicCallbackSub = -1;

            switch (enemy->deathType)
            {
            case ENEMY_DEATH_BOSS:
                enemy->life = 1;
                enemy->canBeDamaged = 0;
                enemy->deathType = ENEMY_DEATH_DESPAWN;
                g_Gui.bossPresent = 0;
                g_ReplayManager->replayEventFlags |= 0x20;
                if (enemy->deathAnm1 >= 0)
                {
                    g_EffectManager.SpawnEffect(enemy->deathAnm1, &enemy->pos, 1, 0xffffffff);
                    g_EffectManager.SpawnEffect(enemy->deathAnm1, &enemy->pos, 1, 0xffffffff);
                    g_EffectManager.SpawnEffect(enemy->deathAnm1, &enemy->pos, 1, 0xffffffff);
                }
                break;
            case ENEMY_DEATH_SCORE_ONLY:
                g_GameManager.AddScore(enemy->score);
                enemy->canDie = 0;
                goto END_BOSS;
            case ENEMY_DEATH_DESPAWN:
                g_GameManager.AddScore(enemy->score);
                enemy->active = 0;
                goto END_BOSS;
            END_BOSS:
                if (enemy->isBoss)
                {
                    g_Gui.bossPresent = 0;
                    enemy->ResetEffectArray();
                }
            case 2:
                if (enemy->itemDrop >= 0)
                {
                    g_EffectManager.SpawnEffect(enemy->deathAnm2 + 4, &enemy->pos, 3,
                                                   0xffffffff);
                    g_ItemManager.SpawnEnemyDrop(&enemy->pos, enemy->itemDrop, collisionOut);
                }
                else if (enemy->itemDrop == -1)
                {
                    if ((i32)arg->randomItemSpawnIdx % 3 == 0)
                    {
                        g_EffectManager.SpawnEffect(enemy->deathAnm2 + 4, &enemy->pos, 6,
                                                       0xffffffff);
                        g_ItemManager.SpawnEnemyDrop(
                            &enemy->pos, g_ItemDropTable[arg->randomItemTableIdx], collisionOut);
                        arg->randomItemTableIdx++;
                        if (arg->randomItemTableIdx >= 32)
                        {
                            arg->randomItemTableIdx = 0;
                        }
                    }
                    arg->randomItemSpawnIdx++;
                }
                if (enemy->isBoss && !g_EnemyManager.spellcardInfo.isActive)
                {
                    removedScore = g_BulletManager.DespawnBullets(8000, TRUE);
                    removedScore = g_EnemyManager.RemoveAllEnemies(8000, removedScore);
                    if (removedScore != 0)
                    {
                        g_GameManager.AddScore(removedScore);
                        g_Gui.ShowBonusScore(removedScore);
                    }
                }
                enemy->life = 0;
                g_ReplayManager->replayEventFlags |= 0x20;
                break;
            }

            g_SoundPlayer.PlaySoundByIdx(i % 2 + 2, 0);
            if (enemy->deathAnm1 >= 0)
            {
                g_EffectManager.SpawnEffect(enemy->deathAnm1, &enemy->pos, 1, 0xffffffff);
                g_EffectManager.SpawnEffect(enemy->deathAnm2 + 4, &enemy->pos, 4, 0xffffffff);
            }
            if (enemy->deathCallbackSub >= 0)
            {
                enemy->bulletRankSpeedLow = -0.5f;
                enemy->bulletRankSpeedHigh = 0.5f;
                enemy->bulletRankAmount1Low = 0;
                enemy->bulletRankAmount1High = 0;
                enemy->bulletRankAmount2Low = 0;
                enemy->bulletRankAmount2High = 0;
                enemy->stackDepth = 0;
                for (l = 0; l < ARRAY_SIZE_SIGNED(enemy->lifeCallbackThreshold); l++)
                {
                    enemy->lifeCallbackThreshold[l] = -1;
                }
                enemy->timerCallbackThreshold = -1;
                enemy->periodicCallbackSub = -1;
                enemy->bulletProps = g_EnemyManager.enemyTemplate.bulletProps;
                enemy->shootInterval = 0;
                g_EclManager.CallEclSub(&enemy->currentContext, (i16)enemy->deathCallbackSub);
                enemy->deathCallbackSub = -1;
            }
        }

    LAB_00421da7:
        if (enemy->damageTintTimer != 0)
        {
            enemy->damageTintTimer--;
            enemy->primaryVm.useColor2 = 0;
        }
        else if (playedDamageSound != 0)
        {
            g_SoundPlayer.PlaySoundByIdx(SOUND_ENEMY_DAMAGED, 0);
            enemy->primaryVm.color2.bytes.r = 255;
            enemy->primaryVm.color2.bytes.g = 128;
            enemy->primaryVm.color2.bytes.b = 192;
            enemy->primaryVm.color2.bytes.a = enemy->primaryVm.color.bytes.a;
            enemy->primaryVm.useColor2 = 1;
            enemy->damageTintTimer = 1;
        }
        else
        {
            enemy->primaryVm.useColor2 = 0;
        }
        if (enemy->isBoss)
        {
            if (!g_Gui.HasCurrentMsgIdx() && enemy->bossId == 0)
            {
                g_Gui.SetBossHealthBar((f32)enemy->life / (f32)enemy->maxLife);
            }

            if (enemy->bossId < 4)
            {
                g_AsciiManager.SetBossDamageTint(enemy->bossId, enemy->primaryVm.useColor2);
            }
        }
        enemy->UpdateEffects();
        if (!g_GameManager.isTimeStopped && !PracticeRuntime::OverlayTimeLock())
        {
            enemy->timer++;
        }
        if (enemy->invincibilityTimer > 0)
        {
            enemy->invincibilityTimer--;
        }
        if (!enemy->hasNoCollision && enemy->active)
        {
            enemy->next = arg->enemyHead[enemy->zLayer];
            arg->enemyHead[enemy->zLayer] = enemy;
        }
    }

    if (arg->timelineTime.current % 200 == 0 && g_GameManager.CheckGameIntegrity())
    {
        return CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS;
    }
    arg->timelineTime++;
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

f32 AngleLerp(f32 start, f32 target, f32 t)
{
    f32 direct;
    f32 wrapped;

    if (start < target)
    {
        direct = target - start;
        wrapped = start + ZUN_2PI - target;
    }
    else
    {
        direct = start - target;
        wrapped = target + ZUN_2PI - start;
        start = target;
    }
    if (direct < wrapped)
    {
        return direct * t + start;
    }
    return wrapped * t + start;
}

u32 EnemyManager::ActualOnDraw(EnemyManager *arg, i32 first, i32 last)
{
    f32 uvDiff;
    f32 cosAngle;
    f32 angle1;
    f32 uvStep;
    VertexTex1DiffuseXyzrhw *trailVert;
    f32 prevAngle;
    f32 currentUvX;
    f32 sinAngle;
    i32 vertexCount;
    f32 xOffset;
    f32 yOffset;
    Enemy *enemy;
    i32 j;
    AnmVm *vm;
    ZunColor baseColor;
    i32 i;
    Float2 scale;

    auto getHistoryPos = [](EnemyHistory *history, i32 idx, i32 maxCount) {
        ZunVec3 cur = history[idx].pos;
        ZunVec3 prev = cur;
        if (idx + 1 < maxCount && history[idx + 1].pos.x >= -990.0f)
        {
            prev = history[idx + 1].pos;
        }
        return prev.Lerp(cur, g_RenderAlpha);
    };

    auto getHistoryAngle = [](EnemyHistory *history, i32 idx, i32 maxCount) {
        f32 cur = history[idx].angle;
        f32 prev = cur;
        if (idx + 1 < maxCount && history[idx + 1].pos.x >= -990.0f)
        {
            prev = history[idx + 1].angle;
        }
        return utils::LerpAngle(prev, cur, g_RenderAlpha);
    };

    for (i = first; i < last; i++)
    {
        enemy = arg->enemyHead[i];
        while (enemy)
        {
            ZunVec3 drawPos = enemy->prevRenderPos.Lerp(enemy->pos, g_RenderAlpha);
            vm = &enemy->vms[0];
            for (j = 0; j < 1; j++, vm++)
            {
                if (vm->anmFileIdx >= 0)
                {
                    if (vm->autoRotate)
                    {
                        vm->SetRotationZ(enemy->angle);
                        vm->updateRotation = 1;
                    }

                    vm->pos = drawPos + vm->offset;
                    vm->pos.z = 0.3f;
                    vm->pos.x += g_GameManager.arcadeRegionTopLeftPos.x;
                    vm->pos.y += g_GameManager.arcadeRegionTopLeftPos.y;

                    g_AnmManager->Draw(vm);
                }
            }

            if (enemy->primaryVmAutoRotate)
            {
                enemy->primaryVm.SetRotationZ(enemy->angle);
                enemy->primaryVm.updateRotation = 1;
            }
            enemy->primaryVm.pos = drawPos + enemy->primaryVm.offset;
            enemy->primaryVm.pos.z = 0.29f;
            if ((enemy->trailFlags & 16) == 0 && !enemy->invisibleOnBomb)
            {
                enemy->primaryVm.pos.x += g_GameManager.arcadeRegionTopLeftPos.x;
                enemy->primaryVm.pos.y += g_GameManager.arcadeRegionTopLeftPos.y;
                g_AnmManager->Draw(&enemy->primaryVm);
            }

            for (j = 1; j < 2; j++, vm++)
            {
                if (vm->anmFileIdx >= 0)
                {
                    if (vm->autoRotate)
                    {
                        vm->SetRotationZ(-enemy->angle);
                        vm->updateRotation = 1;
                    }
                    vm->pos = drawPos + vm->offset;
                    vm->pos.z = 0.3f;
                    vm->pos.x += g_GameManager.arcadeRegionTopLeftPos.x;
                    vm->pos.y += g_GameManager.arcadeRegionTopLeftPos.y;
                    g_AnmManager->Draw(vm);
                }
            }

            if (enemy->trailFlags != 0)
            {
                scale = enemy->primaryVm.scale;

                baseColor = ZunColor::Lerp(enemy->primaryVm.prevColor, enemy->primaryVm.color,
                                           g_RenderAlpha);

                ZunColor origColor = enemy->primaryVm.color;
                Float2 origPrevScale = enemy->primaryVm.prevScale;
                ZunColor origPrevColor = {enemy->primaryVm.prevColor.color};

                if ((enemy->trailFlags & 8) == 0)
                {
                    for (j = enemy->trailNodeStep; j < enemy->trailCount; j += enemy->trailNodeStep)
                    {
                        if (enemy->enemyHistory[j].pos.x < -990.0f)
                        {
                            continue;
                        }

                        if (enemy->primaryVmAutoRotate)
                        {
                            enemy->primaryVm.SetRotationZ(
                                getHistoryAngle(enemy->enemyHistory, j, enemy->trailCount));
                            enemy->primaryVm.prevRotation.z = enemy->primaryVm.rotation.z;
                            enemy->primaryVm.updateRotation = 1;
                        }
                        if ((enemy->trailFlags & 2) != 0)
                        {
                            enemy->primaryVm.scale.x =
                                scale.x - (f32)j * scale.x / (f32)enemy->trailCount;
                            enemy->primaryVm.prevScale.x = enemy->primaryVm.scale.x;
                        }
                        if ((enemy->trailFlags & 4) != 0)
                        {
                            enemy->primaryVm.color.bytes.a =
                                baseColor.bytes.a - baseColor.bytes.a * j / enemy->trailCount;
                            enemy->primaryVm.prevColor.bytes.a = enemy->primaryVm.color.bytes.a;
                        }
                        enemy->primaryVm.pos =
                            getHistoryPos(enemy->enemyHistory, j, enemy->trailCount) +
                            enemy->primaryVm.offset;
                        enemy->primaryVm.pos.z = 0.3f;
                        enemy->primaryVm.pos.x += g_GameManager.arcadeRegionTopLeftPos.x;
                        enemy->primaryVm.pos.y += g_GameManager.arcadeRegionTopLeftPos.y;
                        g_AnmManager->Draw(&enemy->primaryVm);
                    }
                }
                else
                {
                    vertexCount = 0;

                    for (j = 0; j < enemy->trailCount; j += enemy->trailNodeStep)
                    {
                        if (enemy->enemyHistory[j].pos.x < -990.0f)
                        {
                            break;
                        }
                        vertexCount += 2;
                    }
                    if (vertexCount > 2)
                    {
                        uvDiff =
                            enemy->primaryVm.sprite->uvEnd.x - enemy->primaryVm.sprite->uvStart.x;
                        i32 numSegments = (vertexCount / 2) - 1;
                        uvStep = (numSegments > 0) ? (uvDiff / (f32)numSegments) : 0.0f;
                        Float2 drawUv = enemy->primaryVm.prevUvScrollPos.LerpUv(
                            enemy->primaryVm.uvScrollPos, g_RenderAlpha);

                        currentUvX = enemy->primaryVm.sprite->uvEnd.x + drawUv.x;
                        trailVert = enemy->trailVertices;

                        for (j = 0; j < enemy->trailCount;
                             j += enemy->trailNodeStep, currentUvX -= uvStep)
                        {
                            if (enemy->enemyHistory[j].pos.x < -990.0f)
                            {
                                break;
                            }

                            if (j == 0)
                            {
                                angle1 = getHistoryAngle(enemy->enemyHistory, 0, enemy->trailCount);
                            }
                            else
                            {
                                angle1 = AngleLerp(
                                    getHistoryAngle(enemy->enemyHistory, j - 1, enemy->trailCount),
                                    getHistoryAngle(enemy->enemyHistory, j, enemy->trailCount),
                                    0.5f);
                            }

                            if ((enemy->trailFlags & 2) != 0 && j > 0 &&
                                j + enemy->trailNodeStep < enemy->trailCount)
                            {
                                sinAngle = AngleLerp(getHistoryAngle(enemy->enemyHistory,
                                                                     j + enemy->trailNodeStep - 1,
                                                                     enemy->trailCount),
                                                     getHistoryAngle(enemy->enemyHistory,
                                                                     j + enemy->trailNodeStep,
                                                                     enemy->trailCount),
                                                     0.5f);
                                if (fabsf(prevAngle - angle1) < 0.00001f &&
                                    fabsf(angle1 - sinAngle) < 0.00001f)
                                {
                                    vertexCount -= 2;
                                    continue;
                                }
                            }

                            prevAngle = angle1;
                            sinAngle = sinf(angle1);
                            cosAngle = cosf(angle1);
                            xOffset = 0.0f;
                            yOffset = scale.y * enemy->primaryVm.sprite->heightPx / 2.0f;

                            if ((enemy->trailFlags & 2) != 0)
                            {
                                angle1 = 1.0f - (f32)j / (f32)enemy->trailCount;
                                xOffset *= angle1;
                                yOffset *= angle1;
                            }

                            trailVert[1].diffuse.color = enemy->primaryVm.color.color;
                            trailVert[0].diffuse.color = trailVert[1].diffuse.color;

                            if ((enemy->trailFlags & 4) != 0)
                            {
                                trailVert[1].diffuse.bytes.a =
                                    baseColor.bytes.a - baseColor.bytes.a * j / enemy->trailCount;
                                trailVert[0].diffuse.bytes.a = trailVert[1].diffuse.bytes.a;
                            }

                            ZunVec3 drawHistPos =
                                getHistoryPos(enemy->enemyHistory, j, enemy->trailCount);

                            trailVert[0].pos = drawHistPos;
                            trailVert[0].pos.x += cosAngle * xOffset - sinAngle * yOffset + 32.0f;
                            trailVert[0].pos.y += sinAngle * xOffset + cosAngle * yOffset + 16.0f;
                            trailVert[0].textureUV.x = currentUvX;
                            trailVert[0].textureUV.y =
                                enemy->primaryVm.sprite->uvStart.y + drawUv.y;
                            trailVert++;

                            trailVert[0].pos = drawHistPos;
                            trailVert[0].pos.x += cosAngle * xOffset + sinAngle * yOffset + 32.0f;
                            trailVert[0].pos.y += sinAngle * xOffset - cosAngle * yOffset + 16.0f;
                            trailVert[0].textureUV.x = currentUvX;
                            trailVert[0].textureUV.y = enemy->primaryVm.sprite->uvEnd.y + drawUv.y;
                            trailVert++;
                        }
                        if (vertexCount > 2)
                        {
                            g_AnmManager->DrawTriangleStrip(&enemy->primaryVm, enemy->trailVertices,
                                                            vertexCount);
                        }
                    }
                }
                enemy->primaryVm.scale = scale;
                enemy->primaryVm.color = origColor;
                enemy->primaryVm.prevScale = origPrevScale;
                enemy->primaryVm.prevColor = origPrevColor;
            }
            enemy = enemy->next;
        }
    }
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

u32 EnemyManager::OnDraw1(EnemyManager *arg)
{
    return ActualOnDraw(arg, 0, 2);
}

u32 EnemyManager::OnDraw2(EnemyManager *arg)
{
    return ActualOnDraw(arg, 2, 4);
}

ZunResult EnemyManager::AddedCallback(EnemyManager *arg)
{
    if (arg->stgEnmAnmFilename && g_AnmManager->LoadAnms(ANM_FILE_ENEMY, arg->stgEnmAnmFilename,
                                                         ANM_OFFSET_ENEMY) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }
    if (arg->stgEnm2AnmFilename && g_AnmManager->LoadAnms(ANM_FILE_ENEMY2, arg->stgEnm2AnmFilename,
                                                          ANM_OFFSET_ENEMY) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    arg->randomItemSpawnIdx = g_Rng.GetRandomU16InRange(3);
    arg->randomItemTableIdx = g_Rng.GetRandomU16InRange(8);
    arg->spellcardInfo.isActive = 0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    ResetStage4BossChain();
#endif

    ZunVec3 vec = ZunVec3(-999.0f, -999.0f, -999.0f);
    g_AsciiManager.GetBossMarker(0)->pos = vec;
    g_AsciiManager.GetBossMarker(1)->pos = vec;
    g_AsciiManager.GetBossMarker(2)->pos = vec;
    g_AsciiManager.GetBossMarker(3)->pos = vec;
    return ZUN_SUCCESS;
}

ZunResult EnemyManager::DeletedCallback(EnemyManager *arg)
{
    g_AnmManager->ReleaseAnm(ANM_FILE_ENEMY2);
    g_AnmManager->ReleaseAnm(ANM_FILE_ENEMY);
    ZunVec3 vec = ZunVec3(-999.0f, -999.0f, -999.0f);
    g_AsciiManager.GetBossMarker(0)->pos = vec;
    g_AsciiManager.GetBossMarker(1)->pos = vec;
    g_AsciiManager.GetBossMarker(2)->pos = vec;
    g_AsciiManager.GetBossMarker(3)->pos = vec;
    return ZUN_SUCCESS;
}

ZunResult EnemyManager::RegisterChain(const char *stgEnm1, const char *stgEnm2)
{
    EnemyManager *mgr = &g_EnemyManager;
    mgr->Initialize();
    mgr->stgEnmAnmFilename = stgEnm1;
    mgr->stgEnm2AnmFilename = stgEnm2;
    g_EnemyManagerCalcChain.callback = (ChainCallback)OnUpdate;
    g_EnemyManagerCalcChain.addedCallback = NULL;
    g_EnemyManagerCalcChain.deletedCallback = NULL;
    g_EnemyManagerCalcChain.addedCallback = (ChainLifecycleCallback)AddedCallback;
    g_EnemyManagerCalcChain.deletedCallback = (ChainLifecycleCallback)DeletedCallback;
    g_EnemyManagerCalcChain.arg = mgr;
    if (g_Chain.AddToCalcChain(&g_EnemyManagerCalcChain, 10))
    {
        return ZUN_ERROR;
    }

    g_EnemyManagerDrawChain1.callback = (ChainCallback)OnDraw1;
    g_EnemyManagerDrawChain1.addedCallback = NULL;
    g_EnemyManagerDrawChain1.deletedCallback = NULL;
    g_EnemyManagerDrawChain1.arg = mgr;
    if (g_Chain.AddToDrawChain(&g_EnemyManagerDrawChain1, 5) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    g_EnemyManagerDrawChain2.callback = (ChainCallback)OnDraw2;
    g_EnemyManagerDrawChain2.addedCallback = NULL;
    g_EnemyManagerDrawChain2.deletedCallback = NULL;
    g_EnemyManagerDrawChain2.arg = mgr;
    if (g_Chain.AddToDrawChain(&g_EnemyManagerDrawChain2, 7) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    return ZUN_SUCCESS;
}

void EnemyManager::CutChain()
{
    g_Chain.Cut(&g_EnemyManagerCalcChain);
    g_Chain.Cut(&g_EnemyManagerDrawChain1);
    g_Chain.Cut(&g_EnemyManagerDrawChain2);
}

i32 EnemyManager::RemoveAllEnemies(i32 scoreMax, i32 scoreMin)
{
    i32 j;
    i32 i;
    Enemy *enemy;
    i32 totalScore;
    i32 popupScore;

    enemy = this->enemies;
    totalScore = scoreMin;
    popupScore = 2000;
    for (i = 0; i < MAX_ENEMIES; i++, enemy++)
    {
        if (!enemy->active)
        {
            continue;
        }

        if (enemy->isBoss)
        {
            continue;
        }

        enemy->life = 0;
        if (enemy->isProjectile)
        {
            g_ItemManager.SpawnItem(&enemy->pos, ITEM_POINT_BULLET, 1);
            g_AsciiManager.CreatePopup1(&enemy->pos, popupScore,
                                        popupScore >= scoreMax ? 0xffffff00 : 0xffffffff);
            totalScore += popupScore;
            popupScore += 30;
            if (popupScore > scoreMax)
            {
                popupScore = scoreMax;
            }
            if (enemy->trailFlags != 0)
            {
                for (j = 0; j < enemy->trailCount; j += 6)
                {
                    g_ItemManager.SpawnItem(&enemy->enemyHistory[j].pos, ITEM_POINT_BULLET, 1);
                    g_AsciiManager.CreatePopup1(&enemy->enemyHistory[j].pos, popupScore,
                                                popupScore >= scoreMax ? 0xffffff00 : 0xffffffff);
                    totalScore += popupScore;
                    popupScore += 30;
                    if (popupScore > scoreMax)
                    {
                        popupScore = scoreMax;
                    }
                }
            }
        }
        if (!enemy->canDie && enemy->deathCallbackSub >= 0)
        {
            g_EclManager.CallEclSub(&enemy->currentContext, (i16)enemy->deathCallbackSub);
            enemy->deathCallbackSub = -1;
        }
    }
    return totalScore;
}

i32 EnemyManager::HasActiveBoss()
{
    for (i32 i = 0; i < ARRAY_SIZE_SIGNED(this->bosses); i++)
    {
        if (this->bosses[i])
        {
            return 1;
        }
    }
    return 0;
}
