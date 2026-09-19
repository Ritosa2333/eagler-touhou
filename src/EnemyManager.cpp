#include "EnemyManager.hpp"
#include "AnmManager.hpp"
#include "BulletManager.hpp"
#include "Chain.hpp"
#include "ChainPriorities.hpp"
#include "EffectManager.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "Gui.hpp"
#include "Player.hpp"
#include "PracticeRuntime.hpp"
#include "Rng.hpp"
#include "utils.hpp"
#ifdef TH_ENABLE_NETPLAY
#include "netplay/Th06RollbackState.hpp"
#endif

#define ITEM_SPAWNS 3
#define ITEM_TABLES 8

EnemyManager g_EnemyManager;
static ChainElem g_EnemyManagerCalcChain;
static ChainElem g_EnemyManagerDrawChain;
static const u8 g_RandomItems[32] = {
    ITEM_POWER_SMALL, ITEM_POWER_SMALL, ITEM_POINT,       ITEM_POWER_SMALL, ITEM_POINT,       ITEM_POWER_SMALL,
    ITEM_POWER_SMALL, ITEM_POINT,       ITEM_POINT,       ITEM_POINT,       ITEM_POWER_SMALL, ITEM_POWER_SMALL,
    ITEM_POWER_SMALL, ITEM_POINT,       ITEM_POINT,       ITEM_POWER_SMALL, ITEM_POINT,       ITEM_POWER_SMALL,
    ITEM_POINT,       ITEM_POWER_SMALL, ITEM_POINT,       ITEM_POWER_SMALL, ITEM_POINT,       ITEM_POWER_SMALL,
    ITEM_POINT,       ITEM_POWER_SMALL, ITEM_POWER_SMALL, ITEM_POINT,       ITEM_POINT,       ITEM_POINT,
    ITEM_POWER_SMALL, ITEM_POWER_BIG};

#pragma var_order(i, enemy)
void EnemyManager::Initialize()
{
    i32 i;
    Enemy *enemy;

    enemy = &this->enemies[0];
    memset(this, 0, sizeof(EnemyManager));
    enemy = &this->enemyTemplate;
    memset(enemy, 0, sizeof(Enemy));
    for (i = 0; i < ARRAY_SIZE_SIGNED(this->enemyTemplate.vms); i++)
    {
        enemy->vms[i].anmFileIndex = -1;
    }
    enemy->flags.isSlotOccupied = 1;
    enemy->bossTimer.InitializeForPopup();
    enemy->flags.isInteractable = 1;
    enemy->flags.isCollidable = 1;
    enemy->flags.hasBeenInBounds = 0;
    enemy->hitboxDimensions = ZunVec3(12.0f, 12.0f, 12.0f);
    enemy->axisSpeed = ZunVec3(0.0f, 0.0f, 0.0f);
    enemy->angularVelocity = 0.0f;
    enemy->angle = 0.0f;
    enemy->prevAngle = enemy->angle;
    enemy->acceleration = 0.0f;
    enemy->speed = 0.0f;
    enemy->flags.movementMode = 0;
    enemy->flags.shootingDisabled = 0;
    enemy->flags.invertX = 0;
    enemy->flags.isBoss = 0;
    enemy->stackDepth = 0;
    enemy->life = 1;
    enemy->score = 100;
    enemy->deathAnm1 = 0;
    enemy->deathAnm2 = 0;
    enemy->deathAnm3 = 0;
    enemy->shootInterval = 0;
    enemy->shootIntervalTimer.InitializeForPopup();
    enemy->shootOffset = ZunVec3(0.0f, 0.0f, 0.0f);
    enemy->anmExLeft = -1;
    enemy->anmExRight = -1;
    enemy->anmExDefaults = -1;
    enemy->flags.isDamageable = 1;
    enemy->flags.deathMode = 0;
    enemy->deathCallbackSub = -1;
    enemy->flags.shouldClampPos = 0;
    enemy->effectIdx = 0;
    enemy->runInterrupt = -1;
    enemy->lifeCallbackThreshold = -1;
    enemy->timerCallbackThreshold = -1;
    enemy->laserStore = 0;
    enemy->unk_e41 = 0;
    enemy->flags.rotateAnm = 0;
    enemy->bulletRankSpeedLow = -0.5f;
    enemy->bulletRankSpeedHigh = 0.5f;
}

EnemyManager::EnemyManager()
{
    this->Initialize();
}

Enemy *EnemyManager::SpawnEnemy(i32 eclSubId, const ZunVec3 *pos, i16 life, i16 itemDrop, i32 score)
{
    Enemy *newEnemy;
    i32 idx;

    newEnemy = this->enemies;
    idx = 0;
    for (; idx < ARRAY_SIZE_SIGNED(this->enemies) - 1; idx++, newEnemy++)
    {
        if (newEnemy->flags.isSlotOccupied)
            continue;

#ifdef TH_ENABLE_NETPLAY
        if (!Netplay::Th06Rollback::TouchEnemy(newEnemy))
            return nullptr;
#endif

        *newEnemy = this->enemyTemplate;

        if (0 <= life)
            newEnemy->life = life;

        newEnemy->position = *pos;
        newEnemy->prevPosition = newEnemy->position;
        newEnemy->prevAngle = newEnemy->angle;
        newEnemy->primaryVm.UpdatePrev();
        for (AnmVm &vm : newEnemy->vms)
        {
            vm.UpdatePrev();
        }
        g_EclManager.CallEclSub(&newEnemy->currentContext, eclSubId);
        g_EclManager.RunEcl(newEnemy);
        // ECL initialization can immediately change position, angle, and ANM state.
        // Treat that initialized state as both endpoints so a newly spawned enemy
        // cannot interpolate in from the reusable template's stale values.
        newEnemy->prevPosition = newEnemy->position;
        newEnemy->prevAngle = newEnemy->angle;
        newEnemy->primaryVm.UpdatePrev();
        for (AnmVm &vm : newEnemy->vms)
        {
            vm.UpdatePrev();
        }
        newEnemy->color = newEnemy->primaryVm.color;
        newEnemy->itemDrop = itemDrop;

        if (0 <= life)
            newEnemy->life = life;

        if (0 <= score)
            newEnemy->score = score;

        newEnemy->maxLife = newEnemy->life;
        break;
    }
    return newEnemy;
}

void Enemy::ResetEffectArray(Enemy *enemy)
{
    i32 idx;

    for (idx = 0; idx < enemy->effectIdx; idx++)
    {
        if (!enemy->effectArray[idx])
        {
            continue;
        }
        enemy->effectArray[idx]->unk_17a = 1;
        enemy->effectArray[idx] = NULL;
    }
    enemy->effectIdx = 0;
}

void EnemyManager::RunEclTimeline()
{
    ZunVec3 pos4;
    ZunVec3 pos3;
    ZunVec3 pos2;
    ZunVec3 pos1;
    const EclTimelineInstrArgs *args4;
    const EclTimelineInstrArgs *args3;
    const EclTimelineInstrArgs *args2;
    const EclTimelineInstrArgs *args1;
    i32 subrankIncreaseFrame;
    Enemy *spawnedEnemy;
    ZunVec3 tmpVec3;

    if (this->timelineInstr == NULL)
    {
        this->timelineInstr = g_EclManager.timeline;
    }
    if (g_Gui.HasCurrentMsgIdx() == 0)
    {
        // Unclear what this is? It looks like it increases the subrank at
        // regular intervals, where the interval is made shorter based on the
        // number of lives lost?
        subrankIncreaseFrame = 10 * 4 * 60;
        subrankIncreaseFrame -= g_GameManager.livesRemaining * 4 * 60;
        if (this->timelineTime.HasTicked() && this->timelineTime.AsFrames() % subrankIncreaseFrame == 0)
        {
            g_GameManager.IncreaseSubrank(100);
        }
    }
    while (0 <= this->timelineInstr->time)
    {
        if (this->timelineTime.current == this->timelineInstr->time)
        {
            switch (this->timelineInstr->opCode)
            {
            case 0:
                if (!g_Gui.BossPresent())
                {
                    args1 = &this->timelineInstr->args;
                    tmpVec3 = *args1->Var1AsVec();
                    this->SpawnEnemy(this->timelineInstr->arg0, &tmpVec3, args1->ushortVar1, args1->ushortVar2,
                                     args1->uintVar4);
                }
                break;
            case 1:
                if (!g_Gui.BossPresent())
                {
                    tmpVec3 = *this->timelineInstr->args.Var1AsVec();
                    this->SpawnEnemy(this->timelineInstr->arg0, &tmpVec3, -1, ITEM_RANDOM_ITEM, -1);
                }
                break;
            case 2:
                if (!g_Gui.BossPresent())
                {
                    args2 = &this->timelineInstr->args;
                    tmpVec3 = *args2->Var1AsVec();
                    spawnedEnemy = this->SpawnEnemy(this->timelineInstr->arg0, &tmpVec3, args2->ushortVar1,
                                                    args2->ushortVar2, args2->uintVar4);
                    spawnedEnemy->flags.invertX = 1;
                }
                break;
            case 3:
                if (!g_Gui.BossPresent())
                {
                    tmpVec3 = *this->timelineInstr->args.Var1AsVec();
                    spawnedEnemy = this->SpawnEnemy(this->timelineInstr->arg0, &tmpVec3, -1, ITEM_RANDOM_ITEM, -1);
                    spawnedEnemy->flags.invertX = 1;
                }
                break;
            case 4:
                if (!g_Gui.BossPresent())
                {
                    args3 = &this->timelineInstr->args;
                    pos1 = *args3->Var1AsVec();
                    if (args3->Var1AsVec()->x <= -990.0f)
                    {
                        pos1.x = g_Rng.GetRandomF32InRange(g_GameManager.playerMovementAreaSize.x);
                    }
                    if (args3->Var1AsVec()->y <= -990.0f)
                    {
                        pos1.y = g_Rng.GetRandomF32InRange(g_GameManager.playerMovementAreaSize.y);
                    }
                    if (args3->Var1AsVec()->z <= -990.0f)
                    {
                        pos1.z = g_Rng.GetRandomF32InRange(800.0f);
                    }
                    this->SpawnEnemy(this->timelineInstr->arg0, &pos1, args3->ushortVar1, args3->ushortVar2,
                                     args3->uintVar4);
                }
                break;
            case 5:
                if (!g_Gui.BossPresent())
                {
                    pos2 = *this->timelineInstr->args.Var1AsVec();
                    if (pos2.x <= -990.0f)
                    {
                        pos2.x = g_Rng.GetRandomF32InRange(g_GameManager.playerMovementAreaSize.x);
                    }
                    if (pos2.y <= -990.0f)
                    {
                        pos2.y = g_Rng.GetRandomF32InRange(g_GameManager.playerMovementAreaSize.y);
                    }
                    if (pos2.z <= -990.0f)
                    {
                        pos2.z = g_Rng.GetRandomF32InRange(800.0f);
                    }
                    this->SpawnEnemy(this->timelineInstr->arg0, &pos2, -1, ITEM_RANDOM_ITEM, -1);
                }
                break;
            case 6:
                if (!g_Gui.BossPresent())
                {
                    args4 = &this->timelineInstr->args;
                    pos3 = *args4->Var1AsVec();
                    if (args4->Var1AsVec()->x <= -990.0f)
                    {
                        pos3.x = g_Rng.GetRandomF32InRange(g_GameManager.playerMovementAreaSize.x);
                    }
                    if (args4->Var1AsVec()->y <= -990.0f)
                    {
                        pos3.y = g_Rng.GetRandomF32InRange(g_GameManager.playerMovementAreaSize.y);
                    }
                    if (args4->Var1AsVec()->z <= -990.0f)
                    {
                        pos3.z = g_Rng.GetRandomF32InRange(800.0f);
                    }
                    spawnedEnemy = this->SpawnEnemy(this->timelineInstr->arg0, &pos3, args4->ushortVar1,
                                                    args4->ushortVar2, args4->uintVar4);
                    spawnedEnemy->flags.invertX = 1;
                }
                break;
            case 7:
                if (!g_Gui.BossPresent())
                {
                    pos4 = *this->timelineInstr->args.Var1AsVec();
                    if (pos4.x <= -990.0f)
                    {
                        pos4.x = g_Rng.GetRandomF32InRange(g_GameManager.playerMovementAreaSize.x);
                    }
                    if (pos4.y <= -990.0f)
                    {
                        pos4.y = g_Rng.GetRandomF32InRange(g_GameManager.playerMovementAreaSize.y);
                    }
                    if (pos4.z <= -990.0f)
                    {
                        pos4.z = g_Rng.GetRandomF32InRange(800.0f);
                    }
                    spawnedEnemy = this->SpawnEnemy(this->timelineInstr->arg0, &pos4, -1, ITEM_RANDOM_ITEM, -1);
                    spawnedEnemy->flags.invertX = 1;
                }
                break;
            case 8:
                if (g_GameManager.difficulty == EASY && g_GameManager.currentStage == 5 &&
                    this->timelineInstr->arg0 == 1)
                {
                    g_Gui.MsgRead(g_GameManager.character * 10 + 3);
                }
                else
                {
                    g_Gui.MsgRead(this->timelineInstr->arg0 + g_GameManager.character * 10);
                }
                break;
            case 9:
                if (g_Gui.MsgWait())
                {
                    this->timelineTime.Decrement(1);
                    return;
                }
                break;
            case 10:
                this->bosses[this->timelineInstr->args.uintVar1]->runInterrupt = this->timelineInstr->args.uintVar2;
                break;
            case 0xb:
                g_GameManager.currentPower = this->timelineInstr->arg0;
                break;
            case 0xc:
                if (this->bosses[this->timelineInstr->arg0] != NULL &&
                    this->bosses[this->timelineInstr->arg0]->flags.isSlotOccupied)
                {
                    this->timelineTime.Decrement(1);
                    return;
                }
            }
        }
        else if (this->timelineTime.current < this->timelineInstr->time)
        {
            break;
        }

        this->timelineInstr = (EclTimelineInstr *)(((u8 *)this->timelineInstr) + this->timelineInstr->size);
    }
    if (!g_Gui.HasCurrentMsgIdx())
    {
        g_GameManager.counat++;
    }
    return;
}

bool Enemy::HandleLifeCallback()
{

    i32 i;
    Enemy *curEnemy;

    if (this->life < this->lifeCallbackThreshold)
    {
        this->life = this->lifeCallbackThreshold;
        g_EclManager.CallEclSub(&this->currentContext, this->lifeCallbackSub);
        this->lifeCallbackThreshold = -1;
        this->timerCallbackSub = this->deathCallbackSub;
        this->bulletRankSpeedLow = -0.5f;
        this->bulletRankSpeedHigh = 0.5f;
        this->bulletRankAmount1Low = 0;
        this->bulletRankAmount1High = 0;
        this->bulletRankAmount2Low = 0;
        this->bulletRankAmount2High = 0;
        this->stackDepth = 0;

        curEnemy = g_EnemyManager.enemies;
        for (i = 0; i < ARRAY_SIZE_SIGNED(g_EnemyManager.enemies) - 1; i++, curEnemy++)
        {
            if (!curEnemy->flags.isSlotOccupied)
            {
                continue;
            }
            if (curEnemy->flags.isBoss)
            {
                continue;
            }
            curEnemy->life = 0;

            if (!curEnemy->flags.isInteractable && curEnemy->deathCallbackSub >= 0)
            {
                g_EclManager.CallEclSub(&curEnemy->currentContext, curEnemy->deathCallbackSub);
                curEnemy->deathCallbackSub = -1;
            }
        }
        return true;
    }

    return false;
}

bool Enemy::HandleTimerCallback()
{

    Enemy *curEnemy;
    i32 i;

    if (this->flags.isBoss)
    {
        g_Gui.SetSpellcardSeconds((this->timerCallbackThreshold - this->bossTimer.AsFrames()) / 60);
    }

    if (this->HasBossTimerFinished())
    {
        if (this->lifeCallbackThreshold > 0)
        {
            this->life = this->lifeCallbackThreshold;
            this->lifeCallbackThreshold = -1;
        }
        g_EclManager.CallEclSub(&this->currentContext, this->timerCallbackSub);
        this->timerCallbackThreshold = -1;
        this->timerCallbackSub = this->deathCallbackSub;
        this->bossTimer.InitializeForPopup();
        if (!this->flags.isTimeoutSpell)
        {
            g_EnemyManager.spellcardInfo.isCapturing = false;
            if (g_EnemyManager.spellcardInfo.isActive != 0)
            {
                g_EnemyManager.spellcardInfo.isActive++;
            }
            g_BulletManager.RemoveAllBullets(0);
        }

        curEnemy = g_EnemyManager.enemies;
        for (i = 0; i < ARRAY_SIZE_SIGNED(g_EnemyManager.enemies) - 1; i++, curEnemy++)
        {
            if (!curEnemy->flags.isSlotOccupied)
            {
                continue;
            }
            if (curEnemy->flags.isBoss)
            {
                continue;
            }
            curEnemy->life = 0;

            if (!curEnemy->flags.isInteractable && curEnemy->deathCallbackSub >= 0)
            {
                g_EclManager.CallEclSub(&curEnemy->currentContext, curEnemy->deathCallbackSub);
                curEnemy->deathCallbackSub = -1;
            }
        }
        this->bulletRankSpeedLow = -0.5f;
        this->bulletRankSpeedHigh = 0.5f;
        this->bulletRankAmount1Low = 0;
        this->bulletRankAmount1High = 0;
        this->bulletRankAmount2Low = 0;
        this->bulletRankAmount2High = 0;
        this->stackDepth = 0;
        return true;
    }
    return false;
}

void Enemy::Despawn()
{
    if (!this->flags.deathMode)
    {
        this->flags.isSlotOccupied = 0;
    }
    else
    {
        this->flags.isInteractable = 0;
    }
    if (this->flags.isBoss)
    {
        g_Gui.bossPresent = false;
    }
    if (this->effectIdx != 0)
    {
        this->ResetEffectArray(this);
    }
}

void Enemy::ClampPos()
{
    if (this->flags.shouldClampPos)
    {
        if (this->position.x < this->lowerMoveLimit.x)
        {
            this->position.x = this->lowerMoveLimit.x;
        }
        else if (this->position.x > this->upperMoveLimit.x)
        {
            this->position.x = this->upperMoveLimit.x;
        }

        if (this->position.y < this->lowerMoveLimit.y)
        {
            this->position.y = this->lowerMoveLimit.y;
        }
        else if (this->position.y > this->upperMoveLimit.y)
        {
            this->position.y = this->upperMoveLimit.y;
        }
    }
}

ZunResult EnemyManager::RegisterChain(const char *stgEnm1, const char *stgEnm2)
{
    EnemyManager *mgr = &g_EnemyManager;
    mgr->Initialize();
    mgr->stgEnmAnmFilename = stgEnm1;
    mgr->stgEnm2AnmFilename = stgEnm2;
    g_EnemyManagerCalcChain.callback = (ChainCallback)mgr->OnUpdate;
    g_EnemyManagerCalcChain.addedCallback = NULL;
    g_EnemyManagerCalcChain.deletedCallback = NULL;
    g_EnemyManagerCalcChain.addedCallback = (ChainAddedCallback)mgr->AddedCallback;
    g_EnemyManagerCalcChain.deletedCallback = (ChainAddedCallback)mgr->DeletedCallback;
    g_EnemyManagerCalcChain.arg = mgr;
    if (g_Chain.AddToCalcChain(&g_EnemyManagerCalcChain, TH_CHAIN_PRIO_CALC_ENEMYMANAGER))
    {
        return ZUN_ERROR;
    }
    g_EnemyManagerDrawChain.callback = (ChainCallback)mgr->OnDraw;
    g_EnemyManagerDrawChain.addedCallback = NULL;
    g_EnemyManagerDrawChain.deletedCallback = NULL;
    g_EnemyManagerDrawChain.arg = mgr;
    if (g_Chain.AddToDrawChain(&g_EnemyManagerDrawChain, TH_CHAIN_PRIO_DRAW_ENEMYMANAGER))
    {
        return ZUN_ERROR;
    }
    return ZUN_SUCCESS;
}

ChainCallbackResult EnemyManager::OnUpdate(EnemyManager *mgr)
{
    Enemy *curEnemy;
    i32 enemyLifeBeforeDmg;
    i32 enemyVmIdx;
    ZunVec3 enemyHitbox;
    i32 enemyIdx;
    i32 damage;
    bool local_8;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    i32 itemDropState = 0;
    i32 playerDamage[TH06_MULTI_MAX_PLAYERS] = {};
    u8 damageOwnerId = 0;
    i32 damageTotal = 0;
    i32 damageAttributed = 0;
#endif

    local_8 = false;
    mgr->RunEclTimeline();
    for (curEnemy = &mgr->enemies[0], mgr->enemyCount = 0, enemyIdx = 0; enemyIdx < ARRAY_SIZE_SIGNED(mgr->enemies) - 1;
         enemyIdx++, curEnemy++)
    {
        if (!curEnemy->flags.isSlotOccupied)
        {
            continue;
        }
        curEnemy->prevPosition = curEnemy->position;
        curEnemy->prevAngle = curEnemy->angle;
        curEnemy->primaryVm.UpdatePrev();
        for (AnmVm &vm : curEnemy->vms)
        {
            vm.UpdatePrev();
        }
        mgr->enemyCount++;
        curEnemy->Move();

        curEnemy->ClampPos();
        if (curEnemy->flags.hasBeenInBounds == 0 &&
            g_GameManager.IsInBounds(curEnemy->position.x, curEnemy->position.y, curEnemy->primaryVm.sprite->widthPx,
                                     curEnemy->primaryVm.sprite->heightPx))
        {
            curEnemy->flags.hasBeenInBounds = 1;
        }
        if (curEnemy->flags.hasBeenInBounds == 1 &&
            !g_GameManager.IsInBounds(curEnemy->position.x, curEnemy->position.y, curEnemy->primaryVm.sprite->widthPx,
                                      curEnemy->primaryVm.sprite->heightPx))
        {
            curEnemy->flags.isSlotOccupied = 0;
            curEnemy->Despawn();
            continue;
        }
        if (0 <= curEnemy->lifeCallbackThreshold)
        {
            curEnemy->HandleLifeCallback();
        }
        if (0 <= curEnemy->timerCallbackThreshold)
        {
            curEnemy->HandleTimerCallback();
        }
        if (g_EclManager.RunEcl(curEnemy) == ZUN_ERROR)
        {
            curEnemy->flags.isSlotOccupied = 0;
            curEnemy->Despawn();
            continue;
        }
        curEnemy->ClampPos();
        curEnemy->primaryVm.color = curEnemy->color;
        g_AnmManager->ExecuteScript(&curEnemy->primaryVm);
        curEnemy->color = curEnemy->primaryVm.color;
        for (enemyVmIdx = 0; enemyVmIdx < 8; enemyVmIdx++)
        {
            if (0 <= curEnemy->vms[enemyVmIdx].anmFileIndex && g_AnmManager->ExecuteScript(&curEnemy->vms[enemyVmIdx]))
            {
                curEnemy->vms[enemyVmIdx].anmFileIndex = -1;
            }
        }
        local_8 = false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        itemDropState = 0;
        damageOwnerId = 0;
        damageTotal = 0;
        damageAttributed = 0;
#endif
        if (curEnemy->flags.hasBeenInBounds != 0 && !curEnemy->flags.isInvisible)
        {
            enemyLifeBeforeDmg = curEnemy->life;
            if (curEnemy->flags.isCollidable && curEnemy->flags.isInteractable)
            {
                // There's something weird going on here, stack-wise.
                enemyHitbox = curEnemy->HitboxDimensions(1.5f);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                bool hitPlayer = false;
                for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
                {
                    if (IsPlayerGameplayActive(playerId) &&
                        g_Players[playerId].CalcKillBoxCollision(&curEnemy->position, &enemyHitbox) == 1)
                        hitPlayer = true;
                }
                if (hitPlayer && curEnemy->flags.isInteractable && !curEnemy->flags.isBoss)
#else
                if (g_Player.CalcKillBoxCollision(&curEnemy->position, &enemyHitbox) == 1 && curEnemy->flags.isInteractable &&
                    !curEnemy->flags.isBoss)
#endif
                {
                    curEnemy->life -= 10;
                }
            }
            if (curEnemy->flags.isInteractable != 0)
            {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                i32 scoreDamage = 0;
                damage = 0;
                for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
                {
                    playerDamage[playerId] = 0;
                    if (!IsPlayerGameplayActive(playerId))
                        continue;

                    bool playerLaserDuringBomb = false;
                    playerDamage[playerId] = g_Players[playerId].CalcDamageToEnemy(
                        &curEnemy->position, &curEnemy->hitboxDimensions, &playerLaserDuringBomb);
                    if (playerLaserDuringBomb && itemDropState == 0)
                        itemDropState = GetItemAutoCollectStateForPlayer(playerId);
                    scoreDamage += playerDamage[playerId];
                    if (playerDamage[playerId] != 0 &&
                        g_Players[playerId].positionOfLastEnemyHit.y < curEnemy->position.y)
                        g_Players[playerId].positionOfLastEnemyHit = curEnemy->position;

                    if (mgr->spellcardInfo.isActive != 0)
                    {
                        if (!playerLaserDuringBomb)
                        {
                            if (playerDamage[playerId] > 7)
                                playerDamage[playerId] /= 7;
                            else if (playerDamage[playerId] != 0)
                                playerDamage[playerId] = 1;
                        }
                        else if (mgr->spellcardInfo.usedBomb != 0)
                        {
                            if (playerDamage[playerId] > 3)
                                playerDamage[playerId] /= 3;
                            else if (playerDamage[playerId] != 0)
                                playerDamage[playerId] = 1;
                        }
                        else
                        {
                            playerDamage[playerId] = 0;
                        }
                    }
                    damage += playerDamage[playerId];
                    if (playerDamage[playerId] > playerDamage[damageOwnerId])
                        damageOwnerId = playerId;
                }
                damageTotal = damage;
                // Preserve TH06's per-tick 70-damage ceiling. Multiplayer can
                // reach it with combined fire, but cannot exceed the original
                // simulation bound merely because more stable slots exist.
                if (scoreDamage >= 70)
                    scoreDamage = 70;
                if (damage >= 70)
                    damage = 70;
                if (curEnemy->flags.isBoss)
                    damage = static_cast<i32>(
                        static_cast<f32>(damage) * GetMultiplayerBossDamageMultiplier());
                g_GameManager.score = (scoreDamage / 5) * 10 + g_GameManager.score;
#else
                damage = g_Player.CalcDamageToEnemy(&curEnemy->position, &curEnemy->hitboxDimensions, &local_8);
                if (70 <= damage)
                {
                    damage = 70;
                }
                g_GameManager.score = (damage / 5) * 10 + g_GameManager.score;
                if (mgr->spellcardInfo.isActive != 0)
                {
                    if (!local_8)
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
                    else if (mgr->spellcardInfo.usedBomb != 0)
                    {
                        if (damage > 3)
                        {
                            damage = damage / 3;
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
#endif
                if (curEnemy->flags.isDamageable != 0)
                {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    if (damage > 0 && damageTotal > 0)
                    {
                        for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
                        {
                            if (!IsPlayerGameplayActive(playerId) || playerDamage[playerId] <= 0)
                                continue;
                            const i32 contribution = static_cast<i32>(
                                static_cast<std::int64_t>(damage) * playerDamage[playerId] /
                                damageTotal);
                            if (contribution > 0)
                            {
                                AddPlayerDamageDealt(playerId, static_cast<u32>(contribution));
                                damageAttributed += contribution;
                            }
                        }
                        // Integer division may leave a small remainder. Keep
                        // the lower-slot exact-tie rule used by owner selection.
                        if (damageAttributed < damage)
                            AddPlayerDamageDealt(
                                damageOwnerId, static_cast<u32>(damage - damageAttributed));
                    }
#endif
                    curEnemy->life -= damage;
                }
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                if (g_Player.positionOfLastEnemyHit.y < curEnemy->position.y)
                {
                    g_Player.positionOfLastEnemyHit = curEnemy->position;
                }
#endif
            }
            if (0 >= curEnemy->life && curEnemy->flags.isInteractable != 0)
            {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                if (enemyLifeBeforeDmg > 0 && curEnemy->flags.deathMode != 3 &&
                    curEnemy->flags.isDamageable != 0 && damage > 0)
                    AddPlayerEnemiesDefeated(damageOwnerId, 1);
#endif
                curEnemy->lifeCallbackThreshold = -1;
                curEnemy->timerCallbackThreshold = -1;
                switch (curEnemy->flags.deathMode)
                {
                case 3:
                    curEnemy->life = 1;
                    curEnemy->flags.isDamageable = 0;
                    curEnemy->flags.deathMode = 0;
                    g_Gui.bossPresent = 0;
                    g_EffectManager.SpawnParticles(curEnemy->deathAnm1, &curEnemy->position, 1, COLOR_WHITE);
                    g_EffectManager.SpawnParticles(curEnemy->deathAnm1, &curEnemy->position, 1, COLOR_WHITE);
                    g_EffectManager.SpawnParticles(curEnemy->deathAnm1, &curEnemy->position, 1, COLOR_WHITE);
                    break;
                case 1:
                    g_GameManager.AddScore(curEnemy->score);
                    curEnemy->flags.isInteractable = 0;
                    goto LAB_00412a4d;
                case 0:
                    g_GameManager.AddScore(curEnemy->score);
                    curEnemy->flags.isSlotOccupied = 0;
                LAB_00412a4d:
                    if (curEnemy->flags.isBoss)
                    {
                        g_Gui.bossPresent = 0;
                        Enemy::ResetEffectArray(curEnemy);
                    }
                case 2:
                    if (curEnemy->itemDrop >= 0)
                    {
                        g_EffectManager.SpawnParticles(curEnemy->deathAnm2 + 4, &curEnemy->position, 3, 0xffffffff);
                        g_ItemManager.SpawnEnemyDrop(
                            &curEnemy->position, (ItemType)curEnemy->itemDrop,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                            itemDropState
#else
                            local_8
#endif
                        );
                    }
                    else if (curEnemy->itemDrop == ITEM_RANDOM_ITEM)
                    {
                        if (mgr->randomItemSpawnIndex % 3 == 0)
                        {
                            g_EffectManager.SpawnParticles(curEnemy->deathAnm2 + 4, &curEnemy->position, 6,
                                                           COLOR_WHITE);
                            g_ItemManager.SpawnEnemyDrop(&curEnemy->position,
                                                         (ItemType)g_RandomItems[mgr->randomItemTableIndex],
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                                                    itemDropState
#else
                                                    local_8
#endif
                            );
                            mgr->randomItemTableIndex++;
                            if (ARRAY_SIZE_SIGNED(g_RandomItems) <= mgr->randomItemTableIndex)
                            {
                                mgr->randomItemTableIndex = 0;
                            }
                        }
                        mgr->randomItemSpawnIndex++;
                    }
                    if (curEnemy->flags.isBoss && g_EnemyManager.spellcardInfo.isActive == 0)
                    {
                        g_BulletManager.DespawnBullets(12800, false);
                    }
                    curEnemy->life = 0;
                    break;
                }
                g_SoundPlayer.PlaySoundByIdx((SoundIdx)((enemyIdx % 2) + SOUND_2));
                g_EffectManager.SpawnParticles(curEnemy->deathAnm1, &curEnemy->position, 1, 0xffffffff);
                g_EffectManager.SpawnParticles(curEnemy->deathAnm2 + 4, &curEnemy->position, 4, 0xffffffff);
                if (0 <= curEnemy->deathCallbackSub)
                {
                    curEnemy->bulletRankSpeedLow = -0.5;
                    curEnemy->bulletRankSpeedHigh = 0.5;
                    curEnemy->bulletRankAmount1Low = 0;
                    curEnemy->bulletRankAmount1High = 0;
                    curEnemy->bulletRankAmount2Low = 0;
                    curEnemy->bulletRankAmount2High = 0;
                    curEnemy->stackDepth = 0;
                    g_EclManager.CallEclSub(&curEnemy->currentContext, curEnemy->deathCallbackSub);
                    curEnemy->deathCallbackSub = -1;
                }
            }
            if (curEnemy->flags.isBoss != 0 && !g_Gui.HasCurrentMsgIdx())
            {
                g_Gui.SetBossHealthBar(curEnemy->LifePercent());
            }
            if (curEnemy->unk_e41 != 0)
            {
                curEnemy->unk_e41--;
                curEnemy->primaryVm.flags.colorOp = AnmVmColorOp_Modulate;
            }
            else if (enemyLifeBeforeDmg > curEnemy->life)
            {
                g_SoundPlayer.PlaySoundByIdx(SOUND_TOTAL_BOSS_DEATH);
                curEnemy->primaryVm.flags.colorOp = AnmVmColorOp_Add;
                curEnemy->unk_e41 = 4;
            }
            else
            {
                curEnemy->primaryVm.flags.colorOp = AnmVmColorOp_Modulate;
            }
        }
        Enemy::UpdateEffects(curEnemy);
        if (g_GameManager.isTimeStopped == 0 && !PracticeRuntime::OverlayTimeLock())
        {
            curEnemy->bossTimer.Tick();
        }
    }
    bool freezeTimeline = false;
    if (PracticeRuntime::OverlayTimeLock())
    {
        // Upstream TH06 mTimeLock uses currentStage-1 here because gameplay
        // increments GameManager::currentStage before the calc chain starts.
        const u32 stageIndex = static_cast<u32>(g_GameManager.currentStage - 1);
        if (stageIndex < 5 && stageIndex != 2)
        {
            static constexpr i32 midStart[5] = {2008, 2588, 0, 4132, 3374};
            static constexpr i32 midLength[5] = {(24 + 24) * 60, 32 * 60, 0, 40 * 60, (40 + 30) * 60};
            static constexpr i32 midExtraWait[5] = {4 * 60, 15 * 60, 0, 12 * 60, 5 * 60};
            const bool bossExists = g_Gui.BossPresent();
            const i32 curTime = mgr->timelineTime.current;
            if (bossExists && curTime >= midStart[stageIndex] &&
                curTime < midStart[stageIndex] + midLength[stageIndex])
            {
                freezeTimeline = true;
                if (curTime < midStart[stageIndex] + midExtraWait[stageIndex])
                    mgr->timelineTime.SetCurrent(midStart[stageIndex] + midExtraWait[stageIndex]);
            }
        }
    }
    if (!freezeTimeline)
        mgr->timelineTime.Tick();
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

void Enemy::UpdateEffects(Enemy *enemy)
{
    Effect *effect;
    i32 i;

    for (i = 0; i < enemy->effectIdx; i++)
    {
        effect = enemy->effectArray[i];
        if (!effect)
        {
            continue;
        }

        effect->position = enemy->position;
        if (effect->unk_15c < enemy->effectDistance)
        {
            effect->unk_15c += 0.3f;
        }

        effect->angleRelated = utils::AddNormalizeAngle(effect->angleRelated, ZUN_PI / 100);
    }
}

ChainCallbackResult EnemyManager::OnDraw(EnemyManager *mgr)
{
    AnmVm *curEnemyVm;
    Enemy *curEnemy;
    i32 curEnemyVmIdx;
    i32 curEnemyIdx;

    for (curEnemy = &mgr->enemies[0], curEnemyIdx = 0; curEnemyIdx < ARRAY_SIZE_SIGNED(mgr->enemies) - 1;
         curEnemyIdx++, curEnemy++)
    {
        if (!curEnemy->flags.isSlotOccupied)
        {
            continue;
        }
        if (curEnemy->flags.isInvisible)
        {
            continue;
        }

        const ZunVec3 drawPosition = curEnemy->prevPosition.Lerp(curEnemy->position, g_RenderAlpha);
        const f32 drawAngle = utils::LerpAngle(curEnemy->prevAngle, curEnemy->angle, g_RenderAlpha);
        auto drawVm = [&](AnmVm *vm, f32 z, bool autoRotate) {
            const ZunVec3 savedPos = vm->pos;
            const ZunVec3 savedPrevPos = vm->prevPos;
            const ZunVec3 savedRotation = vm->rotation;
            const ZunVec3 savedPrevRotation = vm->prevRotation;

            if (autoRotate)
            {
                vm->rotation.z = drawAngle;
                vm->prevRotation = vm->rotation;
            }
            vm->pos = drawPosition + vm->posOffset;
            vm->pos.z = z;
            vm->prevPos = vm->pos;
            g_AnmManager->Draw2(vm);

            vm->pos = savedPos;
            vm->prevPos = savedPrevPos;
            vm->rotation = savedRotation;
            vm->prevRotation = savedPrevRotation;
        };
        for (curEnemyVm = &curEnemy->vms[0], curEnemyVmIdx = 0; curEnemyVmIdx < 4; curEnemyVmIdx++, curEnemyVm++)
        {
            if (0 <= curEnemyVm->anmFileIndex)
            {
                drawVm(curEnemyVm, 0.495f, curEnemyVm->autoRotate != 0);
            }
        }
        drawVm(&curEnemy->primaryVm, 0.494f, curEnemy->flags.rotateAnm != 0);
        for (curEnemyVmIdx = 4; curEnemyVmIdx < 8; curEnemyVmIdx++, curEnemyVm++)
        {
            if (0 <= curEnemyVm->anmFileIndex)
            {
                drawVm(curEnemyVm, 0.495f, curEnemyVm->autoRotate != 0);
            }
        }
    }
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult EnemyManager::AddedCallback(EnemyManager *enemyManager)
{
    Enemy *enemies = enemyManager->enemies;

    if (enemyManager->stgEnmAnmFilename &&
        g_AnmManager->LoadAnm(ANM_FILE_ENEMY, enemyManager->stgEnmAnmFilename, ANM_OFFSET_ENEMY) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }
    if (enemyManager->stgEnm2AnmFilename &&
        g_AnmManager->LoadAnm(ANM_FILE_ENEMY2, enemyManager->stgEnm2AnmFilename, ANM_OFFSET_ENEMY) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    enemyManager->randomItemSpawnIndex = g_Rng.GetRandomU16InRange(ITEM_SPAWNS);
    enemyManager->randomItemTableIndex = g_Rng.GetRandomU16InRange(ITEM_TABLES);

    enemyManager->spellcardInfo.isActive = 0;
    enemyManager->timelineInstr = NULL;

    return ZUN_SUCCESS;
}

ZunResult EnemyManager::DeletedCallback(EnemyManager *mgr)
{
    g_AnmManager->ReleaseAnm(ANM_FILE_ENEMY2);
    g_AnmManager->ReleaseAnm(ANM_FILE_ENEMY);
    return ZUN_SUCCESS;
}

void EnemyManager::CutChain()
{
    g_Chain.Cut(&g_EnemyManagerCalcChain);
    g_Chain.Cut(&g_EnemyManagerDrawChain);
    return;
}

void Enemy::Move()
{
    if (!this->flags.invertX)
    {
        this->position.x += g_Supervisor.effectiveFramerateMultiplier * this->axisSpeed.x;
    }
    else
    {
        this->position.x -= g_Supervisor.effectiveFramerateMultiplier * this->axisSpeed.x;
    }
    this->position.y += g_Supervisor.effectiveFramerateMultiplier * this->axisSpeed.y;
    this->position.z += g_Supervisor.effectiveFramerateMultiplier * this->axisSpeed.z;
}
