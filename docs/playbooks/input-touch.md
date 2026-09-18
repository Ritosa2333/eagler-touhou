# Input and Touch Playbook

Status: active

## Purpose

Keep keyboard, mouse, touch, gesture, focus and mobile layout behavior tied to the correct owner and logical tick. Preserve Replay determinism while allowing presentation-only controls and helpers.

## Applicability and authority

- Touch is now an all-game adapter capability, not a TH06/TH07 precedent that a
  later title may silently omit.
- The machine-readable parent contract is
  `src/contracts/adapter-capabilities.mts`:
  `touch-controls` is required and `REQUIRED_TOUCH_BEHAVIORS` enumerates the
  shared behavior profile.
- TH08/TH10 share the portable `TouchController.hpp` policy and are covered by
  cross-repository deathbomb/movement/sensitivity gates.
- Device and orientation behavior must still be verified on the named device;
  source/workspace PASS is not a real-device claim.

<!-- knowledge-id: K-INPUT-001 -->
<!-- knowledge-id: K-INPUT-002 -->
## Normal design

Separate the layers:

```text
DOM/device event
    → input bridge / touch manager
    → one logical-frame sample
    → game input owner or Replay owner
    → presentation-only control visuals
```

Touch manager availability and the enable macro are separate concerns. The
shared sensitivity protocol is **100–300%** with 100 as the default; a Runtime
must not retain an older hidden 50% (or wider native) range when the Launcher no
longer exposes it. Preset/preview and four-tap copy are different operations.

Required touch behavior currently includes direct rate-limited and unlimited
drag, digital and free-direction joystick modes, hold/toggle/two-finger focus,
Fire/Bomb/Escape, original deathbomb timing, optional double-tap Bomb,
menu/dialogue navigation, mixed-input isolation and lifecycle cancellation.
The layout editor, magnifier and touch Restart button are Launcher-owned; a new
Runtime reuses them rather than inventing per-game UI/protocol commands.

Bind layout state to orientation: direction, portrait/landscape, viewport editor settings, runtime apply and window-position memory must use the same orientation owner. Focus controls should remain visible; magnifier is opt-in. Dialogue, StageClear, menus, help and fullscreen each need an explicit touch path rather than assuming gameplay controls cover them.

For Replay, record the logical run-state consumed by simulation, not a picture of touch buttons or an unbounded stream of raw browser events. Pause, restart and focus loss must have explicit lifecycle behavior.

## Invariants and pitfalls

- Sample touch delta at most once per logical frame. A permitted first-frame repeat is not a license to repeat later input; later missing samples are zero.
- Network retry, browser redraw and display refresh must never resample the device or feed the same input twice.
- Keyboard, touch and mouse must have one clear owner at each tick.
- Treat pointer/touch lifecycle recovery as implementation robustness, not as a
  product gesture. If an UP/CANCEL is lost or a browser reuses an ID, a later
  DOWN must be able to retire stale ownership before starting the new gesture.
- Ordinary live-control/config snapshots must update their own fields without
  clearing an unrelated active gesture. Only an owner-changing mode transition,
  disabling Touch, or a real lifecycle/context reset should retire it.
- Gameplay/menu/dialogue/replay context changes must invalidate ownership that
  is no longer legal in the new context. Delayed browser events or old snapshots
  must not re-arm a cancelled gesture after blur, visibility, pagehide, pause or
  another lifecycle boundary.
- Recompute the screen-to-game touch mapping after resize, orientation,
  fullscreen or viewport-transform changes. A stale rectangle is a coordinate
  mapping bug, not a new touch behavior.
- Rate-limited direct-touch implementations must clamp the stored reachable
  target at the actual movement bounds. Do not accumulate unreachable
  out-of-bounds displacement that later behaves like movement debt.
- A multi-touch gesture must not depend on one particular participating finger
  receiving the final UP. End/release ownership from the actual surviving
  gesture state, so browser event ordering cannot strand the menu gesture.
- One-shot Bomb/Pause/Confirm pulses expire in the state/context where they were
  produced. A rejected or missed pulse must not queue across death/respawn,
  menu/dialogue changes or another run.
- Restart/fresh-attempt boundaries invalidate the previous raw-touch lifecycle
  for Replay capture. Until a new DOWN creates a new point, orphaned MOVE/UP
  from the old attempt must be ignored.
- Pause-R, Continue, restart and focus loss are not interchangeable state transitions; use the game-specific owner and contract.
- The shared Restart control emits the ordinary R-key path. Every formal
  adapter must map that path to restart-current-run while the pause UI owns
  input; this is a required semantic action, not a per-game feature toggle.
- Touch Bomb must preserve the title's original deathbomb window and must not
  drop a held movement gesture merely because movement output is temporarily
  blocked during death/respawn state.
- Do not use CSS alone to fix an orientation-memory bug. Verify the saved window/layout owner.
- `less motion` reduces mobile presentation motion; it does not delete functionality or gameplay.
- Do not add unrequested D-pad, free remap or save-state features because a feedback item mentions them.

## Superseded approaches

Recording only raw touch graphics, fixing orientation-memory defects with CSS alone, or treating preset/preview as the four-tap copy action is superseded. The logical input owner remains the source of Replay truth.

## Code anchors

- `src/contracts/adapter-capabilities.mts` — required touch and
  option classification.
- `tests/test-portable-touch-controller-contract.mjs` — TH08/TH10
  shared controller, deathbomb and sensitivity boundary.
- `tests/test-required-gameplay-actions.mjs` — shared Restart/R
  semantics across TH06/TH07/TH08/TH10.
- `tests/test-always-hitbox-contract.mjs` — all-game hitbox
  presentation contract.
- sibling TH08/TH10 `portable/input/TouchController.hpp` — canonical
  directory-Runtime policy for those titles.

## Verification

1. Test one input source and one logical frame at a time; log whether the source was keyboard, mouse, touch or Replay.
2. Check focus, blur, pause, restart, menu, dialogue, StageClear and fullscreen transitions.
3. For touch, test portrait and landscape independently, including orientation change after saved layout state.
4. For Replay, compare the input stream and first divergent logical frame; do not rely on a button screenshot.
5. Label desktop browser, emulator/WebView and real-device manual evidence separately.

## Deliberately omitted claims

The static/workspace contracts do not claim iOS/Android real-device orientation,
long-duration mobile stability, or visual feel. Those require the corresponding
browser/device acceptance lane.
