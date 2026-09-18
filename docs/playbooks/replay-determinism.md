# Replay Determinism Playbook

Status: active

## Purpose

Keep Replay playback and deterministic game behavior independent from display refresh, browser callbacks and presentation smoothing.

## Applicability and authority

Applies to:

- Replay input and playback;
- fixed-tick scheduling;
- RNG, ECL, gameplay timers and authoritative state;
- IDBFS save/Replay persistence;
- regression testing after Web or presentation changes.

Authority:

- Each title's Replay and game semantics come from that title's source,
  preserved file-format contract and authoritative fixed-tick behavior.
- Browser persistence evidence proves storage only; it does not prove
  deterministic playback.
- TH06/TH07 Replay and rollback rules are adaptation reference, not permission
  to add MP or Replay-extension behavior to another title.

<!-- knowledge-id: K-REPLAY-001 -->
## Normal design

```text
fixed simulation tick
    ├─ consume one logical input source: live input or Replay
    ├─ advance RNG / ECL / game timers
    ├─ mutate authoritative state
    └─ publish state for presentation

display-paced Draw/Present
    └─ read state and draw; never consume Replay or advance simulation
```

Replay must enter the same logical input consumption path as the original game. Presentation may run zero, one or many times between simulation ticks and must not change the number or order of Replay events consumed.

## Invariants

- Replay input belongs to the fixed simulation tick, not the display callback.
- Display refresh rate must not change logical frame count, RNG sequence, ECL order, collision results, score, life, bomb, stage or ending behavior.
- Interpolated positions are display-only and never enter Replay input, collision, targeting, RNG, ECL or saved state.
- A rollback or replay re-run, where applicable, reuses captured logical input and does not resample DOM/touch/controller state.
- Original Replay v6 bytes and file paths remain compatible.
- IDBFS restore must complete before a run can overwrite existing persisted state; flush is a lifecycle/storage concern, not a new game-side save format.
- TH08 ordinary Replay stays ordinary Replay. Do not import TH06/TH07 multiplayer Replay extensions, EAGX fields or room state.

<!-- knowledge-id: K-REPLAY-003 -->
## Playback launch is not determinism proof

A Replay test that verifies only these facts:

```text
file bytes restored
Replay path opened
playback mode entered
Stage 1 reached
no RuntimeError
```

is a useful storage/lifecycle smoke, but it cannot detect the common failure mode where the Replay input stream still advances while authoritative gameplay gradually diverges. Do not promote such a smoke to Replay correctness.

When an automated determinism proof is actually required, compare fixed-tick authoritative state and report the first divergence. Useful fields include Replay input/cursor, logical frame, Player authoritative position, RNG seed/state, game/stage timer and bounded Enemy/Bullet/Item state hashes. Do not hash presentation sidecars, pointers, padding or interpolated Draw state.

Human gameplay feedback that a Replay's operations visibly diverge is sufficient evidence of failure. It does not identify the root cause, but it overrides earlier launch/storage smokes as an acceptance claim.

For the current TH08 handoff, the user explicitly requested no further Replay automation before the next repaired build is ready; finish semantic repairs and let the user test the Replay manually first.

<!-- knowledge-id: K-REPLAY-002 -->
## Storage and playback evidence are separate

A Replay fixture surviving write → sync → full reload byte-for-byte proves
storage/persistence. A direct-entry or menu smoke that reaches gameplay proves
launch/lifecycle. Neither proves deterministic operation playback. For a
determinism claim, compare fixed-tick authoritative state or perform an
equivalent accepted playback validation.

## Superseded approaches

Menu-only Replay smoke is superseded as a sufficient playback gate because unlock state can block the route. "Direct entry reached Stage 1" is also superseded as a sufficient determinism gate. Historical ReplayX intermediate formats are not part of the supported public compatibility boundary; use the documented compatibility boundary and fail closed for unknown versions.

## Code anchors

Replay authority remains title-specific.

- TH08 directory Runtime: `th08_web/cpp/game/ReplayPlayback.cpp`,
  `ReplayRecording.cpp`, `ReplayStream.*`, `ScoreStore.*`,
  `GameplaySession.*`, plus `sdl-runtime/shell.mjs` for IDBFS/file commands.
- TH10 directory Runtime: the corresponding `th10_web/cpp/game` /
  `cpp/platform` Replay/World owners plus `sdl-runtime/shell.mjs`.
- TH06/TH07: their existing Replay managers/ReplayExtension owners in the
  sibling Runtime repositories.
- Launcher: `src/contracts/adapter-capabilities.mts` and the shared Replay
  manager UI own required behavior and file-management surface, not game
  simulation.

## Verification

After scheduler, input, storage or Replay changes:

1. Run static/build checks for the affected lane and `git diff --check`.
2. Verify a Replay fixture survives write → sync → full reload byte-for-byte.
3. Use the direct-entry Replay hook to reach actual playback when storage/lifecycle coverage is needed, but do not call that determinism PASS.
4. For automated determinism, compare the earliest fixed-tick authoritative divergence; use manual playback acceptance when the claim concerns real user-visible operation fidelity.
5. Run title/Stage 1 plus OGG/MIDI/storage smoke as appropriate and separate static, browser, device and deployment evidence.

Do not weaken checksum or hash-based correctness checks to make a Replay test pass. A test-navigation failure should be repaired in the test setup or direct-entry path, not hidden by removing correctness validation.

## Deliberately omitted claims

Storage, launch and browser smoke results are not promoted to Replay
determinism or real-device acceptance.
