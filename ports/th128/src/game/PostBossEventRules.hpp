#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace th128::postboss {

inline constexpr int kStageClearOpcode = 21;
inline constexpr int kTimeScaleOpcode = 22;
inline constexpr int kCutInOpcode = 30;

namespace origin {
inline constexpr std::uint32_t kCommandDispatch = 0x4232a3;
inline constexpr std::uint32_t kDispatchTable = 0x42497c;
inline constexpr std::uint32_t kStageClearHandler = 0x42454e;
inline constexpr std::uint32_t kTimeScaleHandler = 0x4247f6;
inline constexpr std::uint32_t kCutInHandler = 0x42486a;
inline constexpr std::uint32_t kStageClearEntryCall = 0x424fb0;
inline constexpr std::uint32_t kClearSlotsCall = 0x41fe20;
inline constexpr std::uint32_t kSaveReplayCall = 0x438170;
inline constexpr std::uint32_t kResetPathCall = 0x437050;
inline constexpr std::uint32_t kScoreCall = 0x40e4e0;
inline constexpr std::uint32_t kSetObjectStateCall = 0x411c70;
inline constexpr std::uint32_t kRouteAdvanceCall = 0x4273a0;
inline constexpr std::uint32_t kTimeScaleCall = 0x434a20;
inline constexpr std::uint32_t kSpawnAnmCall = 0x4670b0;
inline constexpr std::uint32_t kManagerBase = 0x4b8950;
inline constexpr std::uint32_t kManagerAnm = 0x5a34;
inline constexpr std::uint32_t kManagerFlags = 0x5a04;
inline constexpr std::uint32_t kManagerBonus = 0x5a38;
inline constexpr std::uint32_t kManagerCounter = 0x5a3c;
inline constexpr std::uint32_t kMessageHandle = 0x6c;
inline constexpr std::uint32_t kMessageField70 = 0x70;
inline constexpr std::uint32_t kMessageField74 = 0x74;
inline constexpr std::uint32_t kMessageField78 = 0x78;
inline constexpr std::uint32_t kMessageTimer = 0x80;
inline constexpr std::uint32_t kStageTable = 0x4b1a30;
inline constexpr std::uint32_t kExtraStageRecord = 0x4b1dc8;
inline constexpr std::uint32_t kObjectStateTarget = 0x4d2e68;
}

inline constexpr int kMinimumPlayableStage = 1;
inline constexpr int kMaximumPlayableStage = 16;
inline constexpr int kMinimumTimeScaleStage = 0;
inline constexpr int kMinimumEndingScene = 0x14;
inline constexpr int kMaximumEndingScene = 0x20;
inline constexpr int kMaximumTimeScaleStage = kMaximumEndingScene;
inline constexpr int kMinimumStatsStage = 0;
inline constexpr int kMaximumStatsStage = 16;
inline constexpr int kMaximumRouteFinalStatsStage = 15;
inline constexpr int kMaximumDifficulty = 4;
inline constexpr int kExtraStage = 16;
inline constexpr int kExtraStageId = 0x17;
inline constexpr int kNextStageBias = 0x11;
inline constexpr std::uint32_t kStageRecordBytes = 40;
inline constexpr int kPlayCountCap = 0x1869f;
inline constexpr int kSuspendBit = 0x10;
inline constexpr int kCutInId = 0xa9;
inline constexpr int kCutInLayer = 0x17;
inline constexpr std::int32_t kCutInOutSlotOffset = -0x64;
inline constexpr int kStageClearEffectId = 0x52;
inline constexpr int kObjectStateValue = 12;
inline constexpr float kPlainTimeScale = 8.0f;
inline constexpr float kFinalTimeScale = 2.0f;
inline constexpr int kDifficultyMultipliers[5] = {5000, 10000, 20000, 40000, 40000};

inline constexpr std::uint32_t kNoIndex = 0xffffffffu;

enum class Status {
    Rejected,
    NoOperation,
    RequiresOwner,
};

enum class OperationKind {
    StageClearEntry,
    ClearSlots,
    SaveReplay,
    ResetPath,
    AddScore,
    SetObjectState,
    RouteAdvance,
    TimeScale,
    SpawnAnm,
};

enum class WriteTarget { GameManager, Message, Stage, StageMirror, NextStageRecord, PlayCount };
enum class WriteMode { OrMask, Assign, AssignHandle, Add, IncrementCapped };

struct Operation {
    OperationKind kind = OperationKind::StageClearEntry;
    std::uint32_t address = 0;
    std::int32_t a = 0;
    std::int32_t b = 0;
    std::int32_t c = 0;
    std::uint32_t receiverBase = 0;
    std::uint32_t receiverOffset = 0;
    std::int32_t outputSlotOffset = 0;
    std::uint32_t sequence = kNoIndex;
};

struct Write {
    WriteTarget target = WriteTarget::GameManager;
    std::uint32_t offset = 0;
    WriteMode mode = WriteMode::Assign;
    std::int64_t value = 0;
    std::int64_t cap = 0;
    std::uint32_t sequence = kNoIndex;
};

enum class EffectKind { Operation, Write };

struct Effect {
    EffectKind kind = EffectKind::Operation;
    std::uint32_t index = 0;
    std::uint32_t sequence = 0;
    std::uint32_t afterOperation = kNoIndex;
};

struct Input {
    int opcode = 0;
    int stage = 0;
    int statsStage = 0;
    int difficulty = 0;
    int rateA = 0;
    int rateB = 0;
    int messageTimer = 0;
    int messageField78 = 0;
    int playCount = 0;
    int suspendFlags = 0;
    int sessionGate = 0;
    int flowGate = 0;
    int flowMode = 0;
};

struct Result {
    Status status = Status::Rejected;
    std::string reason;
    int bonusRaw = 0;
    std::vector<Operation> operations;
    std::vector<Write> writes;
    std::vector<Effect> effects;
};

bool routeFinalStage(int stage);

Result plan(const Input& input);

}
