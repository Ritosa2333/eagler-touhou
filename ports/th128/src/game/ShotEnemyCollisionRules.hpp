#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <span>

namespace th128::shotenemy {

inline constexpr std::uint32_t ShotPoolOffset = 0xfa8;
inline constexpr std::uint32_t ShotSlotBytes = 0x78;
inline constexpr std::uint32_t ShotSlotCount = 256;
inline constexpr std::uint32_t DamageSourcePoolOffset = 0x8edc;
inline constexpr std::uint32_t DamageSourceCount = 2000;
using ShotImage = std::array<std::uint8_t, ShotSlotBytes>;

struct EnemyContactShape {
    std::array<float, 3> combinedPosition{};
    std::array<float, 2> fullSize{};
};

struct ShotCollisionRecordBinding {
    std::uint32_t recordIdentity = 0;
    std::uint8_t kind = 0;
    std::optional<std::uint32_t> nativeCollisionTarget;
};

enum class NativeCallKind {
    None, ResolveRecord, UnsupportedShotKind, CollisionCallback, AnimationLookup,
    DamageSourceSnapshot, DamageSourceContact, SpellModeSnapshot, PostDamageScript, Death
};

struct RequiredNativeCall {
    NativeCallKind kind = NativeCallKind::None;
    std::uint32_t callSite = 0;
    std::uint32_t target = 0;
    std::uint32_t recordIdentity = 0;
    std::uint32_t slot = 0;
    std::uint32_t handle = 0;
};

struct NativeCallBoundary {
    std::uint32_t callSite = 0;
    std::uint32_t target = 0;
};

inline constexpr std::array<NativeCallBoundary, 5> OrdinaryContactAnimationCalls{{
    {0x43ec8a, 0x467780}, {0x43ec95, 0x4679b0}, {0x43ed03, 0x4670b0},
    {0x43ed0d, 0x467780}, {0x43ed43, 0x426f10}
}};
inline constexpr NativeCallBoundary PositiveQueryImpactCall{0x43fcb3, 0x427260};
inline constexpr NativeCallBoundary EnemyDeathCall{0x416715, 0x417370};

enum class ContactContinuation {
    Inactive, Miss, NeedsRecordBinding, NeedsShotKind, NeedsCollisionCallback,
    NeedsAnimationCleanup, InvalidGeometry
};

struct ShotEnemyContactPlan {
    ContactContinuation continuation = ContactContinuation::Inactive;
    ShotImage next{};
    std::array<float, 4> enemyEdges{};
    std::array<float, 4> shotEdges{};
    std::int32_t damagePrefix = 0;
    bool hitMarkerWritten = false;
    bool damageQueryCompleted = false;
    RequiredNativeCall required;
};

ShotEnemyContactPlan planOrdinaryShotEnemyContact(
    const EnemyContactShape& enemy, const ShotImage& shot,
    const std::optional<ShotCollisionRecordBinding>& record = std::nullopt) noexcept;

struct CompletedShotContact {
    ShotImage next{};
    std::int32_t completedDamage=0;
};
std::optional<CompletedShotContact> completeOrdinaryShotContactAnimation(
    const ShotEnemyContactPlan& pending,std::uint32_t genuineReplacementHandle) noexcept;
struct ShotQueryImpactPlan {
    int value=0,flag=0;
    bool soundRequired=false;
};
std::optional<ShotQueryImpactPlan> planShotQueryImpact(int value,int flag) noexcept;

struct ShotEnemyQueryInput {
    EnemyContactShape enemy;
    std::uint32_t previousPlayerTime = 0;
    std::uint32_t currentPlayerTime = 0;
    bool hasOutFlag = true;
    std::uint32_t outFlagBefore = 0;
    std::span<const ShotImage> shots;
    std::span<const ShotCollisionRecordBinding> records;
    std::optional<std::span<const std::uint32_t>> damageSourceFlags;
};

enum class QueryContinuation {
    Complete, NeedsShotPoolSnapshot, NeedsSelectedContact, NeedsDamageSourceSnapshot,
    NeedsDamageSourceContact, InvalidGeometry
};

struct ShotEnemyQueryPlan {
    QueryContinuation continuation = QueryContinuation::NeedsShotPoolSnapshot;
    std::int32_t completedDamage = 0;
    std::uint32_t outFlag = 0;
    bool outFlagWritten = false;
    std::uint32_t examinedShots = 0;
    std::uint32_t examinedDamageSources = 0;
    std::optional<std::uint32_t> selectedShot;
    ShotEnemyContactPlan selectedContact;
    RequiredNativeCall required;
};

ShotEnemyQueryPlan planShotEnemyQuery(const ShotEnemyQueryInput& input) noexcept;

struct EnemyDamageSnapshot {
    std::int32_t playerState = 0;
    std::int32_t controlDamageBypass = 0;
    std::uint32_t enemyFlags = 0;
    std::int32_t protection = 0;
    std::array<std::uint32_t, 6> lifeWords{};
    std::array<std::uint32_t, 2> managerStats{};
    std::optional<std::uint32_t> spellModeFlags;
};

enum class DamageContinuation {
    NeedsCompletedQuery, ControlBypass, ZeroDamage, NeedsSpellModeSnapshot, NeedsNativeContinuation
};

struct EnemyDamagePlan {
    DamageContinuation continuation = DamageContinuation::NeedsCompletedQuery;
    std::array<std::uint32_t, 6> nextLifeWords{};
    std::array<std::uint32_t, 2> nextManagerStats{};
    std::int32_t playerScaledDamage = 0;
    std::int32_t hpDamage = 0;
    bool hpSuppressed = false;
    bool lifeWritten = false;
    bool deathCandidate = false;
    RequiredNativeCall required;
};

EnemyDamagePlan planEnemyDamageAfterCompletedQuery(
    const ShotEnemyQueryPlan& completedQuery, const EnemyDamageSnapshot& enemy) noexcept;

}
