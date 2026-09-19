#pragma once

#include "AnmVm.hpp"
#include "Chain.hpp"
#include "EffectManager.hpp"
#include "GameManager.hpp"
#include "inttypes.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "Multiplayer.hpp"
#endif

extern const char *g_ShooterTable[6];
extern const char *g_ShooterTableFocus[6];

typedef void (*BombCallback)(struct Player *);

typedef enum PlayerState
{
    PLAYER_STATE_ALIVE = 0,
    PLAYER_STATE_SPAWNING = 1,
    PLAYER_STATE_DEAD = 2,
    PLAYER_STATE_INVULNERABLE = 3,
    PLAYER_STATE_BORDER = 4,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Imported multiplayer gameplay states.  They exist only in netplay
    // builds so an ordinary Eagler build retains the original state surface.
    PLAYER_STATE_SPIRIT = 5,
    PLAYER_STATE_ELIMINATED = 6
#endif
} PlayerState;

typedef enum PlayerDirection
{
    MOVEMENT_NONE = 0,
    MOVEMENT_UP = 1,
    MOVEMENT_DOWN = 2,
    MOVEMENT_LEFT = 3,
    MOVEMENT_RIGHT = 4,
    MOVEMENT_UP_LEFT = 5,
    MOVEMENT_UP_RIGHT = 6,
    MOVEMENT_DOWN_LEFT = 7,
    MOVEMENT_DOWN_RIGHT = 8
} PlayerDirection;

typedef enum OptionState
{
    OPTION_HIDDEN = 0,
    OPTION_UNFOCUSED = 1,
    OPTION_FOCUSING = 2,
    OPTION_FOCUSED = 3,
    OPTION_UNFOCUSING = 4
} OptionState;

typedef enum BorderState
{
    BORDER_NONE = 0,
    BORDER_ACTIVE = 1,
    BORDER_READY = 2
} BorderState;

enum PlayerCollisionResult
{
    PLAYER_COLLISION_NONE = 0,
    PLAYER_COLLISION_HIT = 1,
    PLAYER_COLLISION_BOMB = 2,
};

struct BombProjectile
{
    ZunVec3 pos;
    ZunVec3 size;
    i32 lifetime;
    union {
        i32 itemType;
        i32 damage;
    };
};

struct BombClearBox
{
    Float2 pos;
    Float2 size;
    f32 radius;
    f32 radiusGrowth;
    i32 lifetime;
    union {
        i32 itemType;
        i32 damage;
    };
};

struct CachedBombClearBox
{
    bool isBox;
    f32 minX, maxX;
    f32 minY, maxY;
    f32 cx, cy;
    f32 radiusSq;
    i32 itemType;
};

struct PlayerBombSubInfo
{
    i32 state;
    i32 counter;
    f32 custom;
    f32 prevCustom;
    f32 speed;
    f32 angle;
    ZunVec3 pos;
    ZunVec3 prevPos;
    ZunVec3 posHistory[32];
    ZunVec3 velocity;
    ZunVec3 accel;
    AnmVm vms[8];
    Effect *effect;
    ZunTimer timer;
};

struct PlayerBombInfo
{
    static void SubtractCherryDrain(i32 cherryDrain)
    {
        if (g_GameManager.cherry - g_GameManager.globals->cherryStart >= cherryDrain)
        {
            g_GameManager.cherry -= cherryDrain;
        }
        else
        {
            g_GameManager.cherry = g_GameManager.globals->cherryStart;
        }
    }

    ZunBool isInUse;
    ZunBool isFocus;
    i32 bombDuration;
    i32 cherryDrain;
    ZunTimer bombTimer;
    BombCallback bombCalc;
    BombCallback draw;
    BombCallback bombFocusCalc;
    BombCallback drawFocus;
    PlayerBombSubInfo subInfo[128];
};

struct PlayerBullet
{
    f32 *GetPosX()
    {
        return &this->pos.x;
    }

    f32 *GetPosY()
    {
        return &this->pos.y;
    }

    f32 *GetVmPosX()
    {
        return &this->vm.pos.x;
    }

    f32 *GetVmPosY()
    {
        return &this->vm.pos.y;
    }

    AnmVm vm;
    ZunVec3 pos;
    ZunVec3 prevPos;
    ZunVec3 posHistory[16];
    ZunVec3 hitboxSize;
    Float2 velocity;
    Float2 offset;
    f32 speed;
    f32 angle;
    f32 prevAngle;
    ZunTimer timer;
    i16 damage;
    i16 bulletState;
    i16 bulletState2;
    i16 timerIdx;
    i16 optionId;
    i16 trailLength;
    i32 (*updateCallback)(struct Player *, struct PlayerBullet *);
    i32 (*drawCallback)(struct Player *, struct PlayerBullet *);
    i32 (*hitCallback)(struct Player *, struct PlayerBullet *, ZunVec3 *);
    struct ShtEntry *shtEntry;
};

struct PlayerBulletTimer
{
    ZunTimer timer;
    PlayerBullet *bullet;
};

struct Player
{
    static ZunResult RegisterChain(u32 param_1);
    static void CutChain();

    static ZunResult AddedCallback(Player *arg);
    static ZunResult DeletedCallback(Player *arg);
    static u32 OnUpdate(Player *arg);
    static u32 OnDrawHighPrio(Player *arg);
    static u32 OnDrawLowPrio(Player *arg);

    void UpdateBombProjectiles();
    void UpdateBorderAndBombState();
    i32 UpdateDeath();
    void UpdateState();
    void UpdateShots();
    i32 UpdateFireBulletTimer();
    void UpdateUI();

    void DrawBullets();
    void DrawBulletExplosions();

    void ActivateBorder();
    f32 AngleToPlayer(ZunVec3 *pos);
    void BreakBorder();
    void BreakBorderNaturally();

    i32 CalcItemBoxCollision(ZunVec3 *center, ZunVec3 *size);
    i32 CalcKillboxCollision(ZunVec3 *center, ZunVec3 *size);
    i32 CalcLaserHitbox(ZunVec3 *center, ZunVec3 *size, ZunVec3 *origin, f32 rotation,
                        ZunBool canGraze);
    i32 CalcBombCollision(ZunVec3 *center, ZunVec3 *size);
    i32 CalcDamageToEnemy(ZunVec3 *param_1, ZunVec3 *param_2, i32 *param_3);
    i32 CheckGraze(ZunVec3 *center, ZunVec3 *size);

    void Die();
    i32 HandlePlayerInputs();
    void Respawn();
    void ScoreGraze(ZunVec3 *param_1);
    BombClearBox *SpawnGrowingBomb(ZunVec3 *pos, f32 radius, f32 radiusGrowth, i32 lifetime,
                                   i32 itemType);
    BombClearBox *SpawnBombProjectile(ZunVec3 *centerPosition, f32 sizeX, f32 sizeY, i32 itemType);
    static void SpawnBullets(Player *player, u32 timer);
    void StartFireBulletTimer();

    void RebuildBombBoxCache();

    void SetToTopLeftPos(AnmVm *vm)
    {
        vm->pos.x += g_GameManager.arcadeRegionTopLeftPos.x;
        vm->pos.y += g_GameManager.arcadeRegionTopLeftPos.y;
        vm->pos.z = 0.0f;
    }

    ZunTimer *GetBombTimer()
    {
        ZunTimer *timer = &this->bombInfo.bombTimer;
        return timer;
    }

    static void SetVecCorners(ZunVec3 *topLeft, ZunVec3 *bottomRight, ZunVec3 *center,
                              ZunVec3 *size)
    {
        topLeft->x = center->x - size->x * 0.5f;
        topLeft->y = center->y - size->y * 0.5f;
        bottomRight->x = center->x + size->x * 0.5f;
        bottomRight->y = center->y + size->y * 0.5f;
    }

    f32 *GetPosX()
    {
        return &this->pos.x;
    }

    f32 *GetPosY()
    {
        return &this->pos.y;
    }

    void SetFocusEffect(Effect *effect)
    {
        this->focusEffect = effect;
    }

    void UpdatePrev()
    {
        this->playerSprite.UpdatePrev();
        this->optionsSprite[0].UpdatePrev();
        this->optionsSprite[1].UpdatePrev();
        for (i32 i = 0; i < 96; i++)
        {
            if (this->bullets[i].bulletState != 0)
            {
                this->bullets[i].vm.UpdatePrev();
            }
        }
        if (this->bombInfo.isInUse)
        {
            for (i32 i = 0; i < 128; i++)
            {
                this->bombInfo.subInfo[i].prevPos = this->bombInfo.subInfo[i].pos;
                this->bombInfo.subInfo[i].prevCustom = this->bombInfo.subInfo[i].custom;
                for (i32 j = 0; j < 8; j++)
                {
                    this->bombInfo.subInfo[i].vms[j].UpdatePrev();
                }
            }
        }
    }

    AnmVm playerSprite;
    AnmVm optionsSprite[3];
    ZunVec3 pos;
    ZunVec3 prevPos;
    ZunVec3 hitboxTopLeft;
    ZunVec3 hitboxBottomRight;
    ZunVec3 grazeTopLeft;
    ZunVec3 grazeBottomRight;
    ZunVec3 grabItemTopLeft;
    ZunVec3 grabItemBottomRight;
    ZunVec3 hitboxSize;
    ZunVec3 grazeSize;
    ZunVec3 grabItemSize;
    ZunVec3 optionsPosition[2];
    ZunVec3 prevOptionsPosition[2];
    Float2 velocity;
    i32 unused_9d4;
    Effect *focusEffect;
    Effect *eaglerHitboxEffect;
    BombProjectile bombDamageBoxes[112];
    BombClearBox bombClearBoxes[96];
    CachedBombClearBox activeBombClearBoxesCache[96];
    i32 numActiveBombClearBoxes;
    bool dirtyBombBoxes;
    ZunBool isBombing;
    ShtEntry *shtEntries[4];
    f32 horizontalMovementSpeedMultiplierDuringBomb;
    f32 verticalMovementSpeedMultiplierDuringBomb;
    i32 respawnTimer;
    i32 borderInvulnerabilityTime;
    i32 bulletGracePeriod;
    i32 itemType;
    i8 playerState;
    u8 initParam;
    i8 optionState;
    i8 isFocus;
    u8 bombParticleTime;
    i8 hasBorder;
    // pad 2
    ZunTimer focusMovementTimer;
    PlayerDirection playerDirection;
    f32 previousHorizontalSpeed;
    f32 previousVerticalSpeed;
    ZunVec3 positionOfLastEnemyHit;
    ZunVec3 sakuyaTargetPosition;
    ZunBool targetingEnemy;
    PlayerBullet bullets[96];
    PlayerBulletTimer timers[3];
    ZunTimer fireBulletTimer;
    ZunTimer invulnerabilityTimer;
    ZunTimer borderTimer;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Reuse the original two unused words so multiplayer life-transfer state
    // does not grow Player or disturb the surrounding Eagler additions.
    i32 lifeGiveTimer;
    i32 lifeGiveTargetToken;
#else
    i32 unused_16a18;
    i32 unused_16a1c;
#endif
    PlayerBombInfo bombInfo;
    ZunVec3 bombStartPos;
    f32 optionAngle;
    ChainElem *calcChain;
    ChainElem *drawChain1;
    ChainElem *drawChain2;
    Effect *effect;
    Effect *borderEffect;
    struct ShtData *shooterData;
    struct ShtData *shooterDataFocus;
};

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
extern Player g_Players[TH07_MULTI_MAX_PLAYERS];
extern bool g_PlayerActive[TH07_MULTI_MAX_PLAYERS];
extern i32 g_cherryMaxGrazeGrowth[TH07_MULTI_MAX_PLAYERS];
extern i32 g_cherryMaxBreakGrowth[TH07_MULTI_MAX_PLAYERS];

// Compatibility aliases keep the decompiled single-player code readable.
// Multiplayer-specific code should prefer slot-indexed helpers below.
#define g_Player (g_Players[0])
#define g_Player2 (g_Players[1])
#define g_Player3 (g_Players[2])
#define g_Player2Active (g_PlayerActive[1])
#define g_Player3Active (g_PlayerActive[2])

Player *GetPlayerById(u8 playerId);
const Player *GetPlayerByIdConst(u8 playerId);
bool IsPlayerSlotActive(u8 playerId);

constexpr i32 POWER_GIVE_TAPS_REQUIRED = 8;
constexpr i32 POWER_GIVE_TAP_WINDOW = 24;
constexpr i32 POWER_GIVE_AMOUNT = 20;
constexpr i32 POWER_GIVE_PROMPT_AFTER = 4;
extern i32 g_powerGiveTaps[TH07_MULTI_MAX_PLAYERS];
extern i32 g_powerGiveWindow[TH07_MULTI_MAX_PLAYERS];
extern i32 g_teamWipeRetryFrames;

u8 GetActivePlayerMask();
i32 GetActivePlayerCount();
void UpdateTeamWipeRetryCountdown();
bool IsAnyActivePlayerBombing();
bool VerifyThreePlayerLifeTransferSelectionRules();
Player *GetClosestActivePlayer(ZunVec3 *position);
u8 GetPlayerOverlapAlpha(const Player *player);
ZunVec3 GetPlayerPresentationOffset(u8 playerId);
bool IsSharedBorderActive();
void ActivateSharedBorder();
i32 GetPlayerAnmScript(const Player *player, i32 script);
i32 GetPlayerEffectSlot(const Player *player, i32 p1Slot);
#else
extern Player g_Player;
#endif

typedef i32 (*ShtFunc1)(Player *, PlayerBullet *, i32, struct ShtEntry *);
extern ShtFunc1 g_ShtFireFuncs[6];
typedef i32 (*ShtFunc2)(Player *, PlayerBullet *);
extern ShtFunc2 g_ShtUpdateFuncs[6];
typedef i32 (*ShtFunc3)(Player *, PlayerBullet *);
extern ShtFunc3 g_ShtDrawFuncs[2];
typedef i32 (*ShtFunc4)(Player *, PlayerBullet *, ZunVec3 *);
extern ShtFunc4 g_ShtHitFuncs[4];

struct ShtEntry
{
    i16 fireInterval;
    i16 fireOffset;
    Float2 offset;
    Float2 hitboxSize;
    f32 angle;
    f32 speed;
    i16 damage;
    i8 option;
    i8 bulletState2;
    i16 anmFileIdx;
    i16 soundIdx;
    i32 (*fireCallback)(Player *, PlayerBullet *, i32, struct ShtEntry *);
    i32 (*updateCallback)(Player *, PlayerBullet *);
    i32 (*drawCallback)(Player *, PlayerBullet *);
    i32 (*hitCallback)(Player *, PlayerBullet *, ZunVec3 *);
};

struct ShtLevel
{
    ShtEntry *entry;
    i32 requiredPower;
};

struct ShtData
{
    static ZunResult LoadShtData(ShtData **data, const char *shtPath);
    static i32 FireBulletDefault(Player *player, PlayerBullet *bullet, i32 fireTime,
                                 ShtEntry *shtEntry);
    static i32 FireOrbBulletUnfocused(Player *player, PlayerBullet *bullet, i32 fireTime,
                                      ShtEntry *shtEntry);
    static i32 FireOrbBulletFocused(Player *player, PlayerBullet *bullet, i32 fireTime,
                                    ShtEntry *shtEntry);
    static i32 FireHomingBullet(Player *player, PlayerBullet *bullet, i32 fireTime,
                                ShtEntry *shtEntry);
    static i32 FireRotatingOrbBullet(Player *player, PlayerBullet *bullet, i32 fireTime,
                                     ShtEntry *shtEntry);

    static i32 UpdateHomingBullet(Player *player, PlayerBullet *bullet);
    static i32 UpdateHomingBulletFocused(Player *player, PlayerBullet *bullet);
    static i32 UpdateUpwardAcceleratingBullet(Player *player, PlayerBullet *bullet);
    static i32 UpdateOrbLaser(Player *player, PlayerBullet *bullet);
    static i32 UpdatePlayerLaser(Player *player, PlayerBullet *bullet);

    static i32 DrawBulletWithTrail(Player *player, PlayerBullet *bullet);

    static i32 OnMissileHit(Player *player, PlayerBullet *bullet, ZunVec3 *pos);
    static i32 SpawnHitParticles(Player *player, PlayerBullet *bullet, ZunVec3 *pos);

    i16 unused;
    u16 numLevels;
    f32 initialBombs;
    i32 initialRespawnTimer;
    f32 hitboxRadius;
    f32 grabItemRadius;
    f32 itemCollectSpeed;
    f32 itemCollectRadius;
    f32 cherryPenaltyMultiplier;
    f32 pocY;
    f32 speed;
    f32 speedFocus;
    f32 speedDiagonal;
    f32 speedDiagonalFocus;
    ShtLevel *levels;
    ShtEntry *entries;
};

struct ShtRawEntry
{
    i16 fireInterval;
    i16 fireOffset;
    Float2 offset;
    Float2 hitboxSize;
    f32 angle;
    f32 speed;
    i16 damage;
    i8 option;
    i8 bulletState2;
    i16 anmFileIdx;
    i16 soundIdx;
    u32 fireCallback;
    u32 updateCallback;
    u32 drawCallback;
    u32 hitCallback;
};
static_assert(sizeof(ShtRawEntry) == 0x34);

struct ShtRawLevel
{
    u32 entryOffset;
    i32 requiredPower;
};
static_assert(sizeof(ShtRawLevel) == 0x8);

struct ShtRawData
{
    i16 numLevels;
    u16 entryCount;
    f32 initialBombs;
    i32 initialRespawnTimer;
    f32 hitboxRadius;
    f32 grabItemRadius;
    f32 itemCollectSpeed;
    f32 itemCollectRadius;
    f32 cherryPenaltyMultiplier;
    f32 pocY;
    f32 speed;
    f32 speedFocus;
    f32 speedDiagonal;
    f32 speedDiagonalFocus;
    ShtRawLevel levels[];
};
static_assert(sizeof(ShtRawData) == 0x34);
