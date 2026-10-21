#pragma once
#include <cstdint>

namespace th08 {
// Stable word layouts consumed by the validation driver. All pointers remain
// inside the host; these records contain only copied values and source receipts.
constexpr std::uint32_t dialogue_touch_manifest_magic=0x44544d31u; // DTM1
constexpr std::uint32_t dialogue_touch_observation_magic=0x44544f31u; // DTO1
constexpr std::uint32_t dialogue_touch_scene_version=1u;
constexpr std::uint32_t dialogue_touch_manifest_header_words=24u;
constexpr std::uint32_t dialogue_touch_manifest_candidate_words=10u;
constexpr std::uint32_t dialogue_touch_manifest_candidate_capacity=32u;
constexpr std::uint32_t dialogue_touch_manifest_words=
    dialogue_touch_manifest_header_words+
    dialogue_touch_manifest_candidate_words*dialogue_touch_manifest_candidate_capacity;
constexpr std::uint32_t dialogue_touch_observation_words=45u;

enum class DialogueTouchManifestReason:std::uint32_t {
    Ready=0,NoRuntime=1,RuntimeNotReady=2,WrongRun=3,FixtureGateMissing=4,
    WriteLockInvalid=5,SourceMissing=6,InvalidSource=7,CandidateOverflow=8,
    NoConfirmEntry=9,
};
enum class DialogueTouchRequestStatus:std::uint32_t {
    Never=0,Accepted=1,InvalidEntry=2,RuntimeUnavailable=3,WrongRun=4,
    FixtureGateMissing=5,WriteLockInvalid=6,DialogueAlreadyPresent=7,
    EntryNotReferenced=8,TimelineActionMissing=9,OpenFailed=10,
    TouchFireNotHeld=11,
};

// Header words 0..23: magic, version, total words, available, reason,
// runtime generation/pointer/epoch, stage/character/team/difficulty mask,
// ECL byte size and FNV64, timeline count, MSG entry count/size and FNV64,
// ECL/MSG asset ids, candidate count and row stride. Stage-1 normal asset ids
// are 1=ecldata1.ecl and 1..4=msg1a.dat..msg1d.dat.
// Each candidate row is: MSG entry, ECL timeline, ECL frame, ECL difficulty
// bits, ECL byte offset, reference count, first reachable Wait byte offset,
// Wait instruction time, Wait automatic timeout frames, MSG instruction count.
struct DialogueTouchManifestV1 {std::uint32_t words[dialogue_touch_manifest_words]{};};

// Observation words: magic/version/count/available/reason; runtime generation,
// pointer, loop epoch and host frames; app active/in-game, stage/character/flags;
// validation fixture/gate lifecycle and write-lock receipt; dialogue present,
// message, instruction offset/opcode/time, timer, paused frames, minimum wait,
// skippable/selection; current/previous input consumed by Dialogue, fresh
// confirm edge, and consumer sequence; control/ECL frames; touch Fire/context/
// hosted Z; request serial/status, entry/ECL timeline; consumer validity. It
// contains no mutable scene state or pointers.
struct DialogueTouchObservationV1 {std::uint32_t words[dialogue_touch_observation_words]{};};

static_assert(sizeof(DialogueTouchManifestV1)==dialogue_touch_manifest_words*4u);
static_assert(sizeof(DialogueTouchObservationV1)==dialogue_touch_observation_words*4u);
}
