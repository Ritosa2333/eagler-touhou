# TH15 multiplayer economy and teammate actions

The authority for teammate actions is TH11's
`th11_web/cpp/game/MultiplayerBattle.cpp`: `mp_life_receiver`,
`mp_update_rules`, `MultiplayerPilot::revive`, `mp_collect` and directed gifts.
TH15 maps life items to kind 5 and large Power items to kind 3; original game
animation, pickup, option rebuilding and resource caps keep their typed owners.

- Within 20 logical pixels of a teammate, hold Shift without Z for 90 confirmed
  ticks. Prefer a ghost, otherwise the eligible teammate with fewer lives.
  The donor must have a spare life. One hold sends one life until Shift release.
- A ghost is rescued in place, retaining banked lives, with half maximum Power,
  normal base bombs and 280 invulnerable ticks. Native stage reset uses 120.
- Five Z press edges, each separated by at most 24 confirmed ticks, send one
  large Power to the lowest-Power eligible teammate within 20 pixels.
- Deduct a spare life or one Power level only after a transfer item is emitted.
  A full item pool does not consume stock. Power sent to a player who becomes
  a ghost returns to ordinary drops; life items can still add ghost reserves.
- Each chapter/collected life-fragment event adds exactly one shared fragment,
  regardless of how many teammates are alive. Ghosts receive the same banked
  resources but are not automatically rescued by an extend.
- TH15's Normal three-fragment threshold, native Extra thresholds and eight
  spare-life cap are retained. The HUD reads the shared count for every seat.
- User-selected small-Power amounts: 0.02 in a two-seat room, 0.03 in a
  three-seat room. This depends on room size, not surviving players. Large
  Power stays 1.00. Ordinary single-player amounts and full-Power scores stay
  unchanged.

Mobile life/rescue: switch fire off and hold the existing slow button near a
teammate. Power donation uses five distinct fire press edges, as in TH11.
Rescue feedback fades the ghost towards opacity while the confirmed hold
progresses; authoritative animation state is not modified by the feedback.

Gameplay ABI is 13. Old gameplay generations must not join this rule set or
replay with its economy semantics. The existing build identity and Replay
Archive gameplay-version checks reject mismatches explicitly.

Stage replacement resets all pilots while the old scene is detached. Life
and bomb HUD writes are deferred only inside that reset scope; the newly
bound StageGameplay initializes the HUD from the updated stocks. Ordinary
missing-scene errors remain errors. The guard also unwinds on reset failure.
Multiplayer spell timing uses confirmed application frames / 60 instead of
wall time, retaining the native clock encoding/rounding. Replay and spectators
derive the same time from confirmed steps; network waits do not inflate it.
Single-player timing and its native replay callback are unchanged.

Reimu/Marisa/Sanae share 120 startup protection ticks and refresh teammates
to at least 40 ticks on each active bomb update (including its final frame).
Reisen shares 30 ticks on startup and on each shield break, including the last
layer. Persistent shields remain personal; they do not grant perpetual team
invulnerability. Ghosts are excluded and longer existing protection is retained.

The three-seat HUD omits native fragment labels (front.anm scripts 9/11)
and both fragment numeric rows. Life/bomb icons keep their native partial-fill
animations; accounting is unchanged. Seat blocks use a 78-pixel pitch, bombs
at +26 and Power at +54 from the life row, with shared point/graze at y=330.
Two-seat HUD retains fragment labels/numbers and native resource row spacing,
but reduces the seat pitch to 120 pixels and moves shared point/graze to y=342.
Single-player layout is unchanged.

Chapter life-fragment reward thresholds use the chapter's displayed bonus:
1,500,000 in two-seat rooms and 2,000,000 in three-seat rooms, independent
of surviving teammates. The reward notice uses the same threshold as the
award; each eligible chapter still adds exactly one shared fragment. Native
single-player threshold (1,000,000), score gains and fragment accounting remain
unchanged.

Transfer feedback follows TH11 MultiplayerPresentation: an alive donor with
a valid life/rescue receiver shows the confirmed hold percentage above its
player (capped at 100%); otherwise the third/fourth Power tap shows a native
TH15 large-P icon and tap count. Life feedback takes priority. These are draw
copies/text only, with no spawned items or gameplay mutations. The native icon
script initializes once using private copies of its RNG/environment, never
advancing either authoritative RNG stream.

Only the selected local viewpoint's hitbox is drawn, including native focus,
always-visible markers and native fade-out. Generation-tagged ownership is
retained until the animation retires, so changing the selected viewpoint also
changes which marker is visible. This is a rendering filter, not a collision,
animation update, RNG, gameplay ABI or recorded-input change.

TH11 teammate opacity policy is applied to draw copies of each seat's body,
options, shots and attached children: local 255, remote 160,
remote within 48px 72, local ghost 112, remote ghost 88. Gameplay VM alpha,
hitboxes and RNG remain unchanged. Bullet red graze glow is aggregated across
all active seats rather than overwritten by the last collision check.
Bullet per-frame activation/turning and moving-laser transforms now select
the nearest alive pilot from their own origin, as initial emit/ECL already do.
The default world target also excludes ghosts instead of reading seat zero.

Power gifts require at least 2.00P and leave a 1.00P floor after donating
the unchanged 1.00P large item. No gift icon is offered below this threshold.
Dialogue retains TH11's short-tap confirm and stationary 500ms hold-to-skip
semantics, with explicit movement support when the native pilot is ready.
Dragging is not a confirm/skip gesture, auto-fire and double-tap Bomb are
suppressed, and menu/replay/real-death lifecycle cancellation remains intact.
Both single-player and multiplayer TH15 touch adapters supply dialogue movement.

`node portable/multiplayer/economy-check.mjs` exercises the actual resource
policy and ItemCollection, and checks TH11's action-source gates. Set
`TH15_EMSDK` when the SDK is elsewhere. Source/unit checks and compilation are
not human/device acceptance, an actual multiplayer transfer test, or full
Replay parity proof.

The player-facing Chinese rules are maintained in the Launcher repository's
`content/MULTIPLAYER.md`, section `TH15 绀珠传`; its generated HTML and shared
guide tab must be updated together. This document describes the implementation
contract, not a separate balance preset.
