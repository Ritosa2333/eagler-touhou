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

## Formal verification lanes

Every supported title owns a Replay verifier in its Runtime/source repository.
The verifier is maintainer tooling, not a Launcher feature and not a new
Launcher↔Runtime command surface.

The required lanes are:

- `quick`: run every built-in title Demo in the title-owned rotation and compare
  it with published original-derived fixed-tick traces;
- `daily`: run the corpus's declared standard loadout on Lunatic and Extra,
  usually the second character's second configuration, plus a title-specific
  additional special difficulty when one exists. Map that policy through the
  title's real selection model instead of inventing character/shot equivalence;
- `oracle`: explicitly run the original executable/provider to regenerate
  expected traces. This is advanced maintenance, never an implicit part of
  ordinary candidate checks.

`.rpy` files are input fixtures, golden traces are immutable expected results,
and oracle capture is the process that creates proposed expected results. Do
not call all three an "oracle Replay" in manifests or reports because that hides
which authority is being exercised.

## Golden identity and update discipline

A published golden set binds at least the Replay hash, original executable and
resource identity, state-schema/adapter identity, completion evidence, tick
counts and content hashes. The candidate build and its output are run evidence,
not golden provenance.

Quick and daily checks are read-only against golden data. Candidate code cannot
write, replace or bless expected traces. Oracle regeneration and golden
acceptance are separate reviewed operations; a changed expected trace is not a
normal way to make a failing candidate green.

The comparison fails closed on incompatible identity, missing/extra/reordered
ticks, incomplete lifecycle, effective input, RNG or any declared authoritative
state category. It reports the earliest observed divergence without cropping,
frame shifting or resynchronizing later matching state.

Observation remains diagnostic-build-only and read-only. Do not alter normal
single-player logic, the built-in Demo mechanism or the separately compiled
multiplayer Runtime to satisfy a verifier. Multiplayer may own a separate
self-determinism profile and is not required to match retail single-player
state.

## Fix ownership after a divergence

When the first divergence identifies gameplay, collision, RNG, ECL, timer,
Replay consumption or native state-machine semantics, fix the lowest
authoritative title/source owner and propagate it to every downstream Eagler
branch that contains the same core. Do not leave a core semantic correction
only in an Eagler packaging/diagnostic branch.

Browser collection, diagnostic exports, golden manifests and comparison tools
remain in their adapter/test owners. A platform-only workaround must not mask a
shared upstream gameplay bug.

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
- Per-title verifier adapters, corpus manifests, golden traces and commands:
  `tools/replay-verifier/` in each sibling title repository.
- Game-agnostic trace lifecycle, validation, comparison and original Present
  observation infrastructure: sibling
  `eagler-common/testkit/replay-verifier/`.
- Launcher: `src/contracts/adapter-capabilities.mts` and the shared Replay
  manager UI own required behavior and file-management surface, not game
  simulation.

## Verification

After scheduler, input, storage or Replay changes:

1. Run static/build checks for the affected lane and `git diff --check`.
2. Verify a Replay fixture survives write → sync → full reload byte-for-byte.
3. Use the direct-entry Replay hook to reach actual playback when storage/lifecycle coverage is needed, but do not call that determinism PASS.
4. Run the title's `quick` Demo golden gate.
5. For a standard logic regression or release gate, run the `daily` long-Replay
   golden suite and compare the earliest fixed-tick authoritative divergence.
6. Regenerate an oracle only when maintaining the provider/schema/fixture or
   investigating whether the accepted golden itself is wrong; never overwrite
   golden data from an ordinary candidate run.
7. Use manual playback acceptance when the claim concerns real user-visible
   operation fidelity, and run title/Stage 1 plus OGG/MIDI/storage smoke as
   appropriate. Keep static, browser, device and deployment evidence separate.

Do not weaken checksum or hash-based correctness checks to make a Replay test pass. A test-navigation failure should be repaired in the test setup or direct-entry path, not hidden by removing correctness validation.

## Deliberately omitted claims

Storage, launch and browser smoke results are not promoted to Replay
determinism or real-device acceptance.
