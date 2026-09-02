#include "ItemManager.hpp"

#include <cmath>

#include "AnmManager.hpp"
#include "AsciiManager.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "GameWindow.hpp"
#include "Player.hpp"
#include "Rng.hpp"
#include "SoundPlayer.hpp"
#include "utils.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif
#ifdef TH_ENABLE_NETPLAY
#include "netplay/Th06RollbackState.hpp"
#endif

// #include <d3dx8math.h>

ItemManager g_ItemManager;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
namespace
{
constexpr i8 ITEM_STATE_AUTO_P1 = 3;
constexpr i8 ITEM_STATE_AUTO_P3 = 5;
constexpr i8 ITEM_STATE_TRANSFER_P1 = 6;
constexpr i8 ITEM_STATE_TRANSFER_P3 = 8;
constexpr i32 ITEM_TRANSFER_RISE_FRAMES = 20;
constexpr f32 ITEM_TRANSFER_RISE_DISTANCE = 60.0f;
constexpr f32 MULTIPLAYER_RESOURCE_DROP_OFFSET = 16.0f;

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

bool IsMultiplayerTransferState(i32 state)
{
    return MultiplayerGameplay::IsMultiplayer() &&
           state >= ITEM_STATE_TRANSFER_P1 && state <= ITEM_STATE_TRANSFER_P3;
}
} // namespace
#endif

ItemManager::ItemManager() {

};

void ItemManager::SpawnItem(const ZunVec3 *position, ItemType itemType, i32 state)
{
    Item *item;
    i32 idx;

    item = &this->items[this->nextIndex];
    for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->items) - 1; idx++)
    {
        this->nextIndex++;
        if (item->isInUse)
        {
            if (this->nextIndex >= ARRAY_SIZE_SIGNED(this->items) - 1)
            {
                this->nextIndex = 0;
                item = &this->items[0];
            }
            else
            {
                item++;
            }
            continue;
        }
        if (this->nextIndex >= ARRAY_SIZE_SIGNED(this->items) - 1)
        {
            this->nextIndex = 0;
        }
#ifdef TH_ENABLE_NETPLAY
        if (!Netplay::Th06Rollback::TouchItem(item))
            return;
#endif
        item->isInUse = 1;
        item->currentPosition = *position;
        item->prevPosition = item->currentPosition;
        item->sprite.UpdatePrev();
        item->startPosition.x = 0.0f;
        item->startPosition.y = -2.2f;
        item->startPosition.z = 0.0f;
        item->itemType = itemType;
        item->state = state;
        item->timer.InitializeForPopup();
        if (state == 2)
        {
            // From 48.0f to 336.0f
            item->targetPosition.x = g_Rng.GetRandomF32ZeroToOne() * 288.0f + 48.0f;
            // From -64.0 to 128.0f
            item->targetPosition.y = g_Rng.GetRandomF32ZeroToOne() * 192.0f - 64.0f;
            item->targetPosition.z = 0.0;
            item->startPosition = item->currentPosition;
        }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        else if (IsMultiplayerTransferState(state))
        {
            item->targetPosition = *position;
            item->targetPosition.y -= ITEM_TRANSFER_RISE_DISTANCE;
            item->targetPosition.z = 0.0f;
            item->startPosition = item->currentPosition;
        }
#endif
        g_AnmManager->SetAndExecuteScriptIdx(&item->sprite, ANM_SCRIPT_BULLET3_ITEMS_START + itemType);
        item->sprite.color = COLOR_WHITE;
        item->unk_142 = 1;
        return;
    }
    return;
}

void ItemManager::SpawnEnemyDrop(const ZunVec3 *position, ItemType itemType, i32 state)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() &&
        (itemType == ITEM_LIFE || itemType == ITEM_BOMB))
    {
        const i32 activeCount = GetActivePlayerCount();
        if (activeCount > 1)
        {
            i32 ordinal = 0;
            for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
            {
                if (!IsPlayerActive(playerId))
                    continue;

                ZunVec3 separatedPosition;
                GetSeparatedResourceDropPosition(position, ordinal, activeCount,
                                                 &separatedPosition);
                SpawnItem(&separatedPosition, itemType, state);
                ++ordinal;
            }
            return;
        }
    }
#endif
    SpawnItem(position, itemType, state);
}

static const i32 g_PowerUpThresholds[11] = {8, 16, 32, 48, 64, 80, 96, 128, 999, 1, 0};
static const i32 g_PowerItemScore[31] = {10,   20,   30,   40,   50,   60,    70,    80,    90,   100,  200,
                                         300,  400,  500,  600,  700,  800,   900,   1000,  2000, 3000, 4000,
                                         5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 51200};

static inline i32 calculatePointScore(const Item *curItem, i32 scoreAcquiredItemTop, i32 scoreAcquiredItemBottom,
                                      i32 posMultiplier)
{
    return ((i32)curItem->currentPosition.y < 128)
               ? scoreAcquiredItemTop
               : (scoreAcquiredItemBottom - (((i32)curItem->currentPosition.y - 128) * posMultiplier));
}

static const ZunVec3 g_ItemSize(16.0f, 16.0f, 16.0f);

static i32 ItemPlayerPower(const Player *player)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return GetPlayerPower(player ? player->initParam : 0);
#else
    (void)player;
    return g_GameManager.currentPower;
#endif
}

static i32 ItemPlayerBombs(const Player *player)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return GetPlayerBombs(player ? player->initParam : 0);
#else
    (void)player;
    return g_GameManager.bombsRemaining;
#endif
}

static i32 ItemPlayerLives(const Player *player)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return GetPlayerLives(player ? player->initParam : 0);
#else
    (void)player;
    return g_GameManager.livesRemaining;
#endif
}

static void SetItemPlayerPower(const Player *player, i32 value)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    SetPlayerPower(player ? player->initParam : 0, value);
#else
    (void)player;
    g_GameManager.currentPower = static_cast<u16>(value);
#endif
}

static void SetItemPlayerBombs(const Player *player, i32 value)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    SetPlayerBombs(player ? player->initParam : 0, value);
#else
    (void)player;
    g_GameManager.bombsRemaining = static_cast<i8>(value);
#endif
}

static void SetItemPlayerLives(const Player *player, i32 value)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    SetPlayerLives(player ? player->initParam : 0, value);
#else
    (void)player;
    g_GameManager.livesRemaining = static_cast<i8>(value);
#endif
}

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
namespace
{
constexpr i8 ITEM_STATE_AUTO_COLLECT = 1;
constexpr i8 ITEM_STATE_SCATTER = 2;
// TH06 already uses 3..5 as fixed auto-collect targets. Keep those semantics
// intact and reserve 6..8 for deliberate co-op resource transfers. Transfer
// items rise for 20 logical frames, then become the corresponding 3..5 homing
// state. This is the TH06-native equivalent of TH07's transfer trajectory.

bool ItemPlayerActive(const Player *player)
{
    return player && IsPlayerGameplayActive(player->initParam) &&
           (player->playerState == PLAYER_STATE_ALIVE ||
            player->playerState == PLAYER_STATE_INVULNERABLE);
}

bool HasFixedItemTarget(const Item *item)
{
    return item && MultiplayerGameplay::IsMultiplayer() &&
           ((item->state >= ITEM_STATE_AUTO_P1 && item->state <= ITEM_STATE_AUTO_P3) ||
            (item->state >= ITEM_STATE_TRANSFER_P1 && item->state <= ITEM_STATE_TRANSFER_P3));
}

bool IsTransferItem(const Item *item)
{
    return item && MultiplayerGameplay::IsMultiplayer() &&
           item->state >= ITEM_STATE_TRANSFER_P1 && item->state <= ITEM_STATE_TRANSFER_P3;
}

i32 FixedItemTarget(const Item *item)
{
    if (!HasFixedItemTarget(item))
        return -1;
    return IsTransferItem(item)
               ? item->state - ITEM_STATE_TRANSFER_P1
               : item->state - ITEM_STATE_AUTO_P1;
}

i8 FixedItemState(u8 playerId)
{
    return static_cast<i8>(ITEM_STATE_AUTO_P1 + playerId);
}

i8 TransferItemState(u8 playerId)
{
    return static_cast<i8>(ITEM_STATE_TRANSFER_P1 + playerId);
}

Player *ItemTargetPlayer(Item *item)
{
    if (!MultiplayerGameplay::IsMultiplayer())
        return &g_Player;
    const i32 fixed = FixedItemTarget(item);
    if (fixed >= 0 && fixed < TH06_MULTI_MAX_PLAYERS &&
        ItemPlayerActive(&g_Players[fixed]))
        return &g_Players[fixed];
    return GetClosestActivePlayer(item ? &item->currentPosition : nullptr);
}

Player *AutoCollectTarget(Item *item)
{
    if (!MultiplayerGameplay::IsMultiplayer())
        return (ItemPlayerPower(&g_Player) >= 128 && g_Player.positionCenter.y < 128.0f)
                   ? &g_Player
                   : nullptr;

    u8 candidates[TH06_MULTI_MAX_PLAYERS] = {};
    i32 count = 0;
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        Player *player = &g_Players[playerId];
        if (ItemPlayerActive(player) && GetPlayerPower(playerId) >= 128 &&
            player->positionCenter.y < 128.0f)
            candidates[count++] = playerId;
    }
    if (count == 0)
        return nullptr;
    if (count == 1)
        return &g_Players[candidates[0]];
    const i32 itemIndex = item ? static_cast<i32>(item - g_ItemManager.items) : 0;
    return &g_Players[candidates[itemIndex % count]];
}
} // namespace

i32 GetItemAutoCollectStateForPlayer(u8 playerId)
{
    return playerId < TH06_MULTI_MAX_PLAYERS ? FixedItemState(playerId) : 0;
}


i32 GetItemTransferStateForPlayer(u8 playerId)
{
    return playerId < TH06_MULTI_MAX_PLAYERS ? TransferItemState(playerId) : 0;
}

bool ItemManager::CanSpawnItems(i32 count) const
{
    if (count <= 0)
        return true;
    i32 freeSlots = 0;
    for (i32 index = 0; index < ARRAY_SIZE_SIGNED(this->items) - 1; ++index)
    {
        if (!this->items[index].isInUse && ++freeSlots >= count)
            return true;
    }
    return false;
}
#endif

void ItemManager::SyncRenderState()
{
    for (Item &item : this->items)
    {
        if (!item.isInUse)
        {
            continue;
        }
        item.prevPosition = item.currentPosition;
        item.sprite.UpdatePrev();
    }
}

void ItemManager::OnUpdate()
{
    i32 iVar9;
    i32 iVar8;
    i32 itemScore;
    i32 idx3;
    i32 idx2;
    i32 idx;
    Item *curItem;
    f32 fVar5;
    f32 playerAngle;
    i32 itemAcquired;
    Player *targetPlayer;

    curItem = &this->items[0];
    itemAcquired = false;
    this->itemCount = 0;
    for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->items) - 1; idx++, curItem++)
    {
        if (!curItem->isInUse)
        {
            continue;
        }
        curItem->prevPosition = curItem->currentPosition;
        curItem->sprite.UpdatePrev();
        this->itemCount++;
        targetPlayer = &g_Player;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (HasFixedItemTarget(curItem))
        {
            const i32 fixed = FixedItemTarget(curItem);
            if (fixed < 0 || fixed >= TH06_MULTI_MAX_PLAYERS ||
                !ItemPlayerActive(&g_Players[fixed]))
            {
                if (IsTransferItem(curItem))
                    curItem->startPosition = ZunVec3(0.0f, -2.2f, 0.0f);
                curItem->state = 0;
            }
        }
        if (IsTransferItem(curItem))
        {
            const i32 fixed = FixedItemTarget(curItem);
            if (curItem->timer.current < ITEM_TRANSFER_RISE_FRAMES)
            {
                const f32 throwTime =
                    curItem->timer.AsFramesFloat() / static_cast<f32>(ITEM_TRANSFER_RISE_FRAMES);
                const f32 throwEase = 1.0f - powf(1.0f - throwTime, 1.5f);
                curItem->currentPosition =
                    curItem->targetPosition * throwEase +
                    curItem->startPosition * (1.0f - throwEase);
                curItem->timer.Tick();
                g_AnmManager->ExecuteScript(&curItem->sprite);
                continue;
            }
            curItem->state = FixedItemState(static_cast<u8>(fixed));
            curItem->startPosition = ZunVec3(0.0f, 0.0f, 0.0f);
        }
        targetPlayer = ItemTargetPlayer(curItem);
#endif
        if (curItem->state == 2)
        {
            if ((i32)(60 > curItem->timer.current))
            {
                fVar5 = curItem->timer.AsFramesFloat() / 60.0f;
                curItem->currentPosition = curItem->targetPosition * fVar5 + curItem->startPosition * (1.0f - fVar5);
                goto yolo;
            }
            else if ((i32)(curItem->timer.current == 60))
            {
                curItem->startPosition = ZunVec3(0.0f, 0.0f, 0.0f);
            }
        }
        else
        {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            Player *autoCollectTarget =
                (curItem->state == ITEM_STATE_AUTO_COLLECT || HasFixedItemTarget(curItem))
                    ? nullptr
                    : AutoCollectTarget(curItem);
            if (autoCollectTarget)
            {
                targetPlayer = autoCollectTarget;
                curItem->state = MultiplayerGameplay::IsMultiplayer()
                                     ? FixedItemState(targetPlayer->initParam)
                                     : ITEM_STATE_AUTO_COLLECT;
            }
            else if (curItem->state == ITEM_STATE_AUTO_COLLECT &&
                     MultiplayerGameplay::IsMultiplayer())
            {
                targetPlayer = GetClosestActivePlayer(&curItem->currentPosition);
                curItem->state = FixedItemState(targetPlayer->initParam);
            }

            if (curItem->state == ITEM_STATE_AUTO_COLLECT || HasFixedItemTarget(curItem))
            {
                targetPlayer = ItemTargetPlayer(curItem);
                playerAngle = targetPlayer->AngleToPlayer(&curItem->currentPosition);
                sincosmul(&curItem->startPosition, playerAngle, 8.0f);
            }
            else
#else
            if (curItem->state == 1 || (128 <= g_GameManager.currentPower && g_Player.positionCenter.y < 128.0f))
            {
                playerAngle = g_Player.AngleToPlayer(&curItem->currentPosition);
                sincosmul(&curItem->startPosition, playerAngle, 8.0f);
                curItem->state = 1;
            }
            else
#endif
            {
                curItem->startPosition.x = 0.0;
                curItem->startPosition.z = 0.0;
                if (curItem->startPosition.y < -2.2f)
                {
                    curItem->startPosition.y = -2.2f;
                }
            }
        }
        curItem->currentPosition += curItem->startPosition * g_Supervisor.effectiveFramerateMultiplier;
        if (g_GameManager.arcadeRegionSize.y + (f32)GAME_REGION_TOP <= curItem->currentPosition.y)
        {
            curItem->isInUse = 0;
            g_GameManager.DecreaseSubrank(3);
            continue;
        }
        if (curItem->startPosition.y < 3.0f)
        {
            curItem->startPosition.y += g_Supervisor.effectiveFramerateMultiplier * 0.03f;
        }
        else
        {
            curItem->startPosition.y = 3.0f;
        }
    yolo:
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        targetPlayer = ItemTargetPlayer(curItem);
#endif
        if (targetPlayer->CalcItemBoxCollision(&curItem->currentPosition, &g_ItemSize))
        {
            switch (curItem->itemType)
            {
            case ITEM_POWER_SMALL:
                if (ItemPlayerPower(targetPlayer) >= 128)
                {
                    g_GameManager.powerItemCountForScore++;
                    if ((u32)g_GameManager.powerItemCountForScore >= 31)
                    {
                        g_GameManager.powerItemCountForScore = 30;
                    }
                    itemScore = g_PowerItemScore[g_GameManager.powerItemCountForScore];
                    g_GameManager.AddScore(itemScore);
                    g_AsciiManager.CreatePopup1(&curItem->currentPosition, itemScore, itemScore >= 12800 ? -256 : -1);
                }
                else
                {
                    idx2 = 0;
                    while (ItemPlayerPower(targetPlayer) >= g_PowerUpThresholds[idx2])
                    {
                        idx2++;
                    }
                    iVar8 = idx2;
                    g_GameManager.powerItemCountForScore = 0;
                    SetItemPlayerPower(targetPlayer, ItemPlayerPower(targetPlayer) + 1);
                    if (ItemPlayerPower(targetPlayer) >= 128)
                    {
                        SetItemPlayerPower(targetPlayer, 128);
                        g_BulletManager.TurnAllBulletsIntoPoints();
                        g_Gui.ShowFullPowerMode(0);
                    }
                    g_GameManager.AddScore(10);
                    g_Gui.flags.flag2 = 2;
                    while (ItemPlayerPower(targetPlayer) >= g_PowerUpThresholds[idx2])
                    {
                        idx2++;
                    }
                    if (idx2 != iVar8)
                    {
                        g_AsciiManager.CreatePopup1(&curItem->currentPosition, -1, 0xff80c0ff);
                        g_SoundPlayer.PlaySoundByIdx(SOUND_POWERUP);
                    }
                    else
                    {
                        g_AsciiManager.CreatePopup1(&curItem->currentPosition, 10, COLOR_WHITE);
                    }
                }
                g_GameManager.IncreaseSubrank(1);
                break;
            case ITEM_POINT:
                switch (g_GameManager.difficulty)
                {
                case EASY:
                case NORMAL:
                    itemScore = calculatePointScore(curItem, 100000, 60000, 100);
                    g_AsciiManager.CreatePopup1(&curItem->currentPosition, itemScore, itemScore >= 100000 ? -256 : -1);
                    break;
                case HARD:
                    itemScore = calculatePointScore(curItem, 150000, 100000, 180);
                    g_AsciiManager.CreatePopup1(&curItem->currentPosition, itemScore, itemScore >= 150000 ? -256 : -1);
                    break;
                case LUNATIC:
                    itemScore = calculatePointScore(curItem, 200000, 150000, 270);
                    g_AsciiManager.CreatePopup1(&curItem->currentPosition, itemScore, itemScore >= 200000 ? -256 : -1);
                    break;
                case EXTRA:
                    itemScore = calculatePointScore(curItem, 300000, 200000, 400);
                    g_AsciiManager.CreatePopup1(&curItem->currentPosition, itemScore, itemScore >= 300000 ? -256 : -1);
                    break;
                }
                g_GameManager.score += itemScore;
                g_GameManager.pointItemsCollectedInStage++;
                g_GameManager.pointItemsCollected++;
                g_Gui.flags.flag4 = 2;
                if (curItem->currentPosition.y < 128.0f)
                {
                    g_GameManager.IncreaseSubrank(30);
                }
                else
                {
                    g_GameManager.IncreaseSubrank(3);
                }
                break;
            case ITEM_POWER_BIG:
                if (ItemPlayerPower(targetPlayer) >= 128)
                {
                    g_GameManager.powerItemCountForScore += 8;
                    if (31 <= (u32)g_GameManager.powerItemCountForScore)
                    {
                        g_GameManager.powerItemCountForScore = 30;
                    }
                    itemScore = g_PowerItemScore[g_GameManager.powerItemCountForScore];
                    g_GameManager.score += itemScore;
                    g_AsciiManager.CreatePopup1(&curItem->currentPosition, itemScore, itemScore >= 12800 ? -256 : -1);
                }
                else
                {
                    idx3 = 0;
                    while (ItemPlayerPower(targetPlayer) >= g_PowerUpThresholds[idx3])
                    {
                        idx3++;
                    }
                    iVar9 = idx3;
                    SetItemPlayerPower(targetPlayer, ItemPlayerPower(targetPlayer) + 8);
                    if (128 <= ItemPlayerPower(targetPlayer))
                    {
                        SetItemPlayerPower(targetPlayer, 128);
                        g_BulletManager.TurnAllBulletsIntoPoints();
                        g_Gui.ShowFullPowerMode(0);
                    }
                    g_Gui.flags.flag2 = 2;
                    g_GameManager.AddScore(10);
                    while (ItemPlayerPower(targetPlayer) >= g_PowerUpThresholds[idx3])
                    {
                        idx3++;
                    }
                    if (idx3 != iVar9)
                    {
                        g_AsciiManager.CreatePopup1(&curItem->currentPosition, -1, 0xff80c0ff);
                        g_SoundPlayer.PlaySoundByIdx(SOUND_POWERUP);
                    }
                    else
                    {
                        g_AsciiManager.CreatePopup1(&curItem->currentPosition, 10, COLOR_WHITE);
                    }
                }
                break;
            case ITEM_BOMB:
                if (ItemPlayerBombs(targetPlayer) < 8)
                {
                    SetItemPlayerBombs(targetPlayer, ItemPlayerBombs(targetPlayer) + 1);
                    g_Gui.flags.flag1 = 2;
                }
                g_GameManager.IncreaseSubrank(5);
                break;
            case ITEM_LIFE:
                if (ItemPlayerLives(targetPlayer) < 8)
                {
                    SetItemPlayerLives(targetPlayer, ItemPlayerLives(targetPlayer) + 1);
                    g_Gui.flags.flag0 = 2;
                }
                g_GameManager.IncreaseSubrank(200);
                g_SoundPlayer.PlaySoundByIdx(SOUND_1UP);
                break;
            case ITEM_FULL_POWER:
                if (ItemPlayerPower(targetPlayer) < 128)
                {
                    g_BulletManager.TurnAllBulletsIntoPoints();
                    g_Gui.ShowFullPowerMode(0);
                    g_SoundPlayer.PlaySoundByIdx(SOUND_POWERUP);
                    g_AsciiManager.CreatePopup1(&curItem->currentPosition, -1, 0xff80c0ff);
                }
                SetItemPlayerPower(targetPlayer, 128);
                g_GameManager.AddScore(1000);
                g_AsciiManager.CreatePopup1(&curItem->currentPosition, 1000, COLOR_WHITE);
                g_Gui.flags.flag2 = 2;
                break;
            case ITEM_POINT_BULLET:
                itemScore = (g_GameManager.grazeInStage / 3) * 10 + 500;
                if (targetPlayer->bombInfo.isInUse != 0)
                {
                    itemScore = 100;
                }
                g_GameManager.score += itemScore;
                g_AsciiManager.CreatePopup2(&curItem->currentPosition, itemScore, COLOR_WHITE);
                break;
            }
            curItem->isInUse = 0;
            itemAcquired = true;
            continue;
        }
        curItem->timer.Tick();
        g_AnmManager->ExecuteScript(&curItem->sprite);
    }
    if (itemAcquired)
    {
        g_SoundPlayer.PlaySoundByIdx(SOUND_15);
    }
    return;
}

void ItemManager::RemoveAllItems()
{
    Item *cursor;
    i32 idx;

    for (cursor = &this->items[0], idx = 0; idx < ARRAY_SIZE_SIGNED(this->items) - 1; idx += 1, cursor += 1)
    {
        if (!cursor->isInUse)
        {
            continue;
        }
        cursor->state = 1;
    }
    return;
}

void ItemManager::OnDraw()
{
    Item *curItem;
    i32 idx;
    i32 itemAlpha;

    curItem = &this->items[0];
    idx = 0;
    for (; idx < ARRAY_SIZE_SIGNED(this->items) - 1; idx++, curItem++)
    {
        if (curItem->isInUse == 0)
        {
            continue;
        }
        const ZunVec3 drawPosition = curItem->prevPosition.Lerp(curItem->currentPosition, g_RenderAlpha);
        const ZunVec3 savedPos = curItem->sprite.pos;
        const ZunVec3 savedPrevPos = curItem->sprite.prevPos;
        curItem->sprite.pos.x = g_GameManager.arcadeRegionTopLeftPos.x + drawPosition.x;
        curItem->sprite.pos.y = g_GameManager.arcadeRegionTopLeftPos.y + drawPosition.y;
        curItem->sprite.pos.z = 0.01f;
        if (curItem->currentPosition.y < -8.0f)
        {
            curItem->sprite.pos.y = g_GameManager.arcadeRegionTopLeftPos.y + 8.0f;
            if (curItem->unk_142 != 0)
            {
                g_AnmManager->SetActiveSprite(&curItem->sprite, curItem->itemType + 519);
                curItem->unk_142 = 0;
            }
            itemAlpha = 255 - (i32)(((8.0f - curItem->currentPosition.y) * 255.0f) / 128.0f);
            if (itemAlpha < 0x40)
            {
                itemAlpha = 0x40;
            }
            curItem->sprite.color = COLOR_SET_ALPHA3(curItem->sprite.color, itemAlpha);
        }
        else
        {
            if (curItem->unk_142 == 0)
            {
                g_AnmManager->SetActiveSprite(&curItem->sprite, curItem->itemType + 512);
                curItem->unk_142 = 1;
                curItem->sprite.color = COLOR_WHITE;
            }
        }
        curItem->sprite.prevPos = curItem->sprite.pos;
        g_AnmManager->DrawNoRotation(&curItem->sprite);
        curItem->sprite.pos = savedPos;
        curItem->sprite.prevPos = savedPrevPos;
    }
    return;
}
