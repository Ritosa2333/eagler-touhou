# TH12 fixed-tick / high-refresh presentation draft

This experiment separates fixed-tick capture from repeated WebGL presentation.
The core also records output-only presentation generations at existing logical
boundaries. Simulation, input, Replay, collisions, ECL, RNG and ANM execution
retain their existing 60 Hz ownership.

## Repository identity and status

- Canonical checkout: `D:/Programming/TouhouWeb/github-upload/th12`, branch
  `main`, base `b40dfb5c55bec2ebbfa4bdcabdb877a5f9500f27`.
- Experiment: `D:/Programming/TouhouWeb/worktrees/th12-high-refresh`, branch
  `experiment/high-refresh`, HEAD at that same base, with uncommitted changes.
- The separate `experiments` branch remains frozen at
  `492eddf16a3d9389cb529bedf06555b2cafcabe5`.
- Upstream-tracking checkout identity was not established by this task.
- This is an uncommitted implementation draft, not an accepted promotion
  boundary. No commit, push, integration, generated runtime or Launcher metadata
  change is included.

The implementation follows the Launcher interpolation and adaptation-worktree
playbooks. The user's explicit instruction for this task was static editing
only. No tests, syntax checks, build, browser, game or emulator were run.
Runtime behavior, performance, Replay equivalence and full repeated-draw purity
remain unverified. Presentation Lab admission remains unknown.

## Host integration API

`Renderer.capturePresentation(core, fps = {})` reads and copies one fixed-tick
endpoint. Call it immediately after an authoritative tick, after that tick's
state changes, and before the next tick. It creates background geometry, samples
legacy fallback animations at the authoritative integer age, materializes
transient text, and orders the final poses. Its return value owns JS data rather
than views into the WASM scratch draw buffers.

`Renderer.renderPresentation(previous, current, interpolationAlpha = 1)` draws
only those endpoints. It does not read the core, sample an animation, tick a
timeline, advance a timer, consume input, or call a random source. The alpha is
clamped to `[0, 1]`; `0` uses the previous owner position with current authored
appearance, and `1` draws the current endpoint. The normal interpolation delay
is one logical tick. There is no extrapolation.

`Renderer.resetPresentation()` increments the endpoint epoch and resets the
legacy sampler. Discard the host's previous/current pair at the same time.
Use this boundary on start/restart, stage replacement, Replay seek, pause/resume,
focus loss, timing/backlog reset and presentation-mode changes. Capture the
new current endpoint and initialize previous to that same endpoint. An automatic
frame regression also changes the epoch; it cannot detect every same-frame
restart without the host lifecycle call.

Existing `Renderer.render(core, fps)` remains the current-endpoint 60 Hz entry:
it disables native metadata capture with `presentationContinuity: false`,
captures once, then calls `renderPresentation(null, current, 1)`. Its default
output does not opt into interpolation. Do not use this convenience entry for
extra display-paced presentations.

Typical host ownership is:

```js
let previous = null, current = null;

function resetGamePresentation() {
  renderer.resetPresentation();
  current = renderer.capturePresentation(core, presentationOptions());
  previous = current;
}

function afterAuthoritativeTick() {
  const next = renderer.capturePresentation(core, presentationOptions());
  previous = current ?? next;
  current = next;
}

function presentGame(accumulator, fixedStep, smoothingEnabled) {
  if (!current) return;
  const alpha = smoothingEnabled ? accumulator / fixedStep : 1;
  renderer.renderPresentation(previous, current, alpha);
}
```

`presentationOptions()` supplies the existing score, practice, stage-clear,
recorded-FPS and current-FPS fields at capture time. `fps.alwaysHitbox` uses the
original bullet77/child76 static sprite when `_th12_player_state() === 1` and
no focus VM is present. It uses priority 24 and the player owner displacement.
It samples only during capture, creates no core VM, changes no focus input and
does not change hit detection. Existing focus visuals retain their lifecycle.
The selected original font also retains the main integration's `Eagler Unicode`
fallback before the generic family.

The host owns `limitPresentationTo60`: throttle only calls to the presentation
entry. It must not change which tick consumes input, Replay records, wall-time
observation, sound events, game/RNG updates, or endpoint publication. A frozen
scene presents current with alpha 1 (or previous equal to current). The existing
retail backlog/cadence policy must be preserved; this renderer adds no scheduler
and does not authorize catch-up ticks.

`site/game.mjs` is intentionally not edited in this worktree. Integration must
add the capture after the fixed tick and lifecycle resets at the host's real
scene/session boundaries. Cached menu/ending presentation must also keep their
timeline updates on the logical clock.

## Explicit owner policy

The sidecar translates a current owner by
`(previousPosition - currentPosition) * (1 - alpha)`. It never interpolates an
entire ANM VM or writes a presentation value into the core. Sprite, lifecycle
and visibility decisions remain authored-current. Other explicitly classified
visual fields are eligible only with the source-owned metadata below. Logical
current positions continue to drive offscreen/collision/targeting decisions.

| Owner | Position policy |
| --- | --- |
| Player `ffff0000` and options `ffff0001..ffff001f` | Shared player displacement from HUD's authoritative player position. The hit marker uses that same displacement. |
| Focus `ffff0020` | Core metadata identifies the live handle and attached lineage; only that set shares player displacement. Retired focus nodes keep independent ownership. |
| Bullet/projectile `1xxxxxxx` | Stable source owner and VM lineage; phase and nonpositive-duration target relocation form boundaries. |
| Item body/arrow/hint `2/3/4xxxxxxx` | One item displacement; offscreen arrows use its X component only. Appearance or body/arrow transitions snap. |
| Enemy/Boss registered slots | Stable owner position and resolved local/parent positions. ECL300/302, circular geometry setters, Boss-position copies and per-slot parent/offset writes increment owner discontinuity. |
| Boss indicator / extra Bomb shield | Associated with the actual Boss/enemy owner; the indicator follows X while retaining projected Y. |
| UFO ring/fill/health and attached countdown | Associated with the live UFO enemy. Fixed panel text and retained defeat reward text keep separate screen owners. |
| Bomb visuals/orbs/custom effects | Explicit engine origins. Absolute-coordinate sweep roots remain zero. |
| Straight laser main/cap | Stable serial/kind/phase/extension/topology and VM lineage admit fixed-corner interpolation. External position/width/angle writes or followed-Boss discontinuities snap. |
| Curve laser main/cap | Stable source-defined history-rank cross-section roles within an unchanged topology generation. Every destructive cut increments topology; no unclassified history array pairing is admitted. |
| STD camera/primitives and stage wave mesh | Same background generation/flags and primitive/grid lineage; interpolate camera vectors/FOV/fog/shake and classified visual coordinates. |
| UFO fill / spell ring / summon pulse / classified trail or leaf | Stable kind, node lineage, primitive/appearance and vertex-role count. Birth, growth topology and removal snap. |
| Other ANM local position | Only an explicit authored position track with unchanged resolved engine origin. |
| Common ANM rotation / scale / UV / color / alpha | Only recorded active authored velocity or interpolation fields. Owner rewrites do not acquire automatic scalar eligibility. |
| Unknown owners / legacy laser segments | Current endpoint. |

Legacy matching requires the same epoch/stage, consecutive logical frames, matching
draw-state entity/appearance/interrupt with age unchanged or advanced by one,
the same complete set of stable ANM IDs or owner-scoped fallback keys, and
matching visible/mode/layer/blend/anchor/clip/geometry topology. Ambiguous keys
snap. New, removed, hidden, rebound or appearance-changing sets therefore use
current rather than cross-fading ownership. Freeze/repeated-frame captures
also use current, so a stopped owner does not replay old motion.

The native path additionally requires matching core epoch, source owner
revision and complete ancestor ID/revision lineage. Appearance and topology
boundaries gate complete attached owner sets. Explicit VM writes can cause a
one-tick visual snap; uncovered custom writers must add a source-owned boundary
or remain current. Complete lifecycle coverage remains unknown until runtime
and fingerprint evidence exists.

Without the new core ABI, the legacy fallback only smooths player/projectile/
item translation, with 16/64/32 px jump guards. Enemies, UFO, lasers, background
and unknown owners remain current. That fallback is not evidence of full-game
continuity coverage.

Mode0's native floor/nearest-even corner construction runs at the current
endpoint. The presentation offset is appended to the final vertices after
that rounding boundary, so subpixel displacement survives GPU submission.
Ordinary 60 Hz calls use a zero offset and keep the native rounding path.

## Static owner/source audit

- `src/wasm/Exports.cpp::th12_draw` rebuilds a scratch vector from active
  bullet, bomb, item and player state. Entity IDs distinguish draw owners;
  Enemy draw intentionally emits no body fallback. No game update/input/RNG
  call is present in this exporter.
- `src/game/Presentation.cpp::captureAnimationDraw` rebuilds pose/schedule
  vectors. `resolveAnimationTransform` already resolves parent position and
  immediate-parent scale/rotation. Owner translation must therefore be applied
  once after resolution, never once again per ancestor. The new metadata
  explicitly classifies owner-computed projectile heading as visual rotation.
- `src/game/AnmScene.cpp` allocates monotonic node IDs within a scene, inherits
  owner on attached children, and clears owner/parent on detached children.
  Scene reset restarts IDs; `rebind` may retain an ID. Host epochs and the
  limitations above matter at those boundaries.
- `src/game/EnemyManager.cpp::synchronizeEnemyAnimations` rewrites per-slot
  engine coordinates using enemy offsets and linked-slot script positions.
  `EclInterpreter.cpp` 300/302 can replace a position immediately. Because
  these ownership/discontinuity fields are not in the old draw ABI, the new
  presentation metadata records their explicit owner boundaries separately.
- `src/game/PlayerSystem.cpp` positions body/focus on the logical clock.
  Focus release leaves retiring VMs independent of the live focus handle.
  `th12_draw` supplies options with distinct IDs; presentation assigns the
  entire live player set the same player offset instead of separately lerping
  option positions.
- `src/game/ItemPresentation.cpp` owns body/offscreen-arrow visibility, item
  hints, and aggregate lifetime. Its UFO update rewrites the ring, fill child
  and health bar, while `ufo-presentation.mjs` distinguishes live enemy text
  from retained defeat reward text. Metadata attaches only the live moving set
  to its UFO enemy; retained reward coordinates remain their own owner.
- `src/game/PresentationGeometry.cpp::captureCustomGeometry` builds local mesh
  output from the registered owner chain. UFO and stage custom draw helpers
  are `const`; spell/UFO rings, laser main/cap, Sanae leaves and score glyphs
  have different topology/owner contracts. Eligible anchored owners receive
  rigid translation; only classified stable geometry roles admit shape
  interpolation. An unqualified vertex/history array is never sufficient.
- `src/game/LaserDraw.cpp`, `LaserWorld.cpp` and `CustomGeometry.cpp` distinguish
  serial laser owners, traveling heads, retained curve nodes and cap visibility.
  Known main/cap roles interpolate only inside the same recorded topology and
  lifecycle. Equally sized arrays alone do not establish role correspondence.
- `Snapshot.cpp` message/spell/UFO/background captures and
  `BossHud.cpp::captureBossHud` serialize current state into scratch strings.
  Tick text descriptors are materialized during endpoint capture, so an extra
  presentation does not consume a transient string or decrement its timer.
- `renderPresentation` still changes WebGL buffers, texture/cache state and
  same-frame stage-surface scratch targets as rendering requires. Those are
  presentation resources. The static read does not establish complete
  authoritative/rollback fingerprints or Presentation Lab purity.

## Native metadata ABI and integration fragments

`src/game/PresentationMotion.cpp` serializes version 1 output-only metadata:
core epoch/frame, live player/focus identity, background revision, explicit
owner positions/revisions/kinds, node owner/parent/ancestor revisions and
authored continuous-field masks, classified mesh kinds/primitive slots, and
laser serial/phase/topology/extension/followed-Boss identity. Endpoints already
contain the actual mesh positions. It reads VM diagnostics without ticking
them and changes only its output string.

`PresentationTracking.hpp` holds the output-only owner sidecar. Scene reset
changes its epoch. VM bind/rebind/interrupt and explicit authored visual or
builtin coordinate writes change a VM presentation revision. ECL immediate
coordinate/slot changes mark owners; `BulletTargetMotion.cpp` marks
nonpositive-duration relocations. STD direct camera/fog/shake writes and label
seeks mark the background. Successful laser cuts mark topology. These fields
are not read by gameplay, targeting, collisions, RNG or Replay serialization.

The renderer reads this packet only at fixed capture boundaries. Repeated
presentation can recompute loaded logical background geometry, which bypasses
the legacy background sampler. Ordinary `render(core, fps)` disables metadata
capture; the old core ABI retains its conservative fallback.

Source discovery automatically includes the new `.cpp`. To preserve concurrent
main edits, the following shared-file changes are fragments only. In
`src/wasm/Exports.cpp`, add the include and functions inside the existing C ABI:

```cpp
#include "../game/PresentationMotion.hpp"
// Inside extern "C":
const char* th12_presentation_motion() {
  return th12::capturePresentationMotion().c_str();
}
int th12_presentation_motion_size() {
  return int(th12::presentationMotionBuffer.size());
}
```

In `src/wasm/Api.hpp`'s existing C ABI declarations:

```cpp
const char* th12_presentation_motion();
int th12_presentation_motion_size();
```

In `scripts/build.mjs`:

```js
exports.push('_th12_presentation_motion', '_th12_presentation_motion_size');
```

The parent integration separately supplies `_th12_player_state`. None of these
shared files is overwritten here. The independent core diff is supplied in
`HIGH_REFRESH_CORE.patch`; JavaScript and this document are ordinary worktree
changes. Metadata JSON cost and 120/144 Hz visual/cadence behavior remain
unmeasured. Menu/ending clocks still belong to the host's fixed tick.

Required runtime/playbook gates remain not run at the user's instruction;
no acceptance result or complete Presentation Lab coverage is claimed.
