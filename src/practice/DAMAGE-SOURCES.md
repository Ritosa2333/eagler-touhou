# ReimuA damage-source compatibility

This adapter follows TH12's public `reimuADmgSrcs[5]` path at 40e8df and is now
connected to the canonical Runtime. Its purpose is separate from practice
parameters and live cheats: the original source also stores these snapshots for
ordinary ReimuA runs. Core compilation is recorded; no game, Oracle, browser,
syntax-check, test or playback acceptance was performed in this work.

The current adapter gates this path on the enabled THPrac profile, including
ordinary full runs. A replay containing an embedded damage snapshot is rejected
before replay-stage initialization if that profile is disabled.

The source records active entries for stages 2–6, then restores compacted entries
only when the new Player has no active source. An existing active pool owns the
real transition and must survive the native early-return guard. Original/custom
stage Practice and Extra do not use this full-run path.

## Connected ownership boundaries

`DamageSourceBridge.begin` replaces the prior generation for each new run,
retry or replay. It carries the replay/practice/character/shot identity and loads
validated embedded snapshots before independent replay-stage initialization.
Only a full-run ReimuA owner can apply this compatibility path.

`StageFlow::startNextStage` calls `damage::armTransition(flow.stage, stageNumber)`
before `startGame` resets the semantic pool. This preserves the actual current
128 slots and next ID for an adjacent stage. The Runtime also arms this boundary
before its separate full-run Replay transition path. Independent stage selection
does not arm a previous stage's carry.

`GameSession::startGame` calls `damage::onBirth(stageNumber)` after the Player/pool
reset and immediately before synchronous main ECL birth. It restores actual
transition slots first. A replay with active transition sources skips replacing
them with a saved entry; an independent stage with an empty pool restores the
recorded entry. Recording captures an immutable pre-main snapshot at this same
boundary. Saving mid-run reads that entry instead of substituting later state.

The carry retains the semantic source pool without resurrecting dead Bomb/beam
animations. Title exit resets the owner, and each fresh run replaces it. Disabling
the THPrac profile makes this path inapplicable. Live cheats remain disabled
during Replay.

`onBirth` exposes these results through the compatibility state:

| Result | Meaning |
| --- | --- |
| NotApplicable (0) | The current title/run identity does not use this path. |
| Recorded (1) | The actual stage-entry pool, including an empty pool, was captured. |
| Restored (2) | A validated recorded entry was restored. |
| PreservedTransition (3) | Actual active sources already own this transition. |
| Unavailable (4) | An older/native replay has no compatible snapshot for this independent stage. |
| Invalid (-1) | Resource or snapshot data is invalid; the Runtime raises an error. |

Unavailable is a compatibility limitation, not a synthesized empty snapshot or a
claim of native-replay equivalence. There is no user option advertising an
unsupported “preserve damage sources” function.

## Unified Replay payload

The normal `.rpy` input and stage-header format is retained. The Runtime's single
`TH12WEB1` extension carries optional `practice` and `damage` alongside movement
rows. Its enclosing JSON retains schema `th12-web-motion/1` and coordinate scale
128. Length and CRC32 are checked by the common extension reader before either
metadata field is interpreted. No second tail block or sidecar is appended.

`damage` has schema `eagler-touhou/th12-damage-sources/1`, layout
`th12-portable-damage-source/1`, character 0, shot 0, and at most five unique stage
records numbered 2–6. A recorded empty pool is distinct from a missing record.
The current Runtime passes `replay.extension?.damage` to `DamageSourceBridge`
before stage initialization, and supplies `damageSources.metadata()` to
`ReplayRecorder.setWebExtensions` before final serialization.

Each stage record stores exact signed-int32 words:

```text
magic=0x32315344, version=1, stage, nextId(uint32 bits), count<=128, stride=18,
then each active source in original slot order:
  id(uint32 bits), x,y,radius,growth,width,height,widthGrowth,heightGrowth,angle,
  ticks,previousTicks,damage,accumulated,limit,cadence,flags,remainingTime
```

Float fields retain their float32 bit patterns. Every field consumed by
`pw::Manager::tick/damage` is represented explicitly, independently of C++ padding.
Import validates exact version/stride/length, stage identity, unique source IDs
below nextId, active known flags, finite floats, representable remaining time and
safe damage totals. A complete temporary snapshot is validated before replacing
the stored entry.

Native `reimuADmgSrcs` arrays contain a different raw 0x74-byte layout and cannot
be reinterpreted as this semantic format. Unknown versions, native blobs,
duplicate stages and incompatible run identity are rejected. Missing original
heap state cannot be recovered from an ordinary retail Replay.

## Maintainer interfaces and remaining acceptance

The namespace is `th12::practice::damage`; its lifecycle helpers are declared in
`PracticeDamageSources.hpp`. The existing Wasm exports provide begin/reset,
transition arming, snapshot loading, stage words and state inspection. The
JavaScript bridge handles bounded marshaling; actual gameplay restoration stays
at the C++ pre-main boundary.

This connection still needs human observation of ReimuA stage transitions,
independent stage playback, retry/exit/re-entry and assisted Replay behavior.
Compilation does not establish those results. See the short user-review list in
[Launcher 接入](../../docs/LAUNCHER_INTEGRATION.md).
