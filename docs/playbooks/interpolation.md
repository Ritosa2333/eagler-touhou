# Interpolation Playbook

Status: active

## Purpose

Provide the normal architecture and safety checks for high-refresh presentation
without changing the original fixed 60 Hz game simulation. The current TH08 and
TH10 directory Runtimes are the main reference implementations; the rules are
cross-title only where ownership semantics actually match.

## Applicability and authority

Platform ownership lives in [Architecture](../ARCHITECTURE.md). Product-level
presentation semantics live in
[Adapter behavior invariants](../ADAPTER_BEHAVIOR_INVARIANTS.md). This playbook
owns recurring implementation failures and verification methods.

Applies to:

- TH08/TH10 SDL3/Web scheduling and Draw/Present work;
- Player, Enemy, Bullet, Item, camera and attached visual state;
- Draw-side timers, transient text and ANM behavior that may be repeated by a faster presentation loop.

Source experience:

- TH06/TH07 fixed-simulation and draw-only interpolation rules are proven
  reference experience.
- TH08/TH10 provide the current directory-Runtime field/owner-specific
  presentation implementation.
- The TH06 global ANM interpolation failure is a proven TH06 regression and a
  reusable lifecycle warning. It is not proof that another title reproduced
  the same bug.

Authority:

- gameplay semantics come from the target title's canonical source/portable
  contract;
- browser behavior comes from the target Runtime's current SDL3/browser
  implementation and focused tests;
- another title's presentation code supplies adaptation patterns only and
  cannot override target-game semantics.

<!-- knowledge-id: K-INTERP-001 -->
## Normal design

The intended pipeline is:

```text
fixed 60 Hz simulation
    ↓
publish previous/current logical state at each fixed tick
    ↓
renderAlpha = remainingAccumulator / fixedStep
    ↓
display-paced Draw/Present
    ↓
temporary interpolated visual values
```

Simulation owns the logical state. Draw reads a snapshot-like previous/current pair and computes a temporary visual value. The interpolated value is never written back to simulation state, rollback state, save data or Replay data.

The simulation tick alone may:

- consume keyboard, controller, touch or Replay input;
- advance RNG, ECL, gameplay timers, collisions, ownership and targeting;
- advance authored ANM scripts and other game state;
- publish the next previous/current presentation endpoint.

The display loop may draw more than once between ticks. It may interpolate positions, angles and other explicitly visual fields, but it must not resample input, run simulation, advance Replay/RNG/ECL, or use an interpolated coordinate for collision or target selection.

## Invariants

- Logical game time remains 60 Hz even on 120/144 Hz displays.
- Replay input is consumed at fixed simulation ticks only.
- RNG and ECL advance at fixed simulation ticks only.
- Gameplay timers and state transitions do not scale with presentation FPS.
- `prev/current` endpoints are published at the fixed-tick boundary. A shared sidecar may own common ANM visual endpoints when it is field-aware and lifecycle-gated; gameplay owners still own fields whose meaning is not globally uniform.
- Draw-only smoothing never writes into authoritative Player, Enemy, Bullet, Item, effect or rollback state.
- Attached visuals such as options, hitboxes, prompts, names and effects use the same owner presentation offset as the object they follow.
- Authored ANM VM advancement remains on the fixed simulation clock.
- Never lerp an `AnmVm` as a whole struct and never apply an unqualified global position rule. A shared presentation layer may interpolate explicitly classified continuous visual fields such as scale, rotation, color and UV, and may interpolate position only when ownership/eligibility proves the field was not rewritten by an owner.
- If a visual is retained at a fixed endpoint, drawing it uses the current state consistently rather than alternating interpolation step sizes.

<!-- knowledge-id: K-INTERP-003 -->
## Presentation purity and authored-current restoration

TH08 repair history produced direct source/runtime evidence that a presentation
system can become a gameplay change even when the interpolation math itself is
correct. Treat the following rules as hard boundaries:

- A presentation value must never participate in a simulation/state-machine decision. A concrete TH08 regression used an interpolated Item Y coordinate to decide `isOnscreen` and sprite state; the state decision must use the authoritative current position while only the draw coordinate is smoothed.
- Temporary Draw writes to `AnmVm`, Effect, Background or other owner fields must be restored to the state retail would leave after that logical tick. Restoring an interpolated midpoint, or leaving a projected/screen-space scratch value in the VM, lets the next fixed-tick `ExecuteScript`/update start from presentation state.
- A repeated high-refresh Draw must be idempotent within one logical tick. If drawing the same current state twice changes the input to the third draw, the renderer is mutating authoritative/scratch state and is unsafe.
- Tick-scoped visual output must survive repeated presentation. Transient text,
  temporary draw queues and similar data produced for one logical tick cannot be
  consumed or cleared by the first high-refresh Draw. Clear them at the logical
  owner boundary, or save/restore the consumed presentation state around a
  render-only pass. TH06 has direct regression/repair evidence; current TH08 and
  TH10 presentation paths also preserve relevant transient text state, but that
  implementation does not by itself prove an identical historical bug.
- Freeze/stop paths are lifecycle boundaries too. If simulation deliberately
  stops advancing an owner, stale presentation endpoints must not keep replaying
  old motion on every display frame. TH06 time-stop bugs made frozen knives
  re-rotate from an old angle and made the player visibly jitter until the
  presentation endpoints were synchronized to the frozen authoritative state.
  This is direct TH06 evidence and an adopted precaution for other titles.
- Preserve the retail scheduler's logical cadence. TH08 retail skips overdue time slices and executes at most one game-chain tick for one authored render opportunity; a Web accumulator must not catch up by running several `RunCalcChain()` calls back-to-back. Dropping backlog is preferable to changing game time, RNG/ECL/input ordering or Replay cadence.
- Lifecycle identity gates are mandatory for shared ANM sidecars. Script/ANM owner/visibility/lifecycle changes snap rather than interpolate across object reuse. TH06 object-reuse repair and the current TH10 presentation implementation support this rule; do not report it as a separately reproduced TH08 regression without TH08 evidence.

These rules apply even when a Stage 1 smoke reports low Draw/Calc cost. Performance evidence cannot upgrade a mechanism-changing implementation to semantic acceptance.

## Draw-side ownership examples

The following TH08 audit targets are reusable examples of Draw-side ownership
problems. Treat them as classes of failure to check in any title rather than as
a snapshot of one working tree.

### `GameManager::OnDraw`

Retail code advances `isInGameMenu` from `1` to `2` in Draw. A display-paced Draw would therefore advance the transition more than once per logical tick. Move the state transition to fixed-tick ownership while preserving the retail/non-modern path.

### `AsciiManager::OnDrawLowPrio`

The low-priority Draw path calls `ResetStrings()`. Transient strings must live for the intended fixed tick, not disappear after the first of several presentation draws. Text production and cleanup need a clear tick owner.

### `Gui::DrawGameScene`

HUD counters such as `lifeDisplayUpdateFrames`, `powerDisplayUpdateFrames`, `bombDisplayUpdateFrames`, `grazeDisplayUpdateFrames`, `pointDisplayUpdateFrames` and `timeDisplayUpdateFrames` were historically decremented from Draw. The same function also produces HUD text with `AddFormatText`. Move or separate simulation timing and text production before enabling repeated Draw, while preserving the visible one-tick lifetime and native behavior.

### `Ending::UpdateAndDrawFade`

The retail combined function updates `fadeTimer` and draws the fade. A modern
port may split update and draw, but the scheduler must still call the update
once per fixed tick and the draw once per presentation. Do not let a display
callback advance `fadeTimer`.

### `AsciiManager::OnDrawHighPrioImpl`

The `nightBlindnessVm` path can call `SetAndExecuteScriptIdx(..., 105)` from Draw. ANM activation and authored script advancement need fixed-tick ownership; Draw may only update temporary transform/color values and render the already-owned VM.

### Title and Result transient text

Title/Result Draw paths call `AddFormatText()`. Distinguish the producer that creates text for a logical state from the renderer that repeats that state. A presentation-only redraw must not duplicate, consume or prematurely reset transient strings.

<!-- knowledge-id: K-INTERP-002 -->
## Global ANM interpolation pitfall

TH06 previously attempted automatic interpolation of global ANM attributes and experienced lifecycle/VM regressions. The reusable lesson is not "never share ANM presentation infrastructure"; it is "never infer one uniform meaning for every field/lifecycle." Authored ANM advancement stays fixed-tick. TH08 may use a shared field-aware sidecar for common visual fields, but whole-VM interpolation, stale lifecycle endpoints and owner-rewritten positions remain prohibited.

## Superseded approaches

Global automatic interpolation of every ANM VM field is superseded by field-aware/lifecycle-gated presentation state. The opposite extreme - duplicating scale/rotation/color/UV shadows in every UI/effect owner - is also unnecessary when a shared sidecar can prove field ownership safely. Gameplay/world positions and custom geometry may still require owner-specific endpoints. Draw-side mutation that leaks presentation values into the next simulation tick remains prohibited.

## Code anchors

Current TH08/TH10 directory-Runtime reference anchors:

- `portable/sdl/FrameCadence.hpp` and `PresentationCadence.hpp`: display
  cadence versus fixed logical cadence;
- `th08_web/cpp/sdl/GameHost.cpp` /
  `th10_web/cpp/sdl/GameHost.cpp`: SDL loop and presentation scheduling;
- each title's `cpp/game/Presentation*`, player/enemy/bullet/item draw owners,
  background view and GUI view: previous/current or presentation-only state;
- TH08 `cpp/game/EclInterpolation.cpp`, `BulletDrawing.cpp`,
  `BackgroundView.cpp`, `GameplayScene.cpp`: concrete owner-specific
  smoothing examples;
- `portable/sdl/Renderer.cpp`: final vertex submission, including the
  subpixel-preservation boundary.

## Verification

After each structural slice:

1. Build the exact target directory Runtime from its canonical repository and
   record its build identity; do not compare against a stale copied artifact.
2. Run the target Runtime's focused cadence/presentation checks plus
   `git diff --check`.
3. Run the existing title → Stage 1 smoke and check that logical progression stays 60 Hz while display callbacks may be faster.
4. Re-run OGG, MIDI, storage and direct-entry Replay checks. A scheduler change is not accepted from a screenshot alone.
5. Confirm no browser `RuntimeError`, `Aborted` or page error and label Edge/Playwright/real-device evidence separately.

## Deliberately omitted claims

This playbook does not treat one title's accepted presentation path as proof
that another title can share the same ANM field ownership or lifecycle rules.
