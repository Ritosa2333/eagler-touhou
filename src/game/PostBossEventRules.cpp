#include "PostBossEventRules.hpp"

#include <bit>
#include <cstdint>
#include <utility>

namespace th128::postboss {
namespace {

std::int32_t wrapped(std::int64_t value) {
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(static_cast<std::uint64_t>(value)));
}

bool playableStageInBounds(int stage) {
    return stage >= kMinimumPlayableStage && stage <= kMaximumPlayableStage;
}

bool timeScaleStageInBounds(int stage) {
    return stage >= kMinimumTimeScaleStage && stage <= kMaximumTimeScaleStage;
}

bool settlementInBounds(const Input& input) {
    const int statsLimit =
        input.stage == kExtraStage ? kMaximumStatsStage : kMaximumRouteFinalStatsStage;
    return input.statsStage >= kMinimumStatsStage && input.statsStage <= statsLimit
           && input.difficulty >= 0 && input.difficulty <= kMaximumDifficulty
           && input.playCount >= 0 && input.playCount <= kPlayCountCap;
}

std::int32_t routeFinalBonus(const Input& input) {
    const std::int32_t q = input.rateB / 100;
    std::int32_t bonus = wrapped(std::int64_t(q - q % 10) * 100);
    if (input.difficulty >= 0 && input.difficulty <= 4)
        bonus = wrapped(std::int64_t(bonus)
                        + wrapped(std::int64_t(input.rateA / 100)
                                  * kDifficultyMultipliers[input.difficulty]));
    return bonus;
}

std::int32_t extraStageBonus(const Input& input) {
    return wrapped(std::int64_t(wrapped(std::int64_t(input.rateA / 100) + 250)) * 40000);
}

Result rejection(const char* reason) {
    Result result;
    result.reason = reason;
    return result;
}

struct Builder {
    Result result;
    std::uint32_t lastOperation = kNoIndex;

    Operation& operation(OperationKind kind, std::uint32_t address, std::int32_t a = 0,
                         std::int32_t b = 0) {
        Operation value;
        value.kind = kind;
        value.address = address;
        value.a = a;
        value.b = b;
        value.sequence = static_cast<std::uint32_t>(result.effects.size());
        const std::uint32_t previous = lastOperation;
        result.operations.push_back(value);
        lastOperation = static_cast<std::uint32_t>(result.operations.size() - 1);
        append(EffectKind::Operation, lastOperation, previous);
        return result.operations.back();
    }

    void write(WriteTarget target, std::uint32_t offset, WriteMode mode, std::int64_t value,
               std::int64_t cap = 0) {
        Write entry;
        entry.target = target;
        entry.offset = offset;
        entry.mode = mode;
        entry.value = value;
        entry.cap = cap;
        entry.sequence = static_cast<std::uint32_t>(result.effects.size());
        result.writes.push_back(entry);
        append(EffectKind::Write, static_cast<std::uint32_t>(result.writes.size() - 1),
               lastOperation);
    }

    void requireOwner(const char* reason) {
        result.status = Status::RequiresOwner;
        result.reason = reason;
    }

private:
    void append(EffectKind kind, std::uint32_t index, std::uint32_t afterOperation) {
        Effect effect;
        effect.kind = kind;
        effect.index = index;
        effect.sequence = static_cast<std::uint32_t>(result.effects.size());
        effect.afterOperation = afterOperation;
        result.effects.push_back(effect);
    }
};

void playCountWrite(Builder& builder, const Input& input) {
    if (input.playCount >= kPlayCountCap)
        return;
    const auto offset = std::uint32_t(input.statsStage * 0x19b + input.difficulty);
    builder.write(WriteTarget::PlayCount, offset, WriteMode::IncrementCapped, 1, kPlayCountCap);
}

void settleNextStage(Builder& builder, std::uint32_t record, int stageId) {
    builder.write(WriteTarget::NextStageRecord, 0, WriteMode::Assign, record);
    builder.write(WriteTarget::Stage, 0, WriteMode::Assign, stageId);
    builder.write(WriteTarget::StageMirror, 0, WriteMode::Assign, stageId);
}

}

bool routeFinalStage(int stage) {
    switch (stage) {
    case 3:
    case 5:
    case 8:
    case 10:
    case 13:
    case 15:
        return true;
    default:
        return false;
    }
}

Result plan(const Input& input) {
    if (input.opcode != kStageClearOpcode && input.opcode != kTimeScaleOpcode
        && input.opcode != kCutInOpcode) {
        return rejection("unsupported original post-Boss opcode");
    }

    if (input.opcode == kCutInOpcode) {
        if (input.sessionGate != 0) {
            Result result;
            result.status = Status::NoOperation;
            result.reason = "original [0x4b8954+0x74] gate closes the post-Boss cut-in";
            return result;
        }
        Builder builder;
        Operation& spawn =
            builder.operation(OperationKind::SpawnAnm, origin::kSpawnAnmCall, kCutInId, kCutInLayer);
        spawn.c = 0;
        spawn.receiverBase = origin::kManagerBase;
        spawn.receiverOffset = origin::kManagerAnm;
        spawn.outputSlotOffset = kCutInOutSlotOffset;
        builder.write(WriteTarget::Message, origin::kMessageHandle, WriteMode::AssignHandle, 0);
        builder.write(WriteTarget::Message, origin::kMessageField70, WriteMode::Assign, 0);
        builder.write(WriteTarget::Message, origin::kMessageField78, WriteMode::Assign,
                      input.messageTimer < 0
                          ? std::int64_t(wrapped(std::int64_t(input.messageTimer) - 1))
                          : 0);
        builder.write(WriteTarget::Message, origin::kMessageField74, WriteMode::Assign, 0);
        builder.requireOwner("ANM handle must be supplied by the original 0x4670b0 owner");
        return std::move(builder.result);
    }

    if (input.opcode == kTimeScaleOpcode) {
        if (!timeScaleStageInBounds(input.stage))
            return rejection("original MSG22 scene id outside 0..32");
        Builder builder;
        const float scale = routeFinalStage(input.stage) ? kFinalTimeScale : kPlainTimeScale;
        builder.operation(OperationKind::TimeScale, origin::kTimeScaleCall,
                          std::bit_cast<std::int32_t>(scale));
        builder.requireOwner("original MSG22 time-scale helper is an owner callback");
        return std::move(builder.result);
    }

    if (!playableStageInBounds(input.stage))
        return rejection("original MSG21 stage outside the playable 1..16 domain");

    const bool saves = (input.suspendFlags & kSuspendBit) != 0;
    const bool resets = !saves && input.sessionGate != 0 && (input.flowGate & 1) != 0;
    const bool settles = !saves && !resets
                         && (routeFinalStage(input.stage) || input.stage == kExtraStage)
                         && input.flowMode != 1;
    if (settles && !settlementInBounds(input)) {
        return rejection("original settlement indices outside the proven stats, difficulty and "
                         "play-count table");
    }

    Builder builder;
    builder.operation(OperationKind::StageClearEntry, origin::kStageClearEntryCall,
                      kStageClearEffectId, kCutInLayer);
    builder.operation(OperationKind::ClearSlots, origin::kClearSlotsCall);

    if (saves) {
        builder.operation(OperationKind::SaveReplay, origin::kSaveReplayCall);
        builder.requireOwner("byte [0x4b4d50]&0x10 selects the original save branch");
        return std::move(builder.result);
    }
    if (resets) {
        builder.operation(OperationKind::ResetPath, origin::kResetPathCall);
        builder.requireOwner("[[0x4b8a84]+0x1c]+0xa&1 selects the original reset branch");
        return std::move(builder.result);
    }

    if (routeFinalStage(input.stage)) {
        builder.result.bonusRaw = routeFinalBonus(input);
        builder.write(WriteTarget::GameManager, origin::kManagerFlags, WriteMode::OrMask, 0x10);
        builder.operation(OperationKind::AddScore, origin::kScoreCall, builder.result.bonusRaw);
        builder.write(WriteTarget::GameManager, origin::kManagerBonus, WriteMode::Add,
                      builder.result.bonusRaw);
        if (input.flowMode == 1) {
            builder.operation(OperationKind::ResetPath, origin::kResetPathCall);
            builder.requireOwner("[0x4b8a84]+0x10==1 short-circuits the settlement tail");
            return std::move(builder.result);
        }
        builder.write(WriteTarget::GameManager, origin::kManagerFlags, WriteMode::OrMask, 8);
        builder.write(WriteTarget::GameManager, origin::kManagerCounter, WriteMode::Assign, 0);
        playCountWrite(builder, input);
        settleNextStage(builder,
                        origin::kStageTable + (input.statsStage + kNextStageBias)
                            * kStageRecordBytes,
                        input.statsStage + kNextStageBias);
        builder.requireOwner("route-final settlement needs the original score/transition owners");
        return std::move(builder.result);
    }

    if (input.stage == kExtraStage) {
        builder.result.bonusRaw = extraStageBonus(input);
        builder.write(WriteTarget::GameManager, origin::kManagerFlags, WriteMode::OrMask, 0x10);
        builder.operation(OperationKind::AddScore, origin::kScoreCall, builder.result.bonusRaw);
        builder.write(WriteTarget::GameManager, origin::kManagerBonus, WriteMode::Add,
                      builder.result.bonusRaw);
        if (input.flowMode == 1) {
            builder.operation(OperationKind::ResetPath, origin::kResetPathCall);
            builder.requireOwner("[0x4b8a84]+0x10==1 short-circuits the settlement tail");
            return std::move(builder.result);
        }
        playCountWrite(builder, input);
        builder.write(WriteTarget::GameManager, origin::kManagerFlags, WriteMode::OrMask, 8);
        builder.write(WriteTarget::GameManager, origin::kManagerCounter, WriteMode::Assign, 0);
        settleNextStage(builder, origin::kExtraStageRecord, kExtraStageId);
        builder.requireOwner("extra-stage settlement needs the original score/ending owners");
        return std::move(builder.result);
    }

    builder.operation(OperationKind::SetObjectState, origin::kSetObjectStateCall,
                      std::int32_t(origin::kObjectStateTarget), kObjectStateValue);
    builder.operation(OperationKind::RouteAdvance, origin::kRouteAdvanceCall,
                      input.messageField78);
    builder.requireOwner("ordinary stage needs the original object/route owners");
    return std::move(builder.result);
}

}
