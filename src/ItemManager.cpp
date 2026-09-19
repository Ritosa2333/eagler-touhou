#include "ItemManager.hpp"
#include "graphics/ItemPresentation.hpp"

#include "AnmIdx.hpp"
#include "AnmManager.hpp"
#include "AsciiManager.hpp"
#include "BulletManager.hpp"
#include "EffectManager.hpp"
#include "EnemyManager.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "Gui.hpp"
#include "Player.hpp"
#include "Rng.hpp"
#include "SoundPlayer.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif
#ifdef TH_ENABLE_NETPLAY
#include "netplay/Th07RollbackState.hpp"
#endif

i32 g_FullPowerScoreBonus[30] = {10,   20,   30,   40,   50,   60,   70,   80,    90,    100,
                                 200,  300,  400,  500,  600,  700,  800,  900,   1000,  2000,
                                 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000};

i32 g_PowerLevels[9] = {8, 16, 32, 48, 64, 80, 96, 128, 999};

u8 g_ItemDropTable[32] = {0, 0, 1, 0, 1, 0, 0, 7, 1, 1, 0, 0, 7, 1, 1, 0,
                          1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 7, 1, 1, 1, 0, 2};

ItemManager g_ItemManager;

namespace
{
constexpr i8 AUTO_COLLECT_LEGACY = 1;
constexpr i8 LIFE_TRANSFER_P2 = 2;
constexpr i8 LIFE_TRANSFER_P1 = 3;
constexpr i8 LIFE_TRANSFER_P3 = 8;
constexpr i8 AUTO_COLLECT_P1 = 4;
constexpr i8 AUTO_COLLECT_P2 = 5;
constexpr i8 AUTO_COLLECT_P3 = 9;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
constexpr f32 MULTIPLAYER_RESOURCE_DROP_OFFSET = 16.0f;
#endif

bool ItemPlayerActive(const Player *player)
{
    if (!player)
        return false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (!IsPlayerSlotActive(player->initParam) ||
        MultiplayerGameplay::IsPlayerTemporarilyAbsent(player->initParam))
        return false;
#endif
    return player->playerState == PLAYER_STATE_ALIVE ||
           player->playerState == PLAYER_STATE_INVULNERABLE ||
           player->playerState == PLAYER_STATE_BORDER;
}

i32 ItemPlayerPower(const Player *player)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return GetPlayerPower(player ? player->initParam : 0);
#else
    (void)player;
    return (i32)g_GameManager.globals->currentPower;
#endif
}

i32 ItemPlayerBombs(const Player *player)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return GetPlayerBombs(player ? player->initParam : 0);
#else
    (void)player;
    return (i32)g_GameManager.globals->bombsRemaining;
#endif
}

void AddItemPlayerPower(Player *player, i32 amount)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    AddPlayerPower(player ? player->initParam : 0, amount);
#else
    (void)player;
    g_GameManager.AddCurrentPower(amount);
#endif
}

void SetItemPlayerPower(Player *player, i32 amount)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    SetPlayerPower(player ? player->initParam : 0, amount);
#else
    (void)player;
    g_GameManager.SetCurrentPower(amount);
    g_GameManager.RegenerateGameIntegrityCsum();
#endif
}

void AddItemPlayerBombs(Player *player, i32 amount)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    AddPlayerBombs(player ? player->initParam : 0, amount);
#else
    (void)player;
    g_GameManager.AddBombsRemaining(amount);
#endif
}

void AddSharedCherryPlus(i32 amount, Player *player)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        g_GameManager.AddCherryPlusForPlayer(amount, player ? player->initParam : 0);
        return;
    }
#endif
    g_GameManager.AddCherryPlus(amount);
}

void ExtendFromPointThreshold()
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        ExtendAllPlayersFromPoints();
        return;
    }
#endif
    g_GameManager.ExtendFromPoints();
}

void ExtendFromLifeItem(Player *player)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        ExtendPlayerFromItem(player ? player->initParam : 0);
        return;
    }
#endif
    g_GameManager.ExtendFromPoints();
}

i32 AutoCollectMarkerForPlayer(u8 playerId)
{
    switch (playerId)
    {
    case 0: return AUTO_COLLECT_P1;
    case 1: return AUTO_COLLECT_P2;
    case 2: return AUTO_COLLECT_P3;
    default: return 0;
    }
}

i32 LifeTransferMarkerForPlayer(u8 playerId)
{
    switch (playerId)
    {
    case 0: return LIFE_TRANSFER_P1;
    case 1: return LIFE_TRANSFER_P2;
    case 2: return LIFE_TRANSFER_P3;
    default: return 0;
    }
}

i32 FixedItemMarkerPlayer(i8 marker)
{
    if (marker == LIFE_TRANSFER_P1 || marker == AUTO_COLLECT_P1) return 0;
    if (marker == LIFE_TRANSFER_P2 || marker == AUTO_COLLECT_P2) return 1;
    if (marker == LIFE_TRANSFER_P3 || marker == AUTO_COLLECT_P3) return 2;
    return -1;
}

bool IsAutoCollected(const Item *item)
{
    return item && (item->autoCollect == AUTO_COLLECT_LEGACY ||
                    item->autoCollect == AUTO_COLLECT_P1 ||
                    item->autoCollect == AUTO_COLLECT_P2 ||
                    item->autoCollect == AUTO_COLLECT_P3);
}

bool HasFixedItemTarget(const Item *item)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return item && MultiplayerGameplay::IsMultiplayer() &&
           (item->autoCollect == LIFE_TRANSFER_P2 ||
            item->autoCollect == LIFE_TRANSFER_P1 ||
            item->autoCollect == LIFE_TRANSFER_P3 ||
            item->autoCollect == AUTO_COLLECT_P1 ||
            item->autoCollect == AUTO_COLLECT_P2 ||
            item->autoCollect == AUTO_COLLECT_P3);
#else
    (void)item;
    return false;
#endif
}

Player *ItemTargetPlayer(Item *item)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        const i32 fixed = item ? FixedItemMarkerPlayer(item->autoCollect) : -1;
        if (fixed >= 0)
            return &g_Players[fixed];
        return item ? GetClosestActivePlayer(&item->pos) : &g_Player;
    }
#endif
    return &g_Player;
}

bool IsMultiplayerTransferState(i32 state)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return MultiplayerGameplay::IsMultiplayer() && state >= 3 && state <= 5;
#else
    (void)state;
    return false;
#endif
}

bool CanAutoCollect(const Player *player)
{
    if (!ItemPlayerActive(player) || !player->shooterData)
        return false;
    return ((((ItemPlayerPower(player) >= 128) || g_GameManager.difficulty >= DIFF_EXTRA) &&
             player->pos.y < player->shooterData->pocY) ||
            player->hasBorder == BORDER_ACTIVE);
}

Player *AutoCollectTarget(Item *item)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        i32 ids[TH07_MULTI_MAX_PLAYERS];
        i32 count = 0;
        for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            if (IsPlayerSlotActive(playerId) && CanAutoCollect(&g_Players[playerId]))
                ids[count++] = playerId;
        }
        if (count == 0)
            return nullptr;
        if (count == 1)
            return &g_Players[ids[0]];
        const i32 itemIndex = item ? (i32)(item - g_ItemManager.items) : 0;
        return &g_Players[ids[itemIndex % count]];
    }
#endif
    return CanAutoCollect(&g_Player) ? &g_Player : nullptr;
}

i32 RoundRobinPowerTarget(i32 itemIndex)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        i32 ids[TH07_MULTI_MAX_PLAYERS];
        i32 count = 0;
        for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
            if (IsPlayerSlotActive(playerId))
                ids[count++] = playerId;
        return count > 0 ? ids[itemIndex % count] : 0;
    }
#endif
    return 0;
}

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
void GetSeparatedResourceDropPosition(const ZunVec3 *origin, i32 ordinal, i32 count,
                                      ZunVec3 *position)
{
    f32 centerX = origin->x;
    const f32 halfSpan = MULTIPLAYER_RESOURCE_DROP_OFFSET * (count - 1);
    const f32 maximumCenterX = g_GameManager.arcadeRegionSize.x - halfSpan;

    *position = *origin;
    if (centerX < halfSpan)
        centerX = halfSpan;
    if (centerX > maximumCenterX)
        centerX = maximumCenterX;
    position->x = centerX - halfSpan +
                  ordinal * MULTIPLAYER_RESOURCE_DROP_OFFSET * 2.0f;
}
#endif
} // namespace

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
i32 GetLifeTransferSpawnState(u8 targetPlayerId)
{
    switch (targetPlayerId)
    {
    case 0: return 4;
    case 1: return 3;
    case 2: return 5;
    default: return 0;
    }
}
#endif

void AngleToVector(ZunVec3 *vec, f32 angle, f32 speed)
{
    vec->x = cosf(angle) * speed;
    vec->y = sinf(angle) * speed;
}

void GameManager::AddCurrentPower(i32 amount)
{
    if (CheckGameIntegrity())
    {
        NUKE_SUPERVISOR();
    }
    this->globals->currentPower += (f32)amount;
    RegenerateGameIntegrityCsum();
}

ItemManager::ItemManager()
{
}

Item::Item()
{
}

Item *ItemManager::SpawnItem(ZunVec3 *heading, i32 itemType, i32 state)
{
    Item *item;
    i32 i;

    item = &this->items[this->nextIndex];
    const bool isTransfer = IsMultiplayerTransferState(state);
    if (itemType == ITEM_POWER_SMALL || itemType == ITEM_POWER_BIG)
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (isTransfer)
        {
            // A transfer belongs to its explicit receiver. Do not convert it
            // using an unrelated round-robin slot's MAX-power state.
        }
        else if (MultiplayerGameplay::IsMultiplayer())
        {
            const i32 targetId = RoundRobinPowerTarget(this->nextIndex);
            if (GetPlayerPower((u8)targetId) >= 128)
                itemType = ITEM_CHERRY;
        }
        else
#endif
        if ((i32)g_GameManager.globals->currentPower >= 128)
            itemType = ITEM_CHERRY;
    }
    for (i = 0; i < 1100; i++)
    {
        this->nextIndex++;

        if (item->isInUse)
        {
            if (this->nextIndex >= 1100)
            {
                this->nextIndex = 0;
                item = this->items;
            }
            else
            {
                item++;
            }
            continue;
        }
        if (this->nextIndex >= 1100)
        {
            this->nextIndex = 0;
        }
#ifdef TH_ENABLE_NETPLAY
        Netplay::Th07Rollback::TouchItem(item);
#endif
        item->isInUse = 1;
        item->prevPos = item->pos = *heading;
        item->velocity.x = 0.0f;
        item->velocity.y = -2.2f;
        item->velocity.z = 0.0f;
        item->itemType = (u8)itemType;
        item->state = (u8)state;
        item->timer = 0;
        if (state == 2)
        {
            item->targetPos.x = g_Rng.GetRandomFloatInRange(288.0f) + 48.0f;
            item->targetPos.y = g_Rng.GetRandomFloatInRange(192.0f) - 64.0f;
            item->targetPos.z = 0.0f;
            item->velocity = item->pos;
        }
        else if (isTransfer)
        {
            item->targetPos = *heading;
            item->targetPos.y -= 60.0f;
            item->targetPos.z = 0.0f;
            item->velocity = item->pos;
        }
        else if (state == 3)
        {
            item->state = 1;
        }
        else if (state == 4)
        {
            item->state = 0;
        }
        g_AnmManager->SetAnmIdxAndExecuteScript(&item->sprite,
                                                itemType + ANM_SCRIPT_BULLETS_ITEM_SPAWN_ARRAY);
        item->sprite.color.color = 0xffffffff;
        item->sprite.UpdatePrev();
        item->sprite.zWriteDisable = 1;
        if (isTransfer)
        {
            const u8 transferTarget = state == 3 ? 1 : (state == 4 ? 0 : 2);
            item->autoCollect = (i8)LifeTransferMarkerForPlayer(transferTarget);
        }
        else
        {
            item->autoCollect = 0;
        }
        item->isOnscreen = 1;
        break;
    }

    return i < 1100 ? item : &this->items[1100];
}

Item *ItemManager::SpawnEnemyDrop(ZunVec3 *heading, i32 itemType, i32 state)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() &&
        (itemType == ITEM_LIFE || itemType == ITEM_BOMB))
    {
        const i32 activeCount = GetActivePlayerCount();
        if (activeCount > 1)
        {
            Item *first = &this->items[1100];
            i32 ordinal = 0;
            for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
            {
                if (!IsPlayerSlotActive(playerId))
                    continue;

                ZunVec3 position;
                GetSeparatedResourceDropPosition(heading, ordinal, activeCount, &position);
                Item *spawned = SpawnItem(&position, itemType, state);
                if (ordinal == 0)
                    first = spawned;
                ++ordinal;
            }
            return first;
        }
    }
#endif
    return SpawnItem(heading, itemType, state);
}

void ItemManager::OnUpdate()
{
    i32 prevPowerLevel2;
    i32 k;
    i32 prevPowerIdx;
    i32 j;
    Item *item;
    ZunBool itemAcquired;
    f32 playerAngle;
    i32 itemScore;
    f32 itemTimerSecs;
    i32 i;
    Player *targetPlayer;
    Player *autoCollectTarget;

    item = this->items;
    ZunVec3 local_20(0.0f, 0.0f, 16.0f);
    itemAcquired = 0;
    this->activeItemCount = 0;
    this->listTail = &this->listHead;
    this->listHead.next = NULL;

    for (i = 0; i < 1100; i++, item++)
    {
        if (!item->isInUse)
        {
            continue;
        }

        item->sprite.UpdatePrev();
        item->prevPos = item->pos;

        this->activeItemCount++;
        targetPlayer = ItemTargetPlayer(item);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (HasFixedItemTarget(item) && !ItemPlayerActive(targetPlayer))
        {
            item->autoCollect = 0;
            item->state = 0;
            targetPlayer = GetClosestActivePlayer(&item->pos);
        }
#endif
        if (item->state == 2)
        {
            if (item->timer < 60)
            {
                itemTimerSecs = item->timer.AsFloat() / 60.0f;
                item->pos =
                    itemTimerSecs * item->targetPos + item->velocity * (1.0f - itemTimerSecs);
                goto check_collision;
            }
            else if (item->timer == 60)
            {
                item->velocity = ZunVec3(0.0f, 0.0f, 0.0f);
                item->state = 0;
            }
        }
        else if (IsMultiplayerTransferState(item->state))
        {
            if (item->timer < 20)
            {
                const f32 throwTime = item->timer.AsFloat() / 20.0f;
                const f32 throwEase = 1.0f - powf(1.0f - throwTime, 1.5f);
                item->pos =
                    throwEase * item->targetPos +
                    item->velocity * (1.0f - throwEase);
                goto check_collision;
            }
            else if (item->timer == 20)
            {
                item->velocity = ZunVec3(0.0f, 0.0f, 0.0f);
                item->state = 1;
            }
        }
        else
        {
            autoCollectTarget =
                item->state == 1 ||
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                        (HasFixedItemTarget(item) && !IsSharedBorderActive())
#else
                        HasFixedItemTarget(item)
#endif
                    ? nullptr
                    : AutoCollectTarget(item);
            if (autoCollectTarget)
            {
                targetPlayer = autoCollectTarget;
                item->state = 1;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                if (MultiplayerGameplay::IsMultiplayer())
                {
                    if (item->autoCollect == 0)
                        item->autoCollect = (i8)AutoCollectMarkerForPlayer(targetPlayer->initParam);
                }
                else if (targetPlayer->hasBorder == BORDER_ACTIVE)
                {
                    item->autoCollect = AUTO_COLLECT_LEGACY;
                }
#else
                if (targetPlayer->hasBorder == BORDER_ACTIVE)
                    item->autoCollect = AUTO_COLLECT_LEGACY;
#endif
            }
            if (item->state == 1)
            {
                if (targetPlayer->playerState != PLAYER_STATE_DEAD)
                {
                    playerAngle = targetPlayer->AngleToPlayer(&item->pos);
                    AngleToVector(&item->velocity, playerAngle,
                                  targetPlayer->shooterData->itemCollectSpeed);
                }
                else
                {
                    item->velocity.y = -0.5f;
                    item->state = 0;
                }
            }
            else
            {
                item->velocity.x = 0.0f;
                item->velocity.z = 0.0f;
                if (item->velocity.y < -2.2f)
                {
                    item->velocity.y = -2.2f;
                }
            }
        }
        item->pos += item->velocity * g_Supervisor.effectiveFramerateMultiplier;
        if (g_GameManager.arcadeRegionSize.y + 16.0f <= item->pos.y)
        {
            item->isInUse = 0;
            g_GameManager.DecreaseSubrank(3);
            continue;
        }
        if (item->velocity.y < 3.0f)
        {
            item->velocity.y += 0.03f * g_Supervisor.effectiveFramerateMultiplier;
        }
        else
        {
            item->velocity.y = 3.0f;
        }
    check_collision:
        targetPlayer = ItemTargetPlayer(item);
        local_20.x = targetPlayer->shooterData->itemCollectRadius;
        local_20.y = targetPlayer->shooterData->itemCollectRadius;
        if (!(IsMultiplayerTransferState(item->state) && item->timer < 20) &&
            targetPlayer->CalcItemBoxCollision(&item->pos, &local_20))
        {
            g_ReplayManager->replayEventFlags |= 0x40;
            switch (item->itemType)
            {
            case ITEM_POWER_SMALL:
                if (ItemPlayerPower(targetPlayer) >= 128)
                {
                    g_GameManager.powerItemCountForScore++;
                    if ((u32)g_GameManager.powerItemCountForScore >= 31)
                    {
                        g_GameManager.powerItemCountForScore = 30;
                    }
                    itemScore = g_FullPowerScoreBonus[g_GameManager.powerItemCountForScore];
                    g_GameManager.AddScore(itemScore);
                    g_AsciiManager.CreatePopup1(&item->pos, itemScore,
                                                itemScore >= 12800 ? 0xffffff00 : 0xffffffff);
                }
                else
                {
                    j = 0;
                    while (ItemPlayerPower(targetPlayer) >= g_PowerLevels[j])
                    {
                        j++;
                    }
                    prevPowerIdx = j;
                    g_GameManager.powerItemCountForScore = 0;
                    AddItemPlayerPower(targetPlayer, 1);
                    if (ItemPlayerPower(targetPlayer) >= 128)
                    {
                        SetItemPlayerPower(targetPlayer, 128);
                        if (!g_EnemyManager.spellcardInfo.isActive)
                        {
                            g_BulletManager.RemoveAllBullets(1);
                        }
                        g_Gui.ShowStatusPopup(0, 1);
                        this->DespawnAllItems(i);
                    }
                    g_GameManager.AddScore(10);
                    g_Gui.powerDisplayUpdateFrames = 2;
                    while (ItemPlayerPower(targetPlayer) >= g_PowerLevels[j])
                    {
                        j++;
                    }
                    if (j != prevPowerIdx)
                    {
                        g_AsciiManager.CreatePopup1(&item->pos, -1, 0xffffc0a0);
                        g_SoundPlayer.PlaySoundByIdx(SOUND_POWERUP, 0);
                    }
                    else
                    {
                        g_AsciiManager.CreatePopup1(&item->pos, 10, 0xffffffff);
                    }
                }
                g_GameManager.IncreaseSubrank(1);
                break;
            case ITEM_POINT:
                itemScore = item->pos.y < targetPlayer->shooterData->pocY
                                ? 50000
                                : 50000 -
                                      (i32)(item->pos.y - targetPlayer->shooterData->pocY) * 100;
                if (IsAutoCollected(item))
                {
                    itemScore = 50000;
                }
                if (itemScore >= 50000)
                {
                    if (g_GameManager.cherry - g_GameManager.globals->cherryStart > 50000)
                    {
                        itemScore = g_GameManager.cherry - g_GameManager.globals->cherryStart;
                    }
                }
                else if (g_GameManager.cherry - g_GameManager.globals->cherryStart > 50000)
                {
                    itemScore +=
                        (g_GameManager.cherry - g_GameManager.globals->cherryStart - 50000) / 5;
                }
                itemScore -= itemScore % 10;
                g_AsciiManager.CreatePopup1(&item->pos, itemScore,
                                            item->pos.y < targetPlayer->shooterData->pocY ||
                                                    IsAutoCollected(item)
                                                ? 0xffffff00
                                                : 0xffffffff);
                g_GameManager.AddScore(itemScore);
                g_GameManager.globals->pointItemsCollectedThisStage++;
                g_GameManager.globals->pointItemsCollectedForExtend++;
                g_Gui.pointDisplayUpdateFrames = 2;
                if (item->pos.y < 128.0f)
                {
                    g_GameManager.IncreaseSubrank(10);
                }
                else
                {
                    g_GameManager.IncreaseSubrank(3);
                }
                if (g_GameManager.globals->extendsFromPointItems >= 0)
                {
                    for (;;)
                    {
                        if (g_GameManager.difficulty < 4)
                        {
                            if (g_GameManager.globals->extendsFromPointItems < 3)
                            {
                                g_GameManager.globals->nextNeededPointItemsForExtend =
                                    g_GameManager.globals->extendsFromPointItems * 75 + 50;
                            }
                            else if (g_GameManager.globals->extendsFromPointItems < 5)
                            {
                                g_GameManager.globals->nextNeededPointItemsForExtend =
                                    (g_GameManager.globals->extendsFromPointItems - 3) * 150 + 300;
                            }
                            else
                            {
                                g_GameManager.globals->nextNeededPointItemsForExtend =
                                    (g_GameManager.globals->extendsFromPointItems - 5) * 200 + 800;
                            }
                        }
                        else if (g_GameManager.globals->extendsFromPointItems == 0)
                        {
                            g_GameManager.globals->nextNeededPointItemsForExtend = 200;
                        }
                        else if (g_GameManager.globals->extendsFromPointItems == 1)
                        {
                            g_GameManager.globals->nextNeededPointItemsForExtend = 500;
                        }
                        else
                        {
                            g_GameManager.globals->nextNeededPointItemsForExtend =
                                (g_GameManager.globals->extendsFromPointItems - 2) * 500 + 800;
                        }

                        if (g_GameManager.globals->pointItemsCollectedForExtend >=
                            g_GameManager.globals->nextNeededPointItemsForExtend)
                        {
                            ExtendFromPointThreshold();
                            g_GameManager.globals->extendsFromPointItems++;
                            continue;
                        }
                        break;
                    }
                }
                break;
            case ITEM_POWER_BIG:
                if (ItemPlayerPower(targetPlayer) >= 128)
                {
                    g_AsciiManager.CreatePopup1(&item->pos, itemScore,
                                                itemScore >= 1000 ? 0xffffff00 : 0xffffffff);
                }
                else
                {
                    k = 0;
                    while (ItemPlayerPower(targetPlayer) >= g_PowerLevels[k])
                    {
                        k++;
                    }
                    prevPowerLevel2 = k;
                    AddItemPlayerPower(targetPlayer, 8);
                    if (ItemPlayerPower(targetPlayer) >= 128)
                    {
                        SetItemPlayerPower(targetPlayer, 128);
                        if (!g_EnemyManager.spellcardInfo.isActive)
                        {
                            g_BulletManager.RemoveAllBullets(1);
                        }
                        g_Gui.ShowStatusPopup(0, 1);
                        this->DespawnAllItems(i);
                    }
                    g_Gui.powerDisplayUpdateFrames = 2;
                    g_GameManager.AddScore(10);
                    while (ItemPlayerPower(targetPlayer) >= g_PowerLevels[k])
                    {
                        k++;
                    }
                    if (k != prevPowerLevel2)
                    {
                        g_AsciiManager.CreatePopup1(&item->pos, -1, 0xffffc0a0);
                        g_SoundPlayer.PlaySoundByIdx(SOUND_POWERUP, 0);
                    }
                    else
                    {
                        g_AsciiManager.CreatePopup1(&item->pos, 10, 0xffffffff);
                    }
                }
                break;
            case ITEM_BOMB:
                if (ItemPlayerBombs(targetPlayer) < 8)
                {
                    AddItemPlayerBombs(targetPlayer, 1);
                    g_Gui.bombDisplayUpdateFrames = 2;
                }
                g_GameManager.IncreaseSubrank(5);
                break;
            case ITEM_LIFE:
                ExtendFromLifeItem(targetPlayer);
                break;
            case ITEM_FULL_POWER:
                if (ItemPlayerPower(targetPlayer) < 128)
                {
                    g_BulletManager.RemoveAllBullets(1);
                    g_Gui.ShowStatusPopup(0, 1);
                    g_SoundPlayer.PlaySoundByIdx(SOUND_POWERUP, 0);
                    g_AsciiManager.CreatePopup1(&item->pos, -1, 0xffffc0a0);
                    this->DespawnAllItems(i);
                }
                SetItemPlayerPower(targetPlayer, 128);
                g_GameManager.AddScore(1000);
                g_AsciiManager.CreatePopup1(&item->pos, 1000, 0xffffffff);
                g_Gui.powerDisplayUpdateFrames = 2;
                break;
            case ITEM_POINT_BULLET:
                if (!targetPlayer->isBombing)
                {
                    itemScore = g_GameManager.globals->grazeInTotal / 40 * 10 + 300;
                    if (itemScore <= 0)
                    {
                        itemScore = 10;
                    }
                }
                else
                {
                    itemScore = 100;
                }
                g_AsciiManager.CreatePopup2(&item->pos, itemScore, -1);
                g_GameManager.AddScore(itemScore);
                if (!targetPlayer->bombInfo.isInUse)
                {
                    AddSharedCherryPlus(20, targetPlayer);
                }
                else if ((i & 1) == 0)
                {
                    AddSharedCherryPlus(10, targetPlayer);
                }
                else
                {
                    g_GameManager.AddCherry(10);
                }
                break;
            case ITEM_CHERRY_SMALL:
                AddSharedCherryPlus(30, targetPlayer);
                g_GameManager.AddCherry(70);
                break;
            case ITEM_CHERRY:
                if (g_GameManager.IsCherryAtMax())
                {
                    itemScore =
                        item->pos.y < targetPlayer->shooterData->pocY || IsAutoCollected(item)
                            ? 50000
                            : 50000 -
                                  (i32)(item->pos.y - targetPlayer->shooterData->pocY) * 100;
                    itemScore -= itemScore % 10;
                    g_AsciiManager.CreatePopup1(
                        &item->pos, itemScore,
                        item->pos.y < targetPlayer->shooterData->pocY || IsAutoCollected(item)
                            ? 0xffffff00
                            : 0xffffffff);
                    g_GameManager.AddScore(itemScore);
                }
                itemScore = 1000;
                itemScore += g_GameManager.globals->spellCardsCaptured * 100;
                if (!g_GameManager.IsCherryAtMax())
                {
                    g_AsciiManager.CreatePopup1(&item->pos, itemScore, 0xffff4040);
                }
                AddSharedCherryPlus(itemScore, targetPlayer);
                break;
            case ITEM_STAR:
                itemScore = g_GameManager.globals->grazeInTotal / 40 * 10 + 300;
                if (itemScore <= 0)
                {
                    itemScore = 10;
                }
                if (g_GameManager.IsCherryAtMax())
                {
                    g_AsciiManager.CreatePopup1(&item->pos, itemScore, 0xffffffff);
                }
                g_GameManager.AddScore(itemScore);
                itemScore = 100;
                if (!g_GameManager.IsCherryAtMax())
                {
                    g_AsciiManager.CreatePopup1(&item->pos, itemScore, 0xffff4040);
                }
                AddSharedCherryPlus(itemScore, targetPlayer);
                break;
            }
            item->isInUse = 0;
            itemAcquired = 1;
            continue;
        }
        else
        {
            item->timer++;
            if (item->sprite.currentInstruction)
            {
                g_AnmManager->ExecuteScript(&item->sprite);
            }
            Graphics::UpdateItemPresentation(*item, [](AnmVm &vm, i32 sprite) {
                g_AnmManager->SetActiveSprite(&vm, sprite);
            });
            // Leave the state that one authored 60 Hz draw would have left.
            // Interpolated draws may temporarily replace this position, but
            // the next ANM tick must never inherit a display-time midpoint.
            item->sprite.pos.x = g_GameManager.arcadeRegionTopLeftPos.x + item->pos.x;
            item->sprite.pos.y = g_GameManager.arcadeRegionTopLeftPos.y +
                (item->pos.y < -8.0f ? 8.0f : item->pos.y);
            item->sprite.pos.z = 0.01f;
            this->listTail->next = item;
            item->next = NULL;
            this->listTail = item;
        }
    }
    if (itemAcquired)
    {
        g_SoundPlayer.PlaySoundByIdx(SOUND_ITEM_GET, 0);
    }
}

void ItemManager::RemoveAllItems()
{
    Item *item;
    i32 i;

    item = this->items;
    for (i = 0; i < MAX_ITEMS; i++, item++)
    {
        if (!item->isInUse)
        {
            continue;
        }

        item->state = 1;
        item->velocity = ZunVec3(0.0f, -0.5f, 0.0f);
    }
}

void ItemManager::DespawnAllItems(i32 param_1)
{
    Item *item;
    i32 i;

    item = this->items;
    for (i = 0; i < MAX_ITEMS; i++, item++)
    {
        if (item->isInUse == 0 || i == param_1)
        {
            continue;
        }

        if (item->itemType == 0 || item->itemType == 2)
        {
            if (item->velocity.y > -0.5f)
            {
                item->velocity.x = 0.0f;
                item->velocity.y = -0.5f;
                item->velocity.z = 0.0f;
            }
            g_EffectManager.SpawnEffect(0, &item->pos, 1, 0xffffffff);
            item->itemType = 7;
            g_AnmManager->SetAnmIdxAndExecuteScript(&item->sprite, ANM_SCRIPT_BULLETS_ITEM_DESPAWN);
        }
    }
}

void ItemManager::ActivateAllItems()
{
    Item *item;
    i32 i;

    item = this->items;
    for (i = 0; i < MAX_ITEMS; i++, item++)
    {
        if (item->isInUse != 1)
        {
            continue;
        }

        if (item->state == 1)
        {
            item->state = 0;
            item->velocity.x = 0.0f;
            item->velocity.y = -0.9f;
            item->velocity.z = 0.0f;
        }
    }
}

void ItemManager::OnDraw()
{
    Item *item;

    item = this->listHead.next;
    while (item)
    {
        const ZunVec3 authoredPos = item->sprite.pos;
        ZunVec3 drawPos = item->prevPos.Lerp(item->pos, g_RenderAlpha);
        item->sprite.pos.x = g_GameManager.arcadeRegionTopLeftPos.x + drawPos.x;
        item->sprite.pos.y = g_GameManager.arcadeRegionTopLeftPos.y + drawPos.y;
        item->sprite.pos.z = 0.01f;
        if (item->pos.y < -8.0f)
        {
            item->sprite.pos.y = 8.0f + g_GameManager.arcadeRegionTopLeftPos.y;
        }
        g_AnmManager->Draw(&item->sprite);
        item->sprite.pos = authoredPos;
        item = item->next;
    }
}
