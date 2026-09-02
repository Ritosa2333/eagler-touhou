#pragma once

#include "AnmVm.hpp"
#include "ZunTimer.hpp"
#include "inttypes.hpp"

// #include <d3dx8math.h>

enum ItemType // This enum is 1 byte in size on Enemy
{
    ITEM_POWER_SMALL,
    ITEM_POINT,
    ITEM_POWER_BIG,
    ITEM_BOMB,
    ITEM_FULL_POWER,
    ITEM_LIFE,
    ITEM_POINT_BULLET,
    ITEM_NO_ITEM = -1,
};

struct Item
{
    AnmVm sprite;
    ZunVec3 currentPosition;
    ZunVec3 prevPosition;
    ZunVec3 startPosition;
    ZunVec3 targetPosition;
    ZunTimer timer;
    i8 itemType;
    i8 isInUse;
    i8 unk_142;
    i8 state;
};

struct ItemManager
{
    ItemManager();
    void SpawnItem(const ZunVec3 *position, ItemType type, i32 state);
    // Enemy/ECL resource drops use this path. In multiplayer, LIFE and BOMB
    // are duplicated once per active player; direct SpawnItem callers such as
    // player transfers and bullet conversions keep their original semantics.
    void SpawnEnemyDrop(const ZunVec3 *position, ItemType type, i32 state);
    void SyncRenderState();
    void OnUpdate();
    void OnDraw();
    void RemoveAllItems();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    bool CanSpawnItems(i32 count) const;
#endif

    Item items[513];
    i32 nextIndex;
    u32 itemCount;
};

extern ItemManager g_ItemManager;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
i32 GetItemAutoCollectStateForPlayer(u8 playerId);
i32 GetItemTransferStateForPlayer(u8 playerId);
#endif
