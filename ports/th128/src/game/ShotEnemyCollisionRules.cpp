#include "ShotEnemyCollisionRules.hpp"
#include <algorithm>
#include <bit>
#include <cmath>

namespace th128::shotenemy {
namespace {
std::uint32_t word(const ShotImage& image, std::size_t offset) noexcept {
    return std::uint32_t(image[offset]) | std::uint32_t(image[offset + 1]) << 8 |
           std::uint32_t(image[offset + 2]) << 16 | std::uint32_t(image[offset + 3]) << 24;
}
void store(ShotImage& image, std::size_t offset, std::uint32_t value) noexcept {
    for (std::size_t index = 0; index < 4; ++index)
        image[offset + index] = std::uint8_t(value >> (8 * index));
}
float scalar(const ShotImage& image, std::size_t offset) noexcept {
    return std::bit_cast<float>(word(image, offset));
}
std::int32_t signedWord(std::uint32_t value) noexcept {
    return std::bit_cast<std::int32_t>(value);
}
bool edges(const std::array<float, 3>& position, const std::array<float, 2>& size,
           std::array<float, 4>& result) noexcept {
    if (!std::all_of(position.begin(), position.end(), [](float value) { return std::isfinite(value); }) ||
        !std::all_of(size.begin(), size.end(), [](float value) { return std::isfinite(value); }))
        return false;
    result = {float(double(position[0]) - double(size[0]) * .5),
              float(double(position[1]) - double(size[1]) * .5),
              float(double(position[0]) + double(size[0]) * .5),
              float(double(position[1]) + double(size[1]) * .5)};
    return std::all_of(result.begin(), result.end(), [](float value) { return std::isfinite(value); });
}
}

ShotEnemyContactPlan planOrdinaryShotEnemyContact(
    const EnemyContactShape& enemy, const ShotImage& shot,
    const std::optional<ShotCollisionRecordBinding>& record) noexcept {
    ShotEnemyContactPlan result;
    result.next = shot;
    const auto active = word(shot, 0x48);
    if (active == 0 || active == 2)
        return result;
    const auto identity = word(shot, 0x74);
    if (!record || record->recordIdentity != identity) {
        result.continuation = ContactContinuation::NeedsRecordBinding;
        result.required = {NativeCallKind::ResolveRecord, 0x43eb3a, 0, identity};
        return result;
    }
    if (record->kind != 0) {
        result.continuation = ContactContinuation::NeedsShotKind;
        result.required = {NativeCallKind::UnsupportedShotKind, 0x43eb6f, 0, identity};
        return result;
    }
    const std::array<float, 3> position{scalar(shot, 0x14), scalar(shot, 0x18), scalar(shot, 0x1c)};
    const std::array<float, 2> size{scalar(shot, 0x68), scalar(shot, 0x6c)};
    if (!edges(enemy.combinedPosition, enemy.fullSize, result.enemyEdges) || !edges(position, size, result.shotEdges)) {
        result.continuation = ContactContinuation::InvalidGeometry;
        return result;
    }
    const auto& target = result.enemyEdges;
    const auto& moving = result.shotEdges;
    if (target[3] < moving[1] || target[2] < moving[0] || target[1] > moving[3] ||
        target[0] > moving[2] || target[3] < 0) {
        result.continuation = ContactContinuation::Miss;
        result.damageQueryCompleted = true;
        return result;
    }
    if (!record->nativeCollisionTarget) {
        result.continuation = ContactContinuation::NeedsRecordBinding;
        result.required = {NativeCallKind::ResolveRecord, 0x43ebfb, 0, identity};
        return result;
    }
    if (*record->nativeCollisionTarget != 0) {
        result.continuation = ContactContinuation::NeedsCollisionCallback;
        result.required = {NativeCallKind::CollisionCallback, 0x43ec10, *record->nativeCollisionTarget, identity};
        return result;
    }
    store(result.next, 0x58, 1);
    result.hitMarkerWritten = true;
    result.damagePrefix = signedWord(word(shot, 0x60));
    result.continuation = ContactContinuation::NeedsAnimationCleanup;
    result.required = {NativeCallKind::AnimationLookup, 0x43ec8a, 0x467780, identity, 0, word(shot, 0x4c)};
    return result;
}

ShotEnemyQueryPlan planShotEnemyQuery(const ShotEnemyQueryInput& input) noexcept {
    ShotEnemyQueryPlan result;
    result.outFlag = input.outFlagBefore;
    if (input.previousPlayerTime == input.currentPlayerTime) {
        result.continuation = QueryContinuation::Complete;
        return result;
    }
    std::array<float, 4> target;
    if (!edges(input.enemy.combinedPosition, input.enemy.fullSize, target)) {
        result.continuation = QueryContinuation::InvalidGeometry;
        return result;
    }
    if (input.hasOutFlag) {
        result.outFlag = 0;
        result.outFlagWritten = true;
    }
    if (input.shots.size() != ShotSlotCount)
        return result;
    for (std::uint32_t index = 0; index < ShotSlotCount; ++index) {
        ++result.examinedShots;
        const auto& shot = input.shots[index];
        const auto active = word(shot, 0x48);
        if (active == 0 || active == 2)
            continue;
        const auto identity = word(shot, 0x74);
        std::optional<ShotCollisionRecordBinding> binding;
        for (const auto& candidate : input.records)
            if (candidate.recordIdentity == identity) {
                if (binding) {
                    binding.reset();
                    break;
                }
                binding = candidate;
            }
        const auto contact = planOrdinaryShotEnemyContact(input.enemy, shot, binding);
        if (contact.continuation == ContactContinuation::Miss)
            continue;
        result.continuation = contact.continuation == ContactContinuation::InvalidGeometry
            ? QueryContinuation::InvalidGeometry : QueryContinuation::NeedsSelectedContact;
        result.selectedShot = index;
        result.selectedContact = contact;
        result.required = contact.required;
        result.required.slot = index;
        return result;
    }
    if (!input.damageSourceFlags || input.damageSourceFlags->size() != DamageSourceCount) {
        result.continuation = QueryContinuation::NeedsDamageSourceSnapshot;
        result.required = {NativeCallKind::DamageSourceSnapshot, 0x43fd47};
        return result;
    }
    for (std::uint32_t index = 0; index < DamageSourceCount; ++index) {
        ++result.examinedDamageSources;
        if ((*input.damageSourceFlags)[index] & 1u) {
            result.continuation = QueryContinuation::NeedsDamageSourceContact;
            result.required = {NativeCallKind::DamageSourceContact, 0x43fd53, 0, 0, index};
            return result;
        }
    }
    result.continuation = QueryContinuation::Complete;
    return result;
}

std::optional<CompletedShotContact> completeOrdinaryShotContactAnimation(
    const ShotEnemyContactPlan& pending,std::uint32_t genuineReplacementHandle) noexcept {
    if(pending.continuation!=ContactContinuation::NeedsAnimationCleanup||!pending.hitMarkerWritten||
       word(pending.next,0x58)!=1||word(pending.next,0x48)==0||word(pending.next,0x48)==2||
       !genuineReplacementHandle||!std::isfinite(scalar(pending.next,0x2c)))return std::nullopt;
    CompletedShotContact result{pending.next,pending.damagePrefix};
    store(result.next,0x4c,genuineReplacementHandle);
    store(result.next,0x1c,std::bit_cast<std::uint32_t>(.1f));
    store(result.next,0x48,2);
    store(result.next,0x2c,std::bit_cast<std::uint32_t>(float(double(scalar(result.next,0x2c))*.125)));
    return result;
}

std::optional<ShotQueryImpactPlan> planShotQueryImpact(int value,int flag) noexcept {
    if(value>100000)return std::nullopt;
    if(flag)return ShotQueryImpactPlan{value,flag,false};
    const int next=value+2;
    if(next/10000>value/10000)return ShotQueryImpactPlan{std::min(next,100000),flag,true};
    return ShotQueryImpactPlan{std::min(next,100000),next<0?1:flag,false};
}

EnemyDamagePlan planEnemyDamageAfterCompletedQuery(
    const ShotEnemyQueryPlan& query, const EnemyDamageSnapshot& enemy) noexcept {
    EnemyDamagePlan result;
    result.nextLifeWords = enemy.lifeWords;
    result.nextManagerStats = enemy.managerStats;
    if (query.continuation != QueryContinuation::Complete)
        return result;
    const auto damage = enemy.playerState == 0 || enemy.playerState == 2
        ? query.completedDamage / 5 : query.completedDamage;
    result.playerScaledDamage = result.hpDamage = damage;
    if (enemy.controlDamageBypass != 0) {
        result.continuation = DamageContinuation::ControlBypass;
        return result;
    }
    if (damage > 0) {
        if (query.outFlag == 0)
            result.nextManagerStats[1] += std::uint32_t(damage);
        else {
            const auto life = signedWord(enemy.lifeWords[0]);
            const auto increment = damage < life ? std::uint32_t(damage)
                : std::uint32_t(signedWord(std::uint32_t(damage) - enemy.lifeWords[0]) / 4) + enemy.lifeWords[0];
            result.nextManagerStats[0] += increment;
        }
    }
    if (damage == 0) {
        result.continuation = DamageContinuation::ZeroDamage;
        return result;
    }
    if (!enemy.spellModeFlags) {
        result.continuation = DamageContinuation::NeedsSpellModeSnapshot;
        result.required = {NativeCallKind::SpellModeSnapshot, 0x416619, 0x414e50};
        return result;
    }
    if ((*enemy.spellModeFlags & 0x41u) == 0x41u)
        result.hpDamage /= 30;
    result.hpSuppressed = (enemy.enemyFlags & 0x10u) != 0 || enemy.protection > 0;
    if (!result.hpSuppressed) {
        auto& life = result.nextLifeWords;
        if (life[5] & 1u) {
            life[3] -= std::uint32_t(result.hpDamage);
            life[0] = std::uint32_t(signedWord(life[3] - life[4] * 7u) / 7) + life[4];
        } else
            life[0] -= std::uint32_t(result.hpDamage);
        result.lifeWritten = true;
    }
    result.deathCandidate = signedWord(result.nextLifeWords[0]) <= 0 && (enemy.enemyFlags & 0x80u) == 0;
    result.continuation = DamageContinuation::NeedsNativeContinuation;
    result.required = {NativeCallKind::PostDamageScript, 0x41665a, 0x41dea0};
    return result;
}
}
